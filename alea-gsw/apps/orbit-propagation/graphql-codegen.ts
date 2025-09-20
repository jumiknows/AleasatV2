import { type CodegenConfig } from "@graphql-codegen/cli";

const config: CodegenConfig = {
  overwrite: true,
  schema: "../../apps/command-handler/src/schema.graphql",
  documents: ["./src/**/*.graphql", "./src/**/*.ts", "!./src/gql/sdk.ts"],
  emitLegacyCommonJSImports: false,
  ignoreNoDocuments: true, // for better experience with the watcher
  generates: {
    "./src/gql/generated/sdk.ts": {
      plugins: [
        "typescript",
        "typescript-operations",
        "typescript-graphql-request",
      ],
    },
  },
};

export default config;
