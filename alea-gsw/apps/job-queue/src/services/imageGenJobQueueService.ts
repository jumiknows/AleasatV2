import type { ServiceReturnTypeV2, operations } from "@aleasat/job-queue-api";
import { WorkerType } from "@aleasat/schema-registry";
import opentelemetry, { type Span } from "@opentelemetry/api";
import { createJob, getJob } from "../utils/queueUtils.js";

const tracer = opentelemetry.trace.getTracer("imageGenQueueService");

const createImageGenJob = async (
  newJob: operations["createImageJob"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"createImageJob">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"createImageJob">>
  >("createImageJobService", async (span) => {
    try {
      const data = await createJob(WorkerType.ImageGeneration, newJob);
      span.end();
      return {
        success: true,
        data,
      };
    } catch {
      span.end();
      return {
        success: false,
        error: { status: 503, title: "Service unavailable" },
      };
    }
  });

const getImageGenJob = async (
  jobId: string,
): Promise<ServiceReturnTypeV2<"getImageJob">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getImageJob">>
  >("getImageJobService", async (span) => {
    try {
      const job = await getJob(jobId);
      if (!job) {
        span.end();
        return {
          success: false,
          error: { status: 404, title: "Job not found" },
        };
      }
      type Response =
        operations["getImageJob"]["responses"]["200"]["content"]["application/json"];
      span.end();
      return {
        success: true,
        data: job as Response,
      };
    } catch {
      span.end();
      return {
        success: false,
        error: { status: 503, title: "Service unavailable" },
      };
    }
  });

export default { createImageGenJob, getImageGenJob };
