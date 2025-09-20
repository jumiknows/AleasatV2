import "../env.js";

import type { Mission, operations } from "@aleasat/api-lib";
import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import missionExecutionService from "../../../src/services/missionExecutionService.js";

chaiUse(chaiAsPromised);

describe("services/missionExecutionService.ts", () => {
  describe("executeMission Service", () => {
    it.skip("generate correct base64 image", async function () {
      const missionToExecute = {
        id: "fd2fef6e-9032-4bcf-b193-0fa93bd44b1b",
        userId: "someUserId",
        fwVersion: "dwadw",
        createdAt: new Date("2021-09-01T00:00:00.000Z").toISOString(),
        scheduledAt: null,
        sent: false,
        commands: [
          // "GET_UPTIME": {
          //   "id": 30,
          //   "args": [],
          //   "resp": [
          //     {
          //       "seconds": "u32"
          //     }
          //   ]
          // }
          {
            id: "ce8441fb-bb5e-468b-bf9e-8ae889304fc6",

            sequenceNumber: 0,
            timeOffset: 0,

            commandName: "GET_UPTIME",
            commandId: 30,
            arguments: [],

            sentAt: null,

            ranAt: null,
            responses: [],
          },
        ],
      } satisfies Mission;
      const missionSchedule = {
        id: "fd2fef6e-9032-4bcf-b193-0fa93bd44b1b",
        initialOverheadStart: new Date(
          "2021-09-01T00:00:00.000Z",
        ).toISOString(),
        commands: [
          {
            id: "ce8441fb-bb5e-468b-bf9e-8ae889304fc6",
            sequenceNumber: 0,
            scheduledAt: new Date().toISOString(),
          },
        ],
      } satisfies operations["scheduleMissionExecution"]["requestBody"]["content"]["application/json"];

      const executedMission = {
        id: "fd2fef6e-9032-4bcf-b193-0fa93bd44b1b",
        userId: "someUserId",
        fwVersion: "dwadw",
        createdAt: new Date("2021-09-01T00:00:00.000Z").toISOString(),
        scheduledAt: null,
        sent: true,
        commands: [
          {
            id: "ce8441fb-bb5e-468b-bf9e-8ae889304fc6",

            sequenceNumber: 0,
            timeOffset: 0,

            commandName: "GET_UPTIME",
            commandId: 30,
            arguments: [],

            sentAt: new Date().toISOString(),

            ranAt: new Date().toISOString(),
            responses: ["1230"],
          },
        ],
      } satisfies Mission;

      const res = await missionExecutionService.executeMission(
        missionSchedule,
        missionToExecute,
      );

      expect(res).deep.equal({
        data: executedMission,
      });
    });
  });
});
