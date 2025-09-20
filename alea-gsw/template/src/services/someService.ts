import { type ServiceReturnType, type operations } from "@aleasat/api-lib";
import { prisma, Prisma } from "../repositories/prisma.js";
import opentelemetry from "@opentelemetry/api";

const tracer = opentelemetry.trace.getTracer("imageRequestService");

const getSome = async (
  query: operations["getSome"]["parameters"]["query"],
): Promise<ServiceReturnType<"getSome">> =>
  await tracer.startActiveSpan("getSome", async (span) => {
    const some = await (async () => {
      return "some";
    })();

    span.end();
    return {
      data: { some },
    };
  });

export default {
  getSome,
};
