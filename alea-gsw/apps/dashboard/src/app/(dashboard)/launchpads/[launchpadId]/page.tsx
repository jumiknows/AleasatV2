import { auth } from "@/auth";
import { Typography } from "@mui/material";
import launchpadFetcher from "./launchpadFetcher";

export default async function LaunchpadPage({
  params,
}: {
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
    <>
      <Typography>ID: {launchpad.id}</Typography>
      <Typography>Name: {launchpad.name}</Typography>
      <Typography>Firmware: {launchpad.fwCommit}</Typography>
      <br />
      <Typography>Server: {launchpad.node}</Typography>
      <Typography>Port: {launchpad.port}</Typography>
      <Typography>Service Name: {launchpad.serviceName}</Typography>
    </>
  );
}
