import { use as chaiUse, expect } from "chai";
import chaiAsPromised from "chai-as-promised";
import imageRequestService from "../../../src/services/imageRequestService.js";
import { getImageRequestPreviewBase64 } from "./imageRequestServiceConstant.js";

chaiUse(chaiAsPromised);

describe("services/imageRequestService", () => {
  describe("getImageRequestPreview Service", () => {
    it.skip("generate correct base64 image", async function () {
      this.timeout(10000);

      const res = await imageRequestService.getImageRequestPreview({
        SatRotX: -1.9631279225529905,
        SatRotY: -0.48167047998123996,
        SatRotZ: -2.299840649973626,
        SatLat: 49.531022337154305,
        SatLng: -123.11226387630973,
        SatAlt: 431.5162732406352,
      });

      expect(res).deep.equal({
        data: {
          base64: getImageRequestPreviewBase64,
        },
      });
    });
  });
});
