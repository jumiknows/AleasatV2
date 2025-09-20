import "../env.js";

import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import sinon from "sinon";
import { missionQueueApi } from "../../../src/apis/pass-scheduling.js";
import { prisma } from "../../../src/repositories/prisma.js";
import missionService from "../../../src/services/missionService.js";

chaiUse(chaiAsPromised);

describe("services/missionService.ts", () => {
  beforeEach(() => {
    missionQueueApi.queueMission = sinon.stub().callsFake(
      async (v) =>
        await new Promise((resolve) =>
          resolve({
            data: v.mission1,
          }),
        ),
    );
  });

  afterEach(() => {
    sinon.restore();
  });

  describe("getMissions Service", () => {
    it("should return all missions", async () => {
      const expectedMissions = [
        {
          id: "d7934c5e-f374-4ecd-8ad6-d9537e510177",
          fwVersion: "31acs35",
          createdAt: "2022-01-01T00:00:00.000Z",
          userId: "someId",
          scheduledAt: "2022-01-02T00:21:00.000Z",
          status: "created",
          commands: [],
        },
        {
          id: "a1b2c3d4-e5f6-g7h8-i9j0-k1l2m3n4o5p6",
          fwVersion: "42xyz",
          createdAt: "2022-01-03T00:00:30.000Z",
          userId: "anotherId",
          scheduledAt: "2022-01-04T09:00:00.000Z",
          status: "sent",
          commands: [
            {
              sequenceNumber: 0,
              commandId: 32,
              commandName: "TE",
              arguments: ["wd"],
              sentAt: null,
              ranAt: null,
              responses: [],
            },
          ],
        },
      ];

      prisma.mission.findMany = sinon.stub().resolves(
        expectedMissions.map((mission) => ({
          ...mission,
          createdAt: new Date(mission.createdAt),
          scheduledAt: new Date(mission.scheduledAt),
        })),
      );

      prisma.mission.update = sinon.stub().resolves();

      const missions = await missionService.getMissions({});

      expect(missions.data).to.deep.equal(expectedMissions);
    });
  });

  describe("when sent is true", () => {
    it("should return missions that is sent", async () => {
      const missionFindManyStub = sinon.stub().resolves([]);
      prisma.mission.findMany = missionFindManyStub;

      await missionService.getMissions({ sent: true });

      expect(missionFindManyStub.firstCall.args[0]).to.be.deep.include({
        where: { status: "sent" },
      });
    });
  });

  describe("getMission Service", () => {
    it("should return the mission with the specified missionId", async () => {
      const missionId = "d7934c5e-f374-4ecd-8ad6-d9537e510177";
      const expectedMission = {
        id: missionId,
        fwVersion: "31acs35",
        createdAt: "2022-01-01T00:00:00.000Z",
        userId: "someId",
        scheduledAt: "2022-01-02T00:21:00.000Z",
        sent: false,
        commands: [
          {
            sequenceNumber: 0,
            commandId: 32,
            commandName: "TE",
            arguments: ["wd"],
            sentAt: null,
            ranAt: null,
            responses: [],
          },
        ],
      };
      prisma.mission.findUnique = sinon.stub().resolves({
        ...expectedMission,
        createdAt: new Date(expectedMission.createdAt),
        scheduledAt: new Date(expectedMission.scheduledAt),
      });

      const mission = await missionService.getMission(missionId);
      expect(mission.data).to.deep.equal(expectedMission);
    });
  });

  describe("createMission Service", () => {
    it("should create a mission", async () => {
      const mission = {
        id: "d7934c5e-f374-4ecd-8ad6-d9537e510177",
        fwVersion: "31acs35",
        userId: "someId",
        createdAt: "2022-01-01T00:00:00.000Z",
        scheduledAt: "2020-01-03T00:00:10.000Z",
        sent: false,
        commands: [
          {
            sequenceNumber: 0,
            commandId: 322,
            commandName: "TEW",
            arguments: [],
          },
        ],
      };

      prisma.mission.create = sinon.stub().resolves({
        ...mission,
        scheduledAt: new Date(mission.scheduledAt),
        createdAt: new Date(mission.createdAt),
      });

      const createdMission = await missionService.createMission("someId", {
        ...mission,
      });

      expect(createdMission.success && createdMission.data).to.containSubset({
        ...mission,
        userId: "someId",
      });
    });
  });
});
