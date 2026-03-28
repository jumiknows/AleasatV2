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

const SHOW_RESOURCES = false;

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
    document.body.style.overflow = "hidden";

    const onKeyDown = (event: KeyboardEvent) => {
      if (event.key === "Escape") {
        handleClose();
      }
    };

    window.addEventListener("keydown", onKeyDown);

    return () => {
      document.body.style.overflow = "unset";
      window.removeEventListener("keydown", onKeyDown);
    };
  }, []);

  return (
    <Box
      sx={{
        width: "100dvw",
        height: "100dvh",
        background: "#000000",
        position: "fixed",
        top: 0,
        left: 0,
        zIndex: 9999,
        overflowY: "auto",
        overflowX: "hidden",
        WebkitOverflowScrolling: "touch",
        transform: isClosing ? "translateY(-100%)" : "translateY(0)",
        transition: "all 0.4s cubic-bezier(0.4, 0, 0.2, 1)",
        borderBottom: "1px solid rgba(74, 144, 226, 0.2)",
        boxShadow: "0 20px 60px rgba(0, 0, 0, 0.5)",
        "@keyframes slideDown": {
          from: {
            transform: "translateY(-100%)",
            opacity: 0,
          },
          to: {
            transform: "translateY(0)",
            opacity: 1,
          },
        },
        animation: !isClosing ? "slideDown 0.4s cubic-bezier(0.4, 0, 0.2, 1)" : "none",
      }}
      role="presentation"
      onClick={handleClose}
    >
      {/* Header with logo and close button */}
      <Box
        onClick={(event) => event.stopPropagation()}
        sx={{
          display: "flex",
          justifyContent: "space-between",
          alignItems: "center",
          p: { xs: 2, sm: 4 },
          pt: { xs: 2.5, sm: 5 },
          borderBottom: "1px solid rgba(255, 255, 255, 0.1)",
        }}
      >
        <Image
          src="https://artifact.aleasat.space/logos/ALEASAT_Logo.png"
          alt="AleaSat Logo"
          sx={{
            objectFit: "contain",
            height: { xs: 44, sm: 80 },
            width: { xs: 120, sm: 180 },
            transition: "all 0.3s ease",
            "&:hover": {
              transform: "scale(1.05)",
            },
          }}
        />
        <IconButton
          sx={{ 
            color: "white", 
            fontSize: "1.5rem",
            backgroundColor: "rgba(255, 255, 255, 0.1)",
            borderRadius: "50%",
            width: 48,
            height: 48,
            transition: "all 0.3s cubic-bezier(0.4, 0, 0.2, 1)",
            "&:hover": {
              backgroundColor: "rgba(74, 144, 226, 0.2)",
              transform: "rotate(90deg)",
            },
          }}
          onClick={handleClose}
          aria-label="Close menu"
        >
          ✕
        </IconButton>
      </Box>

      {/* Content Layout */}
      <Box
        onClick={(event) => event.stopPropagation()}
        sx={{
          display: "flex",
          minHeight: "calc(100dvh - 96px)",
          position: "relative",
          alignItems: { xs: "stretch", md: "flex-start" },
          justifyContent: "space-between",
          flexDirection: { xs: "column", md: "row" },
          px: { xs: 2, sm: 4, md: 6 },
          py: { xs: 2, sm: 4 },
          gap: { xs: 3, md: 8 },
        }}
      >
        {/* Left Column - Main Navigation */}
        <Box
          sx={{
            display: "flex",
            flexDirection: "column",
            gap: { xs: 1.5, md: 3 },
            justifyContent: "flex-start",
            flex: "0 0 auto",
            width: { xs: "100%", md: "auto" },
          }}
        >
          {links?.map(({ label, path }, index) => (
            <Typography
              key={label}
              component={Link}
              href={path}
              variant="h4"
              onClick={handleClose}
              sx={{
                textDecoration: "none",
                color: pathname === path ? "white" : "rgba(255, 255, 255, 0.7)",
                fontWeight: pathname === path ? "700" : "400",
                fontSize: { xs: "1.2rem", sm: "1.5rem", md: "2rem" },
                lineHeight: 1.2,
                position: "relative",
                padding: { xs: "10px 14px", sm: "12px 20px" },
                borderRadius: "12px",
                backgroundColor: pathname === path ? "rgba(74, 144, 226, 0.15)" : "transparent",
                border: pathname === path ? "1px solid rgba(74, 144, 226, 0.3)" : "1px solid transparent",
                transition: "all 0.3s cubic-bezier(0.4, 0, 0.2, 1)",
                animation: `slideInLeft 0.6s cubic-bezier(0.4, 0, 0.2, 1) ${index * 0.1}s both`,
                "@keyframes slideInLeft": {
                  from: {
                    opacity: 0,
                    transform: "translateX(-30px)",
                  },
                  to: {
                    opacity: 1,
                    transform: "translateX(0)",
                  },
                },
                "&:hover": {
                  color: "white",
                  backgroundColor: "rgba(74, 144, 226, 0.2)",
                  transform: { xs: "none", md: "translateX(10px)" },
                  border: "1px solid rgba(74, 144, 226, 0.4)",
                },
                "&::before": pathname === path ? {
                  content: '""',
                  position: "absolute",
                  left: 0,
                  top: "50%",
                  transform: "translateY(-50%)",
                  width: "4px",
                  height: "60%",
                  background: "linear-gradient(45deg, #4A90E2, #357ABD)",
                  borderRadius: "0 4px 4px 0",
                } : {},
              }}
            >
              {label}
            </Typography>
          ))}
          
          {/* Join Us Button */}
          <Box sx={{ mt: 4 }}>
            <Button
              component={Link}
              href="/join-us"
              onClick={handleClose}
              variant="contained"
              sx={{
                background: "linear-gradient(45deg, #4A90E2, #357ABD)",
                color: "white",
                borderRadius: "30px",
                textTransform: "none",
                fontSize: { xs: "1rem", md: "1.1rem" },
                fontWeight: 600,
                px: { xs: 3, md: 4 },
                py: { xs: 1.5, md: 2 },
                width: { xs: "100%", sm: "auto" },
                boxShadow: "0 8px 25px rgba(74, 144, 226, 0.3)",
                border: "1px solid rgba(255, 255, 255, 0.2)",
                transition: "all 0.3s cubic-bezier(0.4, 0, 0.2, 1)",
                animation: "slideInLeft 0.6s cubic-bezier(0.4, 0, 0.2, 1) 0.4s both",
                "&:hover": {
                  background: "linear-gradient(45deg, #5BA3F5, #4A90E2)",
                  boxShadow: "0 12px 35px rgba(74, 144, 226, 0.4)",
                  transform: "translateY(-3px)",
                },
                "&:active": {
                  transform: "translateY(-1px)",
                },
              }}
            >
              Join Our Team
            </Button>
          </Box>
        </Box>

        {SHOW_RESOURCES && (
          <>
            {/* Center Column - Secondary Links */}
            <Box
              sx={{
                display: "flex",
                flexDirection: "column",
                gap: { xs: 1.5, md: 3 },
                justifyContent: "flex-start",
                flex: "0 0 auto",
                pt: { xs: 0.5, md: 2 },
                width: { xs: "100%", md: "auto" },
              }}
            >
              <Typography
                variant="h6"
                sx={{
                  color: "rgba(255, 255, 255, 0.5)",
                  fontWeight: 600,
                  mb: 1,
                  textTransform: "uppercase",
                  letterSpacing: "0.1em",
                  fontSize: "0.9rem",
                }}
              >
                Resources
              </Typography>
              {[
                { label: "FAQ", path: "/faq" },
                { label: "Privacy Policy", path: "/privacy" },
                { label: "Terms of Service", path: "/terms" },
                { label: "Contact Us", path: "/contact" },
              ].map(({ label, path }, index) => (
                <Typography
                  key={label}
                  component={Link}
                  href={path}
                  variant="body1"
                  onClick={handleClose}
                  sx={{
                    textDecoration: "none",
                    color: "rgba(255, 255, 255, 0.6)",
                    display: "flex",
                    alignItems: "center",
                    gap: 2,
                    lineHeight: 1.5,
                    padding: "8px 16px",
                    borderRadius: "8px",
                    transition: "all 0.3s cubic-bezier(0.4, 0, 0.2, 1)",
                    animation: `slideInRight 0.6s cubic-bezier(0.4, 0, 0.2, 1) ${index * 0.1}s both`,
                    "@keyframes slideInRight": {
                      from: {
                        opacity: 0,
                        transform: "translateX(30px)",
                      },
                      to: {
                        opacity: 1,
                        transform: "translateX(0)",
                      },
                    },
                    "&:hover": {
                      color: "white",
                      backgroundColor: "rgba(255, 255, 255, 0.05)",
                      transform: "translateX(-5px)",
                    },
                  }}
                >
                  {label}
                  <Box
                    component="span"
                    sx={{
                      fontSize: "0.8rem",
                      opacity: 0.7,
                      transition: "all 0.3s ease",
                    }}
                  >
                    ↗
                  </Box>
                </Typography>
              ))}
            </Box>
          </>
        )}

        {/* Right Side - Satellite Image */}
        <Box
          sx={{
            display: { xs: "none", md: "flex" },
            justifyContent: "center",
            alignItems: "flex-start",
            flex: "1 1 auto",
            overflow: "hidden",
            position: "relative",
          }}
        >
          {/* Background glow effect */}
          <Box
            sx={{
              position: "absolute",
              top: "20%",
              right: "20%",
              width: "300px",
              height: "300px",
              background: "radial-gradient(circle, rgba(74, 144, 226, 0.2) 0%, transparent 70%)",
              borderRadius: "50%",
              animation: "pulse 4s ease-in-out infinite",
              "@keyframes pulse": {
                "0%, 100%": { opacity: 0.3, transform: "scale(1)" },
                "50%": { opacity: 0.6, transform: "scale(1.1)" },
              },
            }}
          />
          <Image
            src="/sathd2.png"
            alt="ALEASAT Satellite"
            sx={{
              width: { md: "520px", lg: "650px" },
              height: { md: "520px", lg: "650px" },
              objectFit: "contain",
              transform: {
                md: "rotate(-25deg) scale(1.2) translateY(-60px) translateX(40px)",
                lg: "rotate(-25deg) scale(1.3) translateY(-80px) translateX(70px)",
              },
              filter: "contrast(1.8) brightness(1.1)",
              animation: "float 6s ease-in-out infinite",
              "@keyframes float": {
                "0%, 100%": { transform: "rotate(-25deg) scale(1.3) translateY(-80px) translateX(70px)" },
                "50%": { transform: "rotate(-25deg) scale(1.3) translateY(-90px) translateX(75px)" },
              },
            }}
          />
        </Box>
      </Box>
    </Box>
  );
}
