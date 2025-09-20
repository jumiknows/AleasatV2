/* eslint-disable @typescript-eslint/unbound-method */
import type { operations } from "@aleasat/job-queue-api";
import { Job, Queue } from "bullmq";
import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import sinon, { type SinonStub, type SinonStubbedInstance } from "sinon";
import { Queues } from "../../../src/bullmq/queues.js";
import { prisma } from "../../../src/repositories/prisma.js";
import imageGenJobQueueService from "../../../src/services/imageGenJobQueueService.js";

chaiUse(chaiAsPromised);

describe("services/imageGenJobQueueService.ts", () => {
  let imageGenQueueStub: SinonStubbedInstance<Queue>;

  const job = {
    id: "f7eec2e8-0b5e-4f7c-95fd-8f3c0e58f123",
    userId: "a1b2c3d4-e5f6-7890-abcd-1234567890ab",
    name: "Coastal Scan Request",
    description: "Requesting satellite imagery for coastal erosion study.",
    locationTrackId: "loc-7890abcd-1234-efgh-5678-ijklmnopqrst",
    satelliteRotationX: 12.5,
    satelliteRotationY: -3.1,
    satelliteRotationZ: 45.0,
    AROgroundStationId: 3,
    AROgroundStationPassId: "pass-5678efgh",
    reviewerId: "rev-12345678-abcd-efgh-ijkl-90mnopqrstuv",
    approved: true,
    reviewComment: "Looks good. Proceed with data collection.",
    thumbnailID: "thumb-3456abcd",
    initialPreviewImageId: "prev-1234abcd",
    finalPreviewImageId: "prev-5678efgh",
    imageId: "img-9012ijkl",
    command: {
      id: "cmd-abcdef12-3456-7890-abcd-ef1234567890",
      submitedAt: "2025-07-08T15:00:00Z",
      userId: "a1b2c3d4-e5f6-7890-abcd-1234567890ab",
      imageRequestId: "f7eec2e8-0b5e-4f7c-95fd-8f3c0e58f123",
      command: "CAPTURE_IMAGE",
      size: 2048,
      power: 150,
      passId: "pass-5678efgh",
      sentAt: "2025-07-08T15:05:00Z",
      resultId: "result-abcdef123456",
    },
  } as operations["createImageJob"]["requestBody"]["content"]["application/json"];

  beforeEach(() => {
    imageGenQueueStub = sinon.createStubInstance(Queue);
    sinon.stub(Queues, "imageGen").returns(imageGenQueueStub);
  });

  afterEach(() => {
    sinon.restore();
  });

  describe("createImageGenJob Service", () => {
    it("should create a new image generation job with BullMQ and Prisma and return the new job", async () => {
      const returnJobStub = sinon.createStubInstance(Job);
      returnJobStub.id = "test-id";
      returnJobStub.data = job;
      returnJobStub.getState.resolves("waiting");
      imageGenQueueStub.add.resolves(returnJobStub);
      const prismaCreateJobStub = sinon.stub();
      prisma.$transaction = sinon
        .stub()
        .callsFake(async (callback: (tx: any) => Promise<any>) => {
          const txStub = {
            job: {
              create: prismaCreateJobStub,
            },
          };
          return await callback(txStub);
        });

      const createdJob = await imageGenJobQueueService.createImageGenJob(job);

      sinon.assert.calledOnceWithMatch(
        // eslint-disable-next-line @typescript-eslint/unbound-method
        imageGenQueueStub.add,
        sinon.match.string,
        sinon.match(job),
      );
      sinon.assert.calledOnceWithMatch(
        prismaCreateJobStub,
        sinon.match({
          data: sinon.match({
            id: sinon.match.string,
            name: sinon.match.string,
            type: "IMAGE_GENERATION",
            status: sinon.match.string,
            requestData: job,
          }),
        }),
      );

      expect(createdJob.success && createdJob.data).to.deep.contain({
        id: "test-id",
        requestData: job,
      });
    });

    it("should throw if BullMQ is unavailable", async () => {
      imageGenQueueStub.add.throws();
      prisma.job.create = sinon.stub();

      const createdJob = await imageGenJobQueueService.createImageGenJob(job);
      sinon.assert.notCalled(prisma.job.create as SinonStub);
      expect(!createdJob.success && createdJob.error.status).to.equal(503);
    });

    it("should throw if Prisma is unavailable", async () => {
      const returnJobStub = sinon.createStubInstance(Job);
      returnJobStub.id = "test-id";
      returnJobStub.data = job;
      imageGenQueueStub.add.resolves(returnJobStub);
      prisma.$transaction = sinon
        .stub()
        .callsFake(async (callback: (tx: any) => Promise<any>) => {
          const txStub = {
            job: {
              create: sinon.stub().throws(),
            },
          };
          return await callback(txStub);
        })
        .throws();

      const createdJob = await imageGenJobQueueService.createImageGenJob(job);
      sinon.assert.notCalled(imageGenQueueStub.add);
      expect(!createdJob.success && createdJob.error.status).to.equal(503);
    });
  });

  describe("getOrbitPropJob Service", () => {
    it("should get correct job from Prisma", async () => {
      const job = {};

      const returnJobStub = { id: "test-id", requestData: job };
      prisma.job.findUnique = sinon.stub().resolves(returnJobStub);

      const returnedJob =
        await imageGenJobQueueService.getImageGenJob("test-id");

      expect(returnedJob.success && returnedJob.data).to.deep.contain({
        id: "test-id",
        requestData: job,
      });
    });

    it("should throw if job not found", async () => {
      prisma.job.findUnique = sinon
        .stub()
        .withArgs({ where: { id: "test-id" } })
        .resolves(null);
      const returnedJob =
        await imageGenJobQueueService.getImageGenJob("test-id");
      expect(!returnedJob.success && returnedJob.error.status).to.equal(404);
    });

    it("should throw if Prisma is unavailable", async () => {
      prisma.job.findUnique = sinon
        .stub()
        .withArgs({ where: { id: "test-id" } })
        .throws();
      const returnedJob =
        await imageGenJobQueueService.getImageGenJob("test-id");
      expect(!returnedJob.success && returnedJob.error.status).to.equal(503);
    });
  });
});
