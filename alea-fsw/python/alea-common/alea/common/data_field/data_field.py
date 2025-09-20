from typing import Any, Iterator
from abc import ABC, abstractmethod
import functools

class DataFieldError(Exception):
    def __init__(self, msg: str = None, field: "DataField" = None):
        self._field = field
        if field is None:
            super().__init__(msg)
        else:
            super().__init__(f"{self.__class__.__name__} on field: {field}. {msg}")

    @property
    def field(self) -> "DataField":
        return self._field

class DataFieldEncodeError(DataFieldError):
    pass

class DataFieldDecodeError(DataFieldError):
    pass

class CTypeInfo:
    """Description of type information for C code.

    Attributes:
        type_name (readonly): Name of the C type (e.g. "uint32_t").
        type_enum (readonly): Name of the `data_fmt_field_type_t` enum value for this type (see data_fmt.h)
    """

    def __init__(self, type_name: str, type_enum: str):
        self._type_name = type_name
        self._type_enum = type_enum

    @property
    def type_name(self) -> str:
        """Name of the C type (e.g. "uint32_t").
        """
        return self._type_name

    @property
    def type_enum(self) -> str:
        """Name of the `data_fmt_field_type_t` enum value for this type (see data_fmt.h)
        """
        return self._type_enum

class DataField(ABC):
    """Description of the data type for a field.

    Raises:
        DataFieldError: If array_len == 0.
                        If array_len > 0 and this is a variable size field.

    Attributes:
        name (readonly): The name of this instance of the field.
        array_len (readonly): Length of the array if this is an array field, otherwise 1.
        is_array (readonly): True if this type represents an array, otherwise False.
        c_type (readonly): Information for generating C code relating to this data type.
    """

    _c_type: CTypeInfo = None

    def __init__(self, name: str, array_shape: tuple | int = 1):
        self._name = name

        if isinstance(array_shape, int):
            # Convert to tuple
            self._array_shape = (array_shape,)
        else:
            self._array_shape = array_shape

        if self.array_len <= 0:
            raise DataFieldError("Cannot have array_len == 0", self)

        if self.is_array and (self.size == 0):
            raise DataFieldError("Variable size field cannot be an array", self)

    @property
    def name(self) -> str:
        """The name of this instance of the field.
        """
        return self._name

    @property
    def array_shape(self) -> tuple:
        return self._array_shape

    @property
    def array_len(self) -> int:
        """Length of the array if this is an array field, otherwise 1.
        """
        return functools.reduce(lambda x, y: x * y, self.array_shape)

    @property
    def is_array(self) -> bool:
        """True if this type represents an array, otherwise False.
        """
        return (self.array_len > 1)

    @property
    def c_type(self) -> CTypeInfo:
        """Information for generating C code relating to this data type.
        """
        return self._c_type

    @property
    @abstractmethod
    def size(self) -> int:
        """Number of bytes consumed by the field when serialized.
        Variable length fields should return 0.
        """
        pass

    @abstractmethod
    def encode(self, value: Any) -> bytes:
        """Serializes value to an array of bytes

        Args:
            value: The value to serialize. Implementations may accept various types.

        Raises:
            DataFieldEncodeError if an error occurs during encoding

        Returns:
            The serialized representation of value as bytes
        """
        pass

    @abstractmethod
    def decode(self, data: bytes) -> Any:
        """Deserializes a byte array to a value corresponding to this field type

        Args:
            data: The byte array to deserialize.

        Raises:
            DataFieldDecodeError if an error occurs during decoding

        Returns:
            The deserialized value.
        """
        pass

    def __str__(self) -> str:
        array_str = ""
        if self.is_array:
            array_str = "".join([f"[{dim}]" for dim in self._array_shape])
        return f"{self.name}: {self.__class__.__name__.lower()}{array_str}"

class DataFieldList:
    """Represents an ordered collection of DataFields (essentially describing a complete message).
    """

    def __init__(self, data_fields: list[DataField]):
        """Initializes a new DataFieldList.

        Args:
            data_fields: A list of DataFields

        Raises:
            DataFieldError: If there is more than one field with a variable size.
                            If there is a variable size field that is not the last field.
        """

        self._data_fields = data_fields
        self._validate()

    def _validate(self):
        """Checks that the list of DataFields is valid.

        Raises:
            DataFieldError: If there is more than one field with a variable size.
                            If there is a variable size field that is not the last field.
        """

        var_field_count = list(map(lambda field : field.size, self._data_fields)).count(0)
        if var_field_count > 0:
            if var_field_count > 1:
                raise DataFieldError("More than one data field with variable size in DataFieldList")
            elif self._data_fields[-1].size > 0:
                raise DataFieldError("Variable size data field is not the last field in DataFieldList")

    @property
    def has_variable_field(self) -> bool:
        """True if any DataField in this list has a variable size.
        """
        return any(map(lambda field : (field.size == 0), self._data_fields))

    @property
    def fixed_field_count(self) -> int:
        """Number of fields with fixed size.
        """
        return (len(self) - list(map(lambda field : field.size, self._data_fields)).count(0))

    @property
    def size(self) -> int:
        """The sum of the sizes of the fixed-size fields in the DataFieldList.
        """
        return sum(map(lambda data_field : data_field.size, self._data_fields))

    def __iter__(self) -> Iterator[DataField]:
        return self._data_fields.__iter__()

    def __getitem__(self, val: int | slice) -> DataField | list[DataField]:
        return self._data_fields[val]

    def __len__(self) -> int:
        return len(self._data_fields)

    def __bool__(self) -> bool:
        # Prevent __len__() returning 0 from making this instance Falsy
        return True

    def __str__(self) -> str:
        return f"[{', '.join(map(str, self._data_fields))}]"
