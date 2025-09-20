import NextAuth from "next-auth";
import type { Provider } from "next-auth/providers";
import Credentials from "next-auth/providers/credentials";
import Keycloak from "next-auth/providers/keycloak";

export const providers: Provider[] = [
  Credentials({
    name: "Password",
    credentials: {
      password: { label: "Password", type: "password" },
    },
    authorize: async (credentials) => {
      if (credentials.password === process.env.PAGE_PASSWORD) {
        return { id: "user" };
      }
      return null;
    },
  }),
  Keycloak({
    issuer: "https://keycloak.aleasat.space/realms/aleasat",
    clientId: "aleasat-site",
    client: {
      token_endpoint_auth_method: "none",
    },
  }),
];

export const { auth, handlers, signIn, signOut } = NextAuth({
  providers,
  callbacks: {
    authorized: async ({ auth }) => {
      // Logged in users are authenticated, otherwise redirect to login page
      return !!auth;
    },
  },
  pages: {
    signIn: "/auth/signin",
  },
});

export const providerMap = providers
  .map((provider) => {
    if (typeof provider === "function") {
      const providerData = provider();
      return { id: providerData.id, name: providerData.name };
    } else {
      return { id: provider.id, name: provider.name };
    }
  })
  .filter((provider) => provider.id !== "credentials");
