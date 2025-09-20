"use client";

import { Box, Stack, Typography } from "@mui/material";
import HomeHeroBackground from "./HomeHeroBackground";
import MissionSection from "./missions/MissionSection";

export default function HomeHeroSection() {
  return (
    <>
      <HomeHeroBackground>
        {(scrollPosition) => (
          <Stack
            justifyContent="space-evenly"
            direction="column"
            sx={{
              height: {
                xs: "50vh",
                sm: "100vh",
              },
            }}
          >
            <Box
              sx={{
                display: scrollPosition < 0.3 ? "unset" : "none",
              }}
            >
              <Typography
                variant="h1"
                sx={{
                  fontSize: {
                    xs: "3rem",
                    md: "4rem",
                    lg: "5rem",
                  },
                }}
              >
                ALEASAT
              </Typography>
              <Typography
                sx={{
                  fontSize: {
                    xs: "0.8rem",
                    md: "1rem",
                    lg: "1.5rem",
                  },
                }}
              >
                Earth-observation satellite that can be used by radio amateurs
                to assist in disaster relief operations. Equipped with an
                on-board camera, ALEASAT will allow radio amateurs to request
                imagery of a specific location on Earth, and then downlink that
                imagery directly from the satellite.
              </Typography>
            </Box>
            <Box
              sx={{
                display:
                  scrollPosition >= 0.3 && scrollPosition < 0.6
                    ? "unset"
                    : "none",
              }}
            >
              <Typography
                sx={{
                  fontSize: {
                    xs: "0.8rem",
                    md: "1rem",
                    lg: "1.5rem",
                  },
                  mb: {
                    xs: 2,
                    lg: 8,
                  },
                }}
              >
                In contrast to methods used by most other amateur satellites
                with an Earth observation capability, ALEASAT’s targeted
                approach has direct applications to disaster monitoring and
                relief activities.
              </Typography>
              <Typography
                sx={{
                  fontSize: {
                    xs: "0.8rem",
                    md: "1rem",
                    lg: "1.5rem",
                  },
                }}
              >
                This project can serve as a precursor to a future amateur
                satellite constellation dedicated to disaster monitoring. In
                addition to ALEASAT itself, the team is also building the
                necessary ground station infrastructure in order to communicate
                with the satellite.
              </Typography>
            </Box>

            <Box sx={{ display: scrollPosition >= 0.6 ? "unset" : "none" }}>
              <MissionSection />
            </Box>
          </Stack>
        )}
      </HomeHeroBackground>
    </>
  );
}
