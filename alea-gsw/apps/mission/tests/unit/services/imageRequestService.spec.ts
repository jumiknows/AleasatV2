import "../env.js";

import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import sinon from "sinon";
import { prisma } from "../../../src/repositories/prisma.js";
import type { ImageRequestStatus } from "../../../src/repositories/prisma.js";
import imageRequestService from "../../../src/services/imageRequestService.js";

chaiUse(chaiAsPromised);

describe("services/imageRequestService.ts", () => {
  beforeEach(() => {
    // Reset all stubs before each test
  });

  afterEach(() => {
    sinon.restore();
  });

  describe("getImageRequests Service", () => {
    it("should return all image requests", async () => {
      const expectedImageRequests = [
        {
          id: "d7934c5e-f374-4ecd-8ad6-d9537e510177",
          coordX: 123.45,
          coordY: 67.89,
          coordZ: 10.11,
          QuaternionW: 0.707,
          QuaternionX: 0.0,
          QuaternionY: 0.0,
          QuaternionZ: 0.707,
          fStop: 2.8,
          focus: 50,
          ISO: 100,
          userId: "user123",
          requestedAt: "2024-01-01T00:00:00.000Z",
          completedAt: null,
          status: "PENDING" as ImageRequestStatus,
          retryCount: 0,
        },
        {
          id: "a1b2c3d4-e5f6-g7h8-i9j0-k1l2m3n4o5p6",
          coordX: 234.56,
          coordY: 78.9,
          coordZ: 11.12,
          QuaternionW: 0.5,
          QuaternionX: 0.5,
          QuaternionY: 0.5,
          QuaternionZ: 0.5,
          fStop: 4.0,
          focus: 35,
          ISO: 200,
          userId: "user456",
          requestedAt: "2024-01-02T00:00:00.000Z",
          completedAt: "2024-01-02T00:01:00.000Z",
          status: "COMPLETED" as ImageRequestStatus,
          retryCount: 0,
        },
      ];

      prisma.imageRequest.findMany = sinon.stub().resolves(
        expectedImageRequests.map((request) => ({
          ...request,
          requestedAt: new Date(request.requestedAt),
          completedAt: request.completedAt
            ? new Date(request.completedAt)
            : null,
        })),
      );

      const imageRequests = await imageRequestService.getImageRequests({});

      expect(imageRequests.data).to.deep.equal(expectedImageRequests);
    });

    it("should filter image requests by status", async () => {
      const imageRequestFindManyStub = sinon.stub().resolves([]);
      prisma.imageRequest.findMany = imageRequestFindManyStub;

      await imageRequestService.getImageRequests({ status: "PENDING" });

      expect(imageRequestFindManyStub.firstCall.args[0].where).to.deep.include({
        status: "PENDING",
      });
    });

    it("should filter image requests by userId", async () => {
      const imageRequestFindManyStub = sinon.stub().resolves([]);
      prisma.imageRequest.findMany = imageRequestFindManyStub;

      await imageRequestService.getImageRequests({ userId: "user123" });

      expect(imageRequestFindManyStub.firstCall.args[0].where).to.deep.include({
        userId: "user123",
      });
    });

    it("should handle database errors", async () => {
      const error = new Error("Database connection failed");
      prisma.imageRequest.findMany = sinon.stub().rejects(error);

      await expect(imageRequestService.getImageRequests({})).to.be.rejectedWith(
        error,
      );
    });
  });

  describe("getImageRequestById Service", () => {
    it("should return the image request with the specified id", async () => {
      const imageRequestId = "d7934c5e-f374-4ecd-8ad6-d9537e510177";
      const expectedImageRequest = {
        id: imageRequestId,
        coordX: 123.45,
        coordY: 67.89,
        coordZ: 10.11,
        QuaternionW: 0.707,
        QuaternionX: 0.0,
        QuaternionY: 0.0,
        QuaternionZ: 0.707,
        fStop: 2.8,
        focus: 50,
        ISO: 100,
        userId: "user123",
        requestedAt: "2024-01-01T00:00:00.000Z",
        completedAt: null,
        status: "PENDING" as ImageRequestStatus,
        retryCount: 0,
      };

      prisma.imageRequest.findUnique = sinon.stub().resolves({
        ...expectedImageRequest,
        requestedAt: new Date(expectedImageRequest.requestedAt),
        completedAt: null,
      });

      const imageRequest =
        await imageRequestService.getImageRequestById(imageRequestId);
      expect(imageRequest.data).to.deep.equal(expectedImageRequest);
    });

    it("should return 404 error when image request is not found", async () => {
      prisma.imageRequest.findUnique = sinon.stub().resolves(null);

      const result =
        await imageRequestService.getImageRequestById("non-existent-id");
      expect(result.error).to.deep.equal({
        status: 404,
        title: "Image Request not found",
      });
    });

    it("should handle database errors", async () => {
      const error = new Error("Database connection failed");
      prisma.imageRequest.findUnique = sinon.stub().rejects(error);

      await expect(
        imageRequestService.getImageRequestById("some-id"),
      ).to.be.rejectedWith(error);
    });
  });

  describe("createImageRequest Service", () => {
    it("should create a new image request", async () => {
      const userId = "user123";
      const requestData = {
        coordX: 123.45,
        coordY: 67.89,
        coordZ: 10.11,
        QuaternionW: 0.707,
        QuaternionX: 0.0,
        QuaternionY: 0.0,
        QuaternionZ: 0.707,
        fStop: 2.8,
        focus: 50,
        ISO: 100,
        userId,
        requestedAt: new Date().toISOString(),
      };

      const createdRequest = {
        id: "d7934c5e-f374-4ecd-8ad6-d9537e510177",
        ...requestData,
        requestedAt: new Date(requestData.requestedAt),
        completedAt: null,
        status: "PENDING" as ImageRequestStatus,
        retryCount: 0,
      };

      prisma.imageRequest.create = sinon.stub().resolves(createdRequest);

      const result = await imageRequestService.createImageRequest(
        userId,
        requestData,
      );

      expect(result.success).to.be.true;
      if (result.success) {
        expect(result.data).to.containSubset({
          ...requestData,
          userId,
          status: "PENDING",
          retryCount: 0,
        });
      }
    });

    it("should handle database errors", async () => {
      const error = new Error("Database connection failed");
      prisma.imageRequest.create = sinon.stub().rejects(error);

      const requestData = {
        coordX: 123.45,
        coordY: 67.89,
        coordZ: 10.11,
        QuaternionW: 0.707,
        QuaternionX: 0.0,
        QuaternionY: 0.0,
        QuaternionZ: 0.707,
        fStop: 2.8,
        focus: 50,
        ISO: 100,
        userId: "user123",
        requestedAt: new Date().toISOString(),
      };

      const result = await imageRequestService.createImageRequest(
        "user123",
        requestData,
      );
      expect(result).to.deep.equal({
        success: false,
        error: {
          status: 500,
          title: "Failed to create image request",
        },
      });
    });
  });

  describe("updateImageRequest Service", () => {
    it("should update an existing image request", async () => {
      const imageRequestId = "d7934c5e-f374-4ecd-8ad6-d9537e510177";
      const updateData = {
        status: "PROCESSING" as ImageRequestStatus,
        retryCount: 1,
      };

      const updatedRequest = {
        id: imageRequestId,
        coordX: 123.45,
        coordY: 67.89,
        coordZ: 10.11,
        QuaternionW: 0.707,
        QuaternionX: 0.0,
        QuaternionY: 0.0,
        QuaternionZ: 0.707,
        fStop: 2.8,
        focus: 50,
        ISO: 100,
        userId: "user123",
        requestedAt: new Date(),
        completedAt: null,
        status: "PROCESSING" as ImageRequestStatus,
        retryCount: 1,
      };

      prisma.imageRequest.update = sinon.stub().resolves(updatedRequest);

      const result = await imageRequestService.updateImageRequest(
        imageRequestId,
        updateData,
      );

      expect(result.success).to.be.true;
      if (result.success) {
        expect(result.data).to.containSubset({
          id: imageRequestId,
          status: "PROCESSING",
          retryCount: 1,
        });
      }
    });

    it("should handle record not found errors", async () => {
      const error = new Error("Record to update does not exist");
      error.name = "PrismaClientKnownRequestError";
      prisma.imageRequest.update = sinon.stub().rejects(error);

      const result = await imageRequestService.updateImageRequest(
        "non-existent-id",
        {
          status: "PROCESSING" as ImageRequestStatus,
        },
      );

      expect(result.success).to.be.false;
      if (!result.success) {
        expect(result.error).to.deep.equal({
          status: 404,
          title: "Image request not found",
        });
      }
    });

    it("should handle unknown errors", async () => {
      const error = new Error("Database connection failed");
      error.name = "PrismaClientUnknownRequestError";
      prisma.imageRequest.update = sinon.stub().rejects(error);

      const result = await imageRequestService.updateImageRequest("some-id", {
        status: "PROCESSING" as ImageRequestStatus,
      });

      expect(result.success).to.be.false;
      if (!result.success) {
        expect(result.error).to.deep.equal({
          status: 500,
          title: "An unknown error occurred",
        });
      }
    });
  });

  describe("deleteImageRequest Service", () => {
    it("should delete an existing image request", async () => {
      const imageRequestId = "d7934c5e-f374-4ecd-8ad6-d9537e510177";
      const deletedRequest = {
        id: imageRequestId,
        coordX: 123.45,
        coordY: 67.89,
        coordZ: 10.11,
        QuaternionW: 0.707,
        QuaternionX: 0.0,
        QuaternionY: 0.0,
        QuaternionZ: 0.707,
        fStop: 2.8,
        focus: 50,
        ISO: 100,
        userId: "user123",
        requestedAt: new Date(),
        completedAt: null,
        status: "PENDING" as ImageRequestStatus,
        retryCount: 0,
      };

      prisma.imageRequest.delete = sinon.stub().resolves(deletedRequest);

      const result =
        await imageRequestService.deleteImageRequest(imageRequestId);
      expect(result.success).to.be.true;
      if (result.success) {
        expect(result.data).to.deep.equal({
          ...deletedRequest,
          requestedAt: deletedRequest.requestedAt.toISOString(),
          completedAt: null,
        });
      }
    });

    it("should handle database errors", async () => {
      const error = new Error("Database connection failed");
      prisma.imageRequest.delete = sinon.stub().rejects(error);

      const result = await imageRequestService.deleteImageRequest("some-id");
      expect(result).to.deep.equal({
        success: false,
        error: {
          status: 500,
          title: "Failed to delete image request",
        },
      });
    });

    it("should handle not found errors", async () => {
      const error = new Error("Record to delete does not exist");
      error.name = "PrismaClientKnownRequestError";
      prisma.imageRequest.delete = sinon.stub().rejects(error);

      const result =
        await imageRequestService.deleteImageRequest("non-existent-id");
      expect(result).to.deep.equal({
        success: false,
        error: {
          status: 404,
          title: "Image request not found",
        },
      });
    });
  });
});
