"use client";

import Image from "@aleasat/ui/components/Image";
import { Typography } from "@mui/material";
import Box from "@mui/material/Box";
import { useEffect, useState } from "react";
import ScrollDownSection from "./ScrollDown";

interface HeroProps {
  type: "image" | "video";
  src: string;
  title: string;
}

export default function Hero({ type, src, title }: HeroProps) {
  const [isClient, setIsClient] = useState(false);

  useEffect(() => {
    setIsClient(true);
  }, []);
  return (
    <Box
      sx={{
        position: "sticky",
        top: 0,
        height: "100vh",
        alignItems: "center",
        overflow: "visible",
        zIndex: 0,
      }}
    >
      {type === "image" ? (
        <Image
          sx={{
            objectFit: "cover",
            width: "100%",
            height: "100%",
          }}
          alt={title}
          src={src}
          cover
        />
      ) : (
        isClient && (
          <video
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
            <source src={src} type="video/mp4" />
          </video>
        )
      )}
      <Box
        sx={{
          width: "100%",
          position: "absolute",
          bottom: "20%",
          textAlign: "center",
        }}
      >
        <Typography
          variant="h2"
          sx={{
            color: "white",
            fontWeight: "bold",
            fontSize: { xs: 30, md: 40 },
            whiteSpace: "pre-line",
          }}
        >
          {title}
        </Typography>
        <Box
          display="inline-block"
          sx={{
            color: "#fff",
            fontWeight: "bold",
            backgroundColor: "transparent",
            border: "none",
            "&:hover": {
              backgroundColor: "none",
            },
          }}
          id="scrollDown"
        >
          <ScrollDownSection />
        </Box>
      </Box>
    </Box>
  );
}
