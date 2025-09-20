import type { TransformedSpec } from "@aleasat/command-spec";
import { useSortable } from "@dnd-kit/sortable";
import { CSS } from "@dnd-kit/utilities";
import ArrowDropDownIcon from "@mui/icons-material/ArrowDropDown";
import ArrowDropUpIcon from "@mui/icons-material/ArrowDropUp";
import DeleteIcon from "@mui/icons-material/Delete";
import DragIndicatorIcon from "@mui/icons-material/DragIndicator";
import {
  Autocomplete,
  Box,
  Divider,
  Grid2,
  IconButton,
  List,
  ListItem,
  Paper,
  Stack,
  TextField,
  Typography,
} from "@mui/material";
import { useAtom } from "jotai";
import React, { useState } from "react";
import CommandArgInput from "./CommandArgInput";
import { newMissionCommandsState } from "./recoilState";

export default function CommandInput(props: {
  transformedSpec: TransformedSpec;
  id: number;
}) {
  const [commands, setCommands] = useAtom(newMissionCommandsState);
  const [openArgDrawer, setOpenArgDrawer] = useState(true);
  const { attributes, listeners, setNodeRef, transform, transition } =
    useSortable({ id: props.id });

  const style = {
    transform: CSS.Translate.toString(transform),
    transition,
  };

  const command = commands.find(
    (command) => command.id === props.id,
  )!.commandName;

  return (
    <Paper
      ref={setNodeRef}
      style={{ ...style, margin: "1%", backgroundColor: "#060606" }}
      variant="outlined"
    >
      <Paper variant="outlined" sx={{ padding: "1%" }}>
        <Stack direction="row" sx={{ alignItems: "center" }}>
          <IconButton {...attributes} aria-describedby="" {...listeners}>
            <DragIndicatorIcon />
          </IconButton>
          <Autocomplete
            value={command}
            onChange={(_event: any, newValue: string) => {
              setCommands((commands) => {
                const index = commands.findIndex(
                  (command) => command.id === props.id,
                );
                const newCommands = [...commands];
                newCommands[index] = {
                  ...newCommands[index],
                  commandName: newValue,
                  commandId: props.transformedSpec[newValue].id,
                  args: Array.from({
                    length: props.transformedSpec[newValue].args.length,
                  }).map(() => ""),
                };
                return newCommands;
              });
            }}
            disableClearable
            options={Object.keys(props.transformedSpec)}
            fullWidth
            renderInput={(params) => <TextField {...params} label="Command" />}
          />
          <IconButton onClick={() => setOpenArgDrawer(!openArgDrawer)}>
            {openArgDrawer ? <ArrowDropDownIcon /> : <ArrowDropUpIcon />}
          </IconButton>
          <IconButton
            onClick={() => {
              setCommands((commands) =>
                commands.filter((command) => command.id !== props.id),
              );
            }}
          >
            <DeleteIcon />
          </IconButton>
        </Stack>
      </Paper>
      {command && openArgDrawer && (
        <List>
          <ListItem>
            <Typography variant="h6">Arguments:</Typography>
          </ListItem>
          {props.transformedSpec[command].args.map((arg, i) => (
            <Box key={command + i}>
              {i !== 0 && <Divider />}
              <ListItem>
                <Grid2 container sx={{ width: "100%" }}>
                  <Grid2 size={4}>
                    <Typography>{arg.name}</Typography>
                  </Grid2>
                  <Grid2 size={8}>
                    <CommandArgInput arg={arg} id={props.id} argIndex={i} />
                  </Grid2>
                </Grid2>
              </ListItem>
            </Box>
          ))}
        </List>
      )}
    </Paper>
  );
}
