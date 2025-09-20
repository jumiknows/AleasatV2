import { auth, signIn } from "@/auth";
import SimFetcherGmat from "@/components/aleasim/gmat/SimFetcherGmat";
import { GetObjectCommand, S3Client } from "@aws-sdk/client-s3";
import { getSignedUrl } from "@aws-sdk/s3-request-presigner";
import { env } from "process";

export default async function JobPage({
  params,
}: {
  params: Promise<{ jobId: string }>;
}) {
  const session = await auth();
  const { jobId } = await params;

  if (!session?.sts) {
    await signIn();
    return;
  }

  const s3 = new S3Client({
    endpoint: process.env.NEXT_PUBLIC_MINIO_URL,
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
      Bucket: "gmat",
      Key: jobId,
    }),
    { expiresIn: 60 },
  );

  return (
    <div style={{ width: "100%", height: "80vh" }}>
      <SimFetcherGmat link={signedUrl} />
    </div>
  );
}
