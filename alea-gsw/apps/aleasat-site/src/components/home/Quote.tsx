"use client";

import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import { useEffect, useState } from "react";

export default function Quote() {
  const [isClient, setIsClient] = useState(false);

  useEffect(() => {
    setIsClient(true);
  }, []);
  return (
    <Box
      position="relative"
      height={{ xs: 200, md: "70vh" }}
      bgcolor="#transparent"
    >
      <Box position="fixed" height="100vh" width="100%" top={0} zIndex={-2}>
        {isClient && (
          <video
            data-testid="background-video"
            style={{
              objectFit: "cover",
              width: "100%",
              height: "100%",
            }}
            loop
            muted
            autoPlay
            playsInline
          >
            <source
              src="https://artifact.aleasat.space/stargazing.mp4"
              type="video/mp4"
            />
          </video>
        )}
      </Box>
      <Typography
        variant="h2"
        fontSize={{ xs: "1.1rem", sm: "2rem", md: "3rem" }}
        position="absolute"
        bottom="40%"
        width="100%"
        textAlign="center"
        fontWeight="bold"
      >
        AUDEBIMUS IN INCOGNITUM.
      </Typography>
    </Box>
  );
}
