import type {
  ServiceReturnTypeV2,
  components,
} from "@aleasat/pass-scheduling-api";
import opentelemetry from "@opentelemetry/api";
import { prisma } from "../repositories/prisma.js";

const tracer = opentelemetry.trace.getTracer("missionScheduleService");

const serializeMission = (
  mission: Exclude<
    Awaited<
      ReturnType<
        typeof prisma.mission.findFirst<{
          include: {
            missionSchedule: true;
            commands: {
              include: {
                commandSchedule: true;
              };
            };
          };
        }>
      >
    >,
    null
  >,
): components["schemas"]["ReadMissionScheduleMission"] => ({
  id: mission.id,
  status: mission.status,
  scheduledAt: mission.scheduledAt ? mission.scheduledAt.toISOString() : null,
  schedule: mission.missionSchedule && {
    id: mission.missionSchedule.id,
    initialOverheadStart:
      mission.missionSchedule.initialOverheadStart.toISOString(),
    missionStart: mission.missionSchedule.missionStart.toISOString(),
    missionEnd: mission.missionSchedule.missionEnd.toISOString(),
    directOverheadEnd: mission.missionSchedule.directOverheadEnd.toISOString(),
    indirectOverheadEnd:
      mission.missionSchedule.indirectOverheadEnd.toISOString(),
  },
  commands: mission.commands.map((command) => ({
    id: command.id,
    sequenceNumber: command.sequenceNumber,
    commandId: command.commandId,
    commandName: command.commandName,
    timeOffset: command.timeOffset,
    schedule: command.commandSchedule && {
      id: command.commandSchedule.id,
      start: command.commandSchedule.start.toISOString(),
      end: command.commandSchedule.end.toISOString(),
      overheadEnd: command.commandSchedule.overheadEnd.toISOString(),
    },
  })),
});

const getScheduledMissions = async (): Promise<
  ServiceReturnTypeV2<"getScheduledMissions">
> =>
  await tracer.startActiveSpan("getScheduledMissions", async (span) => {
    const missions = await prisma.mission.findMany({
      include: {
        missionSchedule: true,
        commands: {
          include: {
            commandSchedule: true,
          },
        },
      },
    });

    span.end();
    return {
      success: true as true,
      data: missions.map((mission) => serializeMission(mission)),
    };
  });

const getScheduledMission = async (
  missionId: string,
): Promise<ServiceReturnTypeV2<"getScheduledMission">> =>
  await tracer.startActiveSpan("getScheduledMission", async (span) => {
    const mission = await prisma.mission.findFirst({
      where: {
        id: missionId,
      },
      include: {
        missionSchedule: true,
        commands: {
          include: {
            commandSchedule: true,
          },
        },
      },
    });

    if (mission === null) {
      return {
        success: false as false,
        error: {
          status: 404,
          title: "Mission not found",
        },
      };
    }

    span.end();
    return {
      success: true as true,
      data: serializeMission(mission),
    };
  });

const getScheduleSpaces = async (): Promise<
  ServiceReturnTypeV2<"getScheduleSpaces">
> =>
  await tracer.startActiveSpan("getScheduleSpaces", async (span) => {
    const spaces = await prisma.missionScheduleSpace.findMany({
      orderBy: {
        start: "asc",
      },
    });

    span.end();
    return {
      success: true as true,
      data: spaces.map((space) => ({
        ...space,
        start: space.start.toISOString(),
        end: space.end ? space.end.toISOString() : null,
      })),
    };
  });

export default {
  getScheduledMissions,
  getScheduledMission,
  getScheduleSpaces,
};
