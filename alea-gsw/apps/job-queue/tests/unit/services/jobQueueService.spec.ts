/* eslint-disable @typescript-eslint/unbound-method */
import { Job, Queue } from "bullmq";
import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import sinon, { type SinonStubbedInstance } from "sinon";
import { Queues } from "../../../src/bullmq/queues.js";
import { prisma } from "../../../src/repositories/prisma.js";
import jobQueueService from "../../../src/services/jobQueueService.js";

chaiUse(chaiAsPromised);

describe("services/jobQueueService.ts", () => {
  let gmatQueueStub: SinonStubbedInstance<Queue>;

  beforeEach(() => {
    gmatQueueStub = sinon.createStubInstance(Queue);
    sinon.stub(Queues, "gmat").returns(gmatQueueStub);
  });

  afterEach(() => {
    sinon.restore();
  });

  describe("deleteJob Service", () => {
    it("should delete job from correct BullMQ queue", async () => {
      const prismaJob = { id: "test-id", type: "GMAT" };
      const prismaDeleteStub = sinon.stub();
      prisma.$transaction = sinon
        .stub()
        .callsFake(async (callback: (tx: any) => Promise<any>) => {
          const txStub = {
            job: {
              findUnique: sinon
                .stub()
                .withArgs({ where: { id: "test-id" } })
                .resolves(prismaJob),
              delete: prismaDeleteStub,
            },
          };
          return await callback(txStub);
        });
      const bullMQJob = sinon.createStubInstance(Job);
      gmatQueueStub.getJob.withArgs("test-id").resolves(bullMQJob);

      const response = await jobQueueService.deleteJob("test-id");

      sinon.assert.calledOnceWithMatch(
        prismaDeleteStub,
        sinon.match({
          where: {
            id: "test-id",
          },
        }),
      );
      sinon.assert.calledOnce(bullMQJob.remove);

      expect(response.success).to.be.true;
    });

    it("should throw if job doesn't exist", async () => {
      const prismaDeleteStub = sinon.stub();
      prisma.$transaction = sinon
        .stub()
        .callsFake(async (callback: (tx: any) => Promise<any>) => {
          const txStub = {
            job: {
              findUnique: sinon
                .stub()
                .withArgs({ where: { id: "test-id" } })
                .resolves(null),
              delete: prismaDeleteStub,
            },
          };
          return await callback(txStub);
        });

      const response = await jobQueueService.deleteJob("test-id");

      sinon.assert.notCalled(prismaDeleteStub);

      expect(!response.success && response.error.status).to.equal(404);
    });

    it("should not delete job if in progress", async () => {
      const prismaJob = { id: "test-id", type: "GMAT" };
      const prismaDeleteStub = sinon.stub();
      prisma.$transaction = sinon
        .stub()
        .callsFake(async (callback: (tx: any) => Promise<any>) => {
          const txStub = {
            job: {
              findUnique: sinon
                .stub()
                .withArgs({ where: { id: "test-id" } })
                .resolves(prismaJob),
              delete: prismaDeleteStub,
            },
          };
          return await callback(txStub);
        });
      const bullMQJob = sinon.createStubInstance(Job);
      gmatQueueStub.getJob.withArgs("test-id").resolves(bullMQJob);
      bullMQJob.remove.throws(); // BullMQ will throw if attempting to delete an active job

      const response = await jobQueueService.deleteJob("test-id");

      sinon.assert.notCalled(prismaDeleteStub);

      // job not found in waiting queue
      expect(!response.success && response.error.status).to.equal(404);
    });

    it("should throw if BullMQ is unavailable", async () => {
      const prismaJob = { id: "test-id", type: "GMAT" };
      const prismaDeleteStub = sinon.stub();
      prisma.$transaction = sinon
        .stub()
        .callsFake(async (callback: (tx: any) => Promise<any>) => {
          const txStub = {
            job: {
              findUnique: sinon
                .stub()
                .withArgs({ where: { id: "test-id" } })
                .resolves(prismaJob),
              delete: prismaDeleteStub,
            },
          };
          return await callback(txStub);
        });
      gmatQueueStub.getJob.withArgs("test-id").throws();

      const response = await jobQueueService.deleteJob("test-id");

      sinon.assert.notCalled(prismaDeleteStub);
      expect(!response.success && response.error.status).to.equal(503);
    });

    it("should throw if Prisma is unavailable", async () => {
      prisma.$transaction = sinon
        .stub()
        .callsFake(async (callback: (tx: any) => Promise<any>) => {
          const txStub = {
            job: {
              findUnique: sinon
                .stub()
                .withArgs({ where: { id: "test-id" } })
                .throws(),
            },
          };
          return await callback(txStub);
        })
        .throws();
      const response = await jobQueueService.deleteJob("test-id");
      expect(!response.success && response.error.status).to.equal(503);
    });
  });
});
