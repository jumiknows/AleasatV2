import type { ServiceReturnTypeV2, operations } from "@aleasat/job-queue-api";
import { WorkerType } from "@aleasat/schema-registry";
import opentelemetry, { type Span } from "@opentelemetry/api";
import { createJob, getJob } from "../utils/queueUtils.js";

const tracer = opentelemetry.trace.getTracer("gmatJobQueueService");

const createGMATJob = async (
  newJob: operations["createGMATJob"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"createGMATJob">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"createGMATJob">>
  >("createGMATJobService", async (span) => {
    try {
      const data = await createJob(WorkerType.Gmat, newJob);
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

const getGMATJob = async (
  jobId: string,
): Promise<ServiceReturnTypeV2<"getGMATJob">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getGMATJob">>
  >("getGMATJobService", async (span) => {
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
        operations["getGMATJob"]["responses"]["200"]["content"]["application/json"];
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

export default { createGMATJob, getGMATJob };
