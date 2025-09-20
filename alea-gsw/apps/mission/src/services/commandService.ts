import type { ServiceReturnType, operations } from "@aleasat/mission-api";
import opentelemetry from "@opentelemetry/api";
import { prisma } from "../repositories/prisma.js";

const tracer = opentelemetry.trace.getTracer("commandService");

const patchCommand = async (
  missionId: string,
  commandId: string,
  patch: operations["patchCommand"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnType<"patchCommand">> =>
  await tracer.startActiveSpan("patchCommand", async (span) => {
    const patchedCommand = await prisma.command.update({
      where: {
        id: commandId,
        AND: {
          missionId,
        },
      },
      data: {
        sentAt: patch.sentAt ? new Date(patch.sentAt) : null,
        ranAt: patch.ranAt ? new Date(patch.ranAt) : null,
        responses: patch.responses,
        error: patch.error,
      },
    });

    span.end();
    if (!patchedCommand)
      return { error: { status: 404, title: "Command not found" } };

    return {
      data: {
        ...patchedCommand,
        sentAt: patchedCommand.sentAt
          ? patchedCommand.sentAt.toISOString()
          : null,
        ranAt: patchedCommand.ranAt ? patchedCommand.ranAt.toISOString() : null,
      },
    };
  });
export default {
  patchCommand,
};
