import Image from "@aleasat/ui/components/Image";
import { Box, Container, Paper, Stack, Typography } from "@mui/material";
import React from "react";
import type { ReactNode } from "react";

export default function SignLayout({ children }: { children: ReactNode }) {
  return (
    <Container maxWidth="sm">
      <Paper
        variant="outlined"
        sx={{
          backgroundImage: undefined,
          bgcolor: "transparent",
          width: "100%",
          paddingY: "7.5vh",
          mt: "20vh",
        }}
      >
        <Stack
          sx={{
            alignItems: "center",
            justifyContent: "center",
            height: "100%",
          }}
        >
          <Container maxWidth="xs" sx={{ height: "100%" }}>
            <Stack
              spacing={2}
              sx={{
                height: "100%",
                justifyContent: "center",
              }}
            >
              <Box>
                <Image
                  src="https://artifact.aleasat.space/logos/ALEASAT_Logo.png"
                  alt="AleaSat Logo"
                  sx={{
                    margin: "auto",
                    width: "50%",
                  }}
                />
              </Box>
              <Typography variant="h5" align="center">
                Sign In to ALEASAT
              </Typography>
              <br />
              {children}
            </Stack>
          </Container>
        </Stack>
      </Paper>
    </Container>
  );
}
