import Image from "@aleasat/ui/components/Image";
import { Box, Grid, Typography } from "@mui/material";
// MissionItem.tsx
import React from "react";
import type { MissionItem as MissionItemType } from "./items";

export function MissionItem({ icon, title, description }: MissionItemType) {
  return (
    <Grid container spacing={2} alignItems="flex-start">
      {" "}
      {/* Align items to the top */}
      <Grid item>
        <Image
          src={`https://artifact.aleasat.space/logos/mission-statement-icons/${icon}`}
          alt={`${title} Logo`}
          // width={80}
          // height={80}
          sx={{
            width: { xs: 40, md: 60, lg: 70 },
            height: { xs: 40, md: 60, lg: 70 },
          }}
          style={{ objectFit: "contain" }}
        />
      </Grid>
      <Grid item xs>
        <Box>
          <Typography
            variant="h4"
            sx={{
              fontWeight: "bold",
              mb: 1,
              fontSize: { xs: "0.8rem", md: "1rem", lg: "1.5rem" },
            }}
          >
            {title}
          </Typography>
          <Typography
            sx={{
              fontSize: { xs: "0.6rem", md: "0.8rem", lg: "1rem" },
            }}
          >
            {description}
          </Typography>
        </Box>
      </Grid>
    </Grid>
  );
}
