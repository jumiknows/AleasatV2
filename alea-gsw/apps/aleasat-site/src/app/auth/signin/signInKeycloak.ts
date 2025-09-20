"use server";

import { signIn } from "@/auth";
import { cookies } from "next/headers";

export default async function signInKeycloak(callbackUrl = "/") {
  (await cookies()).set("keycloakLogin", "true", { domain: "aleasat.space" });
  await signIn("keycloak", {
    redirectTo: callbackUrl,
  });
}
