import { prisma } from "../repositories/prisma.js";
import logger from "../utils/logger.js";

await prisma
  .$transaction(async (tx) => {
    const scheduleSpace = await tx.missionScheduleSpace.findFirst({});
    if (!scheduleSpace) {
      await tx.missionScheduleSpace.create({
        data: {
          start: new Date(0),
          end: null,
          duration: 2147483647,
        },
      });
    }
  })
  .catch((error) => {
    logger.trace(error, "Error initializing schedule space");
  });
