from typing import Any, Type
from collections import abc
from enum import Enum
import struct
import re

# Optionally import numpy for advanced array encoding/decoding (including multi-dimensional support)
try:
    import numpy as np
except ImportError:
    np = None

from obcpy.utils import exc
from obcpy.utils import obc_time

from . import data_field

# Base Classes

class PyStructDataField(data_field.DataField):
    """Implementation of `data_field.DataField` that uses the struct module to encode/decode data.
    """

    _byte_order = "!"

    _fmt_char: str = None

    @property
    def format(self) -> str:
        return f"{self._byte_order}{self.array_len}{self._fmt_char}"

    @property
    def size(self) -> int:
        return struct.calcsize(self.format)

    def pre_encode(self, value: Any) -> Any:
        """Prepares a single value for encoding.

        Args:
            value: The original value.

        Returns:
            The value prepared for encoding.
        """
        return value

    def encode(self, value: Any) -> bytes:
        if self.is_array:
            if not isinstance(value, abc.Iterable):
                raise exc.OBCEncodeError(f"{str(value)} is not an Iterable even though the {self.name} field is an array")
            if len(value) != self.array_len:
                raise exc.OBCEncodeError(f"{str(value)} has length {len(value)} while the {self.name} field expects {self.array_len} elements")

            # Make value mutable
            value = list(value)

            # Pre-encode each element
            for i in range(self.array_len):
                value[i] = self.pre_encode(value[i])

            try:
                return struct.pack(self.format, *value)
            except struct.error as e:
                raise exc.OBCEncodeError(f"Error packing {str(value)} for field {self.name}: {str(e)}")
        else:
            value = self.pre_encode(value)
            try:
                return struct.pack(self.format, value)
            except struct.error as e:
                raise exc.OBCEncodeError(f"Error packing {str(value)} for field {self.name}: {str(e)}")

    def decode(self, data: bytes) -> Any:
        try:
            values = struct.unpack(self.format, data)[:self.array_len]
        except struct.error as e:
            raise exc.OBCDecodeError(f"Error unpacking {str(data)} for field {self.name}: {str(e)}")

        values = list(values) # Make values mutable
        if self.is_array:
            # Post-decode each element
            for i in range(self.array_len):
                values[i] = self.post_decode(values[i])
            return values
        else:
            return self.post_decode(values[0])

    def post_decode(self, value: Any) -> Any:
        """Post-processes a single value after decoding.

        Args:
            value: The original decoded value.

        Returns:
            The post-processed value.
        """
        return value

class NumericalDataField(PyStructDataField):
    """Partial implementation of `data_field.DataField` for numerical data types that
    can encode and decode numpy arrays (including multi-dimensional arrays)
    """

    def encode(self, value: Any) -> bytes:
        # numpy support
        if np is not None:
            if isinstance(value, np.ndarray):
                # View array as flattened for encoding
                value = value.flat

        return super().encode(value)

    def decode(self, data: bytes) -> Any:
        decoded = super().decode(data)

        # numpy support
        if np is not None:
            if self.is_array:
                return np.array(decoded).reshape(self.array_shape)

        return decoded

class IntegerDataField(NumericalDataField):
    """Partial implementation of `data_field.DataField` for integer data types that supports
    converting strings to integers before encoding.
    """

    def pre_encode(self, value: Any) -> int:
        # Handle strings
        if isinstance(value, str):
            try:
                value = int(value, 0)
            except ValueError:
                raise exc.OBCEncodeError(f"Failed to encode string (\"{value}\") as integer for field: {self.name}")

        accepted_types = int
        # Allow numpy integer types
        if np is not None:
            accepted_types = (int, np.integer)

        if not isinstance(value, accepted_types):
            raise exc.OBCEncodeError(f"{str(value)} has invalid data type ({type(value)}) for field: {self.name}")

        return value

class FloatDataField(NumericalDataField):
    """Partial implementation of `data_field.DataField` for floating point data types that supports
    converting strings to integers before encoding.
    """

    def pre_encode(self, value: Any) -> float:
        # Handle strings
        if isinstance(value, str):
            try:
                value = float(value)
            except ValueError:
                raise exc.OBCEncodeError(f"Failed to encode string (\"{value}\") as float for field: {self.name}")
        # Handle ints
        elif isinstance(value, int):
            value = float(value)
        elif np is not None:
            if isinstance(value, np.integer):
                value = float(value)

        accepted_types = float
        # Allow numpy integer types
        if np is not None:
            accepted_types = (float, np.floating)

        if not isinstance(value, accepted_types):
            raise exc.OBCEncodeError(f"{str(value)} has invalid data type ({type(value)}) for field: {self.name}")

        return value

# Unsigned Integers

class U8(IntegerDataField):
    _c_type   = data_field.CTypeInfo("uint8_t", "DATA_FMT_FIELD_TYPE_U8")
    _fmt_char = "B"

class U16(IntegerDataField):
    _c_type   = data_field.CTypeInfo("uint16_t", "DATA_FMT_FIELD_TYPE_U16")
    _fmt_char = "H"

class U32(IntegerDataField):
    _c_type   = data_field.CTypeInfo("uint32_t", "DATA_FMT_FIELD_TYPE_U32")
    _fmt_char = "I"

class U64(IntegerDataField):
    _c_type   = data_field.CTypeInfo("uint64_t", "DATA_FMT_FIELD_TYPE_U64")
    _fmt_char = "Q"

# Signed Integers
#   - can use the same DATA_FMT_FIELD_TYPE values since the binary is the same,
#     only the interpretation changes

class S8(IntegerDataField):
    _c_type   = data_field.CTypeInfo("int8_t", "DATA_FMT_FIELD_TYPE_U8")
    _fmt_char = "b"

class S16(IntegerDataField):
    _c_type   = data_field.CTypeInfo("int16_t", "DATA_FMT_FIELD_TYPE_U16")
    _fmt_char = "h"

class S32(IntegerDataField):
    _c_type   = data_field.CTypeInfo("int32_t", "DATA_FMT_FIELD_TYPE_U32")
    _fmt_char = "i"

class S64(IntegerDataField):
    _c_type   = data_field.CTypeInfo("int64_t", "DATA_FMT_FIELD_TYPE_U64")
    _fmt_char = "q"

# Floating Point

class F32(FloatDataField):
    _c_type   = data_field.CTypeInfo("float32", "DATA_FMT_FIELD_TYPE_F32")
    _fmt_char = "f"

class F64(FloatDataField):
    _c_type   = data_field.CTypeInfo("float64", "DATA_FMT_FIELD_TYPE_F64")
    _fmt_char = "d"

# Other Types

class Bool(PyStructDataField):
    """Implementation of `data_field.DataField` for boolean data that accepts the strings
    "true" and "false" (case in-sensitive) for encoding.
    """

    _c_type    = data_field.CTypeInfo("bool", "DATA_FMT_FIELD_TYPE_BOOL")
    _fmt_char  = "?"

    def pre_encode(self, value: Any) -> bool:
        if isinstance(value, str):
            value = value.lower()
            if value == "true":
                value = True
            elif value == "false":
                value = False
            else:
                raise exc.OBCEncodeError(f"Invalid Bool string (\"{value}\") for field: {self.name}")

        if not isinstance(value, bool):
            raise exc.OBCEncodeError(f"{str(value)} has invalid data type ({type(value)}) for field: {self.name}")

        return value

    def post_decode(self, value: int) -> bool:
        return (value == 0x01)

class DateTime(U32):
    """Implementation of `data_field.DataField` for date/time data that accepts strings in the format
    %Y-%m-%d@%H:%M:%S for encoding.
    """

    def pre_encode(self, value: Any) -> int:
        if isinstance(value, str):
            try:
                value = obc_time.OBCDateTime.from_string(value)
            except ValueError:
                raise exc.OBCEncodeError(f"Invalid DateTime string (\"{value}\") for field: {self.name}")
        elif isinstance(value, int):
            value = obc_time.OBCDateTime.from_timestamp(value)

        if not isinstance(value, obc_time.OBCDateTime):
            raise exc.OBCEncodeError(f"{str(value)} has invalid data type ({type(value)}) for field: {self.name}")

        return value.to_timestamp()

    def post_decode(self, value: int) -> obc_time.OBCDateTime:
        return obc_time.OBCDateTime.from_timestamp(value)

# Variable Size Types

class Bytes(data_field.DataField):
    """Implementation of `data_field.DataField` for a variable-length byte array.
    """

    @property
    def size(self) -> int:
        return 0

    def encode(self, value: Any) -> bytes:
        if isinstance(value, str):
            try:
                value = value.encode("ascii")
            except UnicodeEncodeError:
                raise exc.OBCEncodeError(f"Failed to encode string (\"{value}\") as ASCII for field: {self.name}")

        if isinstance(value, bytearray):
            value = bytes(value)

        if not isinstance(value, bytes):
            raise exc.OBCEncodeError(f"{str(value)} has invalid data type ({type(value)}) for field: {self.name}")

        return value

    def decode(self, data: bytes) -> Any:
        return data

class String(Bytes):
    """Implementation of `data_field.DataField` for an ASCII string.
    """

    def decode(self, data: bytes) -> Any:
        try:
            return data.decode("ascii")
        except UnicodeDecodeError:
            raise exc.OBCDecodeError(f"Failed to decode bytes ({data}) as ASCII for field: {self.name}")

# Enum of all fields

class DataFieldImpl(Enum):
    """Enum of all available DataField implementations.
    """

    u8       = U8
    s8       = S8
    u16      = U16
    s16      = S16
    u32      = U32
    s32      = S32
    u64      = U64
    s64      = S64
    f32      = F32
    f64      = F64
    bool     = Bool
    datetime = DateTime
    bytes    = Bytes
    string   = String

    @classmethod
    def create_data_field(cls, name: str, type_str: str) -> data_field.DataField:
        """Create a `data_field.DataField` instance with the given name for the given type.

        The `type_str` must match one of the enum values of this class with optional
        square brackets to indicate an array type.

        e.g. "u32" or "u8[4]"

        Args:
            name: The name for the new DataField
            type_str: The type of the new DataField

        Raises:
            exc.OBCDataFieldError: If the type_str is invalid.

        Returns:
            A new `data_field.DataField` instance.
        """

        TYPE_PATTERN  = r"([A-Za-z0-9]+)((\[[0-9]+\])*)"
        ARRAY_PATTERN = r"\[([0-9]+)\]"

        array_shape = (1,)

        # Parse type_str
        match = re.fullmatch(TYPE_PATTERN, type_str)
        if match is None:
            raise exc.OBCDataFieldError(f"Invalid type string (\"{type_str}\") for field: {name}")
        
        base_type_str = match.group(1)

        # Check for array type
        array_spec = match.group(2)
        if array_spec:
            array_dims = re.findall(ARRAY_PATTERN, array_spec)
            array_shape = tuple(map(int, array_dims))

        try:
            data_field_cls: Type[data_field.DataField] = cls[base_type_str].value
        except KeyError:
            raise exc.OBCDataFieldError(f"Invalid type string (\"{type_str}\") for field: {name}")

        return data_field_cls(name, array_shape = array_shape)
