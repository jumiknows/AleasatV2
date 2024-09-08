import numpy as np
import dataclasses
from pathlib import Path

from alea.sim.kernel.kernel import SharedMemoryModelInterface
from alea.sim.kernel.scheduler import EventPriority
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.generic.powered_unit_model import PoweredUnitModel
from .solar_panel import SolarPanelModel
from scipy.interpolate import interp1d
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.configuration import Configurable

@dataclasses.dataclass
class EPSConfig:
    num_cells                       : int
    interpolate_voltage             : bool
    cell_discharge_capacity         : float
    cell_typ_discharge_current      : float
    cell_voltage_soc_lup_keys       : np.ndarray
    cell_voltage_soc_lup_values     : np.ndarray

    cell_voltage_lup_tbl            : dict[int, float] = None

    #Scipy Interpolate a 1-D function.
    cell_volts_interp1d             : interp1d = None

    def __post_init__(self):
        #create np.ndarrays after json load
        self.cell_voltage_soc_lup_keys = np.array(self.cell_voltage_soc_lup_keys)
        self.cell_voltage_soc_lup_values = np.array(self.cell_voltage_soc_lup_values)
        self.cell_voltage_lup_tbl = {}
        for i in range(len(self.cell_voltage_soc_lup_keys)):
            key = self.cell_voltage_soc_lup_keys[i]
            val = self.cell_voltage_soc_lup_values[i]
            self.cell_voltage_lup_tbl[key] = val

        #generate 1d interpolation table
        self.cell_volts_interp1d = interp1d(self.cell_voltage_soc_lup_keys, self.cell_voltage_soc_lup_values)

class EPSModel(Configurable[EPSConfig], SharedMemoryModelInterface, AbstractModel):
    """
    EPS battery model that models battery state of charge and voltage level based on battery cell characteristics.
    Connects to N x PoweredUnitModels and N x SolarPanelModels to calculate net input and output currents from the battery.
    """
    
    def __init__(self, sim_kernel: AleasimKernel, name: str = "eps", cfg: str | Path | dict | EPSConfig = "eps") -> None:
        super().__init__(name=name, sim_kernel=sim_kernel, cfg=cfg, cfg_cls=EPSConfig)
        self._elem_names = ['battery_soc_percent', 'battery_soc_Ah', 'watt_hour_estimate', 'solar_current', 'load_current', 'eps_voltage', 'power_net', 'power_in', 'power_out']
        self._elem_len = len(self._elem_names)
        self.configure()

    @property
    def saved_state_element_names(self):
        return self._elem_names

    @property
    def saved_state_size(self):
        return self._elem_len
        
    def configure(self):
        cfg = self.cfg
        
        #mAh is the true measure of charge
        #scale discharge capacity (Ah) by number of cells in series
        self._discharge_capacity: float = cfg.num_cells * cfg.cell_discharge_capacity
        
        #update period for power
        self._update_period = self.kernel.timestep
        self._power_in = 0.0
        self._power_out = 0.0
        self._dt_hr =  (self._update_period / 3600.0)
        
        #eps batteries state of charge in amp hours
        #starts at max
        self._eps_soc_ah:float = self._discharge_capacity
        #initial voltage also starts at max
        self._eps_voltage:float = self._calculate_eps_voltage(self._eps_soc_ah)

        self._energy = self.estimate_watt_hours()
    
    def _calculate_eps_voltage(self, soc_ah):
        """
        calcuate series battery cell voltage based on lookup table
        maps state of charge in Ah to voltage of lithium ion cell
        see eps.cfg for more details about the specs
        """
        if self.cfg.interpolate_voltage:
            cell_voltage =  self.cfg.cell_volts_interp1d(soc_ah/self._discharge_capacity*100.0)
        else:
            cell_voltage =  self.cfg.cell_voltage_lup_tbl[round(soc_ah/self._discharge_capacity*100.0, -1)]
            
        return self.cfg.num_cells * cell_voltage
    
    @property
    def voltage(self) -> float:
        """eps voltage at current time"""
        return self._eps_voltage

    @property
    def state_of_charge(self) -> float:
        """return state of charge as a percentage decimal"""
        return self._eps_soc_ah/self._discharge_capacity * 100.0
    
    @property
    def amp_hours(self) -> float:
        """battery amp hours remaining"""
        return self._eps_soc_ah

    def estimate_watt_hours(self) -> float:
        """
        estimate of watt hours remaining based on lowest cell voltage
        this is a conservative approximation because the voltage varies during discharge
        it is better to track the state of charge in amp hours
        """
        v = self.cfg.cell_voltage_lup_tbl[0]
        return v*self._eps_soc_ah

    def connect(self):
        self._powered_units: list[PoweredUnitModel] = self.kernel.get_all_models_of_type(PoweredUnitModel)
        if len(self._powered_units) == 0:
            self.logger.warn('eps is not connected to any loads (PoweredUnits)')

        self._solar_panels: list[SolarPanelModel] = self.kernel.get_all_models_of_type(SolarPanelModel)
        if len(self._solar_panels) == 0:
            self.logger.warn('eps is not connected to any solar panels')

        self.kernel.schedule_event(0, EventPriority.POWER_EVENT_EPS, self.update_eps_state, self._update_period)
    
    def update_eps_state(self):
        """compute net current from solar panels and all powered units and update eps state"""
        
        #calculate the eps voltage based on battery amp hours
        self._eps_voltage = self._calculate_eps_voltage(self._eps_soc_ah)
        
        # compute the input power and current from solar cells
        # assuming ideal MPTT i.e. the EPS optimizes so that current = power / cell_voltage
        self._power_in = 0.0
        for solar_panel in self._solar_panels:
            self._power_in += solar_panel.solar_power_gen
        self._solar_current = self._power_in / self._eps_voltage

        # compute the load power and load current from all the loads
        # here assumptions are made that the load voltages match the eps output so it is not checked
        # in reality the eps has a 3.3V and 5V bus. The cell voltage converted to the bus lines
        # conversion from cell voltage rails to power bus is done through buck converters (which have power loss)
        # TODO factor multiple voltage busses,  load switches, and conversion loss into the power model later on
        self._power_out = 0.0
        self._load_current = 0.0
        for unit in self._powered_units:
            self._power_out += unit.power_usage
            self._load_current += unit.current

        self._eps_soc_ah += (self._solar_current - self._load_current) * self._dt_hr
        
        # stop charging at max capacity
        if self._eps_soc_ah < 0.0:
            self.logger.warn("EPS batteries are fully discharged (0 [Ah]).")
        self._eps_soc_ah = min(max(self._eps_soc_ah, 0.0), self._discharge_capacity)
        
        # not too sure if this is valid but the energy computation is an estimate anyways
        if self._eps_soc_ah < self._discharge_capacity:
            self._energy += (self._power_in - self._power_out) * self._dt_hr

            #if energy goes below 0, raise a warning and clamp it to >= 0.0
            if self._energy < 0:
                self.logger.warn("EPS battery energy has reached 0 [Wh].")
                self._energy = max(self._energy, 0.0)

        # ['battery_soc_percent', 'battery_soc_Ah', 'watt_hour_estimate', 'solar_current', 'load_current', 'eps_voltage', 'power_net', 'power_in', 'power_out']
        self.save_chunk_to_memory(np.array([self.state_of_charge,
                                            self._eps_soc_ah,
                                            self._energy,
                                            self._solar_current,
                                            self._load_current,
                                            self._eps_voltage,
                                            (self._power_in - self._power_out), 
                                            self._power_in, 
                                            self._power_out]))

    @property
    def total_power(self) -> float:
        return self._power_in + self._power_out

    @property
    def power_in(self) -> float:
        return self._power_in

    @property
    def power_out(self) -> float:
        return self._power_out
    
    @property
    def energy(self) -> float:
        """Current level of eps energy storage in Wh"""
        return self._energy
    
    def _force_state_of_charge(self, percentage):
        """for testing mainly"""
        if percentage < 0 or percentage > 100:
            raise ValueError('Cannot force a state of charge outside of the range 0 - 100%')
        self._eps_soc_ah:float = (percentage/100.0) * self._discharge_capacity
        self._eps_voltage:float = self._calculate_eps_voltage(self._eps_soc_ah)
        self._energy = self.estimate_watt_hours()