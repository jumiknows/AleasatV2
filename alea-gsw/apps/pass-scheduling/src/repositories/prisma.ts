import { PrismaClient } from "@prisma/client/pass-scheduling/index.js";
import logger from "../utils/logger.js";
export * from "@prisma/client/pass-scheduling/index.js";

if (!process.env.POSTGRES_URL) {
  logger.warn("POSTGRES_URL environment variable is not set");
}

export const prisma = new PrismaClient();
