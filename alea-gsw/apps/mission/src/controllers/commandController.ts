import { type OperationsRequest, errorWrapper } from "@aleasat/mission-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import commandService from "../services/commandService.js";

const tracer = opentelemetry.trace.getTracer("commandController");

const patchCommand = async (
  req: OperationsRequest<"patchCommand">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const missions = await errorWrapper(
      req,
      res,
      commandService.patchCommand(
        req.params.missionId,
        req.params.commandId,
        req.body,
      ),
    );

    if (missions) {
      res.json(missions);
    }

    span.end();
  });

export default {
  patchCommand,
};
