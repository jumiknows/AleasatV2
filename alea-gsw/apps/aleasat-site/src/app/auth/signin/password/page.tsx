"use client";

import { Button, Stack } from "@mui/material";
import { signIn } from "next-auth/react";
import Link from "next/link";
import React from "react";
import { FormContainer, TextFieldElement } from "react-hook-form-mui";

export default function SignInWithPassword({
  searchParams,
}: {
  searchParams: {
    callbackUrl: string | undefined;
  };
}) {
  return (
    <FormContainer
      onSuccess={async (data) =>
        await signIn("credentials", {
          password: data.password,
          redirectTo: searchParams.callbackUrl,
        })
      }
    >
      <Stack spacing={2} sx={{ height: "100%", justifyContent: "center" }}>
        <TextFieldElement
          label="password"
          name="password"
          type="password"
          required
        />
        <Button type="submit" variant="contained">
          Sign In with Password
        </Button>
        <Link
          href={`/auth/signin${
            searchParams.callbackUrl
              ? `?callbackUrl=${searchParams.callbackUrl}`
              : ""
          }`}
        >
          <Button fullWidth>Back to previous</Button>
        </Link>
      </Stack>
    </FormContainer>
  );
}
