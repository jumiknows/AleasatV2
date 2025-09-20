import type {
  ServiceReturnTypeV2,
  operations,
} from "@aleasat/pass-scheduling-api";
import opentelemetry from "@opentelemetry/api";
import { prisma } from "../repositories/prisma.js";
import {
  commandExecutionOverhead,
  commandExecutionTime,
} from "../scheduling/constants.js";
import { processMissionQueue } from "../scheduling/missionQueue.js";

const tracer = opentelemetry.trace.getTracer("missionQueueService");

const queueMission = async (
  mission: operations["queueMission"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"queueMission">> =>
  await tracer.startActiveSpan("queueMission", async (span) => {
    const missionDuration = mission.commands.reduce((acc, command) => {
      // 1s (Time to execute command) + 0.5s (command overhead) + timeOffset (time wait for this command)
      return (
        acc +
        commandExecutionTime +
        commandExecutionOverhead +
        command.timeOffset
      );
    }, 0);

    const queueMissionTransaction = await prisma.$transaction(async (tx) => {
      const missionQueued = await tx.mission.findUnique({
        where: {
          id: mission.id,
        },
      });

      if (missionQueued !== null) {
        return {
          success: false as false,
          error: "Mission already queued",
        };
      }

      if (mission.scheduledAt) {
        const otherMissionScheduled = await tx.missionSchedule.findFirst({
          where: {
            OR: [
              {
                initialOverheadStart: {
                  lte: new Date(mission.scheduledAt),
                  gte: new Date(
                    new Date(mission.scheduledAt).getTime() +
                      missionDuration * 1000,
                  ),
                },
              },
              {
                indirectOverheadEnd: {
                  lte: new Date(mission.scheduledAt),
                  gte: new Date(
                    new Date(mission.scheduledAt).getTime() +
                      missionDuration * 1000,
                  ),
                },
              },
            ],
          },
        });

        if (otherMissionScheduled !== null) {
          return {
            success: false as false,
            error: "Mission already scheduled at this time",
          };
        }
      }

      await tx.missionQueue.create({
        data: {
          id: mission.id,
          mission: {
            create: {
              id: mission.id,
              scheduledAt: mission.scheduledAt && new Date(mission.scheduledAt),
              commands: {
                createMany: {
                  data: mission.commands.map((command) => ({
                    id: command.id,
                    sequenceNumber: command.sequenceNumber,
                    commandId: command.commandId,
                    commandName: command.commandName,
                    timeOffset: command.timeOffset,
                  })),
                },
              },
            },
          },
        },
      });

      return {
        success: true as true,
        data: {},
      };
    });

    if (!queueMissionTransaction.success) {
      span.end();
      return {
        success: false as false,
        error: {
          status: 400,
          title: queueMissionTransaction.error,
        },
      };
    }

    await processMissionQueue();
    span.end();
    return {
      success: true as true,
      data: {},
    };
  });

const getQueuedMissions = async (): Promise<
  ServiceReturnTypeV2<"getQueuedMissions">
> =>
  await tracer.startActiveSpan("getQueuedMissions", async (span) => {
    const queuedMissions = await prisma.missionQueue.findMany({
      orderBy: {
        queueAt: "asc",
      },
    });

    span.end();
    return {
      success: true as true,
      data: queuedMissions.map((queuedMission) => ({
        ...queuedMission,
        queueAt: queuedMission.queueAt.toISOString(),
      })),
    };
  });

export default {
  queueMission,
  getQueuedMissions,
};
