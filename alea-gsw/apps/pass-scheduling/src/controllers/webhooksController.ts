import type { OperationsRequest } from "@aleasat/pass-scheduling-api";
import type { Response } from "express";
import webhooksService from "../services/webhooksService.js";
import logger from "../utils/logger.js";

const webhooksNewOrbitPropagation = async (
  _req: OperationsRequest<"webhooksNewOrbitPropagation">,
  res: Response,
): Promise<void> => {
  logger.info("New orbit propagation available, updating DB");
  webhooksService
    .webhooksNewOrbitPropagation()
    .catch((err) =>
      logger.warn(err, "Error updating with new orbit propagation"),
    );
  res.json({});
};

export default {
  webhooksNewOrbitPropagation,
};
