import IORedis from "ioredis";

const host = process.env.REDIS_HOST ?? "localhost"; // ex: REDIS_SENTINEL_HOSTS="host1"
const portStr = process.env.REDIS_PORT ?? "6379"; // ex: REDIS_SENTINEL_PORT="26379"
const isSentinel = process.env.REDIS_IS_SENTINEL ?? false;
const seninelGroupName = process.env.REDIS_SENTINEL_GROUP_NAME ?? "redis"; // ex: REDIS_GROUP_NAME="mymaster"

const port = parseInt(portStr, 10);
if (isNaN(port)) {
  throw new Error("REDIS_PORT environment variable is not a valid number");
}

export const ioredis = new IORedis({
  ...(isSentinel
    ? {
        sentinels: [
          {
            host: host,
            port: port,
          },
        ],
        name: seninelGroupName,
      }
    : {
        host: host,
        port: port,
      }),
  maxRetriesPerRequest: null, // https://docs.bullmq.io/bull/patterns/persistent-connections
});
