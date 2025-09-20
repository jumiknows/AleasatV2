import type { StateElement } from "@aleasat/orbit-propagation-api";
import { expect } from "chai";
import {
  db,
  dbKeys,
  dbStateElement,
  dbStateId,
  loadOrbitPropagation,
  state,
} from "../../../src/repositories/orbitPropagationStore.js";

describe("repositories/orbitPropagationStore.ts", () => {
  describe("loadOrbitPropagation function", () => {
    afterEach(async () => {
      dbKeys.length = 0;
      Object.keys(db).forEach((key) => delete db[key]);
      Object.keys(dbStateElement).forEach((key) => {
        //@ts-expect-error
        delete dbStateElement[key];
      });
      state.ready = false;
    });

    describe("loading", () => {
      const stateId = "dab8f3fa-e5c9-41b4-90f4-4512cc5f4974";
      const stateElement: StateElement = {
        type: "keplerian",
        sma: 5441,
        ecc: 541,
        inc: 6213,
        raan: 231,
        aop: 6846,
        ta: 873,
      };

      beforeEach(() => {
        loadOrbitPropagation({
          stateId,
          stateElement,
          result: [
            { utcGregorian: 1709410406887, x: 79, y: 644, z: 9872 },
            { utcGregorian: 1709410477887, x: 4567, y: 5874, z: 243 },
            { utcGregorian: 1709696807843, x: 68417, y: 21.48, z: 874 },
            { utcGregorian: 1709696860843, x: 7534, y: 425.4, z: 8658 },
            { utcGregorian: 1709883208751, x: 1.1894, y: 15621, z: 8745 },
            { utcGregorian: 1709883288751, x: 452, y: 63, z: 5.46 },
          ],
        });
      });

      it("should load the dbKeys", () => {
        expect(Object.keys(db)).to.deep.equal([
          "2024-03-02",
          "2024-03-06",
          "2024-03-08",
        ]);
        expect(Object.keys(db)).to.deep.equal([
          "2024-03-02",
          "2024-03-06",
          "2024-03-08",
        ]);
      });

      it("should load the stateElement", () => {
        expect(dbStateElement).to.equal(stateElement);
      });

      it("should load the stateElementId", () => {
        expect(dbStateId).to.equal(stateId);
      });

      it("should load the db", () => {
        expect(new Float64Array(db["2024-03-02"])).to.deep.equal(
          new Float64Array([
            1709410406887, 79, 644, 9872, 1709410477887, 4567, 5874, 243,
          ]),
        );
        expect(new Float64Array(db["2024-03-06"])).to.deep.equal(
          new Float64Array([
            1709696807843, 68417, 21.48, 874, 1709696860843, 7534, 425.4, 8658,
          ]),
        );
        expect(new Float64Array(db["2024-03-08"])).to.deep.equal(
          new Float64Array([
            1709883208751, 1.1894, 15621, 8745, 1709883288751, 452, 63, 5.46,
          ]),
        );
      });

      describe("load second time", () => {
        const stateId = "ed9f4a7a-d2be-4bc3-90d3-43536432e508";
        const stateElement: StateElement = {
          type: "keplerian",
          sma: 5441,
          ecc: 541,
          inc: 6213,
          raan: 231,
          aop: 6846,
          ta: 873,
        };

        beforeEach(() => {
          loadOrbitPropagation({
            stateId,
            stateElement,
            result: [
              { utcGregorian: 1709696801564, x: 16, y: 21.1148, z: 8714 },
              { utcGregorian: 1709696865646, x: 74, y: 45.4, z: 2158 },
              { utcGregorian: 1709883201236, x: 1.94, y: 156, z: 3245 },
              { utcGregorian: 1709883282334, x: 42, y: 543, z: 426.46 },
              { utcGregorian: 1710110405746, x: 21, y: 2314, z: 122 },
              { utcGregorian: 1710110409871, x: 464, y: 5544, z: 783 },
            ],
          });
        });

        it("should overwrite the dbKeys", () => {
          expect(dbKeys).to.deep.equal([
            "2024-03-06",
            "2024-03-08",
            "2024-03-10",
          ]);
          expect(Object.keys(db)).to.deep.equal([
            "2024-03-06",
            "2024-03-08",
            "2024-03-10",
          ]);
        });

        it("should overwrite the stateElement", () => {
          expect(dbStateElement).to.equal(stateElement);
        });

        it("should overwrite the stateElementId", () => {
          expect(dbStateId).to.equal(stateId);
        });

        it("should overwrite the db", () => {
          expect(new Float64Array(db["2024-03-06"])).to.deep.equal(
            new Float64Array([
              1709696801564, 16, 21.1148, 8714, 1709696865646, 74, 45.4, 2158,
            ]),
          );
          expect(new Float64Array(db["2024-03-08"])).to.deep.equal(
            new Float64Array([
              1709883201236, 1.94, 156, 3245, 1709883282334, 42, 543, 426.46,
            ]),
          );
          expect(new Float64Array(db["2024-03-10"])).to.deep.equal(
            new Float64Array([
              1710110405746, 21, 2314, 122, 1710110409871, 464, 5544, 783,
            ]),
          );
        });
      });
    });
  });
});
