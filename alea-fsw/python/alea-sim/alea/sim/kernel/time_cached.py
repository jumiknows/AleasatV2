from typing import TypeVar, Generic, Callable
from abc import ABC

T = TypeVar("T")

class TimeCachedModel(ABC):
    """A mixin that provides a mechanism to calculate variables on-the-fly and cache their
    results, so that not all values have to be calculated in a periodic "update" function.

    Subclasses should call self.invalidate_cache(current_time) and pass the current simulation
    time whenever the model is requested to update.

    self.invalidate_cache(current_time) should be called before assigning to any cached
    properties to ensure the new property values are associated with the latest timestamp.
    """

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self._last_update_time: int = None

    def invalidate_cache(self, current_time: int):
        """Invalidates any cached properties (methods decorated with @time_cached_property)
        IF current_time is different than self.last_update_time.

        Args:
            current_time: The current simulation time
        """
        self._last_update_time = current_time

    @property
    def last_update_time(self) -> int:
        """Last time that an update was requested for this model. Any cached properties (methods
        decorated with @time_cached_property) will be recomputed if they were previously computed
        at a time other than last_update_time"""
        return self._last_update_time

class time_cached_property(Generic[T]):
    """Descriptor class that can be used as a decorator on instance methods of TimeCachedModel subclasses.

    Transforms the decorated method into a property whose value is computed once and then cached
    until the model is next updated. Similar to @functools.cached_property(), but with the cache
    limited to a single update cycle (rather than the entire lifetime of the object).

    This is implemented by storing a timestamp alongside the actual property value in the object's
    __dict__.

    This descriptor also supports setting the property value directly through assignment (instead of
    generating the value by calling the decorated function). If the property is assigned, the internal
    timestamp will be updated to the TimeCachedModel's last_update_time.

    The property can be manually invalidated (so it will be recomputed on the next access) by deleting
    it (using the Python del keyword).
    """

    def __init__(self, func: Callable[[], T]):
        self._func = func
        self.__doc__ = func.__doc__

        self._name: str = None
        self._attr_name: str = None

    def __set_name__(self, owner, name):
        if not issubclass(owner, TimeCachedModel):
            raise TypeError(f"time_cached_property can only be used on subclasses of TimeCache")

        if self._name is None:
            self._name = name
            self._attr_name = f"_{name}"
        else:
            raise TypeError(f"Cannot assign the same time_cached_property to two different names: {self._name} and {name}")

    def __get__(self, instance: TimeCachedModel, owner=None) -> T:
        if instance is None:
            # Class attribute access --> return the descriptor object (self)
            return self

        attrs = instance.__dict__
        now = instance.last_update_time

        timestamp: int
        value: T

        invoke_func = True

        if now is not None:
            if self._attr_name in attrs:
                timestamp, value = attrs.get(self._attr_name)
                if timestamp is not None:
                    invoke_func = (timestamp != now)

        if invoke_func:
            # Current value is out of date so compute a new value
            value = self._func(instance)
            attrs[self._attr_name] = (now, value)

        return value

    def __set__(self, instance: TimeCachedModel, value: T):
        instance.__dict__[self._attr_name] = (instance.last_update_time, value)

    def __delete__(self, instance: TimeCachedModel):
        del instance.__dict__[self._attr_name]
