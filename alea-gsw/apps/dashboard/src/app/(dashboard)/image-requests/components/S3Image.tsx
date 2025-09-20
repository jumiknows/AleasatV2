import { auth } from "@/auth";
import { GetObjectCommand, S3Client } from "@aws-sdk/client-s3";
import { getSignedUrl } from "@aws-sdk/s3-request-presigner";
import CardMedia from "@mui/material/CardMedia";
import { env } from "next-runtime-env";
import type { CSSProperties } from "react";

export const dynamic = "force-dynamic";

export default async function S3Image({
  Bucket,
  Key,
  style,
}: {
  Bucket: string;
  Key: string;
  style?: CSSProperties;
}) {
  const session = await auth();
  if (session === null)
    return <CardMedia style={{ width: "100%", aspectRatio: "34/24" }} />;

  // can't connect to s3
  if (!session.sts)
    return <CardMedia style={{ width: "100%", aspectRatio: "34/24" }} />;

  const s3 = new S3Client({
    endpoint: env("NEXT_PUBLIC_MINIO_URL")!,
    credentials: {
      accessKeyId: session.sts.AccessKeyId,
      secretAccessKey: session.sts.SecretAccessKey,
      sessionToken: session.sts.SessionToken,
    },
    region: "ubc",
    forcePathStyle: true,
  });

  const signedUrl = await getSignedUrl(
    s3,
    new GetObjectCommand({
      Bucket,
      Key,
    }),
    { expiresIn: 40 },
  );

  return <img style={style} src={`${signedUrl}`} alt="The Image" />;
}
