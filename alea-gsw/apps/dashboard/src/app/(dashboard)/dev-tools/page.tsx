"use client";
import { Container, TextField, Typography } from "@mui/material";
import { useSession } from "next-auth/react";

export default function DevToolsPage() {
  const session = useSession();
  if (session.status !== "authenticated") {
    return null;
  }

  return (
    <Container>
      <Typography>Session Info</Typography>
      <TextField
        fullWidth
        multiline
        value={JSON.stringify(session, undefined, 2)}
      />
    </Container>
  );
}
