import { auth } from "@/auth";
import type { ReadMission } from "@aleasat/mission-api";
import {
  Alert,
  Grid2,
  Paper,
  Skeleton,
  Stack,
  Typography,
} from "@mui/material";
import MissionStatusChip from "./MissionStatusChip";
import Refresher from "./Refresher";
import { getMission } from "./fetcher";

export function MissionDetailsSkeleton({
  missionId,
  mission,
}: {
  missionId: string;
  mission?: ReadMission;
}) {
  return (
    <>
      <Stack
        direction="row"
        sx={{ alignItems: "center", marginBottom: 2 }}
        spacing={4}
      >
        <Typography variant="h5" component="h2">
          {missionId}
        </Typography>
        {mission ? (
          <MissionStatusChip status={mission.status} />
        ) : (
          <Skeleton variant="rounded">
            <MissionStatusChip status="created" />
          </Skeleton>
        )}
      </Stack>
      {mission?.error && (
        <Alert variant="filled" severity="error">
          {mission.error}
        </Alert>
      )}
      <Paper
        variant="outlined"
        sx={{
          padding: 2,
          marginY: 2,
        }}
      >
        <Grid2 container spacing={1}>
          {(
            ["fwVersion", "createdAt", "userId", "status"] as Array<
              keyof ReadMission
            >
          ).map((key) => (
            <Grid2 key={key} size={6}>
              <Typography variant="subtitle2">{key}</Typography>
              <Typography>
                {mission ? (
                  typeof mission[key] === "string" ? (
                    (mission[key] as string)
                  ) : mission[key] instanceof Date ? (
                    (mission[key] as Date).toISOString()
                  ) : (
                    ""
                  )
                ) : (
                  <Skeleton />
                )}
              </Typography>
            </Grid2>
          ))}
          <Grid2 size={6}>
            <Typography variant="subtitle2">&nbsp;</Typography>
            <Typography>&nbsp;</Typography>
          </Grid2>
        </Grid2>
      </Paper>
    </>
  );
}

export default async function MissionDetails({
  missionId,
}: {
  missionId: string;
}) {
  const session = await auth();
  if (!session) return <></>;

  const mission = await getMission({
    missionId,
    accessToken: session.accessToken,
  });

  return (
    <>
      <MissionDetailsSkeleton missionId={missionId} mission={mission} />
      {mission.status !== "sent" && mission.status !== "error" && <Refresher />}
    </>
  );
}
