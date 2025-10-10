import type {
  PropagationParams,
  S3Result,
  SatelliteState,
} from "@aleasat/job-queue-api";
import {
  Worker as BUllMQWorker,
  Queue as BullMQQueue,
  type Processor,
  type QueueOptions,
  type RedisConnection,
  type WorkerOptions,
} from "bullmq";

export enum WorkerType {
  Gmat = "GMAT",
  OrbitPropagation = "ORBIT_PROPAGATION",
  Results = "results", // Special queue https://docs.bullmq.io/guide/returning-job-data#using-a-results-queue
}

export type GMATResultType = {
  result: S3Result;
};

export type OrbitPropagationResultType = {
  result: number[];
};

export type WorkerJobTypes<T extends WorkerType> = {
  [WorkerType.Gmat]: SatelliteState;
  [WorkerType.OrbitPropagation]: PropagationParams;
  [WorkerType.Results]: WorkerResultTypes[T];
};

export type WorkerResultTypes = {
  [WorkerType.Gmat]: GMATResultType;
  [WorkerType.OrbitPropagation]: OrbitPropagationResultType;
  [WorkerType.Results]: null; // Special queue, no return value
};

/**
 * Represents a BullMQ queue with support for stricter typing.
 * @template TWorkerType The type of worker this queue adds to (e.g., Gmat, OrbitPropagation, Result).
 * @template ResultsQueueWorkerType If WorkerType is 'Workers.Result', this type parameter specifies
 *           what data type must be added into the 'Result' queue. For other WorkerType values, this
 *           parameter has no effect. While it has the default value of 'Workers.Result', this is a
 *           dummy value; If WorkerType is 'Workers.Result', you must specify the original worker type
 *           that this queue will return results for.
 */
export class Queue<
  TWorkerType extends WorkerType,
  ResultsQueueWorkerType extends WorkerType = WorkerType.Results, // Dummy value, specify this for the results queue instance
> extends BullMQQueue<
  WorkerJobTypes<ResultsQueueWorkerType>[TWorkerType],
  void
> {
  // biome-ignore lint/complexity/noUselessConstructor: provides typings, not useless
  constructor(
    name: TWorkerType,
    opts?: QueueOptions,
    connection?: typeof RedisConnection,
  ) {
    super(name, opts, connection);
  }
}

/**
 * Represents a BullMQ worker with support for stricter typing.
 * @template TWorkerType The type of worker (e.g., Gmat, OrbitPropagation, Result).
 */
export class Worker<TWorkerType extends WorkerType> extends BUllMQWorker<
  WorkerJobTypes<WorkerType>[TWorkerType],
  void
> {
  // biome-ignore lint/complexity/noUselessConstructor: provides typings, not useless
  constructor(
    name: TWorkerType,
    processor?:
      | string
      | URL
      | null
      | Processor<WorkerJobTypes<WorkerType>[TWorkerType], void>,
    opts?: WorkerOptions,
    connection?: typeof RedisConnection,
  ) {
    super(name, processor, opts, connection);
  }
}
