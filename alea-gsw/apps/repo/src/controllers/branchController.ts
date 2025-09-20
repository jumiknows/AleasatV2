import { type OperationsRequest, errorWrapperV3 } from "@aleasat/repo-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import branchService from "../services/branchService.js";

const tracer = opentelemetry.trace.getTracer("branchController");

const getBranches = async (
  req: OperationsRequest<"getBranches">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getBranchesController", async (span) => {
    const branches = await errorWrapperV3(
      req,
      res,
      async () => await branchService.getBranches(),
    );

    if (!branches) {
      span.end();
      return;
    }

    res.status(200).json(branches);
    span.end();
  });

const getBranchCommits = async (
  req: OperationsRequest<"getBranchCommits">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getBranchCommitsController", async (span) => {
    const commits = await errorWrapperV3(
      req,
      res,
      async () =>
        await branchService.getBranchCommits(req.params.branchName, req.query),
    );

    if (!commits) {
      span.end();
      return;
    }

    res.status(200).send(commits);
    span.end();
  });

export default {
  getBranches,
  getBranchCommits,
};
