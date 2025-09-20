import "../env.js";

import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import chaiSubset from "chai-subset";
import sinon from "sinon";
import { prisma } from "../../../src/repositories/prisma.js";
import commandService from "../../../src/services/commandService.js";

chaiUse(chaiAsPromised);
chaiUse(chaiSubset);

describe("services/commandService.ts", () => {
  afterEach(() => {
    sinon.restore();
  });

  describe("patchCommand Service", () => {
    describe("should patch a command", async () => {
      it("should return the patched command", async () => {
        // Arrange
        const patch = {
          ranAt: "2024-01-01T00:00:00.000Z",
          responses: ["response1", "response2"],
        };

        const patchedCommand = {
          missionId: "d7934c5e-f374-4ecd-8ad6-d9537e510177",
          cmdInstId: 231,
          id: "82b94d21-9700-438a-bf18-f99dd26aeb60",
          sequenceNumber: 0,
          commandId: 43,
          commandName: "PING",
          timeOffset: 34,
          arguments: [],
          sentAt: "2022-01-01T00:00:00.000Z",
          ranAt: "2024-01-01T00:00:00.000Z",
          responses: ["response1", "response2"],
        };

        // Stub the necessary dependencies
        const commandUpdateStub = sinon.stub().resolves({
          ...patchedCommand,
          sentAt: new Date(patchedCommand.sentAt),
          ...patch,
          ranAt: new Date(patch.ranAt),
        });
        prisma.command.update = commandUpdateStub;

        // Act
        const result = await commandService.patchCommand(
          patchedCommand.missionId,
          patchedCommand.id,
          patch,
        );

        // Assert
        expect(result.data).to.deep.equal(patchedCommand);
        expect(commandUpdateStub.firstCall.firstArg).to.containSubset({
          where: {
            id: patchedCommand.id,
            AND: {
              missionId: patchedCommand.missionId,
            },
          },
          data: {
            responses: ["response1", "response2"],
            ranAt: new Date("2024-01-01T00:00:00.000Z"),
          },
        });
      });
    });
  });
});
