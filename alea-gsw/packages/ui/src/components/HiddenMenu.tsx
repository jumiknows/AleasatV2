"use client";

import { Button, Box, IconButton, Typography } from "@mui/material";
import { usePathname } from "next/navigation";
import { useState, useEffect } from "react";

import Image from "./Image";
import Link from "./Link";

interface HiddenMenuProps {
  links?: Array<{ label: string; path: string }>;
  onClose: () => void;
}

export default function HiddenMenu({ links, onClose }: HiddenMenuProps) {
  const pathname = usePathname();
  const [isClosing, setIsClosing] = useState(false);

  const handleClose = () => {
    setIsClosing(true);
    setTimeout(() => {
      onClose();
    }, 300); // Match the animation duration
  };

  // Prevent body scrolling when menu is open
  useEffect(() => {
    document.body.style.overflow = 'hidden';
    return () => {
      document.body.style.overflow = 'unset';
    };
  }, []);

  return (
    <Box
      sx={{
        width: "100vw",
        height: "65vh",
        background: "#000",
        position: "fixed",
        top: 0,
        left: 0,
        zIndex: 9999,
        overflow: "hidden",
        transform: isClosing ? "translateY(-100%)" : "translateY(0)",
        transition: "transform 0.3s ease-in-out",
        "@keyframes slideDown": {
          from: {
            transform: "translateY(-100%)",
          },
          to: {
            transform: "translateY(0)",
          },
        },
        animation: !isClosing ? "slideDown 0.3s ease-in-out" : "none",
      }}
      role="presentation"
      onClick={handleClose}
    >
      {/* Header with logo and close button */}
      <Box
        sx={{
          display: "flex",
          justifyContent: "space-between",
          alignItems: "center",
          p: 3,
          pt: 4,
        }}
      >
        <Image
          src="https://artifact.aleasat.space/logos/ALEASAT_Logo.png"
          alt="AleaSat Logo"
          sx={{
            objectFit: "contain",
            height: 100,
            width: 220,
          }}
        />
        <IconButton
          sx={{ color: "white", fontSize: "2rem" }}
          onClick={handleClose}
        >
          ✕
        </IconButton>
      </Box>

      {/* Content Layout */}
      <Box
        sx={{
          display: "flex",
          height: "calc(75vh - 120px)",
          position: "relative",
          alignItems: "flex-start",
          justifyContent: "space-between",
          px: 6,
          py: 4,
          gap: 8,
        }}
      >
        {/* Left Column - Main Navigation */}
        <Box
          sx={{
            display: "flex",
            flexDirection: "column",
            gap: 2,
            justifyContent: "flex-start",
            flex: "0 0 auto",
          }}
        >
          {links?.map(({ label, path }) => (
            <Typography
              key={label}
              component={Link}
              href={path}
              variant="h4"
              sx={{
                textDecoration: "none",
                color: "#b6bdd6",
                fontWeight: pathname === path ? "bold" : "normal",
                lineHeight: 1.3,
                "&:hover": {
                  color: "white",
                },
              }}
            >
              {label}
            </Typography>
          ))}
          
          {/* Join Us Button */}
          <Box sx={{ mt: 3 }}>
            <Button
              href="/join-us"
              variant="contained"
              sx={{
                background: "linear-gradient(45deg, #4A90E2, #357ABD)",
                color: "white",
                borderRadius: "25px",
                textTransform: "none",
                "&:hover": {
                  background: "linear-gradient(45deg, #5BA3F5, #4A90E2)",
                },
              }}
            >
              Join Us
            </Button>
          </Box>
        </Box>

        {/* Center Column - Footer Links */}
        <Box
          sx={{
            display: "flex",
            flexDirection: "column",
            gap: 4,
            justifyContent: "flex-start",
            flex: "0 0 auto",
            pt: 2,
          }}
        >
          {[
            { label: "FAQ", path: "/faq" },
            { label: "Privacy Policy", path: "/privacy" },
            { label: "Terms of Service", path: "/terms" },
            { label: "Contact Us", path: "/contact" },
          ].map(({ label, path }) => (
            <Typography
              key={label}
              component={Link}
              href={path}
              variant="h6"
              sx={{
                textDecoration: "none",
                color: "#b6bdd6",
                display: "flex",
                alignItems: "center",
                gap: 1,
                lineHeight: 1.3,
                "&:hover": {
                  color: "white",
                },
              }}
            >
              {label}
              <Box
                component="span"
                sx={{
                  fontSize: "0.9rem",
                  opacity: 0.7,
                }}
              >
                ↗
              </Box>
            </Typography>
          ))}
        </Box>

        {/* Right Side - Satellite Image */}
        <Box
          sx={{
            display: "flex",
            justifyContent: "center",
            alignItems: "flex-start",
            flex: "1 1 auto",
            overflow: "hidden",
          }}
        >
          <Image
            src="/sathd2.png"
            alt="ALEASAT Satellite"
            sx={{
              width: "700px",
              height: "700px",
              objectFit: "contain",
              transform: "rotate(-25deg) scale(1.5) translateY(-100px) translateX(90px)",
              filter: "contrast(2)",
            }}
          />
        </Box>
      </Box>
    </Box>
  );
}
