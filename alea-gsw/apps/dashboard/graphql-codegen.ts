import { type CodegenConfig } from "@graphql-codegen/cli";

const config: CodegenConfig = {
  overwrite: true,
  schema: "../../apps/command-handler/src/schema.graphql",
  documents: [
    "./src/**/*.graphql",
    "./src/**/*.ts",
    "./src/**/*.tsx",
    "!./src/gql/generated/sdk.ts",
  ],
  emitLegacyCommonJSImports: false,
  ignoreNoDocuments: true, // for better experience with the watcher
  generates: {
    "./src/gql/generated/sdk.ts": {
      plugins: [
        "typescript",
        "typescript-operations",
        "typescript-graphql-request",
      ],
      hooks: {
        afterOneFileWrite:
          "yarn eslint --no-eslintrc --parser-options ecmaVersion:latest --parser-options sourceType:module --parser-options project:./tsconfig.json --parser @typescript-eslint/parser --plugin @typescript-eslint/eslint-plugin  --fix --rule @typescript-eslint/consistent-type-imports:1",
      },
    },
  },
};

export default config;
