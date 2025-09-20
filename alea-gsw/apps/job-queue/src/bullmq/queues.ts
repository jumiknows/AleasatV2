import { Queue, Worker, WorkerType } from "@aleasat/schema-registry";
import { QueueEvents } from "bullmq";
import { BullMQOtel } from "bullmq-otel";
import { prisma } from "../repositories/prisma.js";
import { connection } from "../repositories/redis.js";
import logger from "../utils/logger.js";
import { defaultJobOptions } from "./config.js";

let gmatQueue: Queue<WorkerType.Gmat> | null = null;
let orbitPropQueue: Queue<WorkerType.OrbitPropagation> | null = null;
let imageGenQueue: Queue<WorkerType.ImageGeneration> | null = null;
let resultsWorker: Worker<WorkerType.Results> | null = null;

async function updateJobStatus(id: string, status: string) {
  try {
    const job = await prisma.job.update({
      where: { id },
      data: { status },
    });
    logger.info(`Job ${id} (${job.type}) status updated to ${status}`);
  } catch (e) {
    logger.error(e, "Failed to update job status in Prisma");
  }
}

async function updateJobResults(id: string, resultData: any) {
  try {
    const job = await prisma.job.update({
      where: { id },
      data: { resultData },
    });
    logger.info(`Job ${id} (${job.type}) results updated`);
  } catch (e) {
    logger.error(e, "Failed to update job results in Prisma");
  }
}

function initGmatQueue() {
  if (!gmatQueue) {
    gmatQueue = new Queue(WorkerType.Gmat, {
      connection,
      telemetry: new BullMQOtel("GMAT Queue"),
      ...defaultJobOptions,
    });
    gmatQueue.on("error", (e) => {
      logger.error(e, "Uncaught error in BullMQ");
    });
    const gmatQueueEvents = new QueueEvents("GMAT", { connection });
    gmatQueueEvents.on(
      "waiting",
      async ({ jobId }) => await updateJobStatus(jobId, "waiting"),
    );
    gmatQueueEvents.on(
      "active",
      async ({ jobId }) => await updateJobStatus(jobId, "active"),
    );
    gmatQueueEvents.on(
      "failed",
      async ({ jobId }) => await updateJobStatus(jobId, "failed"),
    );
  }
}

function initOrbitPropQueue() {
  if (!orbitPropQueue) {
    orbitPropQueue = new Queue(WorkerType.OrbitPropagation, {
      connection,
      telemetry: new BullMQOtel("Orbit Propagation Queue"),
      ...defaultJobOptions,
    });
    orbitPropQueue.on("error", (e) => {
      logger.error(e, "Uncaught error in BullMQ");
    });
    const orbitPropQueueEvents = new QueueEvents(WorkerType.OrbitPropagation, {
      connection,
    });
    orbitPropQueueEvents.on(
      "waiting",
      async ({ jobId }) => await updateJobStatus(jobId, "waiting"),
    );
    orbitPropQueueEvents.on(
      "active",
      async ({ jobId }) => await updateJobStatus(jobId, "active"),
    );
    orbitPropQueueEvents.on(
      "failed",
      async ({ jobId }) => await updateJobStatus(jobId, "failed"),
    );
  }
}

function initImageGenQueue() {
  if (!imageGenQueue) {
    imageGenQueue = new Queue(WorkerType.ImageGeneration, {
      connection,
      telemetry: new BullMQOtel("Image Generation Queue"),
      ...defaultJobOptions,
    });
    imageGenQueue.on("error", (e) => {
      logger.error(e, "Uncaught error in BullMQ");
    });
    const imageGenQueueEvents = new QueueEvents(WorkerType.ImageGeneration, {
      connection,
    });
    imageGenQueueEvents.on(
      "waiting",
      async ({ jobId }) => await updateJobStatus(jobId, "waiting"),
    );
    imageGenQueueEvents.on(
      "active",
      async ({ jobId }) => await updateJobStatus(jobId, "active"),
    );
    imageGenQueueEvents.on(
      "failed",
      async ({ jobId }) => await updateJobStatus(jobId, "failed"),
    );
  }
}

function initResultsWorker() {
  if (resultsWorker) return;

  resultsWorker = new Worker(
    WorkerType.Results,
    async (job) => {
      await updateJobStatus(job.id!, "completed");
      await updateJobResults(job.id!, job.data);
    },
    { connection, telemetry: new BullMQOtel("Result Worker") },
  );
}

export const Queues = {
  gmat(): Queue<WorkerType.Gmat> {
    if (!gmatQueue) {
      throw new Error(
        "GMAT queue is not initialized. Call initGmatQueue to initialize.",
      );
    }
    return gmatQueue;
  },
  orbitProp(): Queue<WorkerType.OrbitPropagation> {
    if (!orbitPropQueue) {
      throw new Error(
        "Orbit propagation queue is not initialized. Call initOrbitPropQueue to initialize.",
      );
    }
    return orbitPropQueue;
  },
  imageGen(): Queue<WorkerType.ImageGeneration> {
    if (!imageGenQueue) {
      throw new Error(
        "Image generation queue is not initialized. Call initImageGenQueue to initialize.",
      );
    }
    return imageGenQueue;
  },
  init() {
    initGmatQueue();
    initOrbitPropQueue();
    initImageGenQueue();
    initResultsWorker();
  },
};
