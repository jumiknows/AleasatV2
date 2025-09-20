import { randomUUID } from "crypto";
import {
  type Queue,
  type WorkerJobTypes,
  type WorkerResultTypes,
  WorkerType,
} from "@aleasat/schema-registry";
import type { Job } from "bullmq";
import { Queues } from "../bullmq/queues.js";
import { prisma } from "../repositories/prisma.js";

export type NonResultsWorkerType = Exclude<WorkerType, WorkerType.Results>;

type JobData<TWorkerType extends NonResultsWorkerType> = {
  id: string;
  requestData: WorkerJobTypes<TWorkerType>[TWorkerType];
  status: string;
  resultData?: WorkerResultTypes[TWorkerType];
};

const getQueue = <TWorkerType extends NonResultsWorkerType>(
  workerType: TWorkerType,
): Queue<WorkerType> => {
  switch (workerType) {
    case WorkerType.Gmat:
      return Queues.gmat();
    case WorkerType.OrbitPropagation:
      return Queues.orbitProp();
    case WorkerType.ImageGeneration:
      return Queues.imageGen();
    default:
      throw new Error(`Unsupported job type: ${workerType}`);
  }
};

const createJob = async <TWorkerType extends NonResultsWorkerType>(
  workerType: TWorkerType,
  newJob: WorkerJobTypes<TWorkerType>[TWorkerType],
): Promise<JobData<TWorkerType>> => {
  let job: Job | null = null;
  await prisma.$transaction(async (tx) => {
    const jobId = randomUUID();

    await tx.job.create({
      data: {
        id: jobId,
        name: workerType,
        type: workerType,
        status: "delayed",
        requestData: JSON.parse(JSON.stringify(newJob)), // for type checker
      },
    });

    const queue = getQueue(workerType);
    job = await queue.add(workerType, newJob, {
      jobId: jobId,
      delay: 10_000, // magic number to delay the job as a hack to avoid race conditions
    });
  });
  await job!.promote(); // job created in db, start the job
  return {
    id: job!.id!,
    requestData: newJob,
    status: await job!.getState(),
  };
};

const getJob = async <TWorkerType extends NonResultsWorkerType>(
  jobId: string,
): Promise<JobData<TWorkerType> | null> => {
  const job = await prisma.job.findUnique({
    where: {
      id: jobId,
    },
  });
  if (!job) {
    return null;
  }
  return {
    id: job.id,
    requestData:
      job.requestData as unknown as WorkerJobTypes<TWorkerType>[TWorkerType],
    status: job.status,
    resultData: job.resultData as WorkerResultTypes[TWorkerType],
  };
};

export { getQueue, createJob, getJob };
