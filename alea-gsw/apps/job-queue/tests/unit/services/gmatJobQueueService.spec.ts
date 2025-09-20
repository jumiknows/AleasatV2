/* eslint-disable @typescript-eslint/unbound-method */
import type { operations } from "@aleasat/job-queue-api";
import { Job, Queue } from "bullmq";
import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import sinon, { type SinonStub, type SinonStubbedInstance } from "sinon";
import { Queues } from "../../../src/bullmq/queues.js";
import { prisma } from "../../../src/repositories/prisma.js";
import gmatJobQueueService from "../../../src/services/gmatJobQueueService.js";

chaiUse(chaiAsPromised);

describe("services/gmatJobQueueService.ts", () => {
  let gmatQueueStub: SinonStubbedInstance<Queue>;

  beforeEach(() => {
    gmatQueueStub = sinon.createStubInstance(Queue);
    sinon.stub(Queues, "gmat").returns(gmatQueueStub);
  });

  afterEach(() => {
    sinon.restore();
  });

  describe("createGMATJob Service", () => {
    it("should create a new GMAT job with BullMQ and Prisma and return the new job", async () => {
      const job = {
        epochFormat: "UTCGregorian",
        epoch: "2024-11-16T00:00:00Z",
        coordinateSystem: "EarthMJ2000Eq",
        stateElement: {
          type: "cartesian",
          x: 1000,
          y: 2000,
          z: 3000,
          vx: 1.5,
          vy: 2.5,
          vz: 3.5,
        },
        stateId: "cartesianStateExample",
      } as operations["createGMATJob"]["requestBody"]["content"]["application/json"];

      const returnJobStub = sinon.createStubInstance(Job);
      returnJobStub.id = "test-id";
      returnJobStub.data = job;
      returnJobStub.getState.resolves("waiting");
      gmatQueueStub.add.resolves(returnJobStub);
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

      const createdJob = await gmatJobQueueService.createGMATJob(job);

      sinon.assert.calledOnceWithMatch(
        // eslint-disable-next-line @typescript-eslint/unbound-method
        gmatQueueStub.add,
        sinon.match.string,
        sinon.match(job),
      );
      sinon.assert.calledOnceWithMatch(
        prismaCreateJobStub,
        sinon.match({
          data: sinon.match({
            id: sinon.match.string,
            name: sinon.match.string,
            type: "GMAT",
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
      const job = {
        epochFormat: "UTCGregorian",
        epoch: "2024-11-16T00:00:00Z",
        coordinateSystem: "EarthMJ2000Eq",
        stateElement: {
          type: "cartesian",
          x: 1000,
          y: 2000,
          z: 3000,
          vx: 1.5,
          vy: 2.5,
          vz: 3.5,
        },
        stateId: "cartesianStateExample",
      } as operations["createGMATJob"]["requestBody"]["content"]["application/json"];

      gmatQueueStub.add.throws();
      prisma.job.create = sinon.stub();

      const createdJob = await gmatJobQueueService.createGMATJob(job);
      sinon.assert.notCalled(prisma.job.create as SinonStub);
      expect(!createdJob.success && createdJob.error.status).to.equal(503);
    });

    it("should throw if Prisma is unavailable", async () => {
      const job = {
        epochFormat: "UTCGregorian",
        epoch: "2024-11-16T00:00:00Z",
        coordinateSystem: "EarthMJ2000Eq",
        stateElement: {
          type: "cartesian",
          x: 1000,
          y: 2000,
          z: 3000,
          vx: 1.5,
          vy: 2.5,
          vz: 3.5,
        },
        stateId: "cartesianStateExample",
      } as operations["createGMATJob"]["requestBody"]["content"]["application/json"];

      const returnJobStub = sinon.createStubInstance(Job);
      returnJobStub.id = "test-id";
      returnJobStub.data = job;
      gmatQueueStub.add.resolves(returnJobStub);
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

      const createdJob = await gmatJobQueueService.createGMATJob(job);
      sinon.assert.notCalled(gmatQueueStub.add);
      expect(!createdJob.success && createdJob.error.status).to.equal(503);
    });
  });

  describe("getGMATJob Service", () => {
    it("should get correct job from Prisma", async () => {
      const job = {
        epochFormat: "UTCGregorian",
        epoch: "2024-11-16T00:00:00Z",
        coordinateSystem: "EarthMJ2000Eq",
        stateElement: {
          type: "cartesian",
          x: 1000,
          y: 2000,
          z: 3000,
          vx: 1.5,
          vy: 2.5,
          vz: 3.5,
        },
        stateId: "cartesianStateExample",
      };

      const returnJobStub = { id: "test-id", requestData: job };
      prisma.job.findUnique = sinon.stub().resolves(returnJobStub);

      const returnedJob = await gmatJobQueueService.getGMATJob("test-id");

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
      const returnedJob = await gmatJobQueueService.getGMATJob("test-id");
      expect(!returnedJob.success && returnedJob.error.status).to.equal(404);
    });

    it("should throw if Prisma is unavailable", async () => {
      prisma.job.findUnique = sinon
        .stub()
        .withArgs({ where: { id: "test-id" } })
        .throws();
      const returnedJob = await gmatJobQueueService.getGMATJob("test-id");
      expect(!returnedJob.success && returnedJob.error.status).to.equal(503);
    });
  });
});
