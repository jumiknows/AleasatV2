import { use as chaiUse, expect } from "chai";
import chaiSubset from "chai-subset";
import type { ArgType } from "../../../src/types.js";
import { argsTransformers } from "../../../src/utils/specTransformers.js";

chaiUse(chaiSubset);

describe("utils/specTransformers.ts", () => {
  describe("argsTransformers function", () => {
    it("should combine array of args or Resps into a single array", () => {
      const argsResp = [
        {
          lon: "f32[13]",
        },
        {
          lm: "f32[3][6]",
        },
        {
          lat: "f32",
        },
        {
          alt: "datetime",
        },
        {
          year: "u8",
        },
        {
          month: "bytes",
        },
        {
          day: "string",
        },
      ] as Record<string, ArgType>[];

      const combined = argsTransformers(argsResp);

      const expected = [
        {
          name: "lon",
          specType: "f32[13]",
          type: "array",
          length: 13,
          elementType: {
            type: "number",
            float: true,
            range: { min: -3.402823466e38, max: 3.402823466e38 },
          },
        },
        {
          name: "lm",
          specType: "f32[3][6]",
          type: "matrix",
          row: 6,
          column: 3,
          elementType: {
            type: "number",
            float: true,
            range: { min: -3.402823466e38, max: 3.402823466e38 },
          },
        },
        {
          name: "lat",
          specType: "f32",
          type: "number",
          float: true,
          range: { min: -3.402823466e38, max: 3.402823466e38 },
        },
        {
          name: "alt",
          specType: "datetime",
          type: "date",
        },
        {
          name: "year",
          specType: "u8",
          type: "number",
          float: false,
          range: { min: 0, max: 255 },
        },
        {
          name: "month",
          specType: "bytes",
          type: "buffer",
        },
        {
          name: "day",
          specType: "string",
          type: "string",
        },
      ];

      combined.forEach((arg) => {
        // @ts-expect-error pretent its not there
        delete arg.serialize;
        // @ts-expect-error pretent its not there
        delete arg.deserialize;
        // @ts-expect-error pretent its not there
        delete arg.constructor;
        "elementType" in arg &&
          // @ts-expect-error pretent its not there
          delete arg.elementType.serialize &&
          // @ts-expect-error pretent its not there
          delete arg.elementType.deserialize &&
          // @ts-expect-error pretent its not there
          delete arg.elementType.constructor;
      });
      expect(combined).to.containSubset(expected);
    });
  });
});
