from pathlib import Path
import abc
import datetime
import logging
import math

from multiprocessing.managers import SharedMemoryManager
from multiprocessing.shared_memory import SharedMemory

import numpy as np

import skyfield
import skyfield.timelib
from skyfield.api import load

from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.scheduler import Scheduler, EventPriority
from alea.sim.kernel.frames import ReferenceFrame, Universe
from alea.sim.kernel.time_utils import skyfield_time_to_gmst_radians
from alea.sim.kernel.time_utils import skyfield_time_to_gmst_radians
from alea.sim.kernel.frame_manager import FrameManager

_sentinel = object()

datestr = datetime.datetime.now().strftime("%d-%m-%y_%H-%M")
SESSION_DATA_PATH = Path(__file__).parent.parent.parent.parent / f"data/{datestr}"

class AleasimKernel():
    SCHEDULER_MAX_PRIORITY = 50

    def __init__(self, dt: float = 0.001, date: skyfield.timelib.Time | datetime.datetime | float = None) -> None:
        logging.basicConfig(level=logging.DEBUG, format="%(msecs)d %(name)s %(levelname)s %(message)s")
        self._logger = logging.getLogger('AleasimKernel')
        
        #time is seconds elapsed since the start of simulation (date + t0)
        #for precision purposes, time is not represented internally as binary floating point
        #to avoid issues such as 2.0 + 1.0 = 3.0000000000000003
        #self._t_n is the integer timestep multiple such that self._t = _t_n * dt
        self._t_n: int = 0
        self._t: float = 0

        #smallest timestep the simulation can take (though adaptive and substep integrators may take multiple steps between dts)
        self._dt: float = float(dt)

        #scheduler init
        self._scheduler = Scheduler(self._get_sim_time, self._advance_sim_time)
        
        #skyfield timescale
        self._timescale = load.timescale()
        #start epoch for the simulation
        if date is None:
            date = datetime.datetime.now(tz=datetime.timezone.utc)

        if isinstance(date, datetime.datetime):
            self._skyfield_time = self._timescale.from_datetime(date)
        elif isinstance(date, float):
            self._skyfield_time = self._timescale.J(date)
        elif isinstance(date, skyfield.timelib.Time):
            self._skyfield_time = date
        else:
            raise TypeError(f"Invalid type for date argument: {type(date)}")

        self._skyfield_epoch_time = self._skyfield_time

        #handle shared memory
        self._smm = SharedMemoryManager()
        self._smm.start()
        self._smm_size = 0

        self._root: AbstractModel = None
            
        #frame manager init
        self._frame_manager = FrameManager(self)

    @property
    def root(self) -> AbstractModel:
        """root model"""
        return self._root

    @root.setter
    def root(self, value: AbstractModel):
        if self._root is not None:
            self.logger.warning("root model of kernel could not be set (it is already set)")
        else:
            self.logger.info(f'root model of kernel set to {value}')
            self._root = value
    
    @property
    def logger(self) -> logging.Logger:
        return self._logger
    
    def set_log_level_all(self, log_level:int):
        """set log level for all models"""
        if self._root is None:
            self.logger.warning("set_log_level_all should not be called when root is none")
            return
        self._root.propagate_log_level(log_level)
    
    @property
    def scheduler(self) -> Scheduler:
        return self._scheduler
        
    @property
    def timescale(self) -> skyfield.timelib.Timescale:
        """
        skyfield.timelib.Timescale object bbuilt using official Earth rotation data
        useful for orbit propagations and frame conversions
        """
        return self._timescale

    @property
    def time(self) -> float:
        """
        Simulation time as accumulated seconds referenced to start epoch.
        This simulation epoch is defined in TAI at self._skyfield_epoch_time.tai.
        """
        return self._t

    @property
    def time_n(self) -> int:
        """
        Return simulation time as multiple of timestep since start
        """
        return self._t_n

    @property
    def skyfield_time(self) -> skyfield.timelib.Time:
        """skyfield.timelib.Time class representation of sim time"""
        return self._skyfield_time

    def skyfield_time_series(self, n_timesteps: int) -> skyfield.timelib.Time:
        (year, month, day, hour, minute, second) = self._skyfield_time.tai_calendar() # Use TAI since we need a uniform time-scale (UTC has jumps due to leap seconds)
        delta_seconds = (n_timesteps * self.timestep)
        end_second = second + delta_seconds
        time_series = self.timescale.tai(year, month, day, hour, minute, np.arange(second, end_second, self.timestep))
        return time_series

    @property
    def timestep(self) -> float:
        return self._dt

    @property
    def timestep_n(self) -> int:
        """timestep integer rep"""
        return 1

    @property
    def date(self) -> datetime.datetime:
        """
        Return internal skyfield time as python datetime (utc timezone)
        Due to the limitations of Python, additional leap seconds cannot be added.
        For calculations, use skyfield time or date_and_leap_second instead.
        """
        return self.skyfield_time.utc_datetime()

    @property
    def date_and_leap_second(self) -> tuple[datetime.datetime, any]:
        """Return internal skyfield time as python datetime (utc timezone) and leap second (usually leap second is 0)"""
        return self.skyfield_time.utc_datetime_and_leap_second()
    
    @property
    def gmst_rad(self) -> float:
        """Greenwich Mean Sidereal Time (GMST) angle of current sim time as radians"""
        return skyfield_time_to_gmst_radians(self.skyfield_time)
    
    @property
    def frame_manager(self) -> FrameManager:
        return self._frame_manager
    
    @property
    def universe(self) -> Universe:
        """Storage object for frame tree"""
        return self._frame_manager.universe
    
    @property
    def eci_frame(self) -> ReferenceFrame:
        """Earth centered inertial frame"""
        return self._frame_manager.eci_frame

    @property
    def body_frame(self) -> ReferenceFrame:
        """Spacecraft body frame"""
        return self._frame_manager.body_frame

    @property
    def ned_frame(self) -> ReferenceFrame:
        """North East Down frame"""
        return self._frame_manager.ned_frame

    @property
    def ecef_frame(self) -> ReferenceFrame:
        """Earth centered Earth fixed frame"""
        return self._frame_manager.ecef_frame

    @property
    def orbit_frame(self) -> ReferenceFrame:
        """Earth centered Earth fixed frame"""
        return self._frame_manager.ecef_frame
    
    def duration_to_integer(self, dur:float) -> int:
        """
        convert duration to integer multiple of timestep
        Ensures integer_multiple*timestep <= dur. Uses math.floor.
        """
        return math.floor(dur/self._dt)

    def add_model(self, model:AbstractModel, create_shared_mem: bool = True, parent: AbstractModel = None):
        """
        Add a model to the simulation.
        Creates shared memory for the model by default for any models implementing SharedMemoryModelInterface.
        If parent is None, the parent will be set to root.
        If the kernel does not currently have a root model, kernel._root will be set to to model
        """
        if self._root is None:
            self._root = model
            self.logger.info(f"added {self._root.name} as root model")
        elif model is self.root:
            self.logger.warning(f'model {model} is already the root model.')
            return
        elif self.root.is_child(model):
            self.logger.warning(f'model {model} is already in the model tree.')
            return
        else:
            if parent is None:
                self.root.add_child(model)
            else:
                parent.add_child(model)

        self.schedule_event(0, EventPriority.CONNECT_EVENT, model.connect)
        self.schedule_event(0, EventPriority.START_EVENT, model.start)
        if create_shared_mem:
            self.create_shared_memory(model)

    #compatibility methods
    def get_model(self, name_or_type: str | type) -> "AbstractModel" :
        """get child from root"""
        if isinstance(name_or_type, str) and self.root.name == name_or_type:
            return self.root
        elif isinstance(name_or_type, type) and isinstance(self.root, name_or_type):
            return self.root
        else:
            return self.root.get_child(name_or_type)

    def get_all_models_of_type(self, cls: type) -> list["AbstractModel"] | list:
        """get all models of type cls, raises an error is cls is not a subclass of AbstractModel"""
        return self._root.get_all_children_of_type(cls)

    def remove_model(self, child: "AbstractModel") -> None:
        """remove child from root"""
        self._root.remove_child(child)
        
    def schedule_event(self, delay:float, priority:int, action:callable, period=-1, argument=(), kwargs = _sentinel):
        """
        Wrapper for scheduler.enter that handles the integer representation of time.
        Designed to be compatible with scheduler.enter.
        """

        if kwargs is _sentinel:
            kwargs = {}

        duration_n = self.duration_to_integer(delay)
        if period != -1:
            period_n = self.duration_to_integer(period)
        else:
            period_n = -1
        self._scheduler.enter(duration_n, priority, action, period_n, argument, kwargs)
        
    
    def _get_sim_time(self) -> int:
        """Return simulation time as multiple of timestep since start"""
        #this function is mainly used by the scheduler
        return self._t_n

    def _advance_sim_time(self, dt_n: int):
        """Increment sim time with multiples of timestep"""
        self._t_n += dt_n
        self._t = (self._t_n * self._dt)
        self._skyfield_time += datetime.timedelta(seconds=(dt_n * self._dt))

    def _update(self):
        """
        execute all events at the current simulation time and advance simulation time by _dt
        """
        dt_n_to_next_event: int = self._scheduler.run_until_empty(blocking=False)
        if type(dt_n_to_next_event) is not int:
            self.logger.warning(f'dt to next event is not an integer ({dt_n_to_next_event}).')

        self._advance_sim_time(dt_n_to_next_event)
    
    def advance(self, duration: float):
        """
        Run the simulation scheduler for x seconds relative to the current simulation time.
        This is a blocking method.
        Since the scheduler operates off integer timesteps, duration will be rounded to an integer multiple of timestep.
        """
        duration_n = self.duration_to_integer(duration)
        self.advance_n(duration_n)
        
    def advance_n(self, duration_n: int):
        """
        Run the simulation scheduler for n timesteps.
        This is a blocking method.
        """
        start_seq = self.scheduler.queue[-1].sequence
        self.scheduler.run(duration_n)

        self.logger.info(f'advanced the scheduler by {duration_n*self._dt} s to simulation time {self.time} s.\n {self.scheduler.queue[-1].sequence - start_seq} events were executed during this period.')

    def step(self):
        """Advance the simulation by 1 timestep (execute all events in that time period)"""
        self._update()
            
    def kill(self):
        self.logger.warn("Killing all shared memory.")
        self._smm.shutdown()
        self._scheduler.purge()
        self._root._children.clear()
        del self._root

    def create_shared_memory(self, model):
        """Create shared memory for any models implementing SharedMemoryModelInterface"""
        if issubclass(type(model), SharedMemoryModelInterface) and not model.shared_memory_enabled:
            size = np.dtype(np.float64).itemsize * (model.saved_state_size + 1) * model.shared_array_length
            model._state_memory = self._smm.SharedMemory(size)
            model._state_array = np.ndarray((model.shared_array_length, model.saved_state_size+1), np.float64, buffer=model._state_memory.buf)
            self._smm_size += size
            model._sm_enabled = True

            self.logger.info(f'Creating shared memory for {model.name} with size: {size/1e6} MB')
            self.logger.info(f'Total shared memory size: {self._smm_size/1e6} MB')
        
class SharedMemoryModelInterface(abc.ABC):
    # default value, shared memory will start overwriting old data after writing to the size limit
    DEFAULT_SHARED_ARRAY_LENGTH = 100000

    def __init__(self, array_length: int | None = None, **kwargs):
        super().__init__(**kwargs)
        if array_length is None:
            self.shared_array_length = self.DEFAULT_SHARED_ARRAY_LENGTH
        else:
            self.shared_array_length = array_length

        self._cnt: int = 0
        self._sm_enabled: bool = False
        self._state_memory: np.ndarray = None
        self._state_array: np.ndarray = None

        self._session_file: Path = SESSION_DATA_PATH / f"{self.name}_data.npy"
        if not SESSION_DATA_PATH.exists():
            SESSION_DATA_PATH.mkdir(parents=True)
        

    @property
    @abc.abstractmethod
    def saved_state_size(self) -> int:
        """
        This property must be implemented by inheriting subclass.
        length of a single chunk for model datastorage, typically composed of float64s
        this information is relevant to shared memory
        """
        raise NotImplementedError()

    @property
    @abc.abstractmethod
    def saved_state_element_names(self) -> list[str]:
        """
        This property must be implemented by inheriting subclass.
        list of names for each element in the saved state
        """
        raise NotImplementedError()

    @property
    def shared_memory(self) -> SharedMemory:
        return self._state_memory

    @property
    def state_array(self) -> np.ndarray:
        """model state date"""
        return self._state_array[:self.arr_size,1:]

    @property
    def time_array(self) -> np.ndarray:
        """model state timeseries data"""
        return self._state_array[:self.arr_size,0]

    @property
    def arr_size(self) -> int:
        return self._cnt
    
    @property
    def shared_memory_enabled(self) -> bool:
        """is shared memory enabled"""
        return self._sm_enabled

    def reset_count(self) -> None:
        self.logger.debug(f"Resetting shared memory index to 0")
        self._cnt = 0

    def save_chunk_to_memory(self, chunk:np.ndarray):
        if self.shared_memory_enabled:
            self._state_array[self._cnt, 1:] = chunk
            self._state_array[self._cnt, 0] = self.kernel.time
            self._cnt+=1
            if self._cnt >= self.shared_array_length:
                # dump contents to .csv 
                # and reset to the start of the shared memory
                self.dump_state_array_to_file()
                self.reset_count()
    
    def dump_state_array_to_file(self) -> None:
        """Dump state array to .npy file"""
        with open(self._session_file, "ab") as f:
            np.save(f, self._state_array[:self._cnt,:])
        self.logger.debug(f"Dumped data to session file (total size {self._session_file.stat().st_size / 1e6} [MB])")
    
    def load_state_array_from_file(self, filepath: str | Path | None = None) -> np.ndarray | None:
        """
        Load state array from self._session_file if it exists and return array.
        This method will return None if session file does not exist.
        Session file can be overridden with filepath.

        Warning: the loaded array could be very large for long running simulation sessions.
        """
        if filepath is None:
            if self._session_file.exists():
                filepath = self._session_file
            else:
                return None
        elif isinstance(filepath, str) and Path(filepath).exists():
            filepath = Path(filepath)
        elif isinstance(filepath, Path) and filepath.exists():
            pass
        else:
            return None
        
        with open(filepath, "rb") as f:
            out = np.load(f)

            # load any additional arrays in file
            # concatenate them to same array
            eof_flag = False
            while not eof_flag:
                try:
                    arr = np.load(f)
                except EOFError:
                    print("reached EOF when reading shared memory save file.")
                    eof_flag = True
                    continue

                out = np.concatenate((out, arr), axis=0)
        
        return out
            
        
        
        
    