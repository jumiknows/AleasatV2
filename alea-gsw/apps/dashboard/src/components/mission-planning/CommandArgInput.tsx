"use client";

import type { ArgType, TransformedSpec } from "@aleasat/command-spec";
import { Autocomplete } from "@mui/material";
import Grid2 from "@mui/material/Grid2";
// import NumberInput from "./NumberInput";
import TextField from "@mui/material/TextField";
import { DateTimeField } from "@mui/x-date-pickers/DateTimeField";
import dayjs, { type Dayjs } from "dayjs";
import { useAtom } from "jotai";
import { useEffect, useState } from "react";
import { newMissionCommandsState } from "./recoilState";

interface Props {
  arg: TransformedSpec[string]["args"][number];
  id: number;
  argIndex: number;
}

const useCommandArg = (
  id: number,
  argIndex: number,
  serialize: (value: any) => string,
  deserialize: (value: string) => any,
): [string, (newValue: any) => void] => {
  const [commands, setCommands] = useAtom(newMissionCommandsState);

  const commandIndex = commands.findIndex((command) => command.id === id);
  const command = commands[commandIndex];
  const arg =
    command.args[argIndex] === "" ? "" : deserialize(command.args[argIndex]);

  const updateArg = (newValue: any) => {
    setCommands((commands) => {
      const index = commands.findIndex((command) => command.id === id);
      const newCommands = [...commands];
      const newArgs = [...newCommands[index].args];
      newArgs[argIndex] = newValue === "" ? "" : serialize(newValue);
      newCommands[index] = { ...newCommands[index], args: newArgs };
      return newCommands;
    });
  };

  return [arg, updateArg];
};

function NumberTypeCommandArgInput(props: Props & { arg: { type: "number" } }) {
  // return <NumberInput min={props.arg.range.min} max={props.arg.range.max} />;
  const [arg, updateArg] = useCommandArg(
    props.id,
    props.argIndex,
    props.arg.serialize,
    props.arg.deserialize,
  );

  return (
    <TextField
      type="number"
      fullWidth
      label={`${props.arg.name} (${props.arg.specType})`}
      slotProps={{
        input: {
          inputProps: {
            min: props.arg.range.min,
            max: props.arg.range.max,
            ...(props.arg.float ? {} : { step: 1 }),
          },
        },
      }}
      value={arg}
      onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
        if (!event.target.value) return updateArg("");
        const newValue = props.arg.float
          ? parseFloat(event.target.value)
          : parseInt(event.target.value);
        if (newValue > props.arg.range.max) {
          updateArg(props.arg.range.max.toString());

          return;
        }
        if (newValue < props.arg.range.min)
          return updateArg(props.arg.range.min.toString());
        updateArg(newValue.toString());
      }}
      error={!arg}
      helperText={!arg ? "This field is required" : undefined}
    />
  );
}

function ArrayNumberTypeCommandArgInput(
  props: Omit<Props, "arg" | "id" | "argIndex"> & { arg: Props["arg"] } & {
    arg: { type: "number" };
  } & {
    arrayIndex: number;
    array: string[];
    setArray: (newValue: string) => void;
  },
) {
  // return <NumberInput min={props.arg.range.min} max={props.arg.range.max} />;
  const [arg, updateArg] = [props.array[props.arrayIndex], props.setArray];

  return (
    <TextField
      type="number"
      fullWidth
      required
      label={`${props.arg.name} (${props.arg.specType})`}
      slotProps={{
        input: {
          inputProps: {
            min: props.arg.range.min,
            max: props.arg.range.max,
            ...(props.arg.float ? {} : { step: 1 }),
          },
        },
      }}
      value={arg}
      onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
        if (!event.target.value) return updateArg("");
        const newValue = props.arg.float
          ? parseFloat(event.target.value)
          : parseInt(event.target.value);
        if (newValue > props.arg.range.max) {
          updateArg(props.arg.range.max.toString());

          return;
        }
        if (newValue < props.arg.range.min)
          return updateArg(props.arg.range.min.toString());
        updateArg(newValue.toString());
      }}
      error={!arg}
      helperText={!arg ? "This field is required" : undefined}
    />
  );
}

function StringTypeCommandArgInput(props: Props & { arg: { type: "string" } }) {
  const [arg, updateArg] = useCommandArg(
    props.id,
    props.argIndex,
    props.arg.serialize,
    props.arg.deserialize,
  );

  // return <NumberInput min={props.arg.range.min} max={props.arg.range.max} />;
  return (
    <TextField
      fullWidth
      required
      label={`${props.arg.name} (${props.arg.specType})`}
      value={arg}
      onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
        updateArg(event.target.value);
      }}
      error={!arg}
      helperText={!arg ? "This field is required" : undefined}
    />
  );
}

function MatrixTypeCommandArgInput(props: Props & { arg: { type: "matrix" } }) {
  const updateArg = useCommandArg(
    props.id,
    props.argIndex,
    props.arg.serialize,
    props.arg.deserialize,
  )[1];
  const [martix, setMatrix] = useState<string[]>(
    Array.from({ length: props.arg.row * props.arg.column }).map(() => ""),
  );

  useEffect(() => {
    const newMatrix: number[][] = [];
    for (let i = 0; i < props.arg.row; i++) {
      newMatrix.push([]);
      for (let j = 0; j < props.arg.column; j++) {
        const value = martix[i * props.arg.column + j];
        newMatrix[i].push(
          props.arg.elementType.float ? parseFloat(value) : parseInt(value),
        );
      }
    }
    updateArg(newMatrix);
  }, [martix]);

  return (
    <Grid2 container columns={props.arg.column} spacing={1}>
      {Array.from({ length: props.arg.row * props.arg.column }).map((_, i) => (
        <Grid2 key={i} size={1}>
          <ArrayNumberTypeCommandArgInput
            arg={{
              ...props.arg.elementType,
              name: `${props.arg.name} [${Math.floor(i / props.arg.column)},${
                i % props.arg.column
              }]`,
              specType: props.arg.specType.split("[")[0] as ArgType,
            }}
            arrayIndex={i}
            array={martix}
            setArray={(newValue: string) => {
              setMatrix((matrix) => {
                const newMatrix = [...matrix];
                newMatrix[i] = newValue;
                return newMatrix;
              });
            }}
          />
        </Grid2>
      ))}
    </Grid2>
  );
}

function ArrayTypeCommandArgInput(props: Props & { arg: { type: "array" } }) {
  const updateArg = useCommandArg(
    props.id,
    props.argIndex,
    props.arg.serialize,
    props.arg.deserialize,
  )[1];
  const [array, setArray] = useState<string[]>(
    Array.from({ length: props.arg.length }).map(() => ""),
  );

  useEffect(() => {
    updateArg(array);
  }, [array]);

  return (
    <Grid2 container columns={props.arg.length} spacing={1}>
      {Array.from({ length: props.arg.length }).map((_, i) => (
        <Grid2 key={i} size={1}>
          <ArrayNumberTypeCommandArgInput
            arg={{
              ...props.arg.elementType,
              name: `${props.arg.name} [${i}]`,
              specType: props.arg.specType.split("[")[0] as ArgType,
            }}
            arrayIndex={i}
            array={array}
            setArray={(newValue: string) => {
              setArray((array) => {
                const newArray = [...array];
                newArray[i] = newValue;
                return newArray;
              });
            }}
          />
        </Grid2>
      ))}
    </Grid2>
  );
}

function DateTypeCommandArgInput(props: Props & { arg: { type: "date" } }) {
  const [arg, updateArg] = useCommandArg(
    props.id,
    props.argIndex,
    props.arg.serialize,
    props.arg.deserialize,
  );

  return (
    <DateTimeField
      fullWidth
      required
      label={`${props.arg.name} (${props.arg.specType})`}
      format="YYYY:MM:DD HH:mm:ss"
      value={arg ? dayjs(arg) : null}
      onChange={(date: Dayjs | null) => {
        if (date)
          try {
            date.toISOString();
          } catch {
            return;
          }
        updateArg(date ?? "");
      }}
    />
  );
}

function BooleanTypeCommandArgInput(
  props: Props & { arg: { type: "boolean" } },
) {
  const [arg, updateArg] = useCommandArg(
    props.id,
    props.argIndex,
    props.arg.serialize,
    props.arg.deserialize,
  );

  return (
    // <Checkbox
    //   required
    //   value={!!arg}
    //   onChange={(event: ChangeEvent<HTMLInputElement>, checked: boolean) => {
    //     updateArg(checked);
    //   }}
    // />

    <Autocomplete
      value={arg.toString()}
      onChange={(_event: any, newValue: string) => {
        updateArg(newValue === "true");
      }}
      disableClearable
      options={["true", "false"]}
      fullWidth
      renderInput={(params) => (
        <TextField
          required
          {...params}
          label={props.arg.name}
          error={arg === ""}
          helperText={arg === "" ? "This field is required" : undefined}
        />
      )}
    />
  );
}

export default function CommandArgInput(props: Props) {
  if (props.arg.type === "number") {
    return <NumberTypeCommandArgInput {...props} arg={props.arg} />;
  }

  if (props.arg.type === "string") {
    return <StringTypeCommandArgInput {...props} arg={props.arg} />;
  }

  if (props.arg.type === "matrix") {
    return <MatrixTypeCommandArgInput {...props} arg={props.arg} />;
  }

  if (props.arg.type === "array") {
    return <ArrayTypeCommandArgInput {...props} arg={props.arg} />;
  }

  if (props.arg.type === "date") {
    return <DateTypeCommandArgInput {...props} arg={props.arg} />;
  }

  if (props.arg.type === "boolean") {
    return <BooleanTypeCommandArgInput {...props} arg={props.arg} />;
  }

  return (
    <>
      <div>Error: Input Type Not Implemented</div>
    </>
  );
}
