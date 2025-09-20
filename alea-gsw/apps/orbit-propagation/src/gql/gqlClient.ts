import { GraphQLClient } from "graphql-request";
import { getSdk } from "./generated/sdk.js";

export const gqlClient = new GraphQLClient(process.env.COMMAND_HANDLER_URL!, {
  fetch,
});

export const gqlSdk = getSdk(gqlClient);
