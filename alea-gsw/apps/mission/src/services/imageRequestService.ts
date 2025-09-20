import type {
  ServiceReturnType,
  ServiceReturnTypeV2,
  components,
  operations,
} from "@aleasat/mission-api";
import opentelemetry from "@opentelemetry/api";
import { type ImageRequestStatus, prisma } from "../repositories/prisma.js";

const tracer = opentelemetry.trace.getTracer("imageRequestService");

const getImageRequests = async (
  query?: operations["getImageRequests"]["parameters"]["query"],
): Promise<ServiceReturnType<"getImageRequests">> =>
  tracer.startActiveSpan("getImageRequests", async (span) => {
    try {
      const imageRequests = await prisma.imageRequest.findMany({
        where: query
          ? {
              status: query.status
                ? (query.status as ImageRequestStatus)
                : undefined,
              userId: query.userId || undefined,
              requestedAt: query.requestedAt
                ? { gte: new Date(query.requestedAt) }
                : undefined,
            }
          : undefined,
      });

      const formattedRequests = imageRequests.map((req) => ({
        id: req.id,
        coordX: req.coordX,
        coordY: req.coordY,
        coordZ: req.coordZ,
        QuaternionW: req.QuaternionW,
        QuaternionX: req.QuaternionX,
        QuaternionY: req.QuaternionY,
        QuaternionZ: req.QuaternionZ,
        requestedAt: req.requestedAt.toISOString(),
        completedAt: req.completedAt ? req.completedAt.toISOString() : null,
        fStop: req.fStop,
        focus: req.focus,
        ISO: req.ISO,
        status: req.status || "PENDING",
        retryCount: req.retryCount || 0,
        userId: req.userId,
      }));

      return { data: formattedRequests };
    } catch (error: unknown) {
      if (error instanceof Error) {
        span.recordException(error);
      } else {
        span.recordException(new Error("An unknown error occurred"));
      }
      throw error;
    } finally {
      span.end();
    }
  });

const getImageRequestById = async (
  imageRequestId: operations["getImageRequestById"]["parameters"]["path"]["id"],
): Promise<ServiceReturnType<"getImageRequestById">> =>
  tracer.startActiveSpan("getImageRequestById", async (span) => {
    const imageRequest = await prisma.imageRequest.findUnique({
      where: { id: imageRequestId },
    });

    if (!imageRequest) {
      span.end();
      return {
        error: {
          status: 404,
          title: "Image Request not found",
        },
      };
    }

    const formattedRequest = {
      id: imageRequest.id,
      coordX: imageRequest.coordX,
      coordY: imageRequest.coordY,
      coordZ: imageRequest.coordZ,
      QuaternionW: imageRequest.QuaternionW,
      QuaternionX: imageRequest.QuaternionX,
      QuaternionY: imageRequest.QuaternionY,
      QuaternionZ: imageRequest.QuaternionZ,
      requestedAt: imageRequest.requestedAt.toISOString(),
      completedAt: imageRequest.completedAt
        ? imageRequest.completedAt.toISOString()
        : null,
      fStop: imageRequest.fStop,
      focus: imageRequest.focus,
      ISO: imageRequest.ISO,
      status: imageRequest.status || "PENDING",
      retryCount: imageRequest.retryCount || 0,
      userId: imageRequest.userId,
    };

    span.end();
    return { data: formattedRequest };
  });

const createImageRequest = async (
  userId: string,
  requestData: operations["createImageRequest"]["requestBody"]["content"]["application/json"],
): Promise<ServiceReturnTypeV2<"createImageRequest">> =>
  tracer.startActiveSpan("createImageRequest", async (span) => {
    try {
      const createdRequest = await prisma.imageRequest.create({
        data: {
          coordX: requestData.coordX,
          coordY: requestData.coordY,
          coordZ: requestData.coordZ,
          QuaternionW: requestData.QuaternionW,
          QuaternionX: requestData.QuaternionX,
          QuaternionY: requestData.QuaternionY,
          QuaternionZ: requestData.QuaternionZ,
          requestedAt: new Date(),
          fStop: requestData.fStop,
          focus: requestData.focus,
          ISO: requestData.ISO,
          userId: userId,
        },
      });

      span.end();
      return {
        success: true as const,
        data: {
          ...createdRequest,
          requestedAt: createdRequest.requestedAt.toISOString(),
          completedAt: createdRequest.completedAt
            ? createdRequest.completedAt.toISOString()
            : null,
        },
      };
    } catch (_error) {
      span.end();
      return {
        success: false as const,
        error: {
          status: 500,
          title: "Failed to create image request",
        },
      };
    }
  });

const updateImageRequest = async (
  imageRequestId: operations["updateImageRequest"]["parameters"]["path"]["id"],
  updateData: components["schemas"]["UpdateImageRequest"],
): Promise<ServiceReturnTypeV2<"updateImageRequest">> =>
  tracer.startActiveSpan("updateImageRequest", async (span) => {
    try {
      const updatedRequest = await prisma.imageRequest.update({
        where: { id: imageRequestId },
        data: {
          ...updateData,
          requestedAt: new Date(),
        },
      });

      const formattedRequest = {
        id: updatedRequest.id,
        coordX: updatedRequest.coordX,
        coordY: updatedRequest.coordY,
        coordZ: updatedRequest.coordZ,
        QuaternionW: updatedRequest.QuaternionW,
        QuaternionX: updatedRequest.QuaternionX,
        QuaternionY: updatedRequest.QuaternionY,
        QuaternionZ: updatedRequest.QuaternionZ,
        requestedAt: updatedRequest.requestedAt.toISOString(),
        completedAt: updatedRequest.completedAt
          ? updatedRequest.completedAt.toISOString()
          : null,
        fStop: updatedRequest.fStop,
        focus: updatedRequest.focus,
        ISO: updatedRequest.ISO,
        status: updatedRequest.status || "PENDING",
        retryCount: updatedRequest.retryCount || 0,
        userId: updatedRequest.userId,
      };

      span.end();
      return {
        success: true as const,
        data: formattedRequest,
      };
    } catch (error) {
      span.end();
      if (error instanceof Error) {
        if (
          error.name === "PrismaClientKnownRequestError" &&
          error.message.includes("Record to update does not exist")
        ) {
          return {
            success: false as const,
            error: {
              status: 404,
              title: "Image request not found",
            },
          };
        }
        return {
          success: false as const,
          error: {
            status: 500,
            title: "An unknown error occurred",
          },
        };
      }
      return {
        success: false as const,
        error: {
          status: 500,
          title: "An unknown error occurred",
        },
      };
    }
  });

const deleteImageRequest = async (
  imageRequestId: operations["deleteImageRequest"]["parameters"]["path"]["id"],
): Promise<ServiceReturnTypeV2<"deleteImageRequest">> =>
  tracer.startActiveSpan("deleteImageRequest", async (span) => {
    try {
      const deletedRequest = await prisma.imageRequest.delete({
        where: { id: imageRequestId },
      });
      span.end();
      return {
        success: true as const,
        data: {
          ...deletedRequest,
          requestedAt: deletedRequest.requestedAt.toISOString(),
          completedAt: deletedRequest.completedAt
            ? deletedRequest.completedAt.toISOString()
            : null,
        },
      };
    } catch (error) {
      span.end();
      if (error instanceof Error) {
        if (
          error.name === "PrismaClientKnownRequestError" &&
          error.message.includes("Record to delete does not exist")
        ) {
          return {
            success: false as const,
            error: {
              status: 404,
              title: "Image request not found",
            },
          };
        }
      }
      return {
        success: false as const,
        error: {
          status: 500,
          title: "Failed to delete image request",
        },
      };
    }
  });

export default {
  getImageRequests,
  getImageRequestById,
  createImageRequest,
  updateImageRequest,
  deleteImageRequest,
};
