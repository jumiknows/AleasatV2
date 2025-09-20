import { auth } from "@/auth";
import { Container } from "@mui/material";
import { PageContainer } from "@toolpad/core";
import type { ReactNode } from "react";
import TabsBar from "./TabsBar";
import launchpadFetcher from "./launchpadFetcher";

export default async function LaunchpadLayout({
  children,
  params,
}: {
  children: ReactNode;
  params: { launchpadId: string };
}) {
  const session = await auth();

  if (!session) {
    return null;
  }

  const launchpad = await launchpadFetcher(
    params.launchpadId,
    session.accessToken,
  );

  return (
    <PageContainer maxWidth={false} title={launchpad.name}>
      <Container maxWidth="lg">
        <TabsBar launchpadId={params.launchpadId} />
        {children}
      </Container>
    </PageContainer>
  );
}
