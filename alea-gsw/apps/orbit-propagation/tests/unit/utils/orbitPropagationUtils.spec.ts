import { expect } from "chai";
import {
  findLocalMaxima,
  getDayFromTime,
} from "../../../src/utils/orbitPropagationUtils.js";

describe("utils/orbitPropagationUtils.ts", () => {
  describe("findLocalMaxima function", () => {
    it("should able to return local maxima", () => {
      expect(
        findLocalMaxima([
          { alt: 2 },
          { alt: 5 },
          { alt: 3 },
          { alt: 8 },
          { alt: 4 },
          { alt: 7 },
          { alt: 1 },
        ]),
      ).to.deep.equal([{ alt: 5 }, { alt: 8 }, { alt: 7 }]);
    });
  });

  describe("getDayFromTime function", () => {
    it("should accept iso date", () => {
      expect(getDayFromTime("2024-01-17T06:10:21.387Z")).to.equal("2024-01-17");
    });

    it("should accept epoch", () => {
      expect(getDayFromTime(1702793499000)).to.equal("2023-12-17");
    });

    it("should accept javascript Date object", () => {
      expect(getDayFromTime(new Date(1588752192000))).to.equal("2020-05-06");
    });
  });
});
