import * as client from "openid-client";
import logger from "./logger.js";

const server = process.env.KEYCLOAK_URL && new URL(process.env.KEYCLOAK_URL);
const clientId = process.env.KEYCLOAK_CLIENT_ID;
const clientSecret = process.env.KEYCLOAK_CLIENT_SECRET;

const config =
  server && clientId && clientSecret
    ? await client.discovery(server, clientId, clientSecret)
    : undefined;

if (!config)
  logger.warn(
    `Missing Keycloak configuration: [${server ? "" : "KEYCLOAK_URL"}${clientId ? "" : " KEYCLOAK_CLIENT_ID"}${clientSecret ? "" : " KEYCLOAK_CLIENT_SECRET"}]`,
  );

let accessToken: string | undefined;
let expiresAt: number | undefined;

export const getServiceAccountAccessToken = config
  ? async (): Promise<string> => {
      if (accessToken && expiresAt && Date.now() < expiresAt) {
        return accessToken;
      }

      const tokenEndpointResponse = await client.clientCredentialsGrant(
        config,
        {},
      );

      accessToken = tokenEndpointResponse.access_token;
      expiresAt = Date.now() + tokenEndpointResponse.expires_in! * 1000;

      return tokenEndpointResponse.access_token;
    }
  : undefined;
