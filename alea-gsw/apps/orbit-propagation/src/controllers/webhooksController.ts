import type { Request, Response } from "express";
import webhooksService from "../services/webhooksService.js";

const webhooksNewGmatOrbitPropagation = async (
  _req: Request,
  res: Response,
): Promise<void> => {
  webhooksService.webhooksNewGmatOrbitPropagation();
  res.json({});
};

export default {
  webhooksNewGmatOrbitPropagation,
};
