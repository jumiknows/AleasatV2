import { type TransformedSpec, getCommandSpec } from "@aleasat/command-spec";
import type { ReadCommand } from "@aleasat/mission-api";
import { Paper, Skeleton, Typography } from "@mui/material";
import MissionCommandResponse from "./MissionCommandResponse";

export default function MissionCommand({
  fwVersion,
  command,
}: {
  fwVersion?: string;
  command?: ReadCommand;
}) {
  let transformedSpec: TransformedSpec | undefined;
  if (fwVersion) {
    ({ transformedSpec } = getCommandSpec({
      commitHash: fwVersion,
    })!);
  }

  return (
    <Paper
      variant="outlined"
      sx={{
        padding: 2,
        marginY: 2,
      }}
    >
      <Typography variant="h6" component="h2">
        {command ? command.commandName : <Skeleton />}
      </Typography>
      {command ? (
        <Typography sx={{ whiteSpace: "pre" }}>
          {JSON.stringify(
            {
              sequenceNumber: command.sequenceNumber,
              commandId: command.commandId,
              commandName: command.commandName,
              timeOffset: command.timeOffset,
              arguments: command.arguments,
              sentAt: command.sentAt,
              ranAt: command.ranAt,
              error: command.error,
            },
            null,
            2,
          )}
        </Typography>
      ) : (
        <Skeleton variant="rectangular">
          <Typography sx={{ whiteSpace: "pre" }}>
            {JSON.stringify(
              {
                sequenceNumber: 1,
                commandId: 2,
                commandName: "GET_MIN_STACK_SPACE GET_MIN_STACK_SPACE",
                timeOffset: 0,
                arguments: [],
                sentAt: "2024-09-10T20:47:19.804Z",
                cmdInstId: null,
                ranAt: "2024-09-10T20:47:19.804Z",
                error: null,
              },
              null,
              2,
            )}
          </Typography>
        </Skeleton>
      )}
      {command && transformedSpec ? (
        command.responses.map((response, i) => (
          <MissionCommandResponse
            key={i}
            resp={transformedSpec[command.commandName].resp![i]}
            response={response}
          />
        ))
      ) : (
        <MissionCommandResponse />
      )}
    </Paper>
  );
}
