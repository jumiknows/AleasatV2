from typing import Any, List, Iterator, Union
from abc import ABC, abstractmethod

from obcpy.utils import exc

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
        exc.OBCDataFieldError: If array_len == 0.
                               If array_len > 0 and this is a variable size field.

    Attributes:
        name (readonly): The name of this instance of the field.
        array_len (readonly): Length of the array if this is an array field, otherwise 1.
        is_array (readonly): True if this type represents an array, otherwise False.
        c_type (readonly): Information for generating C code relating to this data type.
    """

    _c_type: CTypeInfo = None

    def __init__(self, name: str, array_len: int = 1):
        self._name      = name
        self._array_len = array_len

        if self.array_len == 0:
            raise exc.OBCDataFieldError(f"Field ({self.name}) cannot have array_len == 0")

        if self.is_array and (self.size == 0):
            raise exc.OBCDataFieldError(f"Variable size field ({self.name}) cannot be an array")

    @property
    def name(self) -> str:
        """The name of this instance of the field.
        """
        return self._name

    @property
    def array_len(self) -> int:
        """Length of the array if this is an array field, otherwise 0.
        """
        return self._array_len

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

        Returns:
            The serialized representation of value as bytes
        """
        pass

    @abstractmethod
    def decode(self, data: bytes) -> Any:
        """Deserializes a byte array to a value corresponding to this field type

        Args:
            data: The byte array to deserialize.

        Returns:
            The deserialized value.
        """
        pass

    def __str__(self) -> str:
        array_str = ""
        if self.is_array:
            array_str = f"[{self.array_len}]"
        return f"{self.name}: {self.__class__.__name__.lower()}{array_str}"

class DataFieldList:
    """Represents an ordered collection of DataFields (essentially describing a complete message).
    """

    def __init__(self, data_fields: List[DataField]):
        """Initializes a new DataFieldList.

        Args:
            data_fields: A list of DataFields

        Raises:
            exc.OBCDataFieldError: If there is more than one field with a variable size.
                                   If there is a variable size field that is not the last field.
        """

        self._data_fields = data_fields
        self._validate()

    def _validate(self):
        """Checks that the list of DataFields is valid.

        Raises:
            exc.OBCDataFieldError: If there is more than one field with a variable size.
                                   If there is a variable size field that is not the last field.
        """

        var_field_count = list(map(lambda field : field.size, self._data_fields)).count(0)
        if var_field_count > 0:
            if var_field_count > 1:
                raise exc.OBCDataFieldError("More than one data field with variable size in DataFieldList")
            elif self._data_fields[-1].size > 0:
                raise exc.OBCDataFieldError("Variable size data field is not the last field in DataFieldList")

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

    def __getitem__(self, val: Union[int, slice]) -> Union[DataField, List[DataField]]:
        return self._data_fields[val]

    def __len__(self) -> int:
        return len(self._data_fields)

    def __bool__(self) -> bool:
        # Prevent __len__() returning 0 from making this instance Falsy
        return True

    def __str__(self) -> str:
        return f"[{', '.join(map(str, self._data_fields))}]"
