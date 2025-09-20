"use client";

import { Button } from "@mui/material";
import { PageContainer, PageHeaderToolbar } from "@toolpad/core";
import { useSession } from "next-auth/react";
import type { ReactNode } from "react";

function AddLaunchpadButton() {
  const session = useSession();

  return (
    <PageHeaderToolbar>
      {session.data?.user.groups.includes("aleasat-lead") && (
        <Button variant="contained" color="primary" href="/launchpads/new">
          New Launchpad
        </Button>
      )}
    </PageHeaderToolbar>
  );
}

export default function AddLaunchpadButtonPageContainer({
  children,
}: {
  children: ReactNode;
}) {
  return (
    <PageContainer maxWidth={false} slots={{ header: AddLaunchpadButton }}>
      {children}
    </PageContainer>
  );
}
