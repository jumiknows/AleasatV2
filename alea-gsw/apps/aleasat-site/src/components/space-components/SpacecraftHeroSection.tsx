"use client";

import { Typography } from "@mui/material";
import { Box } from "@mui/system";
import { useEffect, useRef, useState } from "react";
import SpacecraftComponentNames from "./SpacecraftComponentNames";

export default function SpacecraftHeroSection() {
  const boxRef = useRef<HTMLDivElement | null>(null);
  const [scrollPosition, setScrollPosition] = useState(0);
  const [shiftPosition, setShiftPosition] = useState(0);
  useEffect(() => {
    const handleScroll = () => {
      if (boxRef.current) {
        const { clientHeight } = boxRef.current;
        const offsetTop = -boxRef.current.getBoundingClientRect().top;
        const scrollPercent = Math.min(
          Math.max(offsetTop / (clientHeight - 2 * window.innerHeight), 0),
          1,
        );
        setScrollPosition(scrollPercent);
        const shiftPercent = Math.min(
          Math.max(
            ((offsetTop - 1 * window.innerHeight) * 1.75) / window.innerHeight,
            0,
          ),
          1,
        );
        setShiftPosition(shiftPercent);
      }
    };
    handleScroll();

    window.addEventListener("scroll", handleScroll);

    return () => {
      window.removeEventListener("scroll", handleScroll);
    };
  }, []);

  return (
    <>
      <Box
        ref={boxRef}
        sx={{
          position: "relative",
          width: "100%",
          height: "300vh",
          "@media (max-aspect-ratio: 16/9)": {
            marginBottom: "unset",
          },
          "@media (min-aspect-ratio: 16/9)": {
            marginBottom: "calc(((100vw * 9) / 16) - 100vh)",
          },
        }}
      >
        <Box
          sx={{
            position: "absolute",
            "@media (max-aspect-ratio: 16/9)": {
              height: "100vh",
            },
            "@media (min-aspect-ratio: 16/9)": {
              aspectRatio: "16/9",
            },
            width: "100%",
            overflow: "hidden",
          }}
        >
          <Box
            sx={{
              "@media (max-aspect-ratio: 16/9)": {
                height: "100%",
              },
              "@media (min-aspect-ratio: 16/9)": {
                width: "100%",
              },
              aspectRatio: "16/9",
              transform: {
                xs: `translate(-66%, -50%)`,
                md: `translate(-50%, -50%)`,
              },
              top: "50%",
              left: "50%",
              position: "absolute",
            }}
          >
            <img
              alt="Render of Earth"
              src="https://artifact.aleasat.space/spacecraft-hero-video/rotate/bg.avif"
              style={{
                height: "100%",
                width: "100%",
              }}
            />
          </Box>
          <Box
            sx={{
              width: "100%",
              height: "100%",
              transform: "translate(-50%, -50%)",
              top: "50%",
              left: "50%",
              position: "absolute",
            }}
          >
            <Box
              sx={{
                position: "absolute",
                width: {
                  xs: "90%",
                  md: "60%",
                },
                top: {
                  xs: "unset",
                  md: "50%",
                },
                bottom: {
                  xs: "10%",
                  md: "unset",
                },
                left: {
                  xs: "50%",
                  md: "25%",
                },
                transform: "translate(-50%, -50%)",
                zIndex: 10,
              }}
            >
              <Typography
                variant="h1"
                sx={{
                  textAlign: "center",
                  fontWeight: 700,
                  fontSize: { xs: "3rem", sm: "4rem", md: "5rem", lg: "6rem" },
                  background: "linear-gradient(135deg, #ffffff8e 0%, #e3f2fd 50%, #4A90E2 100%)",
                  backgroundClip: "text",
                  WebkitBackgroundClip: "text",
                  color: "transparent",
                  textShadow: "none",
                  letterSpacing: "0.02em",
                  lineHeight: 1.1,
                  mb: 3,
                  animation: "fadeInUp 1.5s cubic-bezier(0.4, 0, 0.2, 1)",
                  "@keyframes fadeInUp": {
                    from: {
                      opacity: 0,
                      transform: "translateY(50px)",
                    },
                    to: {
                      opacity: 1,
                      transform: "translateY(0)",
                    },
                  },
                }}
              >
                {/* ALEASAT */}
                <br />
                <Box
                  component="span"
                  sx={{
                    fontSize: "0.7em",
                    fontWeight: 300,
                    background: "linear-gradient(45deg, #000000ff, #000000ff)",
                    backgroundClip: "text",
                    WebkitBackgroundClip: "text",
                    color: "transparent",
                  }}
                >
                  CubeSat Design
                </Box>
              </Typography>
              
              <Typography
                variant="h6"
                sx={{
                  textAlign: "center",
                  color: "rgba(255, 255, 255, 0.9)",
                  fontWeight: 300,
                  fontSize: { xs: "1rem", md: "1.25rem" },
                  maxWidth: "600px",
                  mx: "auto",
                  textShadow: "0 2px 10px rgba(0,0,0,0.5)",
                  animation: "fadeInUp 1.5s cubic-bezier(0.4, 0, 0.2, 1) 0.3s both",
                }}
              >
                {/* Explore our cutting-edge 1U CubeSat designed for Earth observation and disaster relief training */}
              </Typography>
            </Box>
          </Box>
        </Box>
        <Box sx={{ position: "sticky", top: 0 }}>
          <Box
            sx={{
              position: "relative",
              height: "100vh",
            }}
          >
            <Box
              sx={{
                position: "relative",
                "@media (max-aspect-ratio: 16/9)": {
                  height: "100vh",
                },
                "@media (min-aspect-ratio: 16/9)": {
                  aspectRatio: "16/9",
                },
                width: "100%",
                overflow: "hidden",
              }}
            >
              <Box
                sx={{
                  position: "absolute",
                  "@media (max-aspect-ratio: 16/9)": {
                    height: "100%",
                  },
                  "@media (min-aspect-ratio: 16/9)": {
                    width: "100%",
                  },
                  aspectRatio: "16/9",
                  top: "50%",
                  left: "50%",
                  transform: {
                    xs: `translate(${-66 + Math.min(scrollPosition * 2, 1) * 16}%, -50%)`,
                    md: "translate(-50%, -50%)",
                  },
                }}
                style={{
                  filter: `opacity(${1 - Math.min((scrollPosition - 0.4) * 7, 1)})`,
                }}
              >
                {Array.from({ length: 15 }, (_, i) => (
                  <Box
                    key={i}
                    sx={{
                      "@media (max-aspect-ratio: 16/9)": {
                        height: "100%",
                      },
                      "@media (min-aspect-ratio: 16/9)": {
                        width: "100%",
                      },
                      aspectRatio: "16/9",
                      top: "50%",
                      left: "50%",
                      position: "absolute",
                      transform: `translate(-50%, -50%)`,
                    }}
                  >
                    <img
                      alt=""
                      src={`https://artifact.aleasat.space/spacecraft-hero-video/rotate/${String(i * 2 + 30).padStart(4, "0")}.avif`}
                      style={{
                        height: "100%",
                        width: "100%",
                        visibility:
                          Math.floor(Math.min(scrollPosition * 2, 1) * 14) === i
                            ? "visible"
                            : "hidden",
                      }}
                    />
                  </Box>
                ))}
              </Box>
              <Box
                sx={{
                  left: `${-shiftPosition * 25}%`,
                  position: "absolute",
                  width: "100%",
                  height: "100%",
                }}
                style={{
                  filter: `opacity(${Math.max((scrollPosition - 0.45) * 20, 0)})`,
                }}
              >
                {Array.from({ length: 15 }, (_, i) => (
                  <Box
                    key={i}
                    sx={{
                      "@media (max-aspect-ratio: 16/9)": {
                        height: "100%",
                      },
                      "@media (min-aspect-ratio: 16/9)": {
                        width: "100%",
                      },
                      aspectRatio: "16/9",
                      transform: `translate(-50%, -50%)`,
                      top: "50%",
                      left: "50%",
                      position: "absolute",
                    }}
                  >
                    <img
                      alt=""
                      src={`https://artifact.aleasat.space/spacecraft-hero-video/expand/${String(i * 2 + 1).padStart(4, "0")}.avif`}
                      style={{
                        height: "100%",
                        width: "100%",
                        visibility:
                          Math.floor(
                            Math.max(scrollPosition * 2 - 1, 0) * 14,
                          ) === i
                            ? "visible"
                            : "hidden",
                      }}
                    />
                  </Box>
                ))}
                <Box
                  sx={{
                    "@media (max-aspect-ratio: 16/9)": {
                      height: "100%",
                    },
                    "@media (min-aspect-ratio: 16/9)": {
                      width: "100%",
                    },
                    aspectRatio: "16/9",
                    transform: `translate(-50%, -50%)`,
                    top: "50%",
                    left: "50%",
                    position: "absolute",
                    textShadow: {
                      xs: "-2px -2px 2px black, 2px -2px 2px black, -2px 2px 2px black, 2px 2px 2px black",
                      md: "unset",
                    },
                  }}
                  style={{
                    visibility:
                      Math.floor(Math.max(scrollPosition * 2 - 1, 0) * 14) > 13
                        ? "visible"
                        : "hidden",
                  }}
                >
                  <SpacecraftComponentNames />
                </Box>
              </Box>
            </Box>
          </Box>
        </Box>
      </Box>
    </>
  );
}
