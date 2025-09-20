import {
  type OperationsRequest,
  errorWrapper,
  errorWrapperV2,
  responseWithError,
} from "@aleasat/mission-api";
import {
  getAccessTokenFromHeader,
  getUserAccessTokenClaims,
} from "@aleasat/utils";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import missionService from "../services/missionService.js";

const tracer = opentelemetry.trace.getTracer("missionController");

const getMissions = async (
  req: OperationsRequest<"getMissions">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getMissionsController", async (span) => {
    const missions = await errorWrapper(
      req,
      res,
      missionService.getMissions(req.query),
    );

    if (missions) {
      res.json(missions);
    }

    span.end();
  });

const getMission = async (
  req: OperationsRequest<"getMission">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getMissionController", async (span) => {
    const mission = await errorWrapper(
      req,
      res,
      missionService.getMission(req.params.missionId),
    );

    if (mission) {
      res.json(mission);
    }

    span.end();
  });

const createMission = async (
  req: OperationsRequest<"createMission">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("createMissionController", async (span) => {
    const accessToken = getAccessTokenFromHeader(req);
    const user = accessToken && (await getUserAccessTokenClaims(accessToken));

    if (!accessToken || !user) {
      responseWithError(req, res, {
        status: 401,
        title: "Unauthorized",
      });
      return;
    }

    const createdMission = await errorWrapperV2(
      req,
      res,
      missionService.createMission(user.id, req.body),
    );

    if (createdMission) {
      res.json(createdMission);
    }

    span.end();
  });

const patchMission = async (
  req: OperationsRequest<"patchMission">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchMissionController", async (span) => {
    const patchedMission = await errorWrapperV2(
      req,
      res,
      missionService.patchMission(req.params.missionId, req.body),
    );

    if (patchedMission) {
      res.json(patchedMission);
    }

    span.end();
  });

export default {
  getMissions,
  getMission,
  createMission,
  patchMission,
};
