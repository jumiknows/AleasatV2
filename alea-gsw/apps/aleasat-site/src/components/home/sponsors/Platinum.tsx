import Grid from "@mui/material/Grid2";
import GridItem from "./GridItem";

export default function Platinum() {
  return (
    <Grid
      container
      direction={"row"}
      data-testid="platinum"
      spacing={{ xs: 0, sm: 1 }}
      alignItems="center"
    >
      <GridItem
        size={4}
        src="https://artifact.aleasat.space/sponsors/pillon-holdings.png"
      />
      <GridItem
        size={4}
        src="https://artifact.aleasat.space/sponsors/AGI.png"
      />
      <GridItem
        size={4}
        src="https://artifact.aleasat.space/sponsors/rsl.svg"
      />
      <GridItem
        size={5}
        src="https://artifact.aleasat.space/sponsors/sfsu.png"
        taller
      />
      <GridItem
        size={7}
        src="https://artifact.aleasat.space/sponsors/csa.png"
        taller
      />
      <GridItem
        size={5}
        src="https://artifact.aleasat.space/sponsors/ebco.svg"
      />
      <GridItem
        size={7}
        src="https://artifact.aleasat.space/sponsors/triumf.svg"
      />
    </Grid>
  );
}
