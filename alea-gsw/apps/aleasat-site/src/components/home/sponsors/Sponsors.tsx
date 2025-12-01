"use client";

import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import { AnimatePresence } from "framer-motion";
import { useState } from "react";
import Tag from "../../Tag";
import Bronze from "./Bronze";
import Gold from "./Gold";
import Platinum from "./Platinum";
import Silver from "./Silver";

export default function Sponsors() {
  const [active, setActive] = useState<string>("PLATINUM");

  return (
    <Box
      sx={{
        position: "relative",
        background: "radial-gradient(#366DA3, #000 80%)",
        height: { xs: "105vh", md: "100vh" },
        width: "100%",
        textAlign: "center",
      }}
    >
      <Box
        sx={{
          px: {
            xs: 4,
            sm: 6,
            md: 0,
          },
        }}
      >
        <Typography
          component="h1"
          variant="h3"
          sx={{ mb: 5, fontWeight: "bold" }}
        >
          SPONSORS
        </Typography>
        <Typography component="h2" variant="h6" sx={{ mb: 5 }}>
          Join our group of sponsors to make an impact and support our vision for the
          future.
        </Typography>
      </Box>
      <Box
        sx={{
          mx: "auto",
          width: { md: 900, xl: 1000 },
          px: 2,
        }}
      >
        <Box
          data-testid="category-container"
          sx={{
            display: "flex",
            justifyContent: "space-around",
            width: { xs: "100%", sm: "60%" },
            mx: "auto",
            mb: 2,
          }}
        >
          <AnimatePresence>
            {["PLATINUM", "GOLD", "SILVER", "BRONZE"].map((category, key) => (
              <Tag
                key={key}
                onClick={() => setActive(category)}
                isActive={active === category}
                category={category}
              >
                {category}
              </Tag>
            ))}
          </AnimatePresence>
        </Box>
        <Box>
          {active === "PLATINUM" && <Platinum />}
          {active === "GOLD" && <Gold />}
          {active === "SILVER" && <Silver />}
          {active === "BRONZE" && <Bronze />}
        </Box>
      </Box>
    </Box>
  );
}
