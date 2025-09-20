import type { Spec } from "@aleasat/command-spec";
import type { ServiceReturnTypeV2, operations } from "@aleasat/repo-api";
import opentelemetry, { type Span } from "@opentelemetry/api";
import git from "../repositories/git.js";
import specToProto from "../utils/specToProto.js";
import specToService from "../utils/specToService.js";

const tracer = opentelemetry.trace.getTracer("commitService");

const getCommits = async (
  query: operations["getCommits"]["parameters"]["query"],
): Promise<ServiceReturnTypeV2<"getCommits">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getCommits">>
  >("getCommitsService", async (span) => {
    const options: Record<string, string> = {};

    if (query?.offset) {
      options["--skip"] = query.offset.toString();
    }

    if (query?.limit) {
      options["--max-count"] = query.limit.toString();
    }

    const commits = await git.log(options);

    span.end();
    return {
      success: true,
      data: [...commits.all],
    };
  });

const getFile = async (
  commitHash: string,
  query: operations["getFile"]["parameters"]["query"],
): Promise<ServiceReturnTypeV2<"getFile">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getFile">>
  >("getFileService", async (span) => {
    const file = await git.show(`${commitHash}:${query.filePath}`);

    span.end();
    return {
      success: true,
      data: file,
    };
  });

const getObcConfig = async (
  commitHash: string,
): Promise<ServiceReturnTypeV2<"getObcConfig">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getObcConfig">>
  >("getObcConfigService", async (span) => {
    const [cmdSys, cmdSysTest, logSpecs, obcTasks] = await Promise.all([
      git.show(
        `${commitHash}:alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys.json`,
      ),
      git.show(
        `${commitHash}:alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys_test.json`,
      ),
      git.show(
        `${commitHash}:alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/log/data/log_specs.json`,
      ),
      git.show(
        `${commitHash}:alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/task/data/obc_tasks.json`,
      ),
    ]);

    span.end();
    return {
      success: true,
      data: {
        cmd_sys: JSON.parse(cmdSys),
        cmd_sys_test: JSON.parse(cmdSysTest),
        log_specs: JSON.parse(logSpecs),
        obc_tasks: JSON.parse(obcTasks),
      },
    };
  });

const getObcProto = async (
  commitHash: string,
  includeTestSpec: boolean,
): Promise<ServiceReturnTypeV2<"getObcProto">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getObcProto">>
  >("getObcProtoService", async (span) => {
    const [cmdSys, cmdSysTest] = await Promise.all([
      git.show(
        `${commitHash}:alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys.json`,
      ),
      includeTestSpec &&
        git.show(
          `${commitHash}:alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys_test.json`,
        ),
    ]);

    const protoFile = specToProto(
      JSON.parse(cmdSys) as Spec,
      cmdSysTest ? (JSON.parse(cmdSysTest) as Spec) : undefined,
    );

    span.end();
    return {
      success: true,
      data: protoFile,
    };
  });

const getObcGrpcService = async (
  commitHash: string,
  includeTestSpec: boolean,
): Promise<ServiceReturnTypeV2<"getObcGrpcService">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getObcGrpcService">>
  >("getObcProtoService", async (span) => {
    const [cmdSys, cmdSysTest] = await Promise.all([
      git.show(
        `${commitHash}:alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys.json`,
      ),
      includeTestSpec &&
        git.show(
          `${commitHash}:alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys_test.json`,
        ),
    ]);

    const grpcServiceFile = specToService(
      JSON.parse(cmdSys) as Spec,
      cmdSysTest ? (JSON.parse(cmdSysTest) as Spec) : undefined,
    );

    span.end();
    return {
      success: true,
      data: grpcServiceFile,
    };
  });

export default {
  getCommits,
  getFile,
  getObcConfig,
  getObcProto,
  getObcGrpcService,
};
