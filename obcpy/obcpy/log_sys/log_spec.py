from typing import Union, List, Tuple, Iterator, Any, Dict, Type

from obcpy.utils import exc

class OBCLogSignalSpec:
    """A logging system signal specification

    Attributes:
        name (readonly): Name of the signal.
        id (readonly): ID of the signal.
        description (readonly): Descriptor string of the signal
    """

    def __init__(self, name: str, id: int, desc: str):
        self._name = name
        self._id = id
        self._desc = desc

    @property
    def name(self) -> str:
        return self._name

    @property
    def id(self) -> int:
        return self._id

    @property
    def desc(self) -> str:
        return self._desc

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
        return len(self._specs)

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
        return self._specs.__iter__()

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

                    # Signal ID
                    signal_id = signal["id"]
                    if isinstance(signal_id, str):
                        signal_id = int(signal_id, 0)

                    signal_desc = signal["description"]

                    log_signals.append(OBCLogSignalSpec(signal_name, signal_id, signal_desc))

            specs.append(OBCLogGroupSpec(log_name, log_id, log_desc, log_signals))

        return OBCLogGroupSpecs(specs)
