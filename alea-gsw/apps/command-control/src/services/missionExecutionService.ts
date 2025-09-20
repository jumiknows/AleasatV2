import type {
  ServiceReturnTypeV2,
  operations,
} from "@aleasat/command-control-api";
import {
  deserializeArgsResp,
  getCommandSpec,
  serializeArgsResp,
} from "@aleasat/command-spec";
import type { ReadMission as Mission } from "@aleasat/mission-api";
import { obcTimeImmediate } from "@aleasat/mops-client-grpc-node";
import type {
  fwInfoCommandOBCResponse,
  pingCommandOBCResponse,
  resetCommandOBCResponse,
} from "@aleasat/mops-client-grpc-node/obc";
import opentelemetry from "@opentelemetry/api";
import { missionApi } from "../apis/mission.js";
import { obc } from "../apis/obc.js";
import logger from "../utils/logger.js";

const tracer = opentelemetry.trace.getTracer("missionExecutionService");

const executeMission = async (
  scheduledMission: operations["scheduleMissionExecution"]["requestBody"]["content"]["application/json"],
  mission: Mission,
): Promise<Mission | undefined> =>
  await tracer.startActiveSpan("executeMission", async (span) => {
    const [fwInfo, fwInfoErr] = (await obc
      .fw_info({
        timeout: 1,
        dateTime: obcTimeImmediate,
      })
      .then((res) => [res, undefined])
      .catch((err) => {
        logger.error(err, "Failed to get OBC firmware info");
        return [undefined, err];
      })) as [fwInfoCommandOBCResponse, undefined] | [undefined, string];

    if (!fwInfo) {
      await missionApi.patchMission({
        missionId: mission.id,
        updateMission: {
          status: "error",
          error: "Failed to get OBC firmware info: " + fwInfoErr,
        },
      });
      span.end();
      return undefined;
    }

    // We use commit hash as fw version for now
    if (mission.fwVersion !== fwInfo.data?.fwHash.toString(16)) {
      const error = `Mission ${mission.id} has firmware version ${
        mission.fwVersion
      } but OBC has firmware version ${fwInfo.data?.fwHash.toString(16)}`;
      logger.error(error);
      await missionApi.patchMission({
        missionId: mission.id,
        updateMission: {
          status: "error",
          error,
        },
      });
      span.end();
      return undefined;
    }

    const commandSpec = getCommandSpec({
      // We use commit hash as fw version for now
      commitHash: mission.fwVersion,
    })!;

    mission.commands.sort((a, b) => a.sequenceNumber - b.sequenceNumber);
    scheduledMission.commands.sort(
      (a, b) => a.sequenceNumber - b.sequenceNumber,
    );

    for (const [index, command] of mission.commands.entries()) {
      const spec = commandSpec.transformedSpec[command.commandName];

      const commandNameInLowerCase =
        command.commandName.toLowerCase() as keyof typeof obc & string;

      const commandArgs = {
        ...deserializeArgsResp({
          argsRespSpec: spec.args,
          argsResp: command.arguments,
        }),
        timeout: 1,
        dateTime: obcTimeImmediate,
      } as Parameters<(typeof obc)[typeof commandNameInLowerCase]>[number];

      const scheduleAt = scheduledMission.commands[index].scheduledAt;

      await tracer.startActiveSpan("Wait to run command", async (span) => {
        await new Promise((resolve) =>
          setTimeout(resolve, new Date(scheduleAt).getTime() - Date.now()),
        );
        span.end();
      });

      command.sentAt = new Date();

      // @ts-expect-error its right
      const res = await obc[commandNameInLowerCase](commandArgs)
        .then((res) => ({
          status: "fulfilled" as "fulfilled",
          value: res as resetCommandOBCResponse | pingCommandOBCResponse,
        }))
        .catch((err) => ({
          status: "rejected" as "rejected",
          reason: err,
        }));

      if (res.status === "rejected") {
        logger.error(
          "Running mission",
          mission.id,
          "command",
          command.id,
          "failed",
          res.reason,
        );
        command.error = true;
        command.ranAt = command.sentAt;
        mission.error = "Failed to run command: " + res.reason;

        await Promise.all([
          missionApi.patchCommand({
            missionId: mission.id,
            commandId: command.id,
            updateCommand: {
              sentAt: command.sentAt,
              ranAt: command.sentAt,
              error: true,
            },
          }),
          missionApi.patchMission({
            missionId: mission.id,
            updateMission: {
              status: "error",
              error: "Failed to run command: " + res.reason,
            },
          }),
        ]);

        span.end();
        return mission;
      }

      const responses =
        "data" in res.value && res.value.data
          ? serializeArgsResp({
              argsRespSpec: spec.resp,
              // @ts-expect-error {} is assignable to Record<string, unknown>
              argsResp: res.value.data,
            })
          : [];

      command.ranAt =
        "dataHeader" in res.value
          ? res.value.dataHeader!.execDatetime!
          : command.sentAt;
      command.responses = responses;

      await missionApi.patchCommand({
        missionId: mission.id,
        commandId: command.id,
        updateCommand: {
          sentAt: new Date(command.sentAt),
          ranAt: new Date(command.sentAt),
          responses: command.responses,
        },
      });
    }

    await missionApi.patchMission({
      missionId: mission.id,
      updateMission: {
        status: "sent",
      },
    });
    span.end();
    return mission;
  });

const scheduleMissionExecution = async (
  scheduledMission: operations["scheduleMissionExecution"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"scheduleMissionExecution">> => {
  const mission = await missionApi.getMission({
    missionId: scheduledMission.id,
  });

  setTimeout(
    async () =>
      await executeMission(scheduledMission, mission).catch((err) =>
        logger.error(err, "Failed to execute mission"),
      ),
    new Date(scheduledMission.initialOverheadStart).getTime() - Date.now(),
  );

  return {
    success: true,
    data: {},
  };
};

export default {
  executeMission,
  scheduleMissionExecution,
};
