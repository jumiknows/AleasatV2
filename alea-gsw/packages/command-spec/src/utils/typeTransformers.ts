import type { ArgNumberType, ArgType } from "../types.js";

export function typeTransformer(argType: ArgType):
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
    } {
  const dimension = (argType.match(/\[/g) ?? []).length;

  if (dimension === 1)
    return {
      type: "array",
      constructor: Array,
      serialize: JSON.stringify,
      deserialize: JSON.parse,
      length: parseInt(argType.match(/\[[0-9]+\]/)?.[0].slice(1, -1) ?? "0"),
      // @ts-expect-error it should be a number only
      elementType: typeTransformer(
        argType.replace(/\[[0-9]+\]/g, "") as ArgType,
      ),
    };
  if (dimension === 2) {
    const column = parseInt(
      argType.match(/\[[0-9]+\]/)![0].slice(1, -1) ?? "0",
    );
    return {
      type: "matrix",
      constructor: Array,
      serialize: (v: number[][]) => JSON.stringify(v),
      deserialize: (v: string) => {
        const array = JSON.parse(v) as number[];
        const matrix: number[][] = [];
        for (let i = 0; i < array.length; i += column) {
          matrix.push(array.slice(i, i + column));
        }
        return matrix;
      },
      row: parseInt(
        [...argType.matchAll(/\[[0-9]+\]/g)][1][0].slice(1, -1) ?? "0",
      ),
      column,
      // @ts-expect-error it should be a number only
      elementType: typeTransformer(
        argType.replace(/\[[0-9]+\]/g, "") as ArgType,
      ),
    };
  }
  switch (argType) {
    case "u8":
    case "s8":
    case "u16":
    case "s16":
    case "u32":
    case "s32":
    case "u64":
    case "s64":
      return {
        type: "number",
        constructor: Number,
        serialize: (v: number) => v.toString(),
        deserialize: parseInt,
        float: false,
        range: numberTypeRange(argType),
      };
    case "f32":
    case "f64":
      return {
        type: "number",
        constructor: Number,
        serialize: (v: number) => v.toString(),
        deserialize: parseFloat,
        float: true,
        range: numberTypeRange(argType),
      };
    case "bool":
      return {
        type: "boolean",
        constructor: Boolean,
        serialize: (v: boolean) => v.toString(),
        deserialize: (v: string) => v === "true",
      };
    case "datetime":
      return {
        type: "date",
        constructor: Date,
        serialize: (v: Date) => v.toISOString(),
        deserialize: (v: string) => new Date(v),
      };
    case "bytes":
      return {
        type: "buffer",
        constructor: Buffer,
        serialize: (v: Buffer) => v.toString("base64"),
        deserialize: (v: string) => Buffer.from(v, "base64"),
        // from Noah: For bytes go greater than 8 MB though, maybe 2^24
        // https://discord.com/channels/1240882412523753553/1240888920976265287/1252505453414584402
      };
    case "string":
      return {
        type: "string",
        constructor: String,
        serialize: (v: string) => v,
        deserialize: (v: string) => v,
        // from Noah: Idk 2^16 or something for string.
        // https://discord.com/channels/1240882412523753553/1240888920976265287/1252505453414584402
      };
  }
}

export function numberTypeRange(type: ArgNumberType): {
  min: number;
  max: number;
} {
  switch (type) {
    case "u8":
      return { min: 0, max: 255 };
    case "s8":
      return { min: -128, max: 127 };
    case "u16":
      return { min: 0, max: 65535 };
    case "s16":
      return { min: -32768, max: 32767 };
    case "u32":
      return { min: 0, max: 4294967295 };
    case "s32":
      return { min: -2147483648, max: 2147483647 };
    case "u64":
      // biome-ignore lint/correctness/noPrecisionLoss: just for the dev know
      return { min: 0, max: 18446744073709551615 };
    case "s64":
      // biome-ignore lint/correctness/noPrecisionLoss: just for the dev know
      return { min: -9223372036854775808, max: 9223372036854775807 };
    case "f32":
      return { min: -3.402823466e38, max: 3.402823466e38 };
    case "f64":
      return { min: -1.7976931348623157e308, max: 1.7976931348623157e308 };
  }
}
