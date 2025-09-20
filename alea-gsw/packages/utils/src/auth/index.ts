import { createRemoteJWKSet, errors as joseError, jwtVerify } from "jose";

const { JWKSMultipleMatchingKeys, JWSSignatureVerificationFailed } = joseError;

export const keycloakJwks = createRemoteJWKSet(
  new URL(
    "https://keycloak.aleasat.space/realms/aleasat/protocol/openid-connect/certs",
  ),
);

export interface KeycloakAccessTokenClaims {
  exp: number;
  iat: number;
  auth_time: number;
  jti: string;
  iss: string;
  aud: string;
  sub: string;
  typ: string;
  azp: string;
  sid: string;
  acr: string;
  "allowed-origins": string[];
  realm_access: {
    roles: string[];
  };
  resource_access: {
    account: {
      roles: string[];
    };
  };
  scope: string;
  email_verified: boolean;
  name: string;
  groups: string[];
  preferred_username: string;
  given_name: string;
  family_name: string;
  email: string;
}

export interface UserAccessTokenClaims extends KeycloakAccessTokenClaims {
  id: string;
}

export const getUserAccessTokenClaims = async (
  accessToken: string,
): Promise<UserAccessTokenClaims | undefined> => {
  const result = await jwtVerify<KeycloakAccessTokenClaims>(
    accessToken,
    keycloakJwks,
    {
      issuer: "https://keycloak.aleasat.space/realms/aleasat",
    },
  ).catch(async (error) => {
    if (error instanceof JWKSMultipleMatchingKeys) {
      for await (const publicKey of error) {
        try {
          return await jwtVerify<KeycloakAccessTokenClaims>(
            accessToken,
            publicKey,
            {
              issuer: "https://keycloak.aleasat.space/realms/aleasat",
            },
          );
        } catch (innerError) {
          if (innerError instanceof JWSSignatureVerificationFailed) {
            continue;
          }
          console.error(innerError);
        }
      }
      console.error(new JWSSignatureVerificationFailed());
    }
    console.error(error);

    return { payload: undefined, protectedHeader: undefined };
  });

  return result.payload && { ...result.payload, id: result.payload.sub };
};
