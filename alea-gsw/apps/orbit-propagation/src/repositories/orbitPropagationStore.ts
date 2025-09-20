import { mkdirSync, readdirSync } from "fs";
import {
  LocationTrackApi,
  // type components,
} from "@aleasat/api-lib";
import type { components } from "@aleasat/gmat-api";
import type { SatelliteState } from "@aleasat/job-queue-api";
import { PassSchedulingApi } from "@aleasat/pass-scheduling-api";
import { trace } from "@opentelemetry/api";
import { readFile, writeFile } from "fs/promises";
import { type SatRec, sgp4, twoline2satrec } from "satellite.js";
import { getLatestIssOem } from "../apis/issOem.js";
import { jobApi } from "../apis/job-queue.js";
import logger from "../utils/logger.js";
import { getDayFromTime } from "../utils/orbitPropagationUtils.js";
import { prisma } from "./prisma.js";

const tracer = trace.getTracer("orbitPropagationRepository");

export const state = { ready: false };

const passSchedulingApi = new PassSchedulingApi();

const locationTrackApi = new LocationTrackApi();

export const dbKeys: string[] = [];
export const db: Record<string, SharedArrayBuffer> = {};
export let dbStateElement: components["schemas"]["StateElement"] = {
  type: "cartesian",
  x: 0,
  y: 0,
  z: 0,
  vx: 0,
  vy: 0,
  vz: 0,
};
export let dbStateId: string = "00000000-0000-0000-0000-000000000000";

/**
 * ISS (ZARYA)
1 25544U 98067A   23325.72039455  .00014199  00000+0  26082-3 0  9994
2 25544  51.6428 268.2674 0000705 331.4057  97.4213 15.49501531426224
 * 
 */

const initStore = async (): Promise<void> => {
  while (true) {
    const initStoreResult = await tracer.startActiveSpan(
      "initStore",
      async (span) => {
        try {
          mkdirSync("cache", { recursive: true });
          let orbitPropagation:
            | undefined
            | components["schemas"]["GmatOrbitPropagation"];
          const cacheDir = readdirSync("cache");

          if (cacheDir.length === 0) {
            orbitPropagation = await updateOrbitPropagation();
            span.end();
            if (!orbitPropagation) return true;
          }

          if (!orbitPropagation)
            orbitPropagation = (await readFile(
              "cache/cache.json",
              "utf-8",
            ).then((file) =>
              JSON.parse(file),
            )) as components["schemas"]["GmatOrbitPropagation"];

          loadOrbitPropagation(orbitPropagation);
          passSchedulingApi.webhooksNewOrbitPropagation().catch(() => {
            logger.warn(
              "trying to notify pass scheduler in init load, but failed",
            );
          });

          logger.info("init load complete");
          span.end();
          return true;
        } catch (error) {
          logger.warn(error, "init load failed");
        }
        span.end();
        return false;
      },
    );
    if (initStoreResult) return;
    await new Promise((resolve) => setTimeout(resolve, 10000));
  }
};

const runGmatOrbitPropagation = async (satelliteState: SatelliteState) =>
  jobApi
    .createGMATJob({
      satelliteState,
    })
    .catch((e) => {
      logger.warn(e, "Failed to create GMAT job");
    });

const runOrbitPropagation = async (
  satrec: SatRec,
  stateId: string,
): Promise<undefined | components["schemas"]["GmatOrbitPropagation"]> => {
  return await tracer.startActiveSpan("runOrbitPropagation", async (span) => {
    const { position, velocity } = sgp4(satrec, 0);
    if (typeof position === "boolean" || typeof velocity === "boolean") {
      logger.warn("Failed to propagate orbit");
      span.end();
      return undefined;
    }
    const orbitPropagation: components["schemas"]["GmatOrbitPropagation"] = {
      stateElement: {
        type: "cartesian",
        vx: velocity.x,
        vy: velocity.y,
        vz: velocity.z,
        ...position,
      },
      stateId,
      result: [],
    };

    const yearStart = new Date(`1/1/${satrec.epochyr} 0:0:0 Z`);
    const yearStartMS = yearStart.getTime();

    const epoch = Math.floor(yearStartMS + (satrec.epochdays - 1) * 86400000);
    const oneMinute = 1000 * 60;
    const threeWeeks = 60 * 24 * 21;

    for (let i = 0; i < threeWeeks; i++) {
      const { position } = sgp4(satrec, i);
      if (typeof position === "boolean") {
        span.end();
        return undefined;
      }
      orbitPropagation.result.push({
        utcGregorian: epoch + i * oneMinute,
        x: position.x,
        y: position.y,
        z: position.z,
      });
    }

    await writeFile("cache/cache.json", JSON.stringify(orbitPropagation));

    span.end();
    return orbitPropagation;
  });
};

export const loadOrbitPropagation = (
  gmatData: components["schemas"]["GmatOrbitPropagation"],
): void => {
  tracer.startActiveSpan("loadOrbitPropagation", (span) => {
    const dbs: Record<
      string,
      components["schemas"]["GmatOrbitPropagation"]["result"]
    > = {};
    const dayInMs = 1000 * 60 * 60 * 24;
    const start = new Date(gmatData.result[0].utcGregorian).setUTCHours(
      0,
      0,
      0,
      0,
    );
    const days: string[] = [];
    for (let i = 0; i < 64; i++) {
      days.push(getDayFromTime(start + i * dayInMs));
    }
    gmatData.result.forEach((step) => {
      const day = days[((step.utcGregorian - start) / dayInMs) >> 0];
      if (!dbs[day]) dbs[day] = [];
      dbs[day].push(step);
    });
    const oldDbKeys = [...dbKeys];
    dbKeys.length = 0;
    dbKeys.push(...Object.keys(dbs));
    Object.keys(dbs).forEach((key) => {
      db[key] = new SharedArrayBuffer(
        Float64Array.BYTES_PER_ELEMENT * 4 * dbs[key].length,
      );
      const array = new Float64Array(db[key]);
      for (let i = 0; i < dbs[key].length; i++) {
        const step = dbs[key][i];
        const offset = i * 4;
        array[offset] = step.utcGregorian;
        array[offset + 1] = step.x;
        array[offset + 2] = step.y;
        array[offset + 3] = step.z;
      }
    });
    oldDbKeys.forEach((key) => {
      // eslint-disable-next-line @typescript-eslint/no-dynamic-delete
      if (!dbKeys.includes(key)) delete db[key];
    });
    dbStateElement = gmatData.stateElement;
    dbStateId = gmatData.stateId;
    state.ready = true;
    span.end();
  });
};

const updateOrbitPropagation = async (): Promise<
  undefined | components["schemas"]["GmatOrbitPropagation"]
> =>
  await tracer.startActiveSpan("updateOrbitPropagation", async (span) => {
    const tle = await fetch(
      "https://celestrak.org/NORAD/elements/gp.php?CATNR=25544&FORMAT=tle",
    )
      .then(async (res) => await res.text())
      .then((text) => text.split("\n"));
    try {
      twoline2satrec(tle[1], tle[2]);
    } catch (e) {
      logger.warn(e, "Failed to parse TLE");
      span.end();
      return undefined;
    }

    const satrec = twoline2satrec(tle[1], tle[2]);
    const positionAndVelocity = sgp4(satrec, 0);

    if (
      typeof positionAndVelocity.velocity === "boolean" ||
      typeof positionAndVelocity.position === "boolean"
    ) {
      span.end();
      return undefined;
    }

    const cartesian = {
      type: "cartesian",
      ...positionAndVelocity.position,
      vx: positionAndVelocity.velocity.x,
      vy: positionAndVelocity.velocity.y,
      vz: positionAndVelocity.velocity.z,
    } satisfies components["schemas"]["CartesianState"];

    const yearStart = new Date(`1/1/${satrec.epochyr} 0:0:0 Z`);
    const yearStartMS = yearStart.getTime();

    const epoch = Math.floor(yearStartMS + (satrec.epochdays - 1) * 86400000);

    const { id: stateId } = await prisma.stateElement
      .upsert({
        where: {
          epoch_stateElement: {
            stateElement: cartesian,
            epoch,
          },
        },
        create: {
          stateElement: cartesian,
          epoch,
          tle: tle.join("\n"),
        },
        update: {},
        select: {
          id: true,
        },
      })
      .catch((e) => {
        logger.warn(e, "Failed to save state element");
        return {
          id: "00000000-0000-0000-0000-000000000000",
        };
      });

    const orbitPropagation = await runOrbitPropagation(satrec, stateId);
    if (orbitPropagation) loadOrbitPropagation(orbitPropagation);

    const issOem = await getLatestIssOem().catch((e) => {
      logger.warn(e, "Failed to get latest ISS OEM");
      return null;
    });
    // As load generator
    if (issOem)
      runGmatOrbitPropagation({
        epochFormat: "UTCGregorian",
        epoch:
          // To this format: 09 Aug 2024 03:45:21.893
          // 09 Aug 2024
          new Date(issOem.stateVector.epoch).toUTCString().slice(5, 17) +
          // 03:45:21.893
          new Date(issOem.stateVector.epoch)
            .toISOString()
            .slice(11, -1),
        coordinateSystem: "EarthMJ2000Eq",
        stateElement: {
          type: "cartesian",
          ...issOem.stateVector,
        },
        stateId,
      });

    span.end();
    return orbitPropagation;
  });

export const updateStore = async (): Promise<void> => {
  await tracer.startActiveSpan("updateStore", async (span) => {
    passSchedulingApi.webhooksNewOrbitPropagation().catch((e) => {
      logger.warn(e, "trying to notify pass scheduler in update, but failed");
    });
    locationTrackApi.webhookUpdateLocationTrack().catch((e) => {
      logger.warn(e, "trying to notify command request in update, but failed");
    });
    span.end();
  });
};

if (process.env.NODE_ENV !== "testing") {
  await initStore();
  setInterval(updateOrbitPropagation, 1000 * 60 * 60 * 2);
}
