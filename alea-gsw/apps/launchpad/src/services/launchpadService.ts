import crypto from "crypto";
import stream from "stream";
import type { ServiceReturnTypeV2, operations } from "@aleasat/launchpad-api";
import { getUserAccessTokenClaims } from "@aleasat/utils";
import opentelemetry, { type Span } from "@opentelemetry/api";
import k8s from "../apis/k8s.js";
import constants from "../constants.js";
import { prisma } from "../repositories/prisma.js";
import { retryUntilCondition, retryUntilSuccess } from "../utils/retryUntil.js";

const tracer = opentelemetry.trace.getTracer("launchpadService");

const getLaunchpads = async (): Promise<ServiceReturnTypeV2<"getLaunchpads">> =>
  await tracer.startActiveSpan("getLaunchpadsService", async (span) => {
    const launchpads = await prisma.launchpad.findMany({
      include: {
        userSession: true,
      },
    });

    span.end();
    return {
      success: true as true,
      data: launchpads.map((launchpad) => ({
        ...launchpad,
        userSessionId: launchpad.userSession?.id ?? null,
      })),
    };
  });

const createLaunchpad = async (
  newLaunchpad: operations["createLaunchpad"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"createLaunchpad">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"createLaunchpad">>
  >("createLaunchpadUserSessionService", async (span) => {
    const launchpad = await prisma.launchpad.create({
      data: newLaunchpad,
      include: { userSession: true },
    });
    if (!launchpad) {
      span.end();
      return {
        success: false,
        error: {
          status: 500,
          title: "Launchpad not created",
        },
      };
    }

    span.end();
    return {
      success: true,
      data: {
        ...launchpad,
        userSessionId: launchpad.userSession?.id ?? null,
      },
    };
  });

const getLaunchpad = async (
  launchpadId: string,
): Promise<ServiceReturnTypeV2<"getLaunchpad">> =>
  await tracer.startActiveSpan("getLaunchpadService", async (span) => {
    const launchpad = await prisma.launchpad.findUnique({
      where: {
        id: launchpadId,
      },
      include: {
        userSession: true,
      },
    });

    if (!launchpad) {
      span.end();
      return {
        success: false as false,
        error: {
          status: 404,
          title: "Launchpad not found",
        },
      };
    }

    span.end();
    return {
      success: true as true,
      data: {
        ...launchpad,
        userSessionId: launchpad.userSession?.id ?? null,
      },
    };
  });

const patchLaunchpad = async (
  launchpadId: string,
  patch: operations["patchLaunchpad"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"patchLaunchpad">> =>
  await tracer.startActiveSpan("updateLaunchpadService", async (span) => {
    const launchpad = await prisma.launchpad.update({
      where: {
        id: launchpadId,
      },
      data: patch,
      include: {
        userSession: true,
      },
    });

    if (!launchpad) {
      span.end();
      return {
        success: false as false,
        error: {
          status: 404,
          title: "Launchpad not found",
        },
      };
    }

    span.end();
    return {
      success: true as true,
      data: {
        ...launchpad,
        userSessionId: launchpad.userSession?.id ?? null,
      },
    };
  });

const deleteLaunchpad = async (
  launchpadId: string,
): Promise<ServiceReturnTypeV2<"deleteLaunchpad">> =>
  await tracer.startActiveSpan("deleteLaunchpadService", async (span) => {
    const launchpad = await prisma.launchpad.delete({
      where: {
        id: launchpadId,
      },
    });

    if (!launchpad) {
      span.end();
      return {
        success: false as false,
        error: {
          status: 404,
          title: "Launchpad not found",
        },
      };
    }

    span.end();
    return {
      success: true as true,
      data: {},
    };
  });

const getLaunchpadUserSession = async (
  launchpadId: string,
): Promise<ServiceReturnTypeV2<"getLaunchpadUserSession">> =>
  await tracer.startActiveSpan(
    "getLaunchpadUserSessionService",
    async (span) => {
      const userSession = await prisma.userSession.findUnique({
        where: {
          launchpadId,
        },
      });

      if (!userSession) {
        span.end();
        return {
          success: false as false,
          error: {
            status: 404,
            title: "User Session not found",
          },
        };
      }

      span.end();
      return {
        success: true as true,
        data: {
          ...userSession,
          startAt: userSession.startAt.toISOString(),
          endAt: userSession.endAt.toISOString(),
        },
      };
    },
  );

const createLaunchpadUserSession = async (
  launchpadId: string,
  userId: string,
): Promise<ServiceReturnTypeV2<"createLaunchpadUserSession">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"createLaunchpadUserSession">>
  >("createLaunchpadUserSessionService", async (span) => {
    const userSession = await prisma.$transaction(async (tx) => {
      const existUserSession = await tx.userSession.findUnique({
        where: {
          launchpadId,
        },
      });

      if (existUserSession) return undefined;

      const userSession = await tx.userSession.create({
        data: {
          userId,
          launchpadId,
          endAt: new Date(
            new Date().getTime() + 1000 * 60 * 30 /* 30 minutes */,
          ),
        },
      });

      return userSession;
    });

    if (!userSession) {
      span.end();
      return {
        success: false,
        error: {
          status: 409,
          title: "User Session already exists",
        },
      };
    }

    const accessToken = crypto.randomBytes(64).toString("hex");

    span.end();
    return {
      success: true,
      data: {
        accessToken,
        userSession: {
          ...userSession,
          startAt: userSession.startAt.toISOString(),
          endAt: userSession.endAt.toISOString(),
        },
      },
    };
  });

// eslint-disable-next-line @typescript-eslint/explicit-function-return-type
const spawnFwUpdateJob = async (fwCommit: string, nodeName: string) =>
  await k8s?.batchV1Api.createNamespacedJob(k8s.currentNamespace, {
    apiVersion: "batch/v1",
    kind: "Job",
    metadata: {
      name: `firmware-update-${crypto.randomUUID()}`,
    },
    spec: {
      template: {
        metadata: {
          labels: {
            "sidecar.istio.io/inject": "false",
          },
        },
        spec: {
          nodeName,
          initContainers: [
            {
              name: "build-firmware",
              image:
                "registry.gitlab.com/alea-2020/mission-operations/barbours-cut/obc-fw-build-tools:2.0.0",
              command: [
                "/bin/bash",
                "-c",
                `mkdir obc2-firmware && cd obc2-firmware && git init && git remote add origin https://${constants.obcFwRepoDeployToken.user}:${constants.obcFwRepoDeployToken.password}@gitlab.com/alea-2020/command-data-handling/obc2-firmware && git fetch origin $COMMIT && git checkout FETCH_HEAD && ./alea-fsw/obc-fw/scripts/fw_build.sh launchpad ext A --standalone --comms-over-serial && cp alea-fsw/obc-fw/build/_bin/* /build-dir && cp alea-fsw/obc-fw/tools/debug/ccs/* /build-dir`,
              ],
              env: [
                {
                  name: "COMMIT",
                  value: fwCommit,
                },
              ],
              volumeMounts: [
                {
                  name: "build-dir",
                  mountPath: "/build-dir",
                },
              ],
            },
          ],
          containers: [
            {
              name: "flash-launchpad",
              image:
                "registry.gitlab.com/alea-2020/mission-operations/barbours-cut/obc-fw-flasher:1.0.0",
              command: [
                "/bin/bash",
                "/opt/ti/uniflash_8.8.0/dslite.sh",
                "--flash",
                "--run",
                "--config=launchpad.ccxml",
                "ALEA_OBC_launchpad_ext_A_*.mot",
              ],
              securityContext: {
                privileged: true,
              },
              volumeMounts: [
                {
                  name: "build-dir",
                  mountPath: "/root",
                },
              ],
            },
          ],
          volumes: [
            {
              name: "build-dir",
              emptyDir: {},
            },
          ],
          imagePullSecrets: [
            {
              name: "regcred",
            },
          ],
          restartPolicy: "Never",
        },
      },
      backoffLimit: 0,
      ttlSecondsAfterFinished: 60,
    },
  });

const updateLaunchpadFirmware = async (
  launchpadId: string,
  fwCommit: string,
  accessToken: string,
): Promise<ServiceReturnTypeV2<"updateLaunchpadFirmware">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"updateLaunchpadFirmware">>
  >("updateLaunchpadFirmwareService", async (span) => {
    if (!k8s) {
      span.end();
      return {
        success: false,
        error: {
          status: 500,
          title: "Kubernetes API not available",
        },
      };
    }

    const user = await getUserAccessTokenClaims(accessToken);

    const launchpad = await prisma.launchpad.findUnique({
      where: {
        id: launchpadId,
      },
      include: {
        userSession: true,
      },
    });

    if (!user || (launchpad && launchpad.userSession?.userId !== user.id)) {
      return {
        success: false,
        error: {
          status: 401,
          title: "Unauthorized",
        },
      };
    }

    if (!launchpad) {
      span.end();
      return {
        success: false,
        error: {
          status: 404,
          title: "Launchpad not found",
        },
      };
    }

    const fwUpdateJob = await spawnFwUpdateJob(fwCommit, launchpad.node);

    let retries = 0;
    // update launchpad current firmware commit after job succeeded
    setInterval(async () => {
      if (retries++ >= 30) {
        clearInterval(this);
        return;
      }
      const job = await k8s?.batchV1Api
        .readNamespacedJobStatus(
          fwUpdateJob!.body.metadata!.name!,
          k8s.currentNamespace,
          undefined,
        )
        .catch(() => undefined);

      if (!job) {
        clearInterval(this);
        return;
      }

      if (job.body.status!.succeeded! > 0) {
        clearInterval(this);
        await prisma.launchpad.update({
          where: {
            id: launchpadId,
          },
          data: {
            fwCommit,
          },
        });
      }
    }, 1000 * 10 /* 10 seconds */);

    span.end();
    return {
      success: true,
      data: {
        jobName: fwUpdateJob!.body.metadata!.name!,
      },
    };
  });

const getLaunchpadFirmwareLog = async (
  jobName: string,
): Promise<
  | {
      logStream: stream.PassThrough;
      streamStatus: { buildFirmware: number; flashLaunchpad: number };
    }
  | undefined
> => {
  if (!k8s) return undefined;

  // check if job exists
  const pod = await retryUntilSuccess(
    async () =>
      await k8s!.coreV1Api
        .listNamespacedPod(
          k8s!.currentNamespace,
          undefined,
          undefined,
          undefined,
          undefined,
          `job-name=${jobName}`,
        )
        .then((res) => res.body.items[0]),
    5,
    1000,
  );

  if (!pod) return undefined;

  const buildFirmwareLogStream = new stream.PassThrough();
  const flashLaunchpadLogStream = new stream.PassThrough();
  const logStream = new stream.PassThrough();

  let streamBuffer = "";
  const streamStatus = {
    buildFirmware: 0,
    flashLaunchpad: 0,
  };

  buildFirmwareLogStream.on("data", (data) => {
    streamBuffer += data.toString("utf-8");
  });
  buildFirmwareLogStream.on("end", () => {
    streamStatus.buildFirmware = 1;
    buildFirmwareLogStream.destroy();
  });
  flashLaunchpadLogStream.on("data", (data) => {
    streamBuffer += data.toString("utf-8");
  });
  flashLaunchpadLogStream.on("end", () => {
    streamStatus.flashLaunchpad = 1;
    flashLaunchpadLogStream.destroy();
  });

  let steamTimeLength = 0;
  const streamTimeInterval = 250;
  const logStreamPush = setInterval(async () => {
    logStream.push(streamBuffer);
    streamBuffer = "";

    if (
      // end log stream if both build and flash logs are done
      (streamStatus.buildFirmware === 1 && streamStatus.flashLaunchpad === 1) ||
      // end log stream if either build or flash logs failed
      streamStatus.buildFirmware === -1 ||
      streamStatus.flashLaunchpad === -1 ||
      // end log stream if it takes too long (5 minutes)
      steamTimeLength >= 1000 * 60 * 5
    ) {
      clearInterval(logStreamPush);
      logStream.emit("end");
    }

    steamTimeLength += streamTimeInterval;
  }, streamTimeInterval);

  logStream.on("error", () => {
    clearInterval(logStreamPush);
    logStream.destroy();
    buildFirmwareLogStream.destroy();
    flashLaunchpadLogStream.destroy();
  });

  buildFirmwareLogStream.on("error", () => {
    clearInterval(logStreamPush);
    logStream.destroy();
    buildFirmwareLogStream.destroy();
    flashLaunchpadLogStream.destroy();
    streamStatus.buildFirmware = -1;
  });

  flashLaunchpadLogStream.on("error", () => {
    clearInterval(logStreamPush);
    logStream.destroy();
    buildFirmwareLogStream.destroy();
    flashLaunchpadLogStream.destroy();
    streamStatus.flashLaunchpad = -1;
  });

  // wait until init container is running, then start log stream
  retryUntilCondition(
    async () =>
      await k8s!.coreV1Api
        .readNamespacedPod(pod.metadata!.name!, k8s!.currentNamespace)
        .then((res) => res.body),
    (pod) => !!pod.status?.initContainerStatuses![0].state?.running,
    10,
    1000,
  ).then(async (pod) => {
    if (!pod) {
      streamStatus.buildFirmware = -1;
      return;
    }

    await k8s!.log.log(
      k8s!.currentNamespace,
      pod.metadata!.name!,
      "build-firmware",
      buildFirmwareLogStream,
      {
        follow: true,
        pretty: false,
      },
    );
  });

  // after build firmware log stream is done, start flash launchpad log stream
  buildFirmwareLogStream.on("end", async () => {
    // wait until flash launchpad container is running
    retryUntilCondition(
      async () =>
        await k8s!.coreV1Api
          .readNamespacedPod(pod.metadata!.name!, k8s!.currentNamespace)
          .then((res) => res.body),
      (pod) => !!pod.status?.containerStatuses![0].state?.running,
      200,
      1000,
    ).then(async (pod) => {
      if (!pod) {
        streamStatus.flashLaunchpad = -1;
        return;
      }

      await k8s!.log.log(
        k8s!.currentNamespace,
        pod.metadata!.name!,
        "flash-launchpad",
        flashLaunchpadLogStream,
        {
          follow: true,
          pretty: false,
        },
      );
    });
  });

  // return log stream while waiting for containers to start
  return { logStream, streamStatus };
};

export default {
  getLaunchpads,
  createLaunchpad,
  getLaunchpad,
  patchLaunchpad,
  deleteLaunchpad,
  getLaunchpadUserSession,
  createLaunchpadUserSession,
  updateLaunchpadFirmware,
  getLaunchpadFirmwareLog,
};
