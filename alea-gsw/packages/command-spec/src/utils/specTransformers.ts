import type { ArgType } from "../types.js";
import { typeTransformer } from "./typeTransformers.js";

export const argsTransformers = (
  args: Record<string, ArgType>[],
): Array<
  {
    name: string;
    specType: ArgType;
  } & (
    | {
        type: "number";
        constructor: NumberConstructor;
        serialize: (v: number) => string;
        deserialize: (v: string) => number;
        float: boolean;
        range: { min: number; max: number };
      }
    | {
        type: "boolean";
        constructor: BooleanConstructor;
        serialize: (v: boolean) => string;
        deserialize: (v: string) => boolean;
      }
    | {
        type: "date";
        constructor: DateConstructor;
        serialize: (v: Date) => string;
        deserialize: (v: string) => Date;
      }
    | {
        type: "buffer";
        constructor: BufferConstructor;
        serialize: (v: Buffer) => string;
        deserialize: (v: string) => Buffer;
      }
    | {
        type: "array";
        constructor: ArrayConstructor;
        length: number;
        serialize: (v: number[]) => string;
        deserialize: (v: string) => number[];
        elementType: ReturnType<typeof typeTransformer> & { type: "number" };
      }
    | {
        type: "matrix";
        constructor: ArrayConstructor;
        serialize: (v: number[][]) => string;
        deserialize: (v: string) => number[][];
        row: number;
        column: number;
        elementType: ReturnType<typeof typeTransformer> & { type: "number" };
      }
    | {
        type: "string";
        constructor: StringConstructor;
        serialize: (v: string) => string;
        deserialize: (v: string) => string;
      }
  )
> =>
  args.map((arg) => {
    const name = Object.keys(arg)[0];
    return {
      name,
      specType: arg[name],
      ...typeTransformer(arg[name]),
    };
  });
