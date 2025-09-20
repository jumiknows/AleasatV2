"use client";

import { Stack } from "@mui/material";
import Grid from "@mui/material/Grid";
import dynamic from "next/dynamic";
import StepperButton from "../components/StepperButton";
import FOVForm from "./components/FOVForm";

const FOVMap = dynamic(async () => await import("./components/FOVMap"), {
  loading: () => <p>A map is loading</p>,
  ssr: false,
});

const FOVPolygon = dynamic(
  async () => await import("./components/FOVPolygon"),
  {
    loading: () => <p>A map is loading</p>,
    ssr: false,
  },
);

export default function NewImageRequest() {
  return (
    <>
      <Stack style={{ height: "85%" }}>
        <Grid item>
          <FOVForm />
        </Grid>
        <Grid
          item
          container
          style={{
            height: "100%",
            aspectRatio: "4/3",
            justifyContent: "center",
          }}
        >
          <FOVMap initialZoom={9} minZoom={4} maxZoom={10}>
            <FOVPolygon />
          </FOVMap>
        </Grid>
      </Stack>
      <StepperButton />
    </>
  );
}
