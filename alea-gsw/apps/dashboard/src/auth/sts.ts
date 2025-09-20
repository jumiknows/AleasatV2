import {
  AssumeRoleWithWebIdentityCommand,
  STSClient,
} from "@aws-sdk/client-sts";

export const getSTS = async (
  accessToken: string,
): Promise<
  | {
      AccessKeyId: string;
      SecretAccessKey: string;
      SessionToken: string;
      Expiration: number;
    }
  | undefined
> => {
  try {
    const client = new STSClient({
      region: "ubc",
      endpoint: process.env.MINIO_URL,
    });
    const command = new AssumeRoleWithWebIdentityCommand({
      RoleArn: undefined,
      RoleSessionName: undefined,
      WebIdentityToken: accessToken,
    });
    const res = await client.send(command);
    return {
      Expiration: res.Credentials!.Expiration!.getTime(),
      AccessKeyId: res.Credentials!.AccessKeyId!,
      SecretAccessKey: res.Credentials!.SecretAccessKey!,
      SessionToken: res.Credentials!.SessionToken!,
    };
  } catch (error) {
    console.error(error);
    return undefined;
  }
};
