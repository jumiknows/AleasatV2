import datetime
import functools

# From the < and == overrides, automatically generate the remaining comparison operators (<=, >, >=)
@functools.total_ordering
class ALEADateTime:
    """datetime wrapper for use with ALEASAT.

    Datetime's are represented as a traditional calendar date + clock time.
    For communication with ALEASAT they can be converted to a timestamp offset
    from ALEADateTime.EPOCH_BASE (in seconds).

    This class is immutable.

    Attributes:
        date_time (readonly): The underlying datetime.datetime instance
    """

    EPOCH_BASE = datetime.datetime(2020, 1, 1, 0, 0, 0)

    STR_FORMAT = "%Y-%m-%d@%H:%M:%S"

    def __init__(self, date_time: datetime.datetime):
        self._date_time = date_time

    @property
    def date_time(self) -> datetime.datetime:
        """The underlying datetime.datetime instance
        """
        return self._date_time

    @property
    def is_immediate(self) -> bool:
        """The timestamp value 0 (corresponding to ALEADateTime.EPOCH_BASE) is a special value
        indicating "IMMEDIATE" (this is generally used for the command system).

        This property is true if this ALEADateTime represents the "IMMEDIATE" timestamp.

        NOTE: This means the date/time ALEADateTime.EPOCH_BASE cannot actually be represented by
        ALEADateTime.
        """
        return (self.to_timestamp() == 0)

    def to_timestamp(self) -> int:
        """Converts this ALEADateTime to an epoch timestamp.

        Returns:
            The equivalent timestamp for this ALEADateTime
        """
        return int(self.date_time.timestamp() - self.EPOCH_BASE.timestamp())

    def __add__(self, other) -> "ALEADateTime":
        """Adds an integer number of seconds to this ALEADateTime.

        Args:
            other: The number of seconds to add to the timestamp represented by self. Must be an integer.

        Raises:
            ValueError: If the result of the operation is an invalid ALEADateTime.

        Returns:
            A new ALEADateTime representing the timestamp resulting from the addition.
        """
        if not isinstance(other, int):
            return NotImplemented

        return ALEADateTime.from_timestamp(self.to_timestamp() + other)

    def __radd__(self, other) -> "ALEADateTime":
        """Enables support for int + ALEADateTime (support for ALEADateTime + int is supplied by __add__)
        """
        return self + other

    def __sub__(self, other) -> "ALEADateTime":
        """Subtracts an integer number of seconds from this ALEADateTime.

        Args:
            other: The number of seconds to subtract from the timestamp represented by self. Must be an integer.

        Raises:
            ValueError: If the result of the operation is an invalid ALEADateTime.

        Returns:
            A new ALEADateTime representing the timestamp resulting from the subtraction.
        """
        if not isinstance(other, int):
            return NotImplemented

        return self + (-other)

    def __eq__(self, other) -> bool:
        """Equality comparison based on timestamps
        """
        if not isinstance(other, ALEADateTime):
            return NotImplemented

        return self.to_timestamp() == other.to_timestamp()

    def __lt__(self, other) -> bool:
        """Ordering comparison based on timestamps
        """
        if not isinstance(other, ALEADateTime):
            return NotImplemented

        return self.to_timestamp() < other.to_timestamp()

    def __str__(self) -> str:
        """Converts this ALEADateTime to a string representation in the format:
        yyyy-mm-dd hh:mm:ss

        If this ALEADateTime represents the IMMEDIATE timestamp, then "IMMEDIATE"
        is returned instead.
        """
        if self.is_immediate:
            return "IMMEDIATE"
        else:
            return self.date_time.strftime(self.STR_FORMAT)

    @classmethod
    def from_timestamp(cls, timestamp: int) -> "ALEADateTime":
        """Constructs an ALEADateTime from a timestamp.

        Args:
            timestamp: Offset from ALEADateTime.EPOCH_BASE in seconds.

        Raises:
            ValueError: If timestamp < 0.

        Returns:
            An ALEADateTime instance representing the provided timestamp.
        """
        if timestamp < 0:
            raise ValueError(f"Invalid timestamp: {timestamp}")

        return cls(datetime.datetime.fromtimestamp(cls.EPOCH_BASE.timestamp() + timestamp))

    @classmethod
    def from_string(cls, datetime_str: str) -> "ALEADateTime":
        """Constructs an ALEADateTime from a string in the format %Y-%m-%d@%H:%M:%S.

        Args:
            datetime_str: A date/time in string format.

        Raises:
            ValueError: If datetime_str is not a valid date/time.

        Returns:
            An ALEADateTime instance representing the provided date/time string.
        """
        return cls(datetime.datetime.strptime(datetime_str, ALEADateTime.STR_FORMAT))

IMMEDIATE = ALEADateTime.from_timestamp(0)
