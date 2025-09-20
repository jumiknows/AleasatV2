import type {
  ServiceReturnType,
  operations,
} from "@aleasat/pass-scheduling-api";
import { getUserAccessTokenClaims, unauthorizedError } from "@aleasat/utils";
import opentelemetry from "@opentelemetry/api";
import { Prisma, prisma } from "../repositories/prisma.js";

const tracer = opentelemetry.trace.getTracer("groundStationService");

const addGroundStation = async ({
  groundStation,
}: {
  groundStation: operations["addGroundStation"]["requestBody"]["content"]["application/json"];
  accessToken: string | undefined;
}): Promise<ServiceReturnType<"addGroundStation">> =>
  await tracer.startActiveSpan("addGroundStation", async (span) => {
    const createGroundStationResult = await prisma.$transaction(async (tx) => {
      if (groundStation.ownerId) {
        const groundStationCount = await tx.groundStation.count({
          where: {
            ownerId: groundStation.ownerId,
          },
        });
        if (groundStationCount >= 5) return undefined;
      }
      return await tx.groundStation.create({
        data: {
          id: groundStation.id,
          ownerId: groundStation.ownerId,
          name: groundStation.name,
          description: groundStation.description,
          lat: groundStation.lat,
          lng: groundStation.lng,
          minElevation: groundStation.minElevation,
          // aleasat: groundStation.aleasat,
        },
      });
    });

    if (createGroundStationResult === undefined) {
      span.recordException({
        code: 400,
        message: "Over ground station limit",
      });
      span.end();
      return {
        error: {
          status: 400,
          title: "Over ground station limit",
        },
      };
    }

    span.end();
    return {
      data: createGroundStationResult,
    };
  });

const getGroundStations = async ({
  ownerId,
  accessToken,
}: {
  ownerId?: string;
  accessToken: string | undefined;
}): Promise<ServiceReturnType<"getGroundStations">> =>
  await tracer.startActiveSpan("getGroundStations", async (span) => {
    // user request
    if (accessToken) {
      const user = await getUserAccessTokenClaims(accessToken);
      if (!user || (!user.groups.includes("Staff") && user.id !== ownerId)) {
        span.recordException("Unauthorized");
        span.end();
        return unauthorizedError;
      }
    }

    const groundStations = await prisma.groundStation.findMany({
      where: { ownerId },
    });

    span.end();
    return {
      data: groundStations,
    };
  });

const getGroundStation = async ({
  groundStationId,
  accessToken,
}: {
  groundStationId: number;
  accessToken: string | undefined;
}): Promise<ServiceReturnType<"getGroundStation">> =>
  await tracer.startActiveSpan("getGroundStation", async (span) => {
    const groundStation = await prisma.groundStation.findUnique({
      where: { id: groundStationId },
    });

    if (groundStation === null) {
      span.recordException("Not Found");
      span.end();
      return {
        error: {
          title: "Not Found",
          status: 404,
        },
      };
    }

    if (accessToken && groundStation.ownerId !== null) {
      const user = await getUserAccessTokenClaims(accessToken);
      if (
        !user ||
        (!user.groups.includes("Staff") && user.id !== groundStation.ownerId)
      ) {
        span.recordException("Unauthorized");
        span.end();
        return unauthorizedError;
      }
    }

    span.end();
    return {
      data: groundStation,
    };
  });

const patchGroundStation = async ({
  groundStationId,
  groundStation,
  accessToken,
}: {
  groundStationId: number;
  groundStation: operations["patchGroundStation"]["requestBody"]["content"]["application/json"];
  accessToken: string | undefined;
}): Promise<ServiceReturnType<"patchGroundStation">> =>
  await tracer.startActiveSpan("patchGroundStation", async (span) => {
    try {
      const user = accessToken && (await getUserAccessTokenClaims(accessToken));
      if (!user) {
        span.recordException("Unauthorized");
        span.end();
        return unauthorizedError;
      }
      await prisma.groundStation.update({
        where: {
          id: groundStationId,
          ownerId: user.groups.includes("Staff") ? undefined : user.id,
        },
        data: { ...groundStation },
      });
    } catch (e) {
      if (
        e instanceof Prisma.PrismaClientKnownRequestError &&
        e.code === "P2025"
      ) {
        span.recordException("Not Found");
        span.end();
        return {
          error: {
            status: 404,
            title: "Not Found",
          },
        };
      }
      throw e;
    }

    span.end();
    return {
      data: {},
    };
  });

const deleteGroundStation = async ({
  groundStationId,
  accessToken,
}: {
  groundStationId: number;
  accessToken: string | undefined;
}): Promise<ServiceReturnType<"deleteGroundStation">> =>
  await tracer.startActiveSpan("deleteGroundStation", async (span) => {
    try {
      const user = accessToken && (await getUserAccessTokenClaims(accessToken));
      if (!user) {
        span.recordException("Unauthorized");
        span.end();
        return unauthorizedError;
      }
      await prisma.groundStation.delete({
        where: {
          id: groundStationId,
          ownerId: user?.groups.includes("Staff") ? undefined : user?.id,
        },
      });
    } catch (e) {
      if (
        e instanceof Prisma.PrismaClientKnownRequestError &&
        e.code === "P2025"
      ) {
        span.recordException("Not Found");
        span.end();
        return {
          error: {
            status: 404,
            title: "Not Found",
          },
        };
      }
      throw e;
    }

    span.end();
    return {
      data: {},
    };
  });

export default {
  addGroundStation,
  getGroundStations,
  getGroundStation,
  patchGroundStation,
  deleteGroundStation,
};
