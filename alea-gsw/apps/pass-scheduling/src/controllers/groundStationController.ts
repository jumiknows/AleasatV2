import {
  type OperationsRequest,
  responseWithError,
} from "@aleasat/pass-scheduling-api";
import { getAccessTokenFromHeader } from "@aleasat/utils";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import groundStationService from "../services/groundStationService.js";
import logger from "../utils/logger.js";

const tracer = opentelemetry.trace.getTracer("groundStationController");

const addGroundStation = async (
  req: OperationsRequest<"addGroundStation">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("addGroundStationController", async (span) => {
    try {
      const groundStation = await groundStationService.addGroundStation({
        accessToken: getAccessTokenFromHeader(req),
        groundStation: req.body,
      });

      if (groundStation.error) {
        responseWithError(req, res, groundStation.error);
        span.end();
        return;
      }

      res.json(groundStation.data);
    } catch (e) {
      span.recordException(e as Error);
      logger.error(e, "addGroundStationService throw error");

      responseWithError(req, res, {
        title: "Internal Server Error",
        detail: "Internal Server Error",
        status: 500,
      });
    }
    span.end();
  });

const getGroundStations = async (
  req: OperationsRequest<"getGroundStations">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("addGroundStationController", async (span) => {
    try {
      const groundStation = await groundStationService.getGroundStations({
        ownerId: req.query.ownerId,
        accessToken: getAccessTokenFromHeader(req),
      });

      if (groundStation.error) {
        responseWithError(req, res, groundStation.error);
        span.end();
        return;
      }

      res.json(groundStation.data);
    } catch (e) {
      span.recordException(e as Error);
      logger.error(e, "getGroundStationsService throw error");

      responseWithError(req, res, {
        title: "Internal Server Error",
        status: 500,
      });
    }
    span.end();
  });

const getGroundStation = async (
  req: OperationsRequest<"getGroundStation">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getGroundStationController", async (span) => {
    try {
      const groundStation = await groundStationService.getGroundStation({
        groundStationId: parseInt(req.params.groundStationId),
        accessToken: getAccessTokenFromHeader(req),
      });

      if (groundStation.error) {
        responseWithError(req, res, groundStation.error);
        span.end();
        return;
      }

      res.json(groundStation.data);
    } catch (e) {
      span.recordException(e as Error);
      logger.error(e, "getGroundStationService throw error");

      responseWithError(req, res, {
        title: "Internal Server Error",
        status: 500,
      });
    }
    span.end();
  });

const patchGroundStation = async (
  req: OperationsRequest<"patchGroundStation">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("patchGroundStationController", async (span) => {
    try {
      const result = await groundStationService.patchGroundStation({
        groundStationId: parseInt(req.params.groundStationId),
        groundStation: req.body,
        accessToken: getAccessTokenFromHeader(req),
      });

      if (result.error) {
        responseWithError(req, res, result.error);
        span.end();
        return;
      }

      res.json(result.data);
    } catch (e) {
      span.recordException(e as Error);
      logger.error(e, "patchGroundStationService throw error");

      responseWithError(req, res, {
        title: "Internal Server Error",
        status: 500,
      });
    }
    span.end();
  });

const deleteGroundStation = async (
  req: OperationsRequest<"deleteGroundStation">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan(
    "deleteGroundStationController",
    async (span) => {
      try {
        const result = await groundStationService.deleteGroundStation({
          groundStationId: parseInt(req.params.groundStationId),
          accessToken: getAccessTokenFromHeader(req),
        });

        if (result.error) {
          responseWithError(req, res, result.error);
          span.end();
          return;
        }

        res.sendStatus(200);
      } catch (e) {
        span.recordException(e as Error);
        logger.error(e, "deleteGroundStationService throw error");

        responseWithError(req, res, {
          title: "Internal Server Error",
          status: 500,
        });
      }
      span.end();
    },
  );

export default {
  addGroundStation,
  getGroundStations,
  getGroundStation,
  patchGroundStation,
  deleteGroundStation,
};
