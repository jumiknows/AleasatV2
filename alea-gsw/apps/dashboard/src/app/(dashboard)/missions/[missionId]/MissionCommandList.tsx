import { auth } from "@/auth";
import type { ReadCommand } from "@aleasat/mission-api";
import MissionCommand from "./MissionCommand";
import { getMission } from "./fetcher";

export function MissionCommandListSkeleton({
  fwVersion,
  commands,
}: {
  fwVersion?: string;
  commands?: ReadCommand[];
}) {
  if (!commands)
    return Array.from({ length: 5 }).map((_, key) => (
      <MissionCommand key={key} />
    ));

  return commands.map((command, key) => (
    <MissionCommand key={key} fwVersion={fwVersion} command={command} />
  ));
}

export default async function MissionCommandList({
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
      <MissionCommandListSkeleton
        fwVersion={mission.fwVersion}
        commands={mission.commands}
      />
    </>
  );
}
