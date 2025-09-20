import { auth } from "@/auth";
import FirmwareUpdateLog from "./FirmwareUpdateLog";

export default async function FirmwareUpdateLogPage({
  params,
}: {
  params: { jobName: string };
}) {
  const session = await auth();

  if (session === null) {
    return null;
  }

  return (
    <>
      <FirmwareUpdateLog jobName={params.jobName} session={session} />
    </>
  );
}
