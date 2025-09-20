import type { ServiceReturnTypeV2, operations } from "@aleasat/repo-api";
import opentelemetry, { type Span } from "@opentelemetry/api";
import git from "../repositories/git.js";

const tracer = opentelemetry.trace.getTracer("branchService");

const getBranches = async (): Promise<ServiceReturnTypeV2<"getBranches">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getBranches">>
  >("getBranchesService", async (span) => {
    const branches = await git.branch();

    span.end();
    return {
      success: true,
      data: branches.all,
    };
  });

const getBranchCommits = async (
  branchName: string,
  query: operations["getBranchCommits"]["parameters"]["query"],
): Promise<ServiceReturnTypeV2<"getBranchCommits">> =>
  await tracer.startActiveSpan<
    (span: Span) => Promise<ServiceReturnTypeV2<"getBranchCommits">>
  >("getBranchCommitsService", async (span) => {
    const options: string[] = [];

    if (query?.offset) {
      options.push(`--skip=${query.offset}`);
    }

    if (query?.limit) {
      options.push(`--max-count=${query.limit}`);
    }

    const commits = await git.log([...options, branchName]);

    span.end();
    return {
      success: true,
      data: [...commits.all],
    };
  });

export default {
  getBranches,
  getBranchCommits,
};
