import {
  type OperationsRequest,
  errorWrapperV2,
} from "@aleasat/pass-scheduling-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import missionQueueService from "../services/missionQueueService.js";

const tracer = opentelemetry.trace.getTracer("missionQueueController");

const queueMission = async (
  req: OperationsRequest<"queueMission">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("queueMissionController", async (span) => {
    const result = await errorWrapperV2(
      req,
      res,
      missionQueueService.queueMission(req.body),
    );

    if (result !== undefined) {
      res.sendStatus(200);
    }
    span.end();
  });

const getQueuedMissions = async (
  req: OperationsRequest<"getQueuedMissions">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getQueuedMissionsController", async (span) => {
    const result = await errorWrapperV2(
      req,
      res,
      missionQueueService.getQueuedMissions(),
    );

    if (result !== undefined) {
      res.json(result);
    }

    span.end();
  });

export default {
  queueMission,
  getQueuedMissions,
};
