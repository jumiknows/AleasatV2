import datetime
import functools

# From the < and == overrides, automatically generate the remaining comparison operators (<=, >, >=)
@functools.total_ordering
class OBCDateTime:
    """datetime wrapper for use with the OBC.

    Datetime's are represented as a traditional calendar date + clock time.
    For communication with the OBC they can be converted to a timestamp offset
    from OBCDateTime.EPOCH_BASE (in seconds).

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
        """The timestamp value 0 (corresponding to OBCDateTime.EPOCH_BASE) is a special value
        indicating "IMMEDIATE" (this is generally used for the command system).

        This property is true if this OBCDateTime represents the "IMMEDIATE" timestamp.

        NOTE: This means the date/time OBCDateTime.EPOCH_BASE cannot actually be represented by
        OBCDateTime.
        """
        return (self.to_timestamp() == 0)

    def to_timestamp(self) -> int:
        """Converts this OBCDateTime to an epoch timestamp.

        Returns:
            The equivalent timestamp for this OBCDateTime
        """
        return int(self.date_time.timestamp() - self.EPOCH_BASE.timestamp())

    def __add__(self, other) -> "OBCDateTime":
        """Adds an integer number of seconds to this OBCDateTime.

        Args:
            other: The number of seconds to add to the timestamp represented by self. Must be an integer.

        Raises:
            ValueError: If the result of the operation is an invalid OBCDateTime.

        Returns:
            A new OBCDateTime representing the timestamp resulting from the addition.
        """
        if not isinstance(other, int):
            return NotImplemented

        return OBCDateTime.from_timestamp(self.to_timestamp() + other)

    def __radd__(self, other) -> "OBCDateTime":
        """Enables support for int + OBCDateTime (support for OBCDateTime + int is supplied by __add__)
        """
        return self + other

    def __sub__(self, other) -> "OBCDateTime":
        """Subtracts an integer number of seconds from this OBCDateTime.

        Args:
            other: The number of seconds to subtract from the timestamp represented by self. Must be an integer.

        Raises:
            ValueError: If the result of the operation is an invalid OBCDateTime.

        Returns:
            A new OBCDateTime representing the timestamp resulting from the subtraction.
        """
        if not isinstance(other, int):
            return NotImplemented

        return self + (-other)

    def __eq__(self, other) -> bool:
        """Equality comparison based on timestamps
        """
        if not isinstance(other, OBCDateTime):
            return NotImplemented

        return self.to_timestamp() == other.to_timestamp()

    def __lt__(self, other) -> bool:
        """Ordering comparison based on timestamps
        """
        if not isinstance(other, OBCDateTime):
            return NotImplemented

        return self.to_timestamp() < other.to_timestamp()

    def __str__(self) -> str:
        """Converts this OBCDateTime to a string representation in the format:
        yyyy-mm-dd hh:mm:ss

        If this OBCDateTime represents the IMMEDIATE timestamp, then "IMMEDIATE"
        is returned instead.
        """
        if self.is_immediate:
            return "IMMEDIATE"
        else:
            return self.date_time.strftime(self.STR_FORMAT)

    @classmethod
    def from_timestamp(cls, timestamp: int) -> "OBCDateTime":
        """Constructs an OBCDateTime from a timestamp.

        Args:
            timestamp: Offset from OBCDateTime.EPOCH_BASE in seconds.

        Raises:
            ValueError: If timestamp < 0.

        Returns:
            An OBCDateTime instance representing the provided timestamp.
        """
        if timestamp < 0:
            raise ValueError(f"Invalid timestamp: {timestamp}")

        return cls(datetime.datetime.fromtimestamp(cls.EPOCH_BASE.timestamp() + timestamp))

    @classmethod
    def from_string(cls, datetime_str: str) -> "OBCDateTime":
        """Constructs an OBCDateTime from a string in the format %Y-%m-%d@%H:%M:%S.

        Args:
            datetime_str: A date/time in string format.

        Raises:
            ValueError: If datetime_str is not a valid date/time.

        Returns:
            An OBCDateTime instance representing the provided date/time string.
        """
        return cls(datetime.datetime.strptime(datetime_str, OBCDateTime.STR_FORMAT))

IMMEDIATE = OBCDateTime.from_timestamp(0)
