import "./initialize.js";
import opentelemetry from "@opentelemetry/api";
import { missionExecutionApi } from "../apis/command-control.js";
import { prisma } from "../repositories/prisma.js";
import logger from "../utils/logger.js";
import {
  commandExecutionOverhead,
  commandExecutionTime,
  directSchedulingOverhead,
  indirectSchedulingOverhead,
  initialOverhead,
  scheduleAfter,
} from "./constants.js";

const tracer = opentelemetry.trace.getTracer("scheduling.missionQueue");

export const processOneMissionInQueue = async (): Promise<boolean> =>
  await tracer.startActiveSpan(
    "processOneMissionInQueue",
    async (span) =>
      await prisma.$transaction(async (tx) => {
        const missionInQueue = await tx.missionQueue.findFirst({
          orderBy: {
            queueAt: "asc",
          },
          include: {
            mission: {
              include: {
                commands: true,
              },
            },
          },
        });

        if (!missionInQueue) {
          span.end();
          return false;
        }

        const now = new Date();
        const mission = missionInQueue.mission;

        // If mission have a time constraint and it's expired, we mark it as expired
        if (mission.scheduledAt && mission.scheduledAt < now) {
          await tx.missionQueue.delete({
            where: {
              id: missionInQueue.id,
            },
          });
          await tx.mission.update({
            where: {
              id: mission.id,
            },
            data: {
              status: "expired",
            },
          });
          span.end();
          return true;
        }

        // Reference: https://jwst-docs.stsci.edu/jppom/files/73025140/73025141/1/1496672566126/fig2_visit_timing.png
        // Website: https://jwst-docs.stsci.edu/jppom/visit-overheads-timing-model/visit-timing#gsc.tab=0
        const missionDuration = mission.commands.reduce((acc, command) => {
          // 1s (Time to execute command) + 0.5s (command overhead) + timeOffset (time wait for this command)
          return (
            acc +
            commandExecutionTime +
            commandExecutionOverhead +
            command.timeOffset
          );
        }, 0);

        // 5s (Pre mission scheduling overhead) +  2.5s (Pre mission overhead (FW check, etc)) + Mision Duration + 2.5s (Post mission overhead (FW check, etc)) + 5s (Post mission scheduling overhead)
        const schedulingDuration =
          initialOverhead +
          missionDuration +
          directSchedulingOverhead +
          indirectSchedulingOverhead;

        const scheduledAt =
          mission.scheduledAt &&
          mission.scheduledAt.getTime() - initialOverhead * 1000;

        const scheduleSpace = await tx.missionScheduleSpace.findFirst({
          orderBy: {
            start: "asc",
          },
          where: {
            OR: [
              {
                AND: [
                  {
                    end: {
                      gt: new Date(
                        (scheduledAt ?? now.getTime() + scheduleAfter) +
                          missionDuration * 1000,
                      ),
                    },
                  },
                  {
                    duration: {
                      gte: schedulingDuration,
                    },
                  },
                ],
              },
              {
                end: null,
              },
            ],
          },
        });

        if (!scheduleSpace) {
          if (mission.scheduledAt) {
            // If the mission is scheduled, but there is no space for it, we mark it as expired
            await tx.missionQueue.delete({
              where: {
                id: missionInQueue.id,
              },
            });
            await tx.mission.update({
              where: {
                id: mission.id,
              },
              data: {
                status: "expired",
              },
            });
            span.end();
            return true;
          }

          // Should not be possible, but just in case
          logger.error(
            "No schedule space found for mission",
            mission.id,
            "with scheduling duration",
            schedulingDuration,
          );
          span.end();
          return false;
        }

        const startAt =
          (scheduledAt ?? now.getTime() + scheduleAfter) +
          missionDuration * 1000;

        // First delete the schedule space, then create the free space before and after the scheduling
        await tx.missionScheduleSpace.delete({
          where: {
            id: scheduleSpace.id,
          },
        });
        // Create the free space before the scheduling, only if the mission is scheduled and scheduled later than free space start time
        // only if the space is big enough. If not, we just mark the extra space as unavailable
        if (startAt - scheduleSpace.start.getTime() > 30) {
          await tx.missionScheduleSpace.create({
            data: {
              start: scheduleSpace.start,
              end: new Date(startAt),
              duration: Math.floor(
                (startAt - scheduleSpace.start.getTime()) / 1000,
              ),
            },
          });
        }

        const scheduledEndAt = scheduledAt && scheduledAt + schedulingDuration;
        const endAt = scheduledEndAt ?? startAt + schedulingDuration;

        // Create the free space after the scheduling, only if the space is big enough. If not, we just mark the extra space as unavailable
        if (
          scheduleSpace.end === null /* If the scheduleSpace is the last one */
        ) {
          await tx.missionScheduleSpace.create({
            data: {
              start: new Date(endAt),
              end: null,
              duration: 2147483647,
            },
          });
        } else if (
          scheduleSpace.end.getTime() - endAt >
          30 /* If the scheduleSpace is not the last one */
        ) {
          await tx.missionScheduleSpace.create({
            data: {
              start: new Date(endAt),
              end: scheduleSpace.end,
              duration: Math.floor(
                (scheduleSpace.end.getTime() - endAt) / 1000,
              ),
            },
          });
        }

        // Set the time in mission schedule
        const initialOverheadStart = startAt;
        const missionStart = startAt + initialOverhead * 1000;
        const missionEnd = missionStart + missionDuration * 1000;
        const directOverheadEnd = missionEnd + directSchedulingOverhead * 1000;
        const indirectOverheadEnd =
          directOverheadEnd + indirectSchedulingOverhead * 1000;

        await tx.missionSchedule.create({
          data: {
            initialOverheadStart: new Date(initialOverheadStart),
            missionStart: new Date(missionStart),
            missionEnd: new Date(missionEnd),
            directOverheadEnd: new Date(directOverheadEnd),
            indirectOverheadEnd: new Date(indirectOverheadEnd),
            missionId: mission.id,
          },
        });

        // Time to set the mission's command scheduledAt time
        // initial = mission start time
        let currentTimeOffset = startAt + initialOverhead * 1000;

        await Promise.all(
          mission.commands.map(async (command) => {
            const commandScheduledAt = new Date(
              currentTimeOffset + command.timeOffset * 1000,
            );

            currentTimeOffset +=
              commandExecutionTime * 1000 +
              commandExecutionOverhead * 1000 +
              command.timeOffset * 1000;

            return await Promise.all([
              await tx.commandSchedule.create({
                data: {
                  start: commandScheduledAt,
                  end: new Date(currentTimeOffset - 500),
                  overheadEnd: new Date(currentTimeOffset),
                  commandId: command.id,
                },
              }),
              await tx.command.update({
                where: {
                  id: command.id,
                },
                data: {
                  scheduledAt: commandScheduledAt,
                },
              }),
            ]);
          }),
        );

        await tx.missionQueue.delete({
          where: {
            id: missionInQueue.id,
          },
        });

        await tx.mission.update({
          where: {
            id: mission.id,
          },
          data: {
            status: "scheduled",
          },
        });

        return true;
      }),
  );

let missionQueueProcessing = false;

export const processMissionQueue = async (): Promise<void> =>
  await tracer.startActiveSpan("processMissionQueue", async (span) => {
    if (missionQueueProcessing) {
      span.end();
      return;
    }
    missionQueueProcessing = true;
    let itemInQueue = true;
    while (itemInQueue) {
      itemInQueue = await processOneMissionInQueue();
    }
    missionQueueProcessing = false;
  });

const checkForScheduledMission = async (): Promise<void> =>
  await tracer.startActiveSpan("checkForScheduledMission", async (span) => {
    const now = Date.now();

    const expiredMission = await prisma.missionSchedule.findMany({
      where: {
        initialOverheadStart: {
          lt: new Date(now + 10 * 1000),
        },
        mission: {
          status: "queued",
        },
      },
    });

    await Promise.all(
      expiredMission.map(async (mission) => {
        await prisma.mission.update({
          where: {
            id: mission.missionId,
          },
          data: {
            status: "expired",
          },
        });
      }),
    );

    const scheduledMissions = await prisma.missionSchedule.findMany({
      where: {
        initialOverheadStart: {
          // try to recover mission not sent
          gte: new Date(now + 10 * 1000),
          // try to sent mission 1 mins before the mission start
          lt: new Date(now + 130 * 1000),
        },
        mission: {
          status: "scheduled",
        },
      },
      include: {
        mission: {
          include: {
            commands: true,
          },
        },
      },
    });

    if (scheduledMissions.length === 0) {
      span.end();
      return;
    }

    for (const scheduledMission of scheduledMissions) {
      await tracer.startActiveSpan("executeScheduledMission", async (span) => {
        await missionExecutionApi
          .scheduleMissionExecution({
            scheduledMission: {
              id: scheduledMission.mission.id,
              initialOverheadStart: scheduledMission.initialOverheadStart,
              commands: scheduledMission.mission.commands.map((command) => ({
                id: command.id,
                sequenceNumber: command.sequenceNumber,
                scheduledAt: command.scheduledAt!,
              })),
            },
          })
          .catch((err) => {
            logger.error(err, "Error when executing scheduled mission");
          });
        await prisma.mission.update({
          where: {
            id: scheduledMission.mission.id,
          },
          data: {
            status: "sent",
          },
        });
        span.end();
      });
    }
    span.end();
  });

setInterval(
  () =>
    checkForScheduledMission().catch((e) => {
      logger.warn(e, "Error when checking for scheduled mission");
    }),
  5 * 1000,
);
