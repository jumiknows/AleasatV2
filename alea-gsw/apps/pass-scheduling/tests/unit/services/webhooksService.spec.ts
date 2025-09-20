import crypto from "node:crypto";
import {
  PassesFromJSON,
  type components,
} from "@aleasat/orbit-propagation-api";
import { expect } from "chai";
import { readFile } from "fs/promises";
import sinon from "sinon";
import * as td from "testdouble";
import { orbitPropagationApi } from "../../../src/repositories/backend.js";
import { updateGroundStationPasses } from "../../../src/services/webhooksService.js";

afterEach(() => {
  sinon.restore();
  td.reset();
});

describe("services/webhooksService.ts", () => {
  describe("updateGroundStationPasses function", () => {
    it("works", async function () {
      const storedPasses = await readFile(
        "tests/unit/services/mockData/storedPasses.json",
        "utf-8",
      ).then((file) =>
        JSON.parse(file).map((pass) => ({
          ...pass,
          predictedRiseSet: {
            ...pass.predictedRiseSet,
            rise: new Date(pass.predictedRiseSet.rise * 1000),
            set: new Date(pass.predictedRiseSet.set * 1000),
          },
          previousPredictedRiseSet: pass.previousPredictedRiseSet.map(
            (riseSet) => ({
              ...riseSet,
              rise: new Date(riseSet.rise * 1000),
              set: new Date(riseSet.set * 1000),
            }),
          ),
        })),
      );
      const orbitPropagationPasses = (await readFile(
        "tests/unit/services/mockData/orbitPropagationPasses.json",
        "utf-8",
      )
        .then((file) => JSON.parse(file))
        .then((pass) => ({
          ...pass,
          passes: pass.passes.map((pass) => ({
            ...pass,
            rise: {
              ...pass.rise,
              time: new Date(pass.rise.t as string),
            },
            set: {
              ...pass.set,
              time: new Date(pass.set.t as string),
            },
            culmination: {
              ...pass.culmination,
              time: new Date(pass.culmination.t as string),
            },
          })),
        }))) as components["schemas"]["Passes"];
      const expectedResult = await readFile(
        "tests/unit/services/mockData/expectedResult.json",
        "utf-8",
      ).then((file) =>
        JSON.parse(file).map((pass) => ({
          ...pass,
          predictedRiseSet: {
            ...pass.predictedRiseSet,
            rise: new Date(pass.predictedRiseSet.rise as string),
            set: new Date(pass.predictedRiseSet.set as string),
          },
        })),
      );

      const passFindManyStub = sinon.stub().resolves(storedPasses);
      const transactionStub = { pass: { findMany: passFindManyStub } };

      sinon
        .stub(orbitPropagationApi, "findFullPasses")
        // eslint-disable-next-line @typescript-eslint/no-unsafe-argument
        .resolves(PassesFromJSON(orbitPropagationPasses));

      sinon
        .stub(crypto, "randomUUID")
        .returns("00000000-0000-0000-0000-000000000000");

      const passes = await updateGroundStationPasses({
        // @ts-expect-error stub only functional used
        tx: transactionStub,
        groundStation: {
          id: 1,
          lat: 49.2664803,
          lng: -123.2522746,
          minElevation: 30,
        },
        now: new Date(1709795719000),
      });

      passes.sort(
        (a, b) =>
          a.predictedRiseSet.rise.getTime() - b.predictedRiseSet.rise.getTime(),
      );

      expect(passes).to.deep.equal(expectedResult);
    });
  });
});
