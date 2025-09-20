"use client";

import Box from "@mui/material/Box";
import Grid from "@mui/material/Grid";
import Stack from "@mui/material/Stack";
import TextField from "@mui/material/TextField";
import { useAtom, useAtomValue } from "jotai";
import StepperButton from "../components/StepperButton";
import {
  AROGroundStationPassState,
  AROGroundStationSelectedState,
  ImageRequestNameAndDescriptionState,
} from "../recoilState";
import AROGroundStationPassSelect from "./components/AROGroundStationPassSelect";
import AROGroundStationSelect from "./components/AROGroundStationSelect";

export default function RequestDetails() {
  const [imageRequestNameAndDescription, setImageRequestNameAndDescription] =
    useAtom(ImageRequestNameAndDescriptionState);
  const AROGroundStationSelected = useAtomValue(AROGroundStationSelectedState);
  const AROGroundStationPass = useAtomValue(AROGroundStationPassState);

  return (
    <>
      <Grid
        container
        flexDirection={"column"}
        key={0}
        style={{ height: "85%" }}
      >
        <Grid item style={{ height: "8vh" }}></Grid>
        <Grid
          item
          container
          style={{
            width: "100%",
            height: "80%",
            aspectRatio: "34/24",
            justifyContent: "center",
          }}
        >
          <Box style={{ height: "100%", aspectRatio: "34/24" }}>
            <Stack spacing={4}>
              <TextField
                value={imageRequestNameAndDescription.name}
                required
                label="Name of the request"
                onChange={(e) => {
                  setImageRequestNameAndDescription((o) => ({
                    ...o,
                    name: e.target.value,
                  }));
                }}
              />
              <TextField
                rows={3}
                fullWidth
                multiline
                value={imageRequestNameAndDescription.description}
                required
                label="Description of the request"
                onChange={(e) => {
                  setImageRequestNameAndDescription((o) => ({
                    ...o,
                    description: e.target.value,
                  }));
                }}
              />
              <Stack direction="row" spacing={4} sx={{ width: "100%" }}>
                <AROGroundStationSelect />
                <AROGroundStationPassSelect />
              </Stack>
            </Stack>
          </Box>
        </Grid>
      </Grid>
      <StepperButton
        disable={
          imageRequestNameAndDescription.description === "" ||
          imageRequestNameAndDescription.name === "" ||
          (AROGroundStationSelected.id !== -1 && AROGroundStationPass.id === -1)
        }
      />
    </>
  );
}
