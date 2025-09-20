"use client";

import { Paper, Typography } from "@mui/material";
import { useAtomValue } from "jotai";
import { newMissionCommandsState, newMissionState } from "./recoilState";

export default function MissionObjectDisplay() {
  const newMission = useAtomValue(newMissionState);
  const commands = useAtomValue(newMissionCommandsState);

  const mission = { ...newMission };
  mission.commands = commands.map((command, i) => ({
    sequenceNumber: i + 1,
    commandName: command.commandName,
    commandId: command.commandId,
    arguments: command.args,
  }));

  return (
    <Paper variant="outlined" sx={{ padding: "1%" }}>
      <Typography sx={{ whiteSpace: "pre" }}>
        {JSON.stringify(mission, null, 2)}
      </Typography>
    </Paper>
  );
}
