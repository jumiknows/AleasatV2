"use client";

import { MemberData, members } from "@/data/members";
import teams from "@/data/teams";
import { Box, Grid2, Paper, Stack, Typography } from "@mui/material";
import { AnimatePresence, motion } from "framer-motion";
import { useEffect, useState } from "react";
import TagSectionGrid from "../TagSectionGrid";
import PeopleCard from "./PeopleCard";
import SubteamDescription from "./SubteamDescription";

export default function OurMembersSection() {
  return (
    <>
      <Typography variant="h2" sx={{ textAlign: "center", mb: 2 }}>
        Our Members
      </Typography>
      <Stack
        direction="row"
        justifyContent="space-evenly"
        alignItems="stretch"
        useFlexGap
        sx={{ flexWrap: "wrap" }}
      >
        {Object.entries(members)
          .filter(([team]) => !["ADVISOR", "CAPTAIN"].includes(team))
          .map(([team, subSystemMembers], i) => {
            const teams = Array.isArray(subSystemMembers)
              ? { [team]: subSystemMembers }
              : subSystemMembers;
            const teamSize = Object.keys(teams).length;

            return (
              <Box
                key={i}
                sx={{
                  overflowX: "scroll",
                  flexGrow: teamSize,
                  p: 1,
                }}
              >
                <Stack
                  component={Paper}
                  variant="outlined"
                  justifyContent="start"
                  alignItems="center"
                  spacing={2}
                  sx={{
                    backgroundColor: "unset",
                    borderColor: "#fff",
                    p: 2,
                    minWidth: "fit-content",
                    // width: "100%",
                    minHeight: "fit-content",
                    height: "100%",
                  }}
                >
                  <Typography
                    variant="h5"
                    component="h3"
                    sx={{
                      textAlign: "center",
                    }}
                  >
                    {team}
                  </Typography>
                  {teams.lead && (
                    <Stack
                      direction="row"
                      alignItems="start"
                      justifyContent="center"
                    >
                      {teams.lead.map((lead, i) => (
                        <PeopleCard key={i} person={lead} />
                      ))}
                    </Stack>
                  )}
                  <Stack
                    direction="row"
                    useFlexGap
                    justifyContent="start"
                    sx={{
                      flexWrap: "wrap",
                      width: "fit-content",
                      minWidth:
                        teams.lead && Object.keys(teams.lead).length === 1
                          ? "400px"
                          : undefined,
                    }}
                  >
                    {Object.entries(teams)
                      .filter(([teamName]) => teamName !== "lead")
                      .map(([teamName, teamMembers], i) => {
                        const haveTeamLead = teamMembers.some((member) =>
                          member.role.toLowerCase().includes("lead"),
                        );
                        return (
                          <Stack key={`${team}-${i}`} alignItems="start">
                            {teamSize > 1 && (
                              <Typography>{teamName} Team</Typography>
                            )}
                            {teamMembers.map((member, i) => (
                              <Box
                                key={i}
                                sx={{
                                  paddingY: 0.5,
                                  ...(haveTeamLead &&
                                    !member.role
                                      .toLowerCase()
                                      .includes("lead") && {
                                      borderLeft: "1px solid",
                                      marginLeft: 1,
                                      paddingLeft: 1,
                                    }),
                                }}
                              >
                                <PeopleCard person={member} />
                              </Box>
                            ))}
                          </Stack>
                        );
                      })}
                  </Stack>
                </Stack>
              </Box>
            );
          })}
      </Stack>
    </>
  );
}
