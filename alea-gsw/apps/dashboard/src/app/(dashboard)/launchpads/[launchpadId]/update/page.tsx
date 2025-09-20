import { auth } from "@/auth";
import FirmwareUpdate from "./FirmwareUpdate";

export default async function LaunchpadPage({
  params,
}: {
  params: { launchpadId: string };
}) {
  const session = await auth();

  if (!session) {
    return null;
  }

  return (
    <>
      <FirmwareUpdate launchpadId={params.launchpadId} session={session} />
    </>
  );
}
