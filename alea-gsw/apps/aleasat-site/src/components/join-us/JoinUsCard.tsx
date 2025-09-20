import Link from "@aleasat/ui/components/Link";
import { Box, Grid2 as Grid, Typography } from "@mui/material";
import Image from "next/image";
import React from "react";
import type { PositionItem as positionItemType } from "./positions";
export default function JoinUsCard({
  title,
  description,
  skills,
  link,
}: positionItemType) {
  return (
    <Grid
      sx={{
        display: "flex",
        flexDirection: "column",
        padding: { xs: 2, md: 3 },
        border: 1,
        borderRadius: 4,
        gap: { xs: 1.5, md: 2 },
        textWrap: "pretty",
      }}
      data-cy={`join-us-card-${title}`}
    >
      <Typography
        component="h4"
        variant="h4"
        sx={{
          fontWeight: "bold",
        }}
        data-cy={`join-us-position-title-${title}`}
      >
        {title}
      </Typography>
      <Typography
        sx={{
          textWrap: "pretty",
          fontSize: { xs: 14, md: 16 },
        }}
        data-cy={`join-us-position-description-${title}`}
      >
        {description}
      </Typography>
      <Typography
        sx={{
          fontWeight: "bold",
          mt: 1,
        }}
        data-cy={`join-us-position-skills-header-${title}`}
      >
        Required Skills
      </Typography>
      <Box
        sx={{
          display: "flex",
          gap: 1,
          flexWrap: "wrap",
        }}
        data-cy={`join-us-position-skills-box-${title}`}
      >
        {skills.map((skill, index) => {
          return (
            <Typography
              key={index}
              sx={{
                padding: 1,
                paddingX: { xs: 1.5, md: 2 },
                backgroundColor: "rgba(51, 49, 54, 1)",
                borderRadius: 2,
                fontSize: { xs: 12, md: 16 },
              }}
              data-cy={`join-us-skill-${skill}-${index}-${title}`}
            >
              {skill}
            </Typography>
          );
        })}
      </Box>
      <Link
        target="_blank"
        href={link}
        sx={{
          textAlign: "right",
          color: "rgba(130, 144, 155, 1)",
          fontWeight: "bold",
          textDecoration: "none",
          "&:hover": {
            textDecoration: "underline",
          },
        }}
        data-cy="join-us-apply-link"
      >
        Apply Now
        <Image
          src={"https://artifact.aleasat.space/arrow.svg"}
          alt="Arrow"
          width={50}
          height={10}
        />
      </Link>
    </Grid>
  );
}
