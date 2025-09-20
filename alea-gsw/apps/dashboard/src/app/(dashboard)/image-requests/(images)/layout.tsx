import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import type React from "react";

export default function ImageRequestLayout({
  children,
  selected,
}: {
  children: React.ReactNode;
  selected: React.ReactNode;
}) {
  return (
    <Box sx={{ marginTop: "5vh" }}>
      <Container sx={{ pb: "10vh" }}>
        {selected}
        {children}
      </Container>
    </Box>
  );
}
