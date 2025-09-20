import type {
  ServiceReturnType,
  ServiceReturnTypeV2,
  operations,
} from "@aleasat/mission-api";
import opentelemetry from "@opentelemetry/api";
import { missionQueueApi } from "../apis/pass-scheduling.js";
import { prisma } from "../repositories/prisma.js";

const tracer = opentelemetry.trace.getTracer("missionService");

const getMissions = async (
  query: operations["getMissions"]["parameters"]["query"],
): Promise<ServiceReturnType<"getMissions">> =>
  await tracer.startActiveSpan("getMissions", async (span) => {
    const missions = await prisma.mission
      .findMany({
        where: {
          status: query?.sent ? "sent" : undefined,
        },
        include: {
          commands: true,
        },
      })
      .then((missions) =>
        missions.map((mission) => ({
          ...mission,
          createdAt: mission.createdAt.toISOString(),
          scheduledAt: mission.scheduledAt
            ? mission.scheduledAt.toISOString()
            : null,
          commands: mission.commands.map((command) => ({
            ...command,
            sentAt: command.sentAt ? command.sentAt.toISOString() : null,
            ranAt: command.ranAt ? command.ranAt.toISOString() : null,
          })),
        })),
      );

    span.end();
    return {
      data: missions,
    };
  });

const getMission = async (
  missionId: operations["getMission"]["parameters"]["path"]["missionId"],
): Promise<ServiceReturnType<"getMission">> =>
  await tracer.startActiveSpan("getMission", async (span) => {
    const mission = await prisma.mission.findUnique({
      where: {
        id: missionId,
      },
      include: {
        commands: true,
      },
    });

    if (!mission) {
      return {
        error: {
          status: 404,
          title: `Mission not found`,
        },
      };
    }

    const formattedMission = {
      ...mission,
      createdAt: mission.createdAt.toISOString(),
      scheduledAt: mission.scheduledAt
        ? mission.scheduledAt.toISOString()
        : null,
      commands: mission.commands.map((command) => ({
        ...command,
        sentAt: command.sentAt ? command.sentAt.toISOString() : null,
        ranAt: command.ranAt ? command.ranAt.toISOString() : null,
      })),
    };

    span.end();
    return {
      data: formattedMission,
    };
  });

const createMission = async (
  userId: string,
  newMission: operations["createMission"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"createMission">> =>
  await tracer.startActiveSpan("createMission", async (span) => {
    const createdMission = await prisma.mission.create({
      include: {
        commands: true,
      },
      data: {
        fwVersion: newMission.fwVersion,
        userId,
        scheduledAt: newMission.scheduledAt
          ? new Date(newMission.scheduledAt)
          : null,
        commands: {
          createMany: {
            data: newMission.commands.map((command) => ({
              sequenceNumber: command.sequenceNumber,
              commandId: command.commandId,
              commandName: command.commandName,
              timeOffset: command.timeOffset ?? 0,
              arguments: command.arguments,
            })),
          },
        },
      },
    });

    missionQueueApi
      .queueMission({
        passSchedulingMissionWriteOnly: {
          id: createdMission.id,
          scheduledAt: createdMission.scheduledAt,
          commands: createdMission.commands.map((command) => ({
            id: command.id,
            sequenceNumber: command.sequenceNumber,
            commandId: command.commandId,
            commandName: command.commandName,
            timeOffset: command.timeOffset,
          })),
        },
      })
      .then(() => {
        patchMission(createdMission.id, { status: "scheduled" });
      });

    span.end();
    return {
      success: true as true,
      data: {
        ...createdMission,
        createdAt: createdMission.createdAt.toISOString(),
        scheduledAt: createdMission.scheduledAt
          ? createdMission.scheduledAt.toISOString()
          : null,
        commands: createdMission.commands.map((command) => ({
          ...command,
          sentAt: command.sentAt ? command.sentAt.toISOString() : null,
          ranAt: command.ranAt ? command.ranAt.toISOString() : null,
        })),
      },
    };
  });

const patchMission = async (
  missionId: operations["getMission"]["parameters"]["path"]["missionId"],
  patch: operations["patchMission"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"patchMission">> =>
  await tracer.startActiveSpan("patchMission", async (span) => {
    const patchedMission = await prisma.mission.update({
      where: {
        id: missionId,
      },
      data: {
        status: patch.status,
        error: patch.error,
      },
      include: {
        commands: true,
      },
    });

    span.end();
    return {
      success: true as true,
      data: {
        ...patchedMission,
        createdAt: patchedMission.createdAt.toISOString(),
        scheduledAt: patchedMission.scheduledAt
          ? patchedMission.scheduledAt.toISOString()
          : null,
        commands: patchedMission.commands.map((command) => ({
          ...command,
          sentAt: command.sentAt ? command.sentAt.toISOString() : null,
          ranAt: command.ranAt ? command.ranAt.toISOString() : null,
        })),
      },
    };
  });

export default {
  getMissions,
  getMission,
  createMission,
  patchMission,
};
