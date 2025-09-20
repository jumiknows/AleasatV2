import { type OperationsRequest, errorWrapperV3 } from "@aleasat/job-queue-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import gmatJobQueueService from "../services/gmatJobQueueService.js";

const tracer = opentelemetry.trace.getTracer("gmatJobQueueController");

const createGMATJob = async (
  req: OperationsRequest<"createGMATJob">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("gmatJobQueueController", async (span) => {
    const job = await errorWrapperV3(
      req,
      res,
      async () => await gmatJobQueueService.createGMATJob(req.body),
    );

    if (!job) {
      span.end();
      return;
    }

    res.status(200).json(job);
    span.end();
  });

const getGMATJob = async (
  req: OperationsRequest<"getGMATJob">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("gmatJobQueueController", async (span) => {
    const job = await errorWrapperV3(
      req,
      res,
      async () => await gmatJobQueueService.getGMATJob(req.params.jobId),
    );

    if (!job) {
      span.end();
      return;
    }

    res.status(200).json(job);
    span.end();
  });

export default {
  createGMATJob,
  getGMATJob,
};
