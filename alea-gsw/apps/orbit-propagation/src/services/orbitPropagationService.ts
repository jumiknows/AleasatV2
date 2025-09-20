import path, { dirname } from "node:path";
import { fileURLToPath } from "node:url";
import type { operations } from "@aleasat/orbit-propagation-api";
import { context, propagation, trace } from "@opentelemetry/api";
import workerpool from "workerpool";
import {
  db,
  dbKeys,
  dbStateElement,
  dbStateId,
} from "../repositories/orbitPropagationStore.js";
import type {
  workerFindFullPasses,
  workerFindPasses,
} from "../utils/orbitPropagationServiceWorkerFunction.js";
import { getDayFromTime } from "../utils/orbitPropagationUtils.js";

const tracer = trace.getTracer("orbitPropagationService");

export const orbitPropagationServiceWorkerPool: workerpool.WorkerPool =
  workerpool.pool(
    path.join(
      dirname(fileURLToPath(import.meta.url)),
      "../../dist/utils/orbitPropagationServiceWorker.js",
    ),
    { workerType: "thread" },
  );

const getGroundTrack = async (
  params: operations["getGroundTrack"]["parameters"]["query"],
): Promise<
  operations["getGroundTrack"]["responses"]["200"]["content"]["application/json"]
> => {
  return await tracer.startActiveSpan("getGroundTrack", async (span) => {
    const day = getDayFromTime(params.t);

    const contextCarrier = {};
    propagation.inject(context.active(), contextCarrier);

    const groundTrack = await orbitPropagationServiceWorkerPool.exec(
      "workerGetGroundTrack",
      [db[day], params.t, contextCarrier],
    );

    span.end();
    return {
      stateId: dbStateId,
      stateElement: dbStateElement,
      ground_track: groundTrack,
    };
  });
};

const findPasses = async (
  params: operations["findPasses"]["parameters"]["query"],
): Promise<
  operations["findPasses"]["responses"]["200"]["content"]["application/json"]
> => {
  return await tracer.startActiveSpan("findPasses", async (span) => {
    const startTime = new Date(params.start).getTime();
    const endTime = new Date(params.end).getTime();
    const startDay = getDayFromTime(params.start);
    const endDay = getDayFromTime(params.end);
    const maximas = (
      await Promise.all(
        dbKeys.slice(dbKeys.indexOf(startDay), dbKeys.indexOf(endDay) + 1).map(
          async (key) =>
            await tracer.startActiveSpan(
              "findPassesForEachDay",
              async (span) => {
                const contextCarrier = {};
                propagation.inject(context.active(), contextCarrier);

                const passes = await orbitPropagationServiceWorkerPool.exec<
                  typeof workerFindPasses
                >("workerFindPasses", [
                  db[key],
                  params.latitude,
                  params.longitude,
                  params.minElevation,
                  contextCarrier,
                ]);

                span.end();
                return passes;
              },
            ),
        ),
      )
    ).flat();

    const result = {
      latitude: params.latitude,
      longitude: params.longitude,
      stateId: dbStateId,
      stateElement: dbStateElement,
      passes: maximas
        .filter((step) => step.t > startTime && step.t < endTime)
        .map((step, id) => ({
          id,
          rise: {
            t: new Date(step.t).toISOString(),
            x: step.x,
            y: step.y,
            z: step.z,
          },
          set: {
            t: new Date(step.t).toISOString(),
            x: step.x,
            y: step.y,
            z: step.z,
          },
          culmination: {
            ...step,
            t: new Date(step.t).toISOString(),
          },
        })),
    };
    span.end();
    return result;
  });
};

const findFullPasses = async (
  params: operations["findFullPasses"]["parameters"]["query"],
): Promise<
  operations["findFullPasses"]["responses"]["200"]["content"]["application/json"]
> => {
  return await tracer.startActiveSpan("findFullPasses", async (span) => {
    const startDay = getDayFromTime(params.start);
    const fullPasses = (
      await Promise.all(
        dbKeys.slice(dbKeys.indexOf(startDay)).map(
          async (key) =>
            await tracer.startActiveSpan(
              "findFullPassesForEachDay",
              async (span) => {
                const contextCarrier = {};
                propagation.inject(context.active(), contextCarrier);

                const passes = await orbitPropagationServiceWorkerPool.exec<
                  typeof workerFindPasses
                >("workerFindPasses", [
                  db[key],
                  params.latitude,
                  params.longitude,
                  params.minElevation,
                  contextCarrier,
                ]);

                const fullPasses = await orbitPropagationServiceWorkerPool.exec<
                  typeof workerFindFullPasses
                >("workerFindFullPasses", [
                  db,
                  passes,
                  params.latitude,
                  params.longitude,
                  params.minElevation,
                  contextCarrier,
                ]);

                span.end();
                return fullPasses;
              },
            ),
        ),
      )
    ).flat();

    const result = {
      latitude: params.latitude,
      longitude: params.longitude,
      stateId: dbStateId,
      stateElement: dbStateElement,
      passes: fullPasses
        .filter((step) => step.t >= new Date(params.start).getTime())
        .map((step, id) => ({
          id,
          rise: {
            ...step.rise,
            t: new Date(step.rise.t).toISOString(),
          },
          set: {
            ...step.set,
            t: new Date(step.set.t).toISOString(),
          },
          culmination: {
            ...step,
            t: new Date(step.t).toISOString(),
          },
        })),
    };
    span.end();
    return result;
  });
};

export default { getGroundTrack, findPasses, findFullPasses };
