from typing import Iterator

from alea.obcfw.util import spec_utils
import alea.obcfw.task.data

class OBCTaskSpecError(Exception):
    pass

class OBCTaskSpecNotFoundError(OBCTaskSpecError):
    pass

class OBCTaskSpec:
    def __init__(self, name: str, id: int, stack_size: int, priority: int):
        self._name = name
        self._id = id
        self._stack_size = stack_size
        self._priority = priority

    @property
    def name(self) -> str:
        return self._name

    @property
    def id(self) -> int:
        return self._id

    @property
    def stack_size(self) -> int:
        return self._stack_size

    @property
    def priority(self) -> int:
        return self._priority

    def __str__(self) -> str:
        return f"(ID={self.id}) {self.name}: stack={self.stack_size}, priority={self.priority}"

class OBCTaskSpecs:
    """A set of task specifications.
    """
    MAX_ALLOWED_ID    = 31 # The task IDs are used as bit indices in a 32-bit number
    MAX_TASK_NAME_LEN = 22 # This should match the value in FreeRTOSConfig.h

    def __init__(self, specs: list[OBCTaskSpec]):
        self._specs = specs
        self._validate()

    def _validate(self):
        """Validates the specifications in this object.

        Raises:
            OBCTaskSpecError: If a duplicate ID exists in the list of specifications.
        """
        specs_by_id: dict[int, OBCTaskSpec] = {}

        for spec in self._specs:
            # Check if ID already used
            if spec.id in specs_by_id:
                raise OBCTaskSpecError(f"Duplicate ID ({spec.id}) for {specs_by_id[spec.id].name} and {spec.name}")

            specs_by_id[spec.id] = spec

            # Check task name. Comparison is >= because one character must be reserved for the NULL terminator.
            if len(spec.name) >= self.MAX_TASK_NAME_LEN:
                raise OBCTaskSpecError(f"Task name too long: \"{spec.name}\". Must be < {self.MAX_TASK_NAME_LEN} characters")

        max_id = self.max_id
        if max_id > self.MAX_ALLOWED_ID:
            raise OBCTaskSpecError(f"Maximum allowed task ID exceeded by {specs_by_id[max_id].name} task with ID: {max_id}")

    @property
    def max_id(self) -> int:
        """Largest ID any task has in this set of specs.
        """
        return max(map(lambda spec : spec.id, self._specs))

    @property
    def count(self) -> int:
        """Number of available task specs
        """
        return len(self._specs)

    def get(self, name: str = None, id: int = None) -> OBCTaskSpec:
        """Retrieves a task spec by name or by id.

        Either a name or an ID can be provided but not both.

        Args:
            name: The name of the spec to retrieve.
            id: The ID of the spec to retrieve.

        Raises:
            OBCTaskSpecNotFoundError: If there is no spec with the given name or ID.
            RuntimeError: If both a name and ID are provided

        Returns:
            The task spec with the target name or ID.
        """

        if (name is not None) and (id is not None):
            raise RuntimeError("Cannot pass both name and id")

        if name is not None:
            name = name.upper()
            try:
                return next(spec for spec in self._specs if spec.name.upper() == name)
            except StopIteration:
                raise OBCTaskSpecNotFoundError(f"No task with name: {name}")

        if id is not None:
            try:
                return next(spec for spec in self._specs if spec.id == id)
            except StopIteration:
                raise OBCTaskSpecNotFoundError(f"No task with ID: {id}")

    def __iter__(self) -> Iterator[OBCTaskSpec]:
        return self._specs.__iter__()

    def __getitem__(self, val: int | slice) -> OBCTaskSpec | list[OBCTaskSpec]:
        return self._specs[val]

    def __str__(self) -> str:
        return "\n".join(map(str, self._specs))

    @classmethod
    def load(cls, dicts: list[dict] = None) -> "OBCTaskSpecs":
        """Creates an OBCTaskSpecs object loaded with data from either the JSON spec files
        included in this package (if dicts is None) or the dictionaries passed to this function.

        The structure of each dictionary should be as follows (represented as JSON):
        ```
        {
            "<task name>": {
                "id": "0x<ID hex>" | <ID int>,
                "stack_size": "0x<ID hex>" | <ID int>,
                "priority": "0x<ID hex>" | <ID int>,
            },
            ...
        }
        ```

        Raises:
            OBCTaskSpecError: If a duplicate ID exists in the list of specifications.

        Returns:
            An `OBCTaskSpecs` instance representing the aggregated set of task specifications.
        """
        if dicts is None:
            dicts = spec_utils.load_json_resources(alea.obcfw.task.data, "obc_tasks.json")

        specs = []

        for dict_data in dicts:
            for task_name in dict_data:
                task = dict_data[task_name]

                # ID
                task_id = task["id"]
                if isinstance(task_id, str):
                    task_id = int(task_id, 0)

                # Stack Size
                stack_size = task["stack_size"]
                if isinstance(stack_size, str):
                    stack_size = int(stack_size, 0)

                # Priority
                priority = task["priority"]
                if isinstance(priority, str):
                    priority = int(priority, 0)

                specs.append(OBCTaskSpec(task_name, task_id, stack_size, priority))

        return OBCTaskSpecs(specs)
