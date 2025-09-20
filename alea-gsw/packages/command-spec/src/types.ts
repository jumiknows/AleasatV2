export type ArgType = ArgNumberType | "bool" | "datetime" | "bytes" | "string";

export type ArgNumberType =
  | "u8"
  | "s8"
  | "u16"
  | "s16"
  | "u32"
  | "s32"
  | "u64"
  | "s64"
  | "f32"
  | "f64";

export type Spec = Record<
  string,
  {
    id: number;
    args: Record<string, ArgType>[];
    resp: Record<string, ArgType>[] | null;
  }
>;
