import { type OperationsRequest, errorWrapperV3 } from "@aleasat/launchpad-api";
import {
  getAccessTokenFromHeader,
  getUserAccessTokenClaims,
} from "@aleasat/utils";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import launchpadService from "../services/launchpadService.js";

const tracer = opentelemetry.trace.getTracer("commandController");

const getLaunchpads = async (
  req: OperationsRequest<"getLaunchpads">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const launchpad = await errorWrapperV3(
      req,
      res,
      async () => await launchpadService.getLaunchpads(),
    );

    if (!launchpad) {
      span.end();
      return;
    }

    res.status(200).json(launchpad);
    span.end();
  });

const createLaunchpad = async (
  req: OperationsRequest<"createLaunchpad">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const accessToken = getAccessTokenFromHeader(req);
    const user = accessToken && (await getUserAccessTokenClaims(accessToken));

    if (!user) {
      res.status(401).json({
        status: 401,
        title: "Unauthorized",
      });
      span.end();
      return;
    }

    if (!user.groups.includes("aleasat-lead")) {
      res.status(403).json({
        status: 403,
        title: "Forbidden",
      });
      span.end();
      return;
    }

    const launchpad = await errorWrapperV3(
      req,
      res,
      async () => await launchpadService.createLaunchpad(req.body),
    );

    if (!launchpad) {
      span.end();
      return;
    }

    res.status(200).json(launchpad);
    span.end();
  });

const getLaunchpad = async (
  req: OperationsRequest<"getLaunchpad">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const launchpad = await errorWrapperV3(
      req,
      res,
      async () => await launchpadService.getLaunchpad(req.params.launchpadId),
    );

    if (!launchpad) {
      span.end();
      return;
    }

    res.status(200).json(launchpad);
    span.end();
  });

const patchLaunchpad = async (
  req: OperationsRequest<"patchLaunchpad">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const accessToken = getAccessTokenFromHeader(req);
    const user = accessToken && (await getUserAccessTokenClaims(accessToken));

    if (!user) {
      res.status(401).json({
        status: 401,
        title: "Unauthorized",
      });
      span.end();
      return;
    }

    if (!user.groups.includes("aleasat-lead")) {
      res.status(403).json({
        status: 403,
        title: "Forbidden",
      });
      span.end();
      return;
    }

    const launchpad = await errorWrapperV3(
      req,
      res,
      async () =>
        await launchpadService.patchLaunchpad(req.params.launchpadId, req.body),
    );

    if (!launchpad) {
      span.end();
      return;
    }

    res.status(200).json(launchpad);
    span.end();
  });

const deleteLaunchpad = async (
  req: OperationsRequest<"deleteLaunchpad">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const accessToken = getAccessTokenFromHeader(req);
    const user = accessToken && (await getUserAccessTokenClaims(accessToken));

    if (!user) {
      res.status(401).json({
        status: 401,
        title: "Unauthorized",
      });
      span.end();
      return;
    }

    if (!user.groups.includes("aleasat-lead")) {
      res.status(403).json({
        status: 403,
        title: "Forbidden",
      });
      span.end();
      return;
    }

    await errorWrapperV3(
      req,
      res,
      async () =>
        await launchpadService.deleteLaunchpad(req.params.launchpadId),
    );

    res.status(200);
    span.end();
  });

const getLaunchpadUserSession = async (
  req: OperationsRequest<"getLaunchpadUserSession">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const userSession = await errorWrapperV3(
      req,
      res,
      async () =>
        await launchpadService.getLaunchpadUserSession(req.params.launchpadId),
    );

    if (!userSession) {
      span.end();
      return;
    }

    res.status(200).json(userSession);
    span.end();
  });

const createLaunchpadUserSession = async (
  req: OperationsRequest<"createLaunchpadUserSession">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const accessToken = getAccessTokenFromHeader(req);
    const user = accessToken && (await getUserAccessTokenClaims(accessToken));

    if (!user) {
      res.status(401).json({
        status: 401,
        title: "Unauthorized",
      });
      span.end();
      return;
    }

    const userSession = await errorWrapperV3(
      req,
      res,
      async () =>
        await launchpadService.createLaunchpadUserSession(
          req.params.launchpadId,
          user.id,
        ),
    );

    if (!userSession) {
      span.end();
      return;
    }

    res.status(201).json(userSession);
    span.end();
  });

const updateLaunchpadFirmware = async (
  req: OperationsRequest<"updateLaunchpadFirmware">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const accessToken = getAccessTokenFromHeader(req);

    if (!accessToken) {
      res.status(401).json({
        status: 401,
        title: "Unauthorized",
      });
      span.end();
      return;
    }

    const launchpad = await errorWrapperV3(
      req,
      res,
      async () =>
        await launchpadService.updateLaunchpadFirmware(
          req.params.launchpadId,
          req.body.fwCommit,
          accessToken,
        ),
    );

    if (!launchpad) {
      span.end();
      return;
    }

    console.log(launchpad, "aaaa");
    res.status(200).json(launchpad);
    span.end();
  });

const getLaunchpadFirmwareLog = async (
  req: OperationsRequest<"getLaunchpadFirmwareLog">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchCommandController", async (span) => {
    const result = await launchpadService.getLaunchpadFirmwareLog(
      req.query.jobName,
    );

    if (!result) {
      res.status(404).json({
        status: 404,
        title: "Not Found",
      });
      span.end();
      return;
    }

    res.setHeader("Content-Type", "text/event-stream");
    res.setHeader("Transfer-Encoding", "chunked");

    const { logStream, streamStatus } = result;

    logStream.on("data", (data: Buffer) => {
      res.write(data.toString("utf-8"));
    });

    logStream.on("end", () => {
      if (streamStatus.buildFirmware === -1) {
        res.write("Firmware build failed");
        res.status(500).end();
        return;
      }
      if (streamStatus.flashLaunchpad === -1) {
        res.write("Firmware flash failed");
        res.status(500).end();
      }

      res.status(200).end();
    });

    req.on("close", async () => {
      logStream.destroy(new Error("req closed"));
    });
  });

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
