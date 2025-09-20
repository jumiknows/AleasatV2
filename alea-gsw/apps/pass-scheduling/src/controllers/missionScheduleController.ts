import {
  type OperationsRequest,
  errorWrapperV2,
} from "@aleasat/pass-scheduling-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import missionScheduleService from "../services/missionScheduleService.js";

const tracer = opentelemetry.trace.getTracer("missionScheduleController");

const getScheduledMissions = async (
  req: OperationsRequest<"getScheduledMissions">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan(
    "getScheduledMissionsController",
    async (span) => {
      const result = await errorWrapperV2(
        req,
        res,
        missionScheduleService.getScheduledMissions(),
      );

      if (result !== undefined) {
        res.json(result);
      }

      span.end();
    },
  );

const getScheduledMission = async (
  req: OperationsRequest<"getScheduledMission">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan(
    "getScheduledMissionController",
    async (span) => {
      const result = await errorWrapperV2(
        req,
        res,
        missionScheduleService.getScheduledMission(req.params.missionId),
      );

      if (result !== undefined) {
        res.json(result);
      }

      span.end();
    },
  );

const getScheduleSpaces = async (
  req: OperationsRequest<"getScheduleSpaces">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getScheduleSpacesController", async (span) => {
    const result = await errorWrapperV2(
      req,
      res,
      missionScheduleService.getScheduleSpaces(),
    );

    if (result !== undefined) {
      res.json(result);
    }

    span.end();
  });

export default {
  getScheduledMissions,
  getScheduledMission,
  getScheduleSpaces,
};
