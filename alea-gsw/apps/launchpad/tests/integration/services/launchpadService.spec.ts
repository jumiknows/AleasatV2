import type { operations } from "@aleasat/launchpad-api";
import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import chaiSubset from "chai-subset";
import { prisma } from "../../../src/repositories/prisma.js";
import launchpadService from "../../../src/services/launchpadService.js";

chaiUse(chaiAsPromised);
chaiUse(chaiSubset);

describe("services/launchpadService.ts", () => {
  afterEach(async () => {
    await prisma.$transaction([
      prisma.launchpad.deleteMany(),
      prisma.userSession.deleteMany(),
    ]);
  });

  describe("createLaunchpad Service", () => {
    it("should create a launchpad", async () => {
      // Arrange
      const launchpad = {
        name: "Launchpad 1",
        node: "Node 1",
        port: "/dev/ttyUSB0",
        serviceName: "Service 1",
        fwCommit: "123456",
      } satisfies operations["createLaunchpad"]["requestBody"]["content"]["application/json"];

      // Act
      const createdLaunchpad =
        await launchpadService.createLaunchpad(launchpad);

      expect(createdLaunchpad.success).to.be.true;
      if (!createdLaunchpad.success)
        throw new Error("Expected success to be true");

      // Assert
      const launchpadInDb = await prisma.launchpad.findUnique({
        where: { id: createdLaunchpad.data.id },
        include: { userSession: true },
      });

      expect(launchpadInDb).to.exist;
      expect(createdLaunchpad.data).to.containSubset({
        ...launchpadInDb,
        userSessionId: null,
      });
    });
  });
});
