import Grid from "@mui/material/Grid2";
import GridItem from "./GridItem";

export default function Bronze() {
  return (
    <Grid
      container
      direction={"row"}
      data-testid="bronze"
      spacing={{ xs: 0, sm: 1 }}
      alignItems="center"
    >
      <GridItem
        size={4}
        src="https://artifact.aleasat.space/sponsors/ansys.svg"
      />
      <GridItem
        size={2}
        src="https://artifact.aleasat.space/sponsors/csdc.png"
      />
      <GridItem
        size={6}
        src="https://artifact.aleasat.space/sponsors/teksavvy.png"
      />
      <GridItem
        size={6}
        src="https://artifact.aleasat.space/sponsors/valispace.svg"
      />
      <GridItem
        size={6}
        src="https://artifact.aleasat.space/sponsors/solidworks.svg"
      />
      <Grid container size={3} direction="row">
        <GridItem
          size={12}
          src="https://artifact.aleasat.space/sponsors/sfu-compscie.png"
        />
        <GridItem
          size={12}
          src="https://artifact.aleasat.space/sponsors/digikey.svg"
        />
      </Grid>
      <GridItem
        size={{ xs: 3 }}
        src="https://artifact.aleasat.space/sponsors/geoscientists.png"
        taller
      />
      <Grid container size={6}>
        <GridItem
          size={3}
          src="https://artifact.aleasat.space/sponsors/tallysman.png"
        />
        <GridItem
          size={9}
          src="https://artifact.aleasat.space/sponsors/sfu-mechatronic.png"
        />
        <GridItem
          size={12}
          src="https://artifact.aleasat.space/sponsors/agi-wide.png"
        />
      </Grid>
    </Grid>
  );
}
