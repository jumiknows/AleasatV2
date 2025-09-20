import "./instrumentation.js";

import crypto from "node:crypto";
import http from "node:http";
import type { SatelliteState } from "@aleasat/gmat-api";
import type { GMATResult } from "@aleasat/job-queue-api";
import { Queue, Worker } from "bullmq";
import { BullMQOtel } from "bullmq-otel";
import { ioredis } from "./repositories/ioredis.js";
import { s3Bucket } from "./repositories/s3.js";
import orbitPropagationService from "./services/orbitPropagationService.js";
import { uploadObject } from "./services/s3Service.js";
import logger from "./utils/logger.js";

// Add a health check endpoint
http
  .createServer((_req, res) => {
    res.statusCode = 200;
    res.write("OK");
    res.end();
  })
  .listen(process.env.PORT ?? 3000);

const worker = new Worker<SatelliteState>(
  "GMAT",
  async (job) => {
    const output = await orbitPropagationService.runGmatOrbitPropagation(
      job.data,
    );

    if (!output) {
      logger.error("Orbit propagation failed");
      throw new Error("Orbit propagation failed");
    }

    // upload to s3
    await uploadObject(job.id!, s3Bucket, output);

    // add completion to results queue
    const resultData: GMATResult = {
      result: {
        bucket: s3Bucket,
        key: `${job.id!}.json`,
      },
    };

    await resultsQueue.add("gmat", resultData, {
      jobId: job.id,
      removeOnComplete: true,
    });
  },
  {
    connection: ioredis,
    concurrency: 1,
    telemetry: new BullMQOtel("GMAT Worker"),
  },
);

const resultsQueue = new Queue("results", {
  connection: ioredis,
  telemetry: new BullMQOtel("Results Queue"),
});

worker.on("active", (job, prev) => {
  logger.info(`Job ${job.id} is now active; previous status was ${prev}`);
});

worker.on("closed", () => {
  logger.info("Worker closed");
});

worker.on("completed", (job, result) => {
  logger.info(`${job.id} has completed and returned ${result}`);
});

worker.on("drained", () => {
  logger.info("Queue drained");
});

worker.on("error", (err) => {
  logger.error(err, `Error: ${err.message}`);
});

worker.on("failed", (job, err) => {
  logger.error(err, `${job?.id} has failed with ${err.message}`);
});

worker.on("ioredis:close", () => {
  logger.warn("Ioredis connection closed");
});

worker.on("paused", () => {
  logger.info("Worker paused");
});

worker.on("progress", (job, progress) => {
  logger.info(`${job.id} has progressed ${JSON.stringify(progress, null, 2)}`);
});

worker.on("ready", () => {
  logger.info("Worker ready");
});

worker.on("resumed", () => {
  logger.info("Worker resumed");
});

worker.on("stalled", (jobId) => {
  logger.warn(`${jobId} stalled`);
});
