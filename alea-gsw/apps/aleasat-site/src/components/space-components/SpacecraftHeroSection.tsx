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
                  xs: "100%",
                  md: "50%",
                },
                top: {
                  xs: "unset",
                  md: "50%",
                },
                bottom: {
                  xs: "0%",
                  md: "unset",
                },
                left: {
                  xs: "50%",
                  md: "25%",
                },
                transform: "translate(-50%, -50%)",
              }}
            >
              <Typography
                variant="h1"
                sx={{
                  textAlign: "center",
                  fontWeight: 150,
                  textShadow: "0px 0px 5px #444",
                }}
              >
                ALEASAT
                <br />
                Design
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
