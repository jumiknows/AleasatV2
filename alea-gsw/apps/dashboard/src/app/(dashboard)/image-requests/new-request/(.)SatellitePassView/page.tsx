"use client";

import Box from "@mui/material/Box";
import Fade from "@mui/material/Fade";
import Stack from "@mui/material/Stack";
import { useAtomValue } from "jotai";
import dynamic from "next/dynamic";
import StepperButton from "../components/StepperButton";
import {
  newImageRequestActiveStepState,
  selectedPassState,
} from "../recoilState";
import SatellitePassForm from "./components/SatellitePassForm";
import SatellitePassList from "./components/SatellitePassList";

const SatViz = dynamic(async () => await import("./components/SatViz"), {
  ssr: false,
});

export default function NewImageRequest() {
  const { activeStep } = useAtomValue(newImageRequestActiveStepState);
  const fade = activeStep !== 1;
  const selectedPass = useAtomValue(selectedPassState);

  return (
    <>
      <Fade
        in={!fade}
        timeout={{ enter: 1000, exit: 1400 }}
        easing={{ exit: "cubic-bezier(1, 0, 1.0, 1.0)" }}
      >
        <Stack justifyContent="center" sx={{ width: "100%" }}>
          <SatellitePassForm />
          <Stack
            direction={"row"}
            style={{
              height: "70vh",
              width: "100%",
            }}
            justifyContent="center"
          >
            <Box
              sx={{
                height: "70vh",
                width: "20%",
                overflowY: "scroll",
              }}
            >
              <SatellitePassList />
            </Box>
            <Box sx={{ height: "100%", aspectRatio: "4/3" }}>
              {selectedPass.id !== -1 && <SatViz />}
            </Box>
          </Stack>
        </Stack>
      </Fade>
      <StepperButton disable={selectedPass.id === -1} />
    </>
  );
}
