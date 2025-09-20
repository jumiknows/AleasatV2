import NextAuth, { type DefaultSession } from "next-auth";
import type { DefaultJWT } from "next-auth/jwt";
import Keycloak from "next-auth/providers/keycloak";
import { getSTS } from "./sts";

declare module "next-auth/jwt" {
  // biome-ignore lint/style/useNamingConvention: we have no control over the naming
  export interface JWT extends DefaultJWT {
    id: string;
    access_token: string;
    id_token: string;
    groups: string[];
    expires_at: number;
    refresh_token: string;
    sts:
      | {
          AccessKeyId: string;
          SecretAccessKey: string;
          SessionToken: string;
          Expiration: number;
        }
      | undefined;
    error?: "RefreshTokenError";
  }
}

declare module "next-auth" {
  export interface Session extends DefaultSession {
    accessToken: string;
    user: {
      id: string;
      groups: string[];
      name: string;
      email: string;
    };
    sts:
      | {
          AccessKeyId: string;
          SecretAccessKey: string;
          SessionToken: string;
          Expiration: number;
        }
      | undefined;
    error?: "RefreshTokenError";
  }
}

export const { auth, handlers, signIn, signOut } = NextAuth({
  // Configure one or more authentication providers
  providers: [
    Keycloak({
      issuer: "https://keycloak.aleasat.space/realms/aleasat",
      clientId: "alea-dashboard",
      client: {
        token_endpoint_auth_method: "none",
      },
    }),
  ],
  callbacks: {
    async jwt({ token, account, profile }) {
      if (account) {
        token.access_token = account.access_token!;
        token.expires_at = account.expires_at!;
        token.refresh_token = account.refresh_token!;
        token.id_token = account.id_token!;
        token.sts = await getSTS(token.access_token);
        token.id = account.providerAccountId;
      }

      if (profile) {
        // @ts-expect-error configured it has
        token.groups = profile.groups;
      }

      if (Date.now() > token.expires_at * 1000 - 2 * 60 * 1000) {
        try {
          const response = await fetch(
            "https://keycloak.aleasat.space/realms/aleasat/protocol/openid-connect/token",
            {
              method: "POST",
              body: new URLSearchParams({
                client_id: "alea-dashboard",
                grant_type: "refresh_token",
                refresh_token: token.refresh_token,
              }),
            },
          );

          const tokensOrError = await response.json();

          if (!response.ok) throw tokensOrError;

          const newTokens = tokensOrError as {
            access_token: string;
            expires_in: number;
            refresh_token: string;
          };

          token.access_token = newTokens.access_token;
          token.expires_at = Math.floor(
            Date.now() / 1000 + newTokens.expires_in,
          );
          token.refresh_token = newTokens.refresh_token;
        } catch (error) {
          console.error("Error refreshing access_token", error);
          // If we fail to refresh the token, return an error so we can handle it on the page
          token.error = "RefreshTokenError";
        }
      }

      if (!token.sts || Date.now() > token.sts.Expiration - 2 * 60 * 1000) {
        token.sts = await getSTS(token.access_token);
      }

      return token;
    },
    session({ session, token }) {
      session.accessToken = token.access_token;
      session.user.groups = token.groups;
      session.user.id = token.id;
      session.sts = token.sts;
      if (token.error) session.error = token.error;
      return session;
    },
    authorized: ({ request, auth }) => {
      if (auth) return true;

      if (request.nextUrl.pathname !== "/") {
        return false;
      }

      return true;
    },
  },
});
