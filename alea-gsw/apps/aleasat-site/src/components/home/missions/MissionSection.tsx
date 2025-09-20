import { Box, Grid, Typography } from "@mui/material";
import { MissionItem } from "./MissionItem";
import { missionItems } from "./items";

export default function MissionSection() {
  return (
    <Box
      sx={{
        position: "relative",
        textAlign: "center",
        display: "flex",
        alignItems: "center",
      }}
    >
      <Box>
        <Typography
          data-testid="mission-title"
          variant="h2"
          sx={{
            mb: {
              xs: 1,
              lg: 2,
            },
            fontWeight: "bold",
            fontSize: { xs: "1rem", md: "1.5rem", lg: "1.5rem" },
          }}
        >
          OUR MISSION
        </Typography>
        <Typography
          data-testid="mission-description"
          variant="h3"
          sx={{
            mb: {
              xs: 1,
              lg: 3,
            },
            fontSize: { xs: "0.8rem", md: "1rem", lg: "1.5rem" },
          }}
        >
          Teaching science and engineering students about satellite
          communication.
        </Typography>
        <Grid
          container
          spacing={{
            xs: 1,
            lg: 2,
          }}
          justifyContent="center"
          textAlign="left"
        >
          {missionItems.map((item: { title: any }) => (
            <Grid item xs={12} key={item.title}>
              <MissionItem icon={""} description={""} {...item} />
            </Grid>
          ))}
        </Grid>
      </Box>
    </Box>
  );
}
