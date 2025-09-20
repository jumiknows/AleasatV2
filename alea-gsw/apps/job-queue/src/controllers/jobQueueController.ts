import { type OperationsRequest, errorWrapperV3 } from "@aleasat/job-queue-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import jobQueueService from "../services/jobQueueService.js";

const tracer = opentelemetry.trace.getTracer("gmatJobQueueController");

const deleteJob = async (
  req: OperationsRequest<"deleteJob">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("jobQueueController", async (span) => {
    await errorWrapperV3(
      req,
      res,
      async () => await jobQueueService.deleteJob(req.params.jobId),
    );

    res.status(200).json();
    span.end();
  });

export default {
  deleteJob,
};
