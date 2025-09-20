import type { DefaultJobOptions } from "bullmq";

export const defaultJobOptions: DefaultJobOptions = {
  removeOnComplete: {
    age: 24 * 3600, // keep up to 24 hours
    count: 100, // keep up to 100 jobs
  },
  removeOnFail: {
    age: 24 * 7 * 3600, // keep up to a week
  },
  attempts: 3,
  backoff: {
    type: "exponential",
    delay: 1000,
  },
};
