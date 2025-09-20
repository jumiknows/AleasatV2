import type { Response } from "express";
import { responseWithError, type OperationsRequest } from "@aleasat/api-lib";
import someService from "../services/someService.js";
import opentelemetry from "@opentelemetry/api";

const tracer = opentelemetry.trace.getTracer("someController");

const getSome = async (
  req: OperationsRequest<"getImageRequestPreview">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getSomeController", async (span) => {
    try {
      const somes = await someService.getSomePreview(req.query);
      if (somes.error) {
        responseWithError(req, res, somes.error);
        span.end();
        return;
      }
      res.json(somes.data);
    } catch (e) {
      span.recordException(e as Error);
      console.error(e);
      responseWithError(req, res, {
        title: "Internal Server Error",
        status: 500,
      });
    }
    span.end();
  });

export default {
  getSome,
};
