import { type OperationsRequest, errorWrapperV3 } from "@aleasat/repo-api";
import opentelemetry from "@opentelemetry/api";
import type { Response } from "express";
import commitService from "../services/commitService.js";

const tracer = opentelemetry.trace.getTracer("commitController");

const getCommits = async (
  req: OperationsRequest<"getCommits">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getCommitsController", async (span) => {
    const commits = await errorWrapperV3(
      req,
      res,
      async () => await commitService.getCommits(req.query),
    );

    if (!commits) {
      span.end();
      return;
    }

    res.status(200).json(commits);
    span.end();
  });

const getFile = async (
  req: OperationsRequest<"getFile">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getFileController", async (span) => {
    const file = await errorWrapperV3(
      req,
      res,
      async () => await commitService.getFile(req.params.commitHash, req.query),
    );

    if (!file) {
      span.end();
      return;
    }

    res.status(200).send(file);
    span.end();
  });

const getObcConfig = async (
  req: OperationsRequest<"getObcConfig">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getObcConfigController", async (span) => {
    const userSession = await errorWrapperV3(
      req,
      res,
      async () => await commitService.getObcConfig(req.params.commitHash),
    );

    if (!userSession) {
      span.end();
      return;
    }

    res.status(200).json(userSession);
    span.end();
  });

const getObcProto = async (
  req: OperationsRequest<"getObcProto">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getObcProtoController", async (span) => {
    const proto = await errorWrapperV3(
      req,
      res,
      async () =>
        await commitService.getObcProto(
          req.params.commitHash,
          req.query.includeTestSpec ?? false,
        ),
    );

    if (!proto) {
      span.end();
      return;
    }

    res.status(200).send(proto);
    span.end();
  });

const getObcGrpcService = async (
  req: OperationsRequest<"getObcGrpcService">,
  res: Response,
): Promise<void> =>
  await tracer.startActiveSpan("getObcGrpcService", async (span) => {
    const proto = await errorWrapperV3(
      req,
      res,
      async () =>
        await commitService.getObcGrpcService(
          req.params.commitHash,
          req.query.includeTestSpec ?? false,
        ),
    );

    if (!proto) {
      span.end();
      return;
    }

    res.status(200).send(proto);
    span.end();
  });

export default {
  getCommits,
  getFile,
  getObcConfig,
  getObcProto,
  getObcGrpcService,
};
