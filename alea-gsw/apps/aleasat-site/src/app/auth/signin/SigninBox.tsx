"use client";

import { Box, Button, CircularProgress, Typography } from "@mui/material";
import Link from "next/link";
import { useEffect } from "react";
import signInKeycloak from "./signInKeycloak";

export const signinErrors = {
  default: "Unable to sign in.",
  Signin: "Try signing in with a different account.",
  OAuthSignin: "Try signing in with a different account.",
  OAuthCallbackError: "Try signing in with a different account.",
  OAuthCreateAccount: "Try signing in with a different account.",
  EmailCreateAccount: "Try signing in with a different account.",
  Callback: "Try signing in with a different account.",
  OAuthAccountNotLinked:
    "To confirm your identity, sign in with the same account you used originally.",
  EmailSignin: "The e-mail could not be sent.",
  CredentialsSignin:
    "Sign in failed. Check the details you provided are correct.",
  SessionRequired: "Please sign in to access this page.",
};

export default function SignInBox({
  callbackUrl,
  error,
  keycloakLogin,
}: {
  callbackUrl: string | undefined;
  error: keyof typeof signinErrors | undefined;
  keycloakLogin: boolean;
}) {
  useEffect(() => {
    if (keycloakLogin) signInKeycloak(callbackUrl);
  }, [keycloakLogin, callbackUrl]);

  if (keycloakLogin)
    return (
      <>
        <Box sx={{ textAlign: "center" }}>
          <CircularProgress color="secondary" size={60} />
        </Box>
        <Typography variant="h6" align="center">
          Logining with keycloak
        </Typography>
      </>
    );

  return (
    <>
      <Button
        variant="contained"
        onClick={async () => await signInKeycloak(callbackUrl)}
      >
        contunue with ALEA SSO
      </Button>
      <Link
        href={`/auth/signin/password${
          callbackUrl ? `?callbackUrl=${callbackUrl}` : ""
        }`}
      >
        <Button fullWidth>Other Methods</Button>
      </Link>
      {error && <Typography color="error">{signinErrors[error]}</Typography>}
    </>
  );
}
