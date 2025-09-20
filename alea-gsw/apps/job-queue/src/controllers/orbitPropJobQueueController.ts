import { type OperationsRequest, errorWrapperV3 } from "@aleasat/job-queue-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import orbitPropJobQueueService from "../services/orbitPropJobQueueService.js";

const tracer = opentelemetry.trace.getTracer("orbitPropJobQueueController");

const createOrbitPropJob = async (
  req: OperationsRequest<"createOrbitJob">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("orbitPropJobQueueController", async (span) => {
    const job = await errorWrapperV3(
      req,
      res,
      async () => await orbitPropJobQueueService.createOrbitPropJob(req.body),
    );

    if (!job) {
      span.end();
      return;
    }

    res.status(200).json(job);
    span.end();
  });

const getOrbitPropJob = async (
  req: OperationsRequest<"getOrbitJob">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("orbitPropJobQueueController", async (span) => {
    const job = await errorWrapperV3(
      req,
      res,
      async () =>
        await orbitPropJobQueueService.getOrbitPropJob(req.params.jobId),
    );

    if (!job) {
      span.end();
      return;
    }

    res.status(200).json(job);
    span.end();
  });

export default {
  createOrbitPropJob,
  getOrbitPropJob,
};
