import { auth } from "@/auth";
import type { ReadMission } from "@aleasat/mission-api";
import type { ReadMissionScheduleMission } from "@aleasat/pass-scheduling-api";
import { Box, Grid2, Paper, Skeleton, Typography } from "@mui/material";
import MissionCountDown from "./MissionCountDown";
import { getMission, getMissionSchedule } from "./fetcher";

export function MissionScheduleSkeleton({
  mission,
  missionSchedule,
}: {
  mission?: ReadMission;
  missionSchedule?: ReadMissionScheduleMission;
}) {
  return (
    <Box>
      <Typography variant="h5" component="h2">
        Schedule{" "}
        {missionSchedule?.schedule &&
          mission &&
          mission.status !== "sent" &&
          mission.status !== "error" && (
            <MissionCountDown
              scheduledTime={new Date(
                missionSchedule.schedule.missionStart,
              ).getTime()}
            />
          )}
      </Typography>
      <Paper
        variant="outlined"
        sx={{
          padding: 2,
          marginY: 2,
        }}
      >
        <Grid2 container spacing={1}>
          <Grid2 size={12}>
            <Typography variant="subtitle2">Start</Typography>
            <Typography>
              {missionSchedule?.schedule ? (
                new Date(missionSchedule.schedule.missionStart).toLocaleString()
              ) : (
                <Skeleton />
              )}
            </Typography>
          </Grid2>
          <Grid2 size={12}>
            <Typography variant="subtitle2">End</Typography>
            <Typography>
              {missionSchedule?.schedule ? (
                new Date(missionSchedule.schedule.missionEnd).toLocaleString()
              ) : (
                <Skeleton />
              )}
            </Typography>
          </Grid2>
          <Grid2 size={12}>
            <Typography variant="subtitle2">Duration</Typography>
            <Typography>
              {missionSchedule?.schedule ? (
                Math.round(
                  (new Date(missionSchedule.schedule.missionEnd).getTime() -
                    new Date(missionSchedule.schedule.missionStart).getTime()) /
                    1000,
                ) + " s"
              ) : (
                <Skeleton />
              )}
            </Typography>
          </Grid2>
        </Grid2>
      </Paper>
    </Box>
  );
}

export default async function MissionSchedule({
  missionId,
}: {
  missionId: string;
}) {
  const session = await auth();
  if (!session) return <></>;

  const [mission, missionSchedule] = await Promise.all([
    getMission({ missionId, accessToken: session.accessToken }),
    getMissionSchedule({
      missionId,
      accessToken: session.accessToken,
    }),
  ]);

  if (!missionSchedule?.schedule) return <MissionScheduleSkeleton />;

  return (
    <MissionScheduleSkeleton
      mission={mission}
      missionSchedule={missionSchedule}
    />
  );
}
