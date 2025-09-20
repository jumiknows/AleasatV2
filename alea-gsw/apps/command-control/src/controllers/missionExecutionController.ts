import {
  type OperationsRequest,
  errorWrapper,
} from "@aleasat/command-control-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import missionExecutionService from "../services/missionExecutionService.js";

const tracer = opentelemetry.trace.getTracer("missionExecutionController");

const scheduleMissionExecution = async (
  req: OperationsRequest<"scheduleMissionExecution">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan(
    "scheduleMissionExecutionController",
    async (span) => {
      const mission = await errorWrapper(
        req,
        res,
        missionExecutionService.scheduleMissionExecution(req.body),
      );

      if (mission) {
        res.json(mission);
      }

      span.end();
    },
  );

export default {
  scheduleMissionExecution,
};
