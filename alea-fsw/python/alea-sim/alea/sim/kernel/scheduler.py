"""
Adaption of Python's built-in scheduler class from the sched module
Changes:
* Change event object from namedtuple to dataclass
* Support periodic events (rescheduling after being executed)
* modified run() to run for a certain duration
author: Yousif

Python's description - 
A generally useful event scheduler class.

Each instance of this class manages its own queue.
No multi-threading is implied; you are supposed to hack that
yourself, or use a single instance per application.

Each instance is parametrized with two functions, one that is
supposed to return the current time, one that is supposed to
implement a delay.  You can implement real-time scheduling by
substituting time and sleep from built-in module time, or you can
implement simulated time by writing your own functions.  This can
also be used to integrate scheduling with STDWIN events; the delay
function is allowed to modify the queue.  Time can be expressed as
integers or floating point numbers, as long as it is consistent.

Events are specified by tuples (time, priority, action, argument, kwargs).
As in UNIX, lower priority numbers mean higher priority; in this
way the queue can be maintained as a priority queue.  Execution of the
event means calling the action function, passing it the argument
sequence in "argument" (remember that in Python, multiple function
arguments are be packed in a sequence) and keyword parameters in "kwargs".
The action function may be an instance method so it
has another way to reference private data (besides global variables).
"""

import time
import heapq
from itertools import count
import threading
from time import monotonic as _time
from dataclasses import dataclass, field
from typing import Callable
from enum import Enum, IntEnum
from time import monotonic

__all__ = ["Scheduler"]
_sentinel = object()
_seqeunce_generator = count()

class EventPriority(IntEnum):
    """
    Event priorities for different types of simulation events.
    A lower number represents a higher priority.
    """
    CONNECT_EVENT = 0
    START_EVENT = 1
    KERNEL_EVENT = 2

    ORBIT_DYNAMICS_EVENT = 10
    ATTITUDE_DYNAMICS_EVENT = 12
    EARTH_MAGNETIC_FIELD_EVENT = 13

    #coordinate transformations should be updated to current time instance BEFORE other models use them
    #since coordinate updates needs current dynamics info, dynamics events are updated first
    COORDINATE_EVENT = 20

    HARDWARE_EVENT = 30
    
    #power ticks update after hardware status changes
    POWER_EVENT = 40

    #update values in eps model after all the other models updates (sensor, actuators, solar panels, etc)
    POWER_EVENT_SOLAR = 41
    POWER_EVENT_EPS = 42

@dataclass(frozen=True)
class Event:
    """
    Scheduler event with the following properties:
    
    time -
        Numeric type compatible with the return value of the
        timefunc function passed to the constructor.
    priority -
        Events scheduled for the same time will be executed
        in the order of their priority.
    period - 
        Numeric time offset for next periodic event, setting this to -1 will result in a single time event.
    action - 
        Executing the event means executing action(*argument, **kwargs)
    argument - 
        argument is a sequence holding the positional
        arguments for the action.
    kwargs - 
        kwargs is a dictionary holding the keyword
        arguments for the action.
    sequence -
        A continually increasing sequence number that
        separates events if time and priority are equal.
    """

    time: float | int
    priority: int
    period: float | int
    action: Callable
    argument: tuple
    kwargs: dict
    sequence: int = field(default_factory=lambda: next(_seqeunce_generator))
    
    #for heapqueue comparisons
    def __lt__(self, other: "Event"):
        """self < other"""
        if self.time != other.time:
            return self.time < other.time
        elif self.priority != other.priority:
            return self.priority < other.priority
        else:
            return self.sequence < other.sequence

    #for heapqueue comparisons
    def __le__(self, other: "Event"):
        """self <= other"""
        if self.time != other.time:
            return self.time <= other.time
        elif self.priority != other.priority:
            return self.priority <= other.priority
        else:
            return self.sequence <= other.sequence

    def __iter__(self):
        return iter((self.time, self.priority, self.is_periodic, self.period, self.action, self.argument, self.kwargs, self.sequence))
    
    def __call__(self):
        self.action(*self.argument, **self.kwargs) 

class Scheduler:

    def __init__(self, timefunc=_time, delayfunc=time.sleep):
        """Initialize a new instance, passing the time and delay
        functions"""
        self._queue = []
        self._lock = threading.RLock()
        self.timefunc = timefunc
        self.delayfunc = delayfunc
        self._debug_realtime = False

    def enterabs(self, time, priority, action, period=-1, argument=(), kwargs=_sentinel):
        """Enter a new event in the queue at an absolute time.

        Returns an ID for the event which can be used to remove it,
        if necessary.

        """
        if kwargs is _sentinel:
            kwargs = {}

        with self._lock:
            event = Event(time, priority, period, action, argument, kwargs)
            heapq.heappush(self._queue, event)
        return event # The ID

    def enter(self, delay, priority, action, period=-1, argument=(), kwargs=_sentinel):
        """A variant that specifies the time as a relative time.

        This is actually the more commonly used interface.

        """
        time = self.timefunc() + delay
        return self.enterabs(time, priority, action, period, argument, kwargs)

    def cancel(self, event):
        """Remove an event from the queue.

        This must be presented the ID as returned by enter().
        If the event is not in the queue, this raises ValueError.

        """
        with self._lock:
            self._queue.remove(event)
            heapq.heapify(self._queue)

    def empty(self):
        """Check whether the queue is empty."""
        with self._lock:
            return not self._queue
        
    def purge(self):
        """Clear all events from queue"""
        with self._lock:
            self._queue.clear()
        
    def run(self, duration: int | float) -> int | float:
        """Execute events while now < (start_time + duration)
        Returns duration till next task
        """
        # localize variable access to minimize overhead
        lock = self._lock
        q = self._queue
        delayfunc = self.delayfunc
        timefunc = self.timefunc
        pop = heapq.heappop
        end_time = timefunc() + duration

        # if self._debug_realtime:
        #     self._debug_time_arr = []
        #     self._debug_time_map = {}

        while True:
            if self._debug_realtime:
                start_rt = monotonic()
            now = timefunc()
            with lock:
                if not q:
                    break
                event: Event = q[0]

                if event.time > now:
                    delay = True
                elif event.time < now:
                    raise Exception(f'A past event {event} is being scheduled at a future time {now}!')
                else:
                    delay = False
                    pop(q)

            if delay:
                #if the next event is beyond the duration, return
                if event.time > end_time:
                    return event.time - now

                delayfunc(event.time - now)
                    
                pop(q)

            #execute event
            event()
            
            #scchedule the next event if it is periodic
            if event.period > 0:
                self.enter(event.period, event.priority, event.action, event.period, event.argument, event.kwargs)
                
            # # Let other threads run
            # delayfunc(type(now)(0))

            # if self._debug_realtime:
            #     dur = monotonic() - start_rt
            #     # print(f'schudler took {dur} s to finish action block.')
            #     try:
            #         self._debug_time_arr.append(dur)
            #     except:
            #         self._debug_time_arr = [dur,]
                    
            #     try:
            #         self._debug_time_map[event.action.__qualname__].append(dur)
            #     except:
            #         self._debug_time_map[event.action.__qualname__] = [dur,]

    def run_until_empty(self, blocking=True):
        """Execute events until the queue is empty.
        If blocking is False executes the scheduled events due to
        expire soonest (if any) and then return the deadline of the
        next scheduled call in the scheduler.

        When there is a positive delay until the first event, the
        delay function is called and the event is left in the queue;
        otherwise, the event is removed from the queue and executed
        (its action function is called, passing it the argument).  If
        the delay function returns prematurely, it is simply
        restarted.

        It is legal for both the delay function and the action
        function to modify the queue or to raise an exception;
        exceptions are not caught but the scheduler's state remains
        well-defined so run() may be called again.

        A questionable hack is added to allow other threads to run:
        just after an event is executed, a delay of 0 is executed, to
        avoid monopolizing the CPU when other threads are also
        runnable.

        """
        # localize variable access to minimize overhead
        lock = self._lock
        q = self._queue
        delayfunc = self.delayfunc
        timefunc = self.timefunc
        pop = heapq.heappop
        while True:
            now = timefunc()
            with lock:
                if not q:
                    break
                event: Event = q[0]
                if event.time > now:
                    delay = True
                elif event.time < now:
                    raise Exception(f'A past event {event} is being scheduled at a future time {now}!')
                else:
                    delay = False
                    pop(q)
            if delay:
                if not blocking:
                    return event.time - now
                delayfunc(event.time - now)
                
                pop(q)

            event()
            #modification to original lib to add periodic event self scheduling
            if event.period > 0:
                self.enter(event.period, event.priority, event.action, event.period, event.argument, event.kwargs)
            delayfunc(type(timefunc())(0))   # Let other threads run

    @property
    def queue(self):
        """An ordered list of upcoming events.

        Events are named tuples with fields for:
            time, priority, action, arguments, kwargs

        """
        # Use heapq to sort the queue rather than using 'sorted(self._queue)'.
        # With heapq, two events scheduled at the same time will show in
        # the actual order they would be retrieved.
        with self._lock:
            events = self._queue[:]
        return list(map(heapq.heappop, [events]*len(events)))
