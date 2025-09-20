import type { ServiceReturnTypeV2 } from "@aleasat/job-queue-api";
import opentelemetry, { type Span } from "@opentelemetry/api";
import { prisma } from "../repositories/prisma.js";
import { type NonResultsWorkerType, getQueue } from "../utils/queueUtils.js";

const tracer = opentelemetry.trace.getTracer("jobQueueService");

const deleteJob = async (
  jobId: string,
): Promise<ServiceReturnTypeV2<"deleteJob">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"deleteJob">>
  >("deleteJobService", async (span) => {
    let isJobFound = true;
    try {
      await prisma.$transaction(async (tx) => {
        const job = await tx.job.findUnique({
          where: {
            id: jobId,
          },
        });
        if (!job) {
          isJobFound = false;
          throw new Error("Job not found in waiting queue");
        }

        const queue = getQueue(job.type as NonResultsWorkerType);

        const bullMQJob = await queue.getJob(jobId);
        try {
          await bullMQJob?.remove();
        } catch {
          isJobFound = false;
          throw new Error("Job not found in waiting queue");
        }

        await tx.job.delete({
          where: {
            id: jobId,
          },
        });
      });

      span.end();
      return { success: true, data: {} };
    } catch {
      if (!isJobFound) {
        span.end();
        return {
          success: false,
          error: { status: 404, title: "Job not found in waiting queue" },
        };
      }

      span.end();
      return {
        success: false,
        error: { status: 503, title: "Service unavailable" },
      };
    }
  });

export default { deleteJob };
