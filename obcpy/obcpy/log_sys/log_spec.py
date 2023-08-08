from typing import Union, List, Tuple, Iterator, Any, Dict, Type
from enum import IntEnum

from obcpy.data_fmt import data_field
from obcpy.data_fmt import data_field_impl

from obcpy.utils import exc

class OBCLogLevel(IntEnum):
    DEBUG   = 0
    INFO    = 1
    WARNING = 2
    ERROR   = 3

class OBCLogSignalSpec:
    """A logging system signal specification

    Attributes:
        name (readonly): Name of the signal.
        id (readonly): ID of the signal.
        description (readonly): Descriptor string of the signal
    """

    def __init__(self, level: OBCLogLevel, name: str, id: int, desc: str, data: data_field.DataFieldList):
        self._level = level
        self._name = name
        self._id = id
        self._desc = desc
        self._data = data

    @property
    def level(self) -> OBCLogLevel:
        return self._level

    @property
    def name(self) -> str:
        return self._name

    @property
    def id(self) -> int:
        return self._id

    @property
    def desc(self) -> str:
        return self._desc
    
    @property
    def data(self) -> data_field.DataFieldList:
        return self._data

    @property
    def has_data_fields(self) -> bool:
        return (self._data and (self._data.fixed_field_count > 0))
    
    def encode_data(self, *data) -> bytes:
        """Serializes the provided data to an array of bytes according to this signal specification.

        Raises:
            exc.OBCEncodeError: If an incorrect number of data fields is provided or one of the data fields
                                has an invalid type or value.

        Returns:
            The serialized data fields as a byte array.
        """
        if len(data) != len(self.data):
            raise exc.OBCEncodeError(f"Incorrect number of data fields ({len(data)}) for signal {self.name}")

        raw_bytes = bytearray()
        for i in range(len(data)):
            raw_bytes.extend(self.data[i].encode(data[i]))

        return raw_bytes

    def decode_data(self, data_bytes: bytes) -> Union[bytes, Dict[str, Any]]:
        """Deserializes data from a byte array.

        Args:
            data_bytes: The raw bytes of the signal data.

        Raises:
            exc.OBCDecodeError: If this signal is specified to have no response or a raw response.
                                If the number of bytes in data_bytes does not match the expected number of bytes.
                                If an error occurs decoding a data field.

        Returns:
            If this signal is specified to have no data fields, an empty dictionary is returned.
            If this signal is specified to have data fields, the deserialized fields are returned
            as a dictionary mapping field names to values.
        """

        if self.data is None:
            raise exc.OBCDecodeError(f"No data fields specified for signal: {self.name}")

        fields = {}

        if (len(data_bytes) < self.data.size) or (not self.data.has_variable_field and (len(data_bytes) != self.data.size)):
            # If there's less data than expected -> raise an error
            # OR
            # If there's no variable fields AND there isn't exactly as much data as we expect -> raise an error
            raise exc.OBCDecodeError(f"Invalid data length ({len(data_bytes)} bytes) for signal: {self.name}")

        offset = 0
        for i in range(len(self.data)):
            field_name = self.data[i].name
            field_size = self.data[i].size

            if field_size > 0:
                # Fixed-size field
                field_data = data_bytes[offset:(offset + field_size)]
            else:
                # Variable-size field
                field_data = data_bytes[offset:]

            field_value = self.data[i].decode(field_data)

            offset += len(field_data)

            fields[field_name] = field_value

        return fields

    def __str__(self) -> str:
        return f"(ID={self.id}) {self.name}: {str(self.desc)}"

class OBCLogGroupSpec:
    """Specification for a single log group.

    Attributes:
        name (readonly): Name of the log group.
        id (readonly): ID of the log group.
        desc (readonly): Brief description of the log group.
        signals (readonly): List of signals belonging to the group
    """

    def __init__(self, name: str, id: int, desc: str, signals: List[OBCLogSignalSpec]):
        self._name = name
        self._id = id
        self._desc = desc
        self._signals = signals

    @property
    def name(self) -> str:
        return self._name

    @property
    def id(self) -> int:
        return self._id

    @property
    def desc(self) -> str:
        return self._desc

    @property
    def signals(self) -> List[OBCLogSignalSpec]:
        return self._signals

    def __str__(self) -> str:
        return f"(ID={self.id}) {self.name}: {self.desc}. Signals=[{self.signals}]"

class OBCLogGroupSpecs:
    """A set of log system specifications for log IDs and their signals.
    """

    def __init__(self, log_specs: List[OBCLogGroupSpec]):
        self._log_specs = log_specs
        self._validate()

    def _validate(self):
        """Validates the specifications in this object.

        Raises:
            exc.OBCCmdSysSpecError: If a duplicate ID exists in the list of specifications.
        """
        specs_by_id: Dict[int, OBCLogGroupSpec] = {}

        for spec in self._log_specs:
            # Check if ID already used
            if spec.id in specs_by_id:
                raise exc.OBCLogSysSpecError(f"Duplicate ID ({spec.id}) for {specs_by_id[spec.id].name} and {spec.name}")

            specs_by_id[spec.id] = spec

    @property
    def max_id(self) -> int:
        """Largest ID any log group has in this set of specs.
        """
        return max(map(lambda spec : spec.id, self._specs))

    @property
    def count(self) -> int:
        """Number of available log group specs
        """
        return len(self._log_specs)

    def get(self, group_name: str = None, group_id: int = None, signal_name: str = None, signal_id: int = None) -> Tuple[OBCLogGroupSpec, OBCLogSignalSpec]:
        """Retrieves a log group and signal spec by name or by id.

        For both the group and signal specs, either a name or an ID can be provided but not both.

        Args:
            name: The name of the spec to retrieve.
            id: The ID of the spec to retrieve.

        Raises:
            exc.OBCLogNotFoundError: If there is no spec with the given name or ID.
            RuntimeError: If both a name and ID are provided

        Returns:
            The command system spec with the target name or ID.
        """

        if (group_name is not None) and (group_id is not None):
            raise RuntimeError("Cannot pass both group name and id")

        if (signal_name is not None) and (signal_id is not None):
            raise RuntimeError("Cannot pass both signal name and id")

        # First, find our group spec
        group_spec: OBCLogGroupSpec = None

        if group_name is not None:
            group_name = group_name.upper()
            try:
                group_spec = next(spec for spec in self._log_specs if spec.name.upper() == group_name)
            except StopIteration:
                raise exc.OBCLogNotFoundError(f"No log group with group name: {group_name}")

        if group_id is not None:
            try:
                group_spec = next(spec for spec in self._log_specs if spec.id == group_id)
            except StopIteration:
                raise exc.OBCLogNotFoundError(f"No log group with ID: {group_id}")

        # Next, find our signal spec
        signal_spec: OBCLogSignalSpec = None

        if signal_name is not None:
            signal_name = signal_name.upper()
            try:
                signal_spec = next(spec for spec in group_spec.signals if spec.name.upper() == signal_name)
            except StopIteration:
                raise exc.OBCLogNotFoundError(f"No log signal with group name: {signal_name}")

        if signal_id is not None:
            try:
                signal_spec = next(spec for spec in group_spec.signals if spec.id == signal_id)
            except StopIteration:
                raise exc.OBCLogNotFoundError(f"No log signal with ID: {signal_id}")
        
        return group_spec, signal_spec

    def __iter__(self) -> Iterator[OBCLogGroupSpec]:
        return self._log_specs.__iter__()

    def __getitem__(self, val: Union[int, slice]) -> Union[OBCLogGroupSpec, List[OBCLogGroupSpec]]:
        return self._log_specs[val]

    def __str__(self) -> str:
        return "\n".join(map(str, self._log_specs))

    @classmethod
    def from_json(cls, json_blob) -> "OBCLogGroupSpecs":
        """Parses a JSON blob (parsed by `json.load`) containing log group specifications.

        The original format of the JSON blob should be as follows:
        ```
        {
            "<log group name>": {
                "id": "0x<ID hex>" | <ID int>,
                "description": "<description string>",
                "signals": {
                    "<signal name>": {
                        "id": "0x<ID hex>" | <ID int>,
                        "description": "<description string>"
                    },
                    ...
                }
            },
            ...
        }
        ```

        Raises:
            exc.OBCLogSysSpecError: If a duplicate log group ID exists in the list of specifications.

        Returns:
            An `OBCLogGroupSpecs` instance representing the aggregated set of log group specifications.
        """

        specs = []

        for log_name in json_blob:
            log = json_blob[log_name]

            # Log Group ID
            log_id = log["id"]
            if isinstance(log_id, str):
                log_id = int(log_id, 0)
            
            # Log description
            log_desc = log["description"]
            
            # Signals
            log_signals = []
            if "signals" in log.keys():
                for signal_name in log["signals"]:
                    signal = log["signals"][signal_name]
                    
                    if "level" in signal:
                        level = OBCLogLevel[signal["level"]]
                    else:
                        raise exc.OBCLogSysSpecError(f"No level field found in signal \"{signal}\"")
                    
                    # Data
                    if not ("data" in signal):
                        data = None
                    else:
                        fields = []
                        for field in signal["data"]:
                            for field_name in field:
                                field_type = field[field_name]
                                fields.append(data_field_impl.DataFieldImpl.create_data_field(field_name, field_type))
                        data = data_field.DataFieldList(fields)

                    # Signal ID
                    signal_id = signal["id"]
                    if isinstance(signal_id, str):
                        signal_id = int(signal_id, 0)

                    signal_desc = signal["description"]

                    log_signals.append(OBCLogSignalSpec(level, signal_name, signal_id, signal_desc, data))

            specs.append(OBCLogGroupSpec(log_name, log_id, log_desc, log_signals))

        return OBCLogGroupSpecs(specs)
