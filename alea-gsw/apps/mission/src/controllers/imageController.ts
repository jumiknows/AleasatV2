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
import imageRequestService from "../services/imageRequestService.js";

const tracer = opentelemetry.trace.getTracer("imageController");

const getImageRequests = async (
  req: OperationsRequest<"getImageRequests">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getImageRequestsController", async (span) => {
    const imageRequests = await errorWrapper(
      req,
      res,
      imageRequestService.getImageRequests(req.query),
    );

    if (imageRequests) {
      res.json(imageRequests);
    }

    span.end();
  });

const getImageRequestById = async (
  req: OperationsRequest<"getImageRequestById">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan(
    "getImageRequestByIdController",
    async (span) => {
      const imageRequest = await errorWrapper(
        req,
        res,
        imageRequestService.getImageRequestById(req.params.id),
      );

      if (imageRequest) {
        res.json(imageRequest);
      }

      span.end();
    },
  );

const createImageRequest = async (
  req: OperationsRequest<"createImageRequest">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("createImageRequestController", async (span) => {
    const accessToken = getAccessTokenFromHeader(req);
    const user = accessToken && (await getUserAccessTokenClaims(accessToken));

    if (!accessToken || !user) {
      responseWithError(req, res, {
        status: 401,
        title: "Unauthorized",
      });
      return;
    }

    const createdImageRequest = await errorWrapperV2(
      req,
      res,
      imageRequestService.createImageRequest(user.id, req.body),
    );

    if (createdImageRequest) {
      res.json(createdImageRequest);
    }

    span.end();
  });

const updateImageRequest = async (
  req: OperationsRequest<"updateImageRequest">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("updateImageRequestController", async (span) => {
    const patchedImageRequest = await errorWrapperV2(
      req,
      res,
      imageRequestService.updateImageRequest(req.params.id, req.body),
    );

    if (patchedImageRequest) {
      res.json(patchedImageRequest);
    }

    span.end();
  });

const deleteImageRequest = async (
  req: OperationsRequest<"deleteImageRequest">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("deleteImageRequestController", async (span) => {
    const deletedImageRequest = await errorWrapperV2(
      req,
      res,
      imageRequestService.deleteImageRequest(req.params.id),
    );

    if (deletedImageRequest) {
      res.json(deletedImageRequest);
    }

    span.end();
  });

export default {
  getImageRequests,
  getImageRequestById,
  createImageRequest,
  updateImageRequest,
  deleteImageRequest,
};
