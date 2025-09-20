"use client";

import { Configuration, LaunchpadApi } from "@aleasat/launchpad-api";
import { Button, Stack, Typography } from "@mui/material";
import type { Session } from "next-auth";
import { useRouter } from "next/navigation";
import { useState } from "react";
import BranchList from "./BranchList";
import CommitList from "./CommitList";

export default function LaunchpadPage({
  launchpadId,
  session,
}: {
  launchpadId: string;
  session: Session;
}) {
  const router = useRouter();
  const [branch, setBranch] = useState<string | undefined>(undefined);
  const [commit, setCommit] = useState<string | undefined>(undefined);

  const launchpadApi = new LaunchpadApi(
    new Configuration({
      accessToken: session.accessToken,
    }),
  );

  const updateFirmware = async () => {
    if (!commit) {
      return;
    }
    const updateJob = await launchpadApi.updateLaunchpadFirmware({
      launchpadId,
      updateLaunchpadFirmwareRequest: {
        fwCommit: commit,
      },
    });
    router.push(`/launchpads/${launchpadId}/update/log/${updateJob.jobName}`);
  };

  return (
    <>
      <Stack spacing={2}>
        <BranchList branch={branch} setBranch={setBranch} />
        <CommitList branch={branch} commit={commit} setCommit={setCommit} />
        <Typography>
          fwHash = {commit ? parseInt(commit.slice(0, 8), 16) : "N/A"}
        </Typography>
        <Button
          variant="contained"
          onClick={async () => await updateFirmware()}
        >
          Update
        </Button>
      </Stack>
    </>
  );
}
