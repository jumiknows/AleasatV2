"use client";

import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import { useAtomValue } from "jotai";
import {
  AROGroundStationPassState,
  AROGroundStationSelectedState,
  ImageRequestNameAndDescriptionState,
  selectedPassState,
} from "../../recoilState";
import PreviewImage from "./PreviewImage";

export default function ReviewDetials() {
  const selectedPass = useAtomValue(selectedPassState);
  const AROGroundStationSelected = useAtomValue(AROGroundStationSelectedState);
  const AROGroundStationPass = useAtomValue(AROGroundStationPassState);
  const imageRequestNameAndDescription = useAtomValue(
    ImageRequestNameAndDescriptionState,
  );

  return (
    <Container>
      <Stack
        justifyContent="center"
        alignContent="center"
        sx={{ width: "100%" }}
      >
        <Box sx={{ width: "50%" }}>
          <PreviewImage />
        </Box>
        <Typography>Name: {imageRequestNameAndDescription.name}</Typography>
        <Typography>
          Description: {imageRequestNameAndDescription.description}
        </Typography>
        <Typography>
          At Time: {new Date(selectedPass.culmination.t).toString()}
        </Typography>
        <br />
        <Typography>
          Extra Ground Station:{" "}
          {AROGroundStationSelected.id !== -1
            ? AROGroundStationSelected.name
            : "None"}
        </Typography>
        <Typography>
          Transfer Time:{" "}
          {AROGroundStationSelected.id !== -1 &&
          AROGroundStationPass.culmination.t
            ? new Date(AROGroundStationPass.culmination.t).toString()
            : "N/A"}
        </Typography>
      </Stack>
    </Container>
  );
}
