import type { Spec } from "../types.js";
import { argsTransformers } from "../utils/specTransformers.js";
import e779fd15 from "./e779fd15/index.js";
import fa322db1 from "./fa322db1/index.js";

export const specs = { e779fd15, fa322db1 };
export type Obc2FirmwareCommit = keyof typeof specs;
export const commits = Object.keys(specs);

export type TransformedSpec = Record<
  string,
  {
    id: number;
    args: ReturnType<typeof argsTransformers>;
    resp: ReturnType<typeof argsTransformers> | null;
  }
>;

export const specTransformer = (spec: Spec): TransformedSpec => {
  const transformedSpec: TransformedSpec = {};

  Object.keys(spec).forEach((key) => {
    const args = spec[key].args;
    const resp = spec[key].resp;
    transformedSpec[key] = {
      ...spec[key],
      args: argsTransformers(args),
      resp: resp && argsTransformers(resp),
    };
  });

  return transformedSpec;
};

export const transformedSpecs: Record<string, TransformedSpec> = {};

commits.forEach((commit) => {
  transformedSpecs[commit] = specTransformer({
    ...(specs[commit as keyof typeof specs].cmdSys as unknown as Spec),
    ...(specs[commit as keyof typeof specs].cmdSysTest as unknown as Spec),
  } as Spec);
});
