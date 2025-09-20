import {
  type OperationsRequest,
  responseWithError,
} from "@aleasat/orbit-propagation-api";
import type { Response } from "express";
import { state } from "../repositories/orbitPropagationStore.js";
import orbitPropagationService from "../services/orbitPropagationService.js";

const getGroundTrack = async (
  req: OperationsRequest<"getGroundTrack">,
  res: Response,
): Promise<void> => {
  if (!state.ready) {
    res.sendStatus(500);
    return;
  }
  res.json(await orbitPropagationService.getGroundTrack({ ...req.query }));
};

const findPasses = async (
  req: OperationsRequest<"findPasses">,
  res: Response,
): Promise<void> => {
  if (!state.ready) {
    res.sendStatus(500);
    return;
  }
  const start = new Date(req.query.start).getTime();
  const end = new Date(req.query.end).getTime();
  if (
    // more than five days
    start > Date.now() + 5 * 24 * 60 * 60 * 1000 ||
    // start later than end
    start > end ||
    // range longer than 12 days
    end > start + 12 * 24 * 60 * 60 * 1000
  ) {
    responseWithError(req, res, {
      type: "Range Not accepted",
      title: "Range Not accepted",
      status: 403,
      detail:
        "Given start and/or end is is invalid, start can't be more than five days later, end can't be earlier than start, range can't be longer than 12 days",
    });
    return;
  }

  res.json(await orbitPropagationService.findPasses({ ...req.query }));
};

const findFullPasses = async (
  req: OperationsRequest<"findFullPasses">,
  res: Response,
): Promise<void> => {
  if ("X-Userinfo" in req.headers) {
    responseWithError(req, res, {
      status: 403,
      title: "Forbidden",
      detail: "Forbidden",
    });
    return;
  }

  if (!state.ready) {
    res.sendStatus(500);
    return;
  }

  res.json(await orbitPropagationService.findFullPasses({ ...req.query }));
};

export default {
  getGroundTrack,
  findPasses,
  findFullPasses,
};
