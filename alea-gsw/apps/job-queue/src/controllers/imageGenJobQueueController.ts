import { type OperationsRequest, errorWrapperV3 } from "@aleasat/job-queue-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import imageGenJobQueueService from "../services/imageGenJobQueueService.js";

const tracer = opentelemetry.trace.getTracer("imageGenJobQueueController");

const createImageGenJob = async (
  req: OperationsRequest<"createImageJob">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("imageGenJobQueueController", async (span) => {
    const job = await errorWrapperV3(
      req,
      res,
      async () => await imageGenJobQueueService.createImageGenJob(req.body),
    );

    if (!job) {
      span.end();
      return;
    }

    res.status(200).json(job);
    span.end();
  });

const getImageGenJob = async (
  req: OperationsRequest<"getImageJob">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("imageGenJobQueueController", async (span) => {
    const job = await errorWrapperV3(
      req,
      res,
      async () =>
        await imageGenJobQueueService.getImageGenJob(req.params.jobId),
    );

    if (!job) {
      span.end();
      return;
    }

    res.status(200).json(job);
    span.end();
  });

export default { createImageGenJob, getImageGenJob };
