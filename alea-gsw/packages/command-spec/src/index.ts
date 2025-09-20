import {
  type Obc2FirmwareCommit,
  type TransformedSpec,
  commits,
  specs,
  transformedSpecs,
} from "./specs/index.js";
import type { Spec } from "./types.js";

export const getCommandSpec = (
  args:
    | { commitHash: string; fwVersion?: undefined }
    | { commitHash?: undefined; fwVersion: number },
):
  | {
      spec: Spec;
      transformedSpec: TransformedSpec;
    }
  | undefined => {
  const { commitHash, fwVersion } = args;
  const commit = (commitHash ?? fwVersion.toString(16)) as Obc2FirmwareCommit;
  if (!commits.includes(commit)) return undefined;

  return {
    spec: {
      ...specs[commit].cmdSys,
      ...specs[commit].cmdSysTest,
    } as unknown as Spec,
    transformedSpec: transformedSpecs[commit],
  };
};

export const serializeArgsResp = ({
  argsRespSpec,
  argsResp,
}: {
  argsRespSpec: TransformedSpec[string]["resp"];
  argsResp: Record<string, unknown>;
}): string[] => {
  const serializedArgsResp: string[] = [];

  if (!argsRespSpec) return serializedArgsResp;

  const args = Object.keys(argsResp);

  argsRespSpec.forEach((arg, i) => {
    serializedArgsResp.push(
      (arg.serialize as (v: unknown) => string)(argsResp[args[i]]),
    );
  });

  return serializedArgsResp;
};

export const deserializeArgsResp = ({
  argsRespSpec,
  argsResp,
}: {
  argsRespSpec: TransformedSpec[string]["resp"];
  argsResp: string[];
}): Record<string, unknown> => {
  const deserializedArgsResp: Record<string, unknown> = {};

  if (!argsRespSpec) return deserializedArgsResp;

  argsRespSpec.forEach((arg, i) => {
    deserializedArgsResp[arg.name] = arg.deserialize(argsResp[i]);
  });

  return deserializedArgsResp;
};

export * from "./types.js";
export * from "./specs/index.js";
