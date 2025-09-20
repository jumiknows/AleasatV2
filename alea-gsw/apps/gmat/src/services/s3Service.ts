import { PutObjectCommand } from "@aws-sdk/client-s3";
import { s3 } from "../repositories/s3.js";

const uploadObject = async (
  key: string,
  bucket: string,
  data: string,
  dataType: string = "application/json",
): Promise<void> => {
  await s3.send(
    new PutObjectCommand({
      Bucket: bucket,
      Key: key,
      Body: data,
      ContentType: dataType,
    }),
  );
};

export { uploadObject };
