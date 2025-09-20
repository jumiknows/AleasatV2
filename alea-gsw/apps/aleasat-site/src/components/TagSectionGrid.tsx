"use client";
import { Box, Typography } from "@mui/material";
import { AnimatePresence } from "framer-motion";
import { useState } from "react";
import Tag from "./Tag";

export default function TagSectionGrid({
  filters = [
    "ALL",
    "ADVISOR",
    "CAPTAIN",
    "ADMIN",
    "AOCS",
    "CDH",
    "COMMS",
    "EPS",
    "HARDWARE",
    "STRUCTURE",
    "MOPS",
    "GROUND",
    "PAYLOAD",
  ],
  title,
  children,
  activeFilter,
  setActiveFilter = (filter) => {
    setActive(filter);
  },
  maxWidth,
}: {
  filters?: string[];
  title: string;
  children: React.ReactNode;
  activeFilter: string;
  setActiveFilter: (filter: string) => void;
  maxWidth?: string;
}) {
  const [active, setActive] = useState(activeFilter);

  return (
    <>
      <Box
        sx={{
          position: "relative",
          background: "#000",
          textAlign: "center",
          pt: 0,
          pb: { xs: 15, md: 4 },
          px: {
            xs: 4,
            sm: 6,
            md: 15,
          },
          display: "flex",
          alignItems: "center",
          flexDirection: "column",
        }}
      >
        <Box
          sx={{
            display: "flex",
            alignItems: "center",
            flexDirection: "column",
            width: "100%",
          }}
        >
          <Typography
            component="h1"
            variant="h3"
            data-cy="team-section-title"
            sx={{ mt: 8, fontWeight: "bold" }}
          >
            {title}
          </Typography>
          <Box
            sx={{
              display: "flex",
              justifyContent: "center",
              width: { sm: "100%", md: "80%" },
              mx: "auto",
              gap: { xs: 2, sm: 4, md: 5 },
              flexWrap: "wrap",
              my: 8,
            }}
            data-cy="filter-buttons-container"
          >
            <AnimatePresence>
              {filters.map((category) => (
                <Tag
                  key={category}
                  onClick={() => {
                    setActiveFilter(category);
                    setActive(category);
                  }}
                  isActive={active === category}
                  category={category}
                >
                  {category}
                </Tag>
              ))}
            </AnimatePresence>
          </Box>

          <Box
            maxWidth={maxWidth ?? "xl"}
            data-cy="team-members-container"
            sx={{
              padding: 0,
              width: "100%",
            }}
          >
            <AnimatePresence mode="wait">{children}</AnimatePresence>
          </Box>
        </Box>
      </Box>
    </>
  );
}
