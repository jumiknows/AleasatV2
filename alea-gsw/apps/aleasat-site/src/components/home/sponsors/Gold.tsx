import { useTheme } from "@mui/material";
import Grid from "@mui/material/Grid2";
import useMediaQuery from "@mui/material/useMediaQuery";
import GridItem from "./GridItem";

export default function Gold() {
  const theme = useTheme();
  const isDesktop = useMediaQuery(theme.breakpoints.up("sm"));

  return (
    <Grid
      container
      direction={{ xs: "column", sm: "row" }}
      alignItems="center"
      spacing={{ sm: 1 }}
      data-testid="gold"
      sx={{ height: "100%" }}
    >
      <GridItem
        size={{ xs: 12, sm: 8 }}
        src="https://artifact.aleasat.space/sponsors/ubc-ece.png"
        taller={isDesktop}
        width={"100%"}
      />
      <GridItem
        size={{ xs: 12, sm: 4 }}
        src="https://artifact.aleasat.space/sponsors/AGI.png"
        taller={isDesktop}
        width={"100%"}
      />
      <GridItem
        size={{ xs: 12, sm: 6 }}
        src="https://artifact.aleasat.space/sponsors/harwin.png"
        taller={isDesktop}
        width={"100%"}
      />
      <GridItem
        size={{ xs: 12, sm: 6 }}
        src="https://artifact.aleasat.space/sponsors/altium.svg"
        taller={isDesktop}
        width={"100%"}
      />
    </Grid>
  );
}
