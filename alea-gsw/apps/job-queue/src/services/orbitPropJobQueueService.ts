import type { ServiceReturnTypeV2, operations } from "@aleasat/job-queue-api";
import { WorkerType } from "@aleasat/schema-registry";
import opentelemetry, { type Span } from "@opentelemetry/api";
import { createJob, getJob } from "../utils/queueUtils.js";

const tracer = opentelemetry.trace.getTracer("gmatJobQueueService");

const createOrbitPropJob = async (
  newJob: operations["createOrbitJob"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"createOrbitJob">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"createOrbitJob">>
  >("createOrbitPropJob", async (span) => {
    try {
      const data = await createJob(WorkerType.OrbitPropagation, newJob);
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

const getOrbitPropJob = async (
  jobId: string,
): Promise<ServiceReturnTypeV2<"getOrbitJob">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getOrbitJob">>
  >("getOrbitPropJobService", async (span) => {
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
        operations["getOrbitJob"]["responses"]["200"]["content"]["application/json"];
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

export default { createOrbitPropJob, getOrbitPropJob };
