import {
  type obcClient,
  obcDefinition,
} from "@aleasat/mops-client-grpc-node/obc";
import { createChannel, createClient } from "nice-grpc";
import logger from "../utils/logger.js";

if (!process.env.OBC_GRPC_HOST) {
  logger.warn("OBC_GRPC_HOST is not defined");
}

const channel = createChannel(process.env.OBC_GRPC_HOST ?? "");

export const obc: obcClient = createClient(obcDefinition, channel);
