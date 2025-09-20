from typing import Iterator, Any
from enum import IntEnum

from alea.common.data_field import data_field
from alea.common.data_field import data_field_impl

from alea.obcfw.util import spec_utils
import alea.obcfw.log.data

class OBCLogSpecError(Exception):
    pass

class OBCLogSignalSpecError(OBCLogSpecError):
    def __init__(self, msg: str = None, spec: "OBCLogSignalSpec" = None):
        self._spec = spec
        if spec is None:
            super().__init__(msg)
        else:
            super().__init__(f"{self.__class__.__name__} on log signal: {spec.name} ({spec.id}). {msg}")

    @property
    def spec(self) -> "OBCLogSignalSpec":
        return self._spec

class OBCLogSignalDataFieldError(OBCLogSignalSpecError):
    def __init__(self, msg: str = None, data_field_error: data_field.DataFieldError = None, spec: "OBCLogSignalSpec" = None):
        self._data_field_error = data_field_error
        if data_field_error is None:
            super().__init__(msg, spec)
        else:
            super().__init__(str(data_field_error), spec)

    @property
    def data_field_error(self) -> data_field.DataFieldError:
        return self._data_field_error

class OBCLogSignalEncodeError(OBCLogSignalDataFieldError):
    pass

class OBCLogSignalDecodeError(OBCLogSignalDataFieldError):
    pass

class OBCLogGroupSpecError(OBCLogSpecError):
    pass

class OBCLogSpecNotFoundError(OBCLogSpecError):
    pass

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
            OBCLogSignalEncodeError: If an incorrect number of data fields is provided or one of the data fields
                                     has an invalid type or value.

        Returns:
            The serialized data fields as a byte array.
        """
        if len(data) != len(self.data):
            raise OBCLogSignalEncodeError(f"Incorrect number of data fields ({len(data)})", spec=self)

        raw_bytes = bytearray()
        for i in range(len(data)):
            try:
                encoded = self.data[i].encode(data[i])
            except data_field.DataFieldEncodeError as e:
                raise OBCLogSignalDataFieldError(data_field_error=e, spec=self)

            raw_bytes.extend(encoded)

        return raw_bytes

    def decode_data(self, data_bytes: bytes) -> bytes | dict[str, Any]:
        """Deserializes data from a byte array.

        Args:
            data_bytes: The raw bytes of the signal data.

        Raises:
            OBCLogSignalDecodeError: If this signal is specified to have no response or a raw response.
                                     If the number of bytes in data_bytes does not match the expected number of bytes.
                                     If an error occurs decoding a data field.

        Returns:
            If this signal is specified to have no data fields, an empty dictionary is returned.
            If this signal is specified to have data fields, the deserialized fields are returned
            as a dictionary mapping field names to values.
        """

        if self.data is None:
            raise OBCLogSignalDecodeError(f"No data fields specified for this signal", spec=self)

        fields = {}

        if (len(data_bytes) < self.data.size) or (not self.data.has_variable_field and (len(data_bytes) != self.data.size)):
            # If there's less data than expected -> raise an error
            # OR
            # If there's no variable fields AND there isn't exactly as much data as we expect -> raise an error
            raise OBCLogSignalDecodeError(f"Invalid data length ({len(data_bytes)} bytes)", spec=self)

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

            try:
                field_value = self.data[i].decode(field_data)
            except data_field.DataFieldDecodeError as e:
                raise OBCLogSignalDecodeError(data_field_error=e, spec=self)

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

    def __init__(self, name: str, id: int, desc: str, signals: list[OBCLogSignalSpec]):
        self._name = name
        self._id = id
        self._desc = desc
        self._signals = signals

        self._validate()

    def _validate(self):
        signals_by_id: dict[id, OBCLogSignalSpec] = {}

        for signal in self._signals:
            # Check if ID already used
            if signal.id in signals_by_id:
                raise OBCLogGroupSpecError(f"Duplicate signal ID ({signal.id}) for {signals_by_id[signal.id].name} and {signal.name}")

            signals_by_id[signal.id] = signal

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
    def signals(self) -> list[OBCLogSignalSpec]:
        return self._signals

    def __str__(self) -> str:
        return f"(ID={self.id}) {self.name}: {self.desc}. Signals=[{self.signals}]"

class OBCLogGroupSpecs:
    """A set of log system specifications for log IDs and their signals.
    """

    def __init__(self, log_specs: list[OBCLogGroupSpec]):
        self._log_specs = log_specs
        self._validate()

    def _validate(self):
        """Validates the specifications in this object.

        Raises:
            OBCLogGroupSpecError: If a duplicate ID exists in the list of specifications.
        """
        specs_by_id: dict[int, OBCLogGroupSpec] = {}

        for spec in self._log_specs:
            # Check if ID already used
            if spec.id in specs_by_id:
                raise OBCLogGroupSpecError(f"Duplicate ID ({spec.id}) for {specs_by_id[spec.id].name} and {spec.name}")

            specs_by_id[spec.id] = spec

    @property
    def max_id(self) -> int:
        """Largest ID any log group has in this set of specs.
        """
        return max(map(lambda spec : spec.id, self._log_specs))

    @property
    def count(self) -> int:
        """Number of available log group specs
        """
        return len(self._log_specs)

    def get(self, group_name: str = None, group_id: int = None, signal_name: str = None, signal_id: int = None) -> tuple[OBCLogGroupSpec, OBCLogSignalSpec]:
        """Retrieves a log group and signal spec by name or by id.

        For both the group and signal specs, either a name or an ID can be provided but not both.

        Args:
            name: The name of the spec to retrieve.
            id: The ID of the spec to retrieve.

        Raises:
            OBCLogSpecNotFoundError: If there is no spec with the given name or ID.
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
                raise OBCLogSpecNotFoundError(f"No log group with group name: {group_name}")

        if group_id is not None:
            try:
                group_spec = next(spec for spec in self._log_specs if spec.id == group_id)
            except StopIteration:
                raise OBCLogSpecNotFoundError(f"No log group with ID: {group_id}")

        # Next, find our signal spec
        signal_spec: OBCLogSignalSpec = None

        if signal_name is not None:
            signal_name = signal_name.upper()
            try:
                signal_spec = next(spec for spec in group_spec.signals if spec.name.upper() == signal_name)
            except StopIteration:
                raise OBCLogSpecNotFoundError(f"No log signal with group name: {signal_name}")

        if signal_id is not None:
            try:
                signal_spec = next(spec for spec in group_spec.signals if spec.id == signal_id)
            except StopIteration:
                raise OBCLogSpecNotFoundError(f"No log signal with ID: {signal_id}")
        
        return group_spec, signal_spec

    def __iter__(self) -> Iterator[OBCLogGroupSpec]:
        return self._log_specs.__iter__()

    def __getitem__(self, val: int | slice) -> OBCLogGroupSpec | list[OBCLogGroupSpec]:
        return self._log_specs[val]

    def __str__(self) -> str:
        return "\n".join(map(str, self._log_specs))

    @classmethod
    def load(cls, dicts: list[dict] = None) -> "OBCLogGroupSpecs":
        """Creates an OBCLogGroupSpecs object loaded with data from either the JSON spec files
        included in this package (if dicts is None) or the dictionaries passed to this function.

        The original structure of each dictionary should be as follows (represented as JSON):
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
            OBCLogGroupSpecError: If a duplicate log group ID exists in the list of specifications or
                                  a duplicate signal ID exists within a single log group.
            OBCLogSignalSpecError: If a log signal spec is invalid.

        Returns:
            An `OBCLogGroupSpecs` instance representing the aggregated set of log group specifications.
        """
        if dicts is None:
            dicts = spec_utils.load_json_resources(alea.obcfw.log.data, "log_specs.json")

        specs = []

        for dict_data in dicts:
            for log_name in dict_data:
                log = dict_data[log_name]

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
                            raise OBCLogSignalSpecError(f"No level field found in signal \"{signal}\"")

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
