"use client";

import { signIn, useSession } from "next-auth/react";

export default function ForceReAuth() {
  const session = useSession();
  if (session.data?.error) signIn("keycloak");
  return <></>;
}
