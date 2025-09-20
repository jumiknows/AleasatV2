import { expect } from "chai";
import { deserializeArgsResp, serializeArgsResp } from "../../src/index.js";
import { argsTransformers } from "../../src/utils/specTransformers.js";

describe("index.ts", () => {
  const spec = argsTransformers([
    {
      numberArg: "u32",
    },
    {
      stringArg: "string",
    },
    {
      booleanArg: "bool",
    },
    {
      dateTimeArg: "datetime",
    },
  ]);

  describe("serializeArgsResp function", () => {
    it("should serialize Args or Resp", () => {
      const arg = {
        numberArg: 1,
        stringArg: "string",
        booleanArg: true,
        dateTimeArg: new Date("2024-06-27T02:43:02.429Z"),
      };

      const serializedArgs = serializeArgsResp({
        argsRespSpec: spec,
        argsResp: arg,
      });

      expect(serializedArgs).to.deep.equal([
        "1",
        "string",
        "true",
        "2024-06-27T02:43:02.429Z",
      ]);
    });
  });

  describe("deserializeArgsResp function", () => {
    it("should deserialize Args or Resp", () => {
      const args = ["1", "string", "true", "2024-06-27T02:43:02.429Z"];

      const deserializedArgs = deserializeArgsResp({
        argsRespSpec: spec,
        argsResp: args,
      });

      expect(deserializedArgs).to.deep.equal({
        numberArg: 1,
        stringArg: "string",
        booleanArg: true,
        dateTimeArg: new Date("2024-06-27T02:43:02.429Z"),
      });
    });
  });
});
