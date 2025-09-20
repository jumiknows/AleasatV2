import type { ConnectionOptions } from "bullmq";

const host = process.env.REDIS_HOST ?? "localhost";
const portStr = process.env.REDIS_PORT ?? "6379";
const isSentinel = process.env.REDIS_IS_SENTINEL ?? false;
const sentinelGroupName = process.env.REDIS_SENTINEL_GROUP_NAME ?? "redis";

const port = parseInt(portStr, 10);
if (isNaN(port)) {
  throw new Error("REDIS_PORT environment variable is not a valid number");
}

export const connection: ConnectionOptions = isSentinel
  ? {
      sentinels: [
        {
          host: host,
          port: port,
        },
      ],
      name: sentinelGroupName,
      enableOfflineQueue: false,
    }
  : {
      host: host,
      port: port,
      enableOfflineQueue: false,
    };
