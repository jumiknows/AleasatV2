from typing import Union, List, Iterator, Any, Dict, Type

from obcpy.data_fmt import data_field
from obcpy.data_fmt import data_field_impl

from obcpy.utils import exc

class OBCCmdSysSpec:
    """Specification for a single command.

    Attributes:
        name (readonly): Name of the command.
        id (readonly): ID of the command.
        args (readonly): List of argument fields. Cannot be None. If the command accepts no arguments, pass an empty list.
        resp (readonly): Specification for the response to the command.
                         If the command does not send a response at all (not even an empty response), pass None.
                         If the command sends a response with no data, pass an empty list.
                         If the command sends a response with raw data, pass `RAW_RESPONSE`.
                         If the command sends a response with fields, pass a list of `OBCCmdSysField` instances.
    """

    def __init__(self, name: str, id: int, args: data_field.DataFieldList, resp: data_field.DataFieldList):
        self._name = name
        self._id = id
        self._args = args
        self._resp = resp

    @property
    def name(self) -> str:
        return self._name

    @property
    def id(self) -> int:
        return self._id

    @property
    def args(self) -> data_field.DataFieldList:
        return self._args

    @property
    def has_args_fields(self) -> bool:
        return (self._args and (self._args.fixed_field_count > 0))

    @property
    def resp(self) -> data_field.DataFieldList:
        return self._resp

    @property
    def has_resp_fields(self) -> bool:
        return (self._resp and (self._resp.fixed_field_count > 0))

    def encode_args(self, *args) -> bytes:
        """Serializes the provided arguments to an array of bytes according to this command specification.

        Raises:
            exc.OBCEncodeError: If the incorrect number of arguments is provided or one of the arguments
                                has an invalid type or value.

        Returns:
            The serialized arguments as a byte array.
        """
        if len(args) != len(self.args):
            raise exc.OBCEncodeError(f"Incorrect number of arguments ({len(args)}) for command: {self.name}")

        data = bytearray()
        for i in range(len(args)):
            data.extend(self.args[i].encode(args[i]))

        return data

    def decode_resp(self, resp_data: bytes) -> Union[bytes, Dict[str, Any]]:
        """Deserializes response data from a byte array.

        Args:
            resp_data: The raw bytes of the response data.

        Raises:
            exc.OBCDecodeError: If this command is specified to have no response or a raw response.
                                If the number of bytes in resp_data does not match the expected number of bytes.
                                If an error occurs decoding a response field.

        Returns:
            If this command is specified to have an empty response, an empty dictionary is returned.
            If this command is specified to have response fields, the deserialized fields are returned
            as a dictionary mapping field names to values.
        """

        if self.resp is None:
            raise exc.OBCDecodeError(f"No response specified for command: {self.name}")

        fields = {}

        if (len(resp_data) < self.resp.size) or (not self.resp.has_variable_field and (len(resp_data) != self.resp.size)):
            # If there's less data than expected -> raise an error
            # OR
            # If there's no variable fields AND there isn't exactly as much data as we expect -> raise an error
            raise exc.OBCDecodeError(f"Invalid resp length ({len(resp_data)} bytes) for command: {self.name}")

        offset = 0
        for i in range(len(self.resp)):
            field_name = self.resp[i].name
            field_size = self.resp[i].size

            if field_size > 0:
                # Fixed-size field
                field_data = resp_data[offset:(offset + field_size)]
            else:
                # Variable-size field
                field_data = resp_data[offset:]

            field_value = self.resp[i].decode(field_data)

            offset += len(field_data)

            fields[field_name] = field_value

        return fields

    def __str__(self) -> str:
        resp_str = str(self.resp) if self.resp else "NO RESPONSE"

        return f"(ID={self.id}) {self.name}: {str(self.args)} --> {resp_str}"

class OBCCmdSysSpecs:
    """A set of command system specifications for commands, their arguments, and their responses.
    """

    def __init__(self, specs: List[OBCCmdSysSpec]):
        self._specs = specs
        self._validate()

    def _validate(self):
        """Validates the specifications in this object.

        Raises:
            exc.OBCCmdSysSpecError: If a duplicate ID exists in the list of specifications.
        """
        specs_by_id: Dict[int, OBCCmdSysSpec] = {}

        for spec in self._specs:
            # Check if ID already used
            if spec.id in specs_by_id:
                raise exc.OBCCmdSysSpecError(f"Duplicate ID ({spec.id}) for {specs_by_id[spec.id].name} and {spec.name}")

            specs_by_id[spec.id] = spec

    @property
    def max_id(self) -> int:
        """Largest ID any command has in this set of specs.
        """
        return max(map(lambda spec : spec.id, self._specs))

    @property
    def count(self) -> int:
        """Number of available command system specs
        """
        return len(self._specs)

    def get(self, name: str = None, id: int = None) -> OBCCmdSysSpec:
        """Retrieves a command system spec by name or by id.

        Either a name or an ID can be provided but not both.

        Args:
            name: The name of the spec to retrieve.
            id: The ID of the spec to retrieve.

        Raises:
            exc.OBCCmdNotFoundError: If there is no spec with the given name or ID.
            RuntimeError: If both a name and ID are provided

        Returns:
            The command system spec with the target name or ID.
        """

        if (name is not None) and (id is not None):
            raise RuntimeError("Cannot pass both name and id")

        if name is not None:
            name = name.upper()
            try:
                return next(spec for spec in self._specs if spec.name.upper() == name)
            except StopIteration:
                raise exc.OBCCmdNotFoundError(f"No command with name: {name}")

        if id is not None:
            try:
                return next(spec for spec in self._specs if spec.id == id)
            except StopIteration:
                raise exc.OBCCmdNotFoundError(f"No command with ID: {id}")

    def __iter__(self) -> Iterator[OBCCmdSysSpec]:
        return self._specs.__iter__()

    def __getitem__(self, val: Union[int, slice]) -> Union[OBCCmdSysSpec, List[OBCCmdSysSpec]]:
        return self._specs[val]

    def __str__(self) -> str:
        return "\n".join(map(str, self._specs))

    @classmethod
    def from_json(cls, *json_blobs) -> "OBCCmdSysSpecs":
        """Parses a collection of JSON blobs (parsed by `json.load`) containing command system specifications.

        The original format of each JSON blob should be as follows:
        ```
        {
            "<command name>": {
                "id": "0x<ID hex>" | <ID int>,
                "args": [
                    {"<field name>": "<field type>"},
                    ...
                ],
                "resp": null | [
                    {"<field name>": "<field type>"},
                    ...
                ]
            },
            ...
        }
        ```

        Raises:
            exc.OBCCmdSysSpecError: If a duplicate ID exists in the list of specifications.
            exc.OBCDataFieldError:  If the args or resp specifications are invalid.

        Returns:
            An `OBCCmdSysSpecs` instance representing the aggregated set of command system specifications.
        """

        specs = []

        for json_data in json_blobs:
            for cmd_name in json_data:
                cmd = json_data[cmd_name]
                # Args
                arg_fields = []
                for arg in cmd["args"]:
                    for arg_name in arg:
                        arg_field_type = arg[arg_name]
                        arg_fields.append(data_field_impl.DataFieldImpl.create_data_field(arg_name, arg_field_type))
                args = data_field.DataFieldList(arg_fields)

                # Response
                if cmd["resp"] is None:
                    resp = None
                else:
                    resp_fields = []
                    for resp_field in cmd["resp"]:
                        for resp_field_name in resp_field:
                            resp_field_type = resp_field[resp_field_name]
                            resp_fields.append(data_field_impl.DataFieldImpl.create_data_field(resp_field_name, resp_field_type))
                    resp = data_field.DataFieldList(resp_fields)

                # Command ID
                cmd_id = cmd["id"]
                if isinstance(cmd_id, str):
                    cmd_id = int(cmd_id, 0)

                specs.append(OBCCmdSysSpec(cmd_name, cmd_id, args, resp))

        return OBCCmdSysSpecs(specs)
