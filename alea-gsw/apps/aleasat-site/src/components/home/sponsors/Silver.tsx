import Grid from "@mui/material/Grid2";
import GridItem from "./GridItem";

export default function Silver() {
  return (
    <Grid
      container
      direction={"row"}
      spacing={{ xs: 0, sm: 1 }}
      alignItems="center"
      data-testid="silver"
    >
      <GridItem
        size={6}
        src="https://artifact.aleasat.space/sponsors/ubc-apsci.png"
      />
      <GridItem
        size={2}
        src="https://artifact.aleasat.space/sponsors/csa-circle.png"
      />
      <GridItem
        size={4}
        src="https://artifact.aleasat.space/sponsors/affinity.png"
      />
      <GridItem
        size={3}
        src="https://artifact.aleasat.space/sponsors/sfu-apsci.png"
        taller
      />
      <GridItem
        size={3}
        src="https://artifact.aleasat.space/sponsors/geoscientists.png"
        taller
      />
      <Grid container size={6}>
        <GridItem
          size={8}
          src="https://artifact.aleasat.space/sponsors/innovative.png"
        />
        <GridItem
          size={4}
          src="https://artifact.aleasat.space/sponsors/candor.png"
        />
        <GridItem
          size={6}
          src="https://artifact.aleasat.space/sponsors/ieee.webp"
        />
        <GridItem
          size={6}
          src="https://artifact.aleasat.space/sponsors/mda.png"
        />
      </Grid>
      <GridItem
        size={4}
        src="https://artifact.aleasat.space/sponsors/hansen.png"
      />
      <GridItem
        size={2}
        src="https://artifact.aleasat.space/sponsors/sfuaerospace.svg"
      />
      <GridItem
        size={6}
        src="https://artifact.aleasat.space/sponsors/ubc-engphys.png"
      />
    </Grid>
  );
}
