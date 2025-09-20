"use client";

import teams from "@/data/teams";
import { Box, Container, Grid2, Link, Stack, Typography } from "@mui/material";
import Image from "next/image";
import { useEffect, useState } from "react";

export default function SubsystemListSection() {
  const [currentTeam, setTeam] = useState(teams[0]);

  useEffect(() => {
    const changeTeam = () => {
      setTeam((team) => {
        const currentIndex = teams.indexOf(team);
        const nextIndex = (currentIndex + 1) % teams.length;
        return teams[nextIndex];
      });
    };
    const interval = setInterval(changeTeam, 4000);
    return () => clearInterval(interval);
  }, []);

  return (
    <>
      <Container maxWidth="lg">
        <Typography variant="h2" sx={{ textAlign: "center", mb: 2, mt: 20 }}>
          Our Subsystems
        </Typography>
        <Grid2
          container
          sx={{
            pb: 15,
          }}
        >
          <Grid2 size={{ xs: 12, md: 6 }} order={{ xs: 2, md: 1 }}>
            <Stack>
              {teams.map((team, i) => (
                <Box
                  key={i}
                  sx={{
                    p: 1,
                    border: "solid #fff",
                  }}
                  style={{
                    borderWidth:
                      team === currentTeam
                        ? "1px 0px 1px 0px"
                        : // : "0px 1px 0px 0px",
                          "0px",
                  }}
                >
                  <Typography variant="h5" component="h3">
                    {team.name}
                  </Typography>
                  {/* <Typography>{team.description}</Typography> */}
                </Box>
              ))}
            </Stack>
          </Grid2>
          <Grid2
            size={{ xs: 12, md: 6 }}
            order={{ xs: 1, md: 2 }}
            sx={{
              // border: "solid #fff",
              // borderWidth: "1px 1px 1px 0px",
              p: 2,
            }}
          >
            <Typography
              variant="h4"
              component="h3"
              sx={{
                textAlign: "center",
                mb: 2,
              }}
            >
              {currentTeam.name}
            </Typography>
            <Box
              sx={{
                position: "relative",
                width: "70%",
                aspectRatio: "16/9",
                marginX: "auto",
              }}
            >
              <Image src={currentTeam.imageUrl} fill alt={currentTeam.name} />
            </Box>
            <Typography
              sx={{
                mt: 2,
              }}
            >
              {currentTeam.description}
            </Typography>
          </Grid2>
        </Grid2>
      </Container>
    </>
  );
}
