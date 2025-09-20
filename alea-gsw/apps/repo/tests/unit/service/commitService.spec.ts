import fs from "fs";
import { expect } from "chai";
import sinon from "sinon";
import commitService from "../../../src/services/commitService.js";

afterEach(() => {
  sinon.restore();
});

describe("services/commitService.ts", () => {
  describe("getFile", () => {
    it("should return the file content", async () => {
      const expectedFile = fs.readFileSync(
        "tests/unit/data/docker_git.sh",
        "utf-8",
      );

      const actualFile = await commitService.getFile(
        "7f5462142e6dee8c60dd72e18f1f0f8fb69bb701",
        { filePath: "tools/docker/docker_git.sh" },
      );

      expect(actualFile.success).to.be.true;
      if (!actualFile.success) return;
      expect(actualFile.data).equal(expectedFile);
    });
  });

  describe("getObcConfig", async () => {
    const expectedObcConfig = JSON.parse(
      fs.readFileSync("tests/unit/data/obcConfig.json", "utf-8"),
    );

    it("should return the configs", async () => {
      const actualObcConfig = await commitService.getObcConfig(
        "7f5462142e6dee8c60dd72e18f1f0f8fb69bb701",
      );

      expect(actualObcConfig.success).to.be.true;
      if (!actualObcConfig.success) return;
      expect(actualObcConfig.data).deep.equal(expectedObcConfig);
    });
  });

  describe("getObcProto", async () => {
    const actualObcProtoWithoutTestSpec = fs.readFileSync(
      "tests/unit/data/obcProtoWithoutTestSpec.proto",
      "utf-8",
    );
    const actualObcProtoWithTestSpec = fs.readFileSync(
      "tests/unit/data/obcProtoWithTestSpec.proto",
      "utf-8",
    );

    it("should return the proto with test spec", async () => {
      const actualObcProto = await commitService.getObcProto(
        "7f5462142e6dee8c60dd72e18f1f0f8fb69bb701",
        true,
      );

      expect(actualObcProto.success).to.be.true;
      if (!actualObcProto.success) return;
      expect(actualObcProto.data).equal(actualObcProtoWithTestSpec);
    });

    it("should return the proto without test spec", async () => {
      const actualObcProto = await commitService.getObcProto(
        "7f5462142e6dee8c60dd72e18f1f0f8fb69bb701",
        false,
      );

      expect(actualObcProto.success).to.be.true;
      if (!actualObcProto.success) return;
      expect(actualObcProto.data).equal(actualObcProtoWithoutTestSpec);
    });
  });

  describe("getObcGrpcService", async () => {
    const actualObcGrpcServiceWithoutTestSpec = fs.readFileSync(
      "tests/unit/data/obcGrpcServiceWithoutTestSpec.py",
      "utf-8",
    );
    const actualObcGrpcServiceWithTestSpec = fs.readFileSync(
      "tests/unit/data/obcGrpcServiceWithTestSpec.py",
      "utf-8",
    );

    it("should return the grpc service with test spec", async () => {
      const actualObcGrpcService = await commitService.getObcGrpcService(
        "7f5462142e6dee8c60dd72e18f1f0f8fb69bb701",
        true,
      );

      expect(actualObcGrpcService.success).to.be.true;
      if (!actualObcGrpcService.success) return;
      expect(actualObcGrpcService.data).equal(actualObcGrpcServiceWithTestSpec);
    });

    it("should return the grpc service without test spec", async () => {
      const actualObcGrpcService = await commitService.getObcGrpcService(
        "7f5462142e6dee8c60dd72e18f1f0f8fb69bb701",
        false,
      );

      expect(actualObcGrpcService.success).to.be.true;
      if (!actualObcGrpcService.success) return;
      expect(actualObcGrpcService.data).equal(
        actualObcGrpcServiceWithoutTestSpec,
      );
    });
  });
});
