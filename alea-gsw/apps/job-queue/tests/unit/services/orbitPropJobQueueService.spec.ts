/* eslint-disable @typescript-eslint/unbound-method */
import type { operations } from "@aleasat/job-queue-api";
import { Job, Queue } from "bullmq";
import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import sinon, { type SinonStub, type SinonStubbedInstance } from "sinon";
import { Queues } from "../../../src/bullmq/queues.js";
import { prisma } from "../../../src/repositories/prisma.js";
import orbitPropJobQueueService from "../../../src/services/orbitPropJobQueueService.js";

chaiUse(chaiAsPromised);

describe("services/orbitPropJobQueueService.ts", () => {
  let orbitPropQueueStub: SinonStubbedInstance<Queue>;

  beforeEach(() => {
    orbitPropQueueStub = sinon.createStubInstance(Queue);
    sinon.stub(Queues, "orbitProp").returns(orbitPropQueueStub);
  });

  afterEach(() => {
    sinon.restore();
  });

  describe("createOrbitPropJob Service", () => {
    it("should create a new orbit propagation job with BullMQ and Prisma and return the new job", async () => {
      const job = {
        tleLine1:
          "1 25544U 98067A   24151.54791667  .00016717  00000-0  10270-3 0  9992",
        tleLine2:
          "2 25544  51.6423 145.1223 0004461  33.1624 326.9870 15.50923407209465",
        simulationTime: 3600,
      } as operations["createOrbitJob"]["requestBody"]["content"]["application/json"];

      const returnJobStub = sinon.createStubInstance(Job);
      returnJobStub.id = "test-id";
      returnJobStub.data = job;
      returnJobStub.getState.resolves("waiting");
      orbitPropQueueStub.add.resolves(returnJobStub);
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

      const createdJob = await orbitPropJobQueueService.createOrbitPropJob(job);

      sinon.assert.calledOnceWithMatch(
        // eslint-disable-next-line @typescript-eslint/unbound-method
        orbitPropQueueStub.add,
        sinon.match.string,
        sinon.match(job),
      );
      sinon.assert.calledOnceWithMatch(
        prismaCreateJobStub,
        sinon.match({
          data: sinon.match({
            id: sinon.match.string,
            name: sinon.match.string,
            type: "ORBIT_PROPAGATION",
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
        tleLine1:
          "1 25544U 98067A   24151.54791667  .00016717  00000-0  10270-3 0  9992",
        tleLine2:
          "2 25544  51.6423 145.1223 0004461  33.1624 326.9870 15.50923407209465",
        simulationTime: 3600,
      } as operations["createOrbitJob"]["requestBody"]["content"]["application/json"];

      orbitPropQueueStub.add.throws();
      prisma.job.create = sinon.stub();

      const createdJob = await orbitPropJobQueueService.createOrbitPropJob(job);
      sinon.assert.notCalled(prisma.job.create as SinonStub);
      expect(!createdJob.success && createdJob.error.status).to.equal(503);
    });

    it("should throw if Prisma is unavailable", async () => {
      const job = {
        tleLine1:
          "1 25544U 98067A   24151.54791667  .00016717  00000-0  10270-3 0  9992",
        tleLine2:
          "2 25544  51.6423 145.1223 0004461  33.1624 326.9870 15.50923407209465",
        simulationTime: 3600,
      } as operations["createOrbitJob"]["requestBody"]["content"]["application/json"];

      const returnJobStub = sinon.createStubInstance(Job);
      returnJobStub.id = "test-id";
      returnJobStub.data = job;
      orbitPropQueueStub.add.resolves(returnJobStub);
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

      const createdJob = await orbitPropJobQueueService.createOrbitPropJob(job);
      sinon.assert.notCalled(orbitPropQueueStub.add);
      expect(!createdJob.success && createdJob.error.status).to.equal(503);
    });
  });

  describe("getOrbitPropJob Service", () => {
    it("should get correct job from Prisma", async () => {
      const job = {
        tleLine1:
          "1 25544U 98067A   24151.54791667  .00016717  00000-0  10270-3 0  9992",
        tleLine2:
          "2 25544  51.6423 145.1223 0004461  33.1624 326.9870 15.50923407209465",
        simulationTime: 3600,
      };

      const returnJobStub = { id: "test-id", requestData: job };
      prisma.job.findUnique = sinon.stub().resolves(returnJobStub);

      const returnedJob =
        await orbitPropJobQueueService.getOrbitPropJob("test-id");

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
        await orbitPropJobQueueService.getOrbitPropJob("test-id");
      expect(!returnedJob.success && returnedJob.error.status).to.equal(404);
    });

    it("should throw if Prisma is unavailable", async () => {
      prisma.job.findUnique = sinon
        .stub()
        .withArgs({ where: { id: "test-id" } })
        .throws();
      const returnedJob =
        await orbitPropJobQueueService.getOrbitPropJob("test-id");
      expect(!returnedJob.success && returnedJob.error.status).to.equal(503);
    });
  });
});
