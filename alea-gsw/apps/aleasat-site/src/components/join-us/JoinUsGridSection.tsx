"use client";
import { Grid, Typography } from "@mui/material";
import { useMemo, useState } from "react";
import TagSectionGrid from "../TagSectionGrid";
import JoinUsCard from "./JoinUsCard";
import { type PositionItem, positionItems } from "./positions";

const filters = [
  "ALL",
  "ADMIN",
  "AOCS",
  "CDH",
  "COMMS",
  "EPS",
  "HARDWARE",
  "STRUCTURE",
  "MOPS",
  "PAYLOAD",
];

export default function JoinUsSectionGrid() {
  const [active, setActive] = useState("ALL");

  const filteredItems = useMemo(() => {
    return active === "ALL"
      ? positionItems
      : positionItems.filter(({ team }: { team: string }) => team === active);
  }, [active]);

  return (
    <TagSectionGrid
      title="JOIN OUR TEAM"
      activeFilter={active}
      setActiveFilter={setActive}
      filters={filters}
      data-cy="join-us-tag-section"
    >
      <Grid
        container
        spacing={{ xs: 6, md: 8 }}
        textAlign="left"
        data-cy="join-us-grid-container"
      >
        {filteredItems.length > 0 ? (
          filteredItems.map((item: PositionItem, index: number) => (
            <Grid
              item
              xs={12}
              sm={6}
              lg={4}
              xl={3}
              key={`${item.title}-${index}`}
              data-cy={`join-us-position-card-${item.title}-${index}`}
            >
              <JoinUsCard {...item} />
            </Grid>
          ))
        ) : (
          <Grid item xs={12} data-cy="join-us-no-openings-message">
            <Typography variant="h6" sx={{ color: "#fff", mt: 3 }}>
              {active === "ALL" ? (
                <>
                  Sorry, no openings for ALEASAT are available as of right now.
                  Please follow our socials to see when they are available.
                </>
              ) : (
                <>
                  Sorry, no openings for this subsystem are available as of
                  right now. Please follow our socials to see when they are
                  available.
                </>
              )}
            </Typography>
          </Grid>
        )}
      </Grid>
    </TagSectionGrid>
  );
}
