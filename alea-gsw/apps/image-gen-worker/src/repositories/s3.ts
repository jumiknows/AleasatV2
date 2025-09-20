import {
  CreateBucketCommand,
  HeadBucketCommand,
  S3Client,
  S3ServiceException,
} from "@aws-sdk/client-s3";
import logger from "../utils/logger.js";

const s3Endpoint = process.env.S3_ENDPOINT ?? "http://localhost:443";

export const s3 = new S3Client({
  endpoint: s3Endpoint,
  region: "null", // needs a region param
  credentials: {
    accessKeyId: process.env.S3_ACCESS_KEY_ID ?? "",
    secretAccessKey: process.env.S3_SECRET_ACCESS_KEY ?? "",
  },
  forcePathStyle: true,
});

export const s3Bucket = "gmat";

(async () => {
  // stupid bodge to pass smoke test
  if (!process.env.S3_ENDPOINT) {
    return;
  }

  try {
    await s3.send(new HeadBucketCommand({ Bucket: s3Bucket }));
  } catch (err) {
    if (err instanceof S3ServiceException && err.name === "NotFound") {
      await s3.send(new CreateBucketCommand({ Bucket: s3Bucket }));
      logger.info(`Bucket '${s3Bucket}' created.`);
    } else {
      logger.error(err, "Error checking/creating bucket");
      throw err;
    }
  }
})();
