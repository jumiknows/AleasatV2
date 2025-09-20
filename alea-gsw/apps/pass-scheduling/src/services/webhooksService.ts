import crypto from "node:crypto";
import { trace } from "@opentelemetry/api";
import type { DefaultArgs } from "@prisma/client/pass-scheduling/runtime/library.js";
import { orbitPropagationApi } from "../repositories/backend.js";
import {
  type Prisma,
  type PrismaClient,
  prisma,
} from "../repositories/prisma.js";
import logger from "../utils/logger.js";

const tracer = trace.getTracer("webhooksService");

type UpsertPass =
  | {
      id: string;
      groundStationId: number;
      action: "add";
      predictedRiseSet: {
        rise: Date;
        riseX: number;
        riseY: number;
        riseZ: number;
        set: Date;
        setX: number;
        setY: number;
        setZ: number;
        stateElementId: string;
      };
    }
  | {
      id: string;
      oldPredictedRiseSetId: string;
      action: "update";
      predictedRiseSet: {
        rise: Date;
        riseX: number;
        riseY: number;
        riseZ: number;
        set: Date;
        setX: number;
        setY: number;
        setZ: number;
        stateElementId: string;
      };
    }
  | {
      id: string;
      action: "delete";
      predictedRiseSet: {
        rise: Date;
        set: Date;
      };
    };

export const updateGroundStationPasses = async ({
  tx,
  // storedPasses,
  // orbitPropagationPasses,
  groundStation,
  now,
  addNewPass = true,
}: {
  tx: Omit<
    PrismaClient<Prisma.PrismaClientOptions, never, DefaultArgs>,
    "$connect" | "$disconnect" | "$on" | "$transaction" | "$use" | "$extends"
  >;
  groundStation: {
    id: number;
    lat: number;
    lng: number;
    minElevation: number;
  };
  now: Date;
  addNewPass?: boolean;
}): Promise<UpsertPass[]> => {
  const upsertPasses: UpsertPass[] = [];

  const storedPasses = await tx.pass.findMany({
    where: {
      predictedRiseSet: { rise: { gte: now } },
      groundStationId: { equals: groundStation.id },
    },
    orderBy: { predictedRiseSet: { rise: "asc" } },
    select: {
      id: true,
      groundStationId: true,
      predictedRiseSet: true,
      previousPredictedRiseSet: true,
    },
  });

  const orbitPropagation = await orbitPropagationApi.findFullPasses({
    start: new Date(),
    latitude: groundStation.lat,
    longitude: groundStation.lng,
    minElevation: groundStation.minElevation,
  });

  const orbitPropagationPasses = orbitPropagation.passes
    .map((pass) => ({
      ...pass,
      rise: pass.rise.t,
      riseX: pass.rise.x,
      riseY: pass.rise.y,
      riseZ: pass.rise.z,
      set: pass.set.t,
      setX: pass.set.x,
      setY: pass.set.y,
      setZ: pass.set.z,
    }))
    .filter((pass) => pass.rise > now);

  storedPasses.forEach((pass) => {
    const matchPass = orbitPropagationPasses.find(
      (p) =>
        Math.abs(p.rise.getTime() - pass.predictedRiseSet.rise.getTime()) <=
          60 * 20 * 1000 ||
        Math.abs(p.set.getTime() - pass.predictedRiseSet.set.getTime()) <=
          60 * 20 * 1000,
    );
    if (matchPass) return;

    // pass in db, but not in op
    upsertPasses.push({
      id: pass.id,
      predictedRiseSet: pass.predictedRiseSet,
      action: "delete",
    });
  });

  orbitPropagationPasses.forEach((opPass) => {
    // try to match a pass in db that is predicted in orbit propagation service
    const matchPass = storedPasses.find(
      (dbPass) =>
        Math.abs(
          opPass.rise.getTime() - dbPass.predictedRiseSet.rise.getTime(),
        ) <=
          60 * 20 * 1000 ||
        Math.abs(
          opPass.set.getTime() - dbPass.predictedRiseSet.set.getTime(),
        ) <=
          60 * 20 * 1000,
    );

    // update the matched pass
    if (matchPass) {
      upsertPasses.push({
        id: matchPass.id,
        action: "update",
        oldPredictedRiseSetId: matchPass.predictedRiseSet.id,
        predictedRiseSet: {
          rise: opPass.rise,
          riseX: opPass.riseX,
          riseY: opPass.riseY,
          riseZ: opPass.riseZ,
          set: opPass.set,
          setX: opPass.setX,
          setY: opPass.setY,
          setZ: opPass.setZ,
          stateElementId: orbitPropagation.stateId,
        },
      });
      return;
    }

    // no match, add new pass if addNewPass is true
    if (addNewPass)
      upsertPasses.push({
        id: crypto.randomUUID(),
        action: "add",
        groundStationId: groundStation.id,
        predictedRiseSet: {
          rise: opPass.rise,
          riseX: opPass.riseX,
          riseY: opPass.riseY,
          riseZ: opPass.riseZ,
          set: opPass.set,
          setX: opPass.setX,
          setY: opPass.setY,
          setZ: opPass.setZ,
          stateElementId: orbitPropagation.stateId,
        },
      });
  });

  return upsertPasses;
};

const updateAleasatGroundStationsPasses = async (
  tx: Omit<
    PrismaClient<Prisma.PrismaClientOptions, never, DefaultArgs>,
    "$connect" | "$disconnect" | "$on" | "$transaction" | "$use" | "$extends"
  >,
  now: Date,
): Promise<UpsertPass[]> => {
  const upsertPasses: UpsertPass[] = [];

  const aleasatGroundStations = await tx.groundStation.findMany({
    where: { aleasat: true },
  });

  for (const groundStation of aleasatGroundStations) {
    await tracer.startActiveSpan(
      "updateAleasatGroundStationsPasses",
      async (span) => {
        span.setAttributes({
          "groundStation.name": groundStation.name,
          "groundStation.id": groundStation.id,
        });

        upsertPasses.push(
          ...(await updateGroundStationPasses({
            tx,
            now,
            groundStation,
          })),
        );
        span.end();
      },
    );
  }

  return upsertPasses;
};

const updateAROGroundStationsPasses = async (
  tx: Omit<
    PrismaClient<Prisma.PrismaClientOptions, never, DefaultArgs>,
    "$connect" | "$disconnect" | "$on" | "$transaction" | "$use" | "$extends"
  >,
  now: Date,
): Promise<UpsertPass[]> => {
  const upsertPasses: UpsertPass[] = [];

  const AROGroundStations = await tx.groundStation.findMany({
    where: { aleasat: false },
  });

  for (const groundStation of AROGroundStations) {
    await tracer.startActiveSpan(
      "updateAROGroundStationsPasses",
      async (span) => {
        span.setAttributes({
          "groundStation.name": groundStation.name,
          "groundStation.id": groundStation.id,
        });

        upsertPasses.push(
          ...(await updateGroundStationPasses({
            groundStation,
            now,
            tx,
            addNewPass: false,
          })),
        );
        span.end();
      },
    );
  }

  return upsertPasses;
};

const webhooksNewOrbitPropagation = async (): Promise<void> => {
  await tracer.startActiveSpan("webhooksNewOrbitPropagation", async (span) => {
    const now = new Date();

    const upsertPasses: UpsertPass[] = [];

    const currentStateElementId = await orbitPropagationApi
      .getGroundTrack({
        t: new Date(),
      })
      .then((res) => res.stateId);

    // acquire lock
    await prisma.$transaction(
      async (tx) => {
        // check if update to current
        const passUsingCurrentStateElementId = await tx.pass.findFirst({
          where: {
            predictedRiseSet: { stateElementId: currentStateElementId },
          },
        });

        // exit if current
        if (passUsingCurrentStateElementId !== null) return;

        upsertPasses.push(
          ...(await updateAleasatGroundStationsPasses(tx, now)),
        );
        upsertPasses.push(...(await updateAROGroundStationsPasses(tx, now)));

        const firstPassStored = await tx.pass.findFirst({
          where: { predictedRiseSet: { rise: { lt: now } } },
          orderBy: { predictedRiseSet: { rise: "desc" } },
          select: {
            id: true,
          },
          take: 1,
        });

        upsertPasses.sort(
          (a, b) =>
            a.predictedRiseSet.rise.getTime() -
            b.predictedRiseSet.rise.getTime(),
        );

        const upsertPassesWithNextPassId: Array<
          | (UpsertPass & {
              action: "add" | "update";
              previousPassId: string | undefined;
            })
          | (UpsertPass & { action: "delete" })
        > = [];

        (
          upsertPasses.filter((p) => p.action !== "delete") as Array<
            UpsertPass & { action: "add" | "update" }
          >
        ).forEach((p, i) =>
          upsertPassesWithNextPassId.push({
            ...p,
            previousPassId:
              i === 0
                ? firstPassStored
                  ? firstPassStored.id
                  : undefined
                : upsertPasses[i - 1].id,
          }),
        );

        upsertPassesWithNextPassId.push(
          ...(upsertPasses.filter((p) => p.action === "delete") as Array<
            UpsertPass & { action: "delete" }
          >),
        );

        for (const pass of upsertPassesWithNextPassId) {
          switch (pass.action) {
            case "add":
              await tx.pass.create({
                data: {
                  id: pass.id,
                  groundStation: {
                    connect: { id: pass.groundStationId },
                  },
                  predictedRiseSet: {
                    create: pass.predictedRiseSet,
                  },
                  previousPass:
                    pass.previousPassId !== undefined
                      ? {
                          connect: { id: pass.previousPassId },
                        }
                      : undefined,
                },
              });
              break;
            case "update":
              await tx.pass.update({
                where: { id: pass.id },
                data: {
                  predictedRiseSet: {
                    create: pass.predictedRiseSet,
                  },
                  previousPass:
                    pass.previousPassId !== undefined
                      ? {
                          connect: { id: pass.previousPassId },
                        }
                      : undefined,
                },
              });
              await tx.predictedRiseSet.update({
                where: { id: pass.oldPredictedRiseSetId },
                data: {
                  previousPassId: pass.id,
                },
              });
              break;
            case "delete":
              await tx.pass.delete({
                where: { id: pass.id },
              });
              break;
          }
        }
      },
      { isolationLevel: "Serializable", maxWait: 20000, timeout: 20000 },
    );

    span.end();
  });
};

if (process.env.NODE_ENV !== "testing")
  webhooksNewOrbitPropagation()
    .then(() => {
      logger.info("Init DB update complete");
    })
    .catch((e) => {
      logger.warn(e, "Try to init DB update but failed");
    });

export default { webhooksNewOrbitPropagation };
