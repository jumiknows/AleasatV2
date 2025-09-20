import { Container, Grid2, Typography } from "@mui/material";
import { Suspense } from "react";
import MissionCommandList, {
  MissionCommandListSkeleton,
} from "./MissionCommandList";
import MissionDetails, { MissionDetailsSkeleton } from "./MissionDetails";
import MissionSchedule, { MissionScheduleSkeleton } from "./MissionSchedule";

export const dynamic = "force-dynamic";

export default function MissionPage({
  params,
}: {
  params: { missionId: string };
}) {
  return (
    <Container maxWidth="lg">
      <Typography variant="h4" component="h1">
        Mission
      </Typography>
      <Grid2 container spacing={3}>
        <Grid2 size={9}>
          <Suspense
            fallback={<MissionDetailsSkeleton missionId={params.missionId} />}
          >
            <MissionDetails missionId={params.missionId} />
          </Suspense>
        </Grid2>
        <Grid2 size={3}>
          <Suspense fallback={<MissionScheduleSkeleton />}>
            <MissionSchedule missionId={params.missionId} />
          </Suspense>
        </Grid2>
      </Grid2>
      <Suspense fallback={<MissionCommandListSkeleton />}>
        <MissionCommandList missionId={params.missionId} />
      </Suspense>
    </Container>
  );
}
