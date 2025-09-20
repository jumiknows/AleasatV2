"use client";

import { useAtomValue } from "jotai";
import SatellitePassViz from "../../components/SatellitePassViz";
import {
  newImageRequestActiveStepState,
  selectedPassState,
} from "../../recoilState";

// biome-ignore lint/correctness/noEmptyPattern: for backwards compatibility
export default function SatelliteImagePassViz({}: { zoom: boolean }) {
  const selectedPass = useAtomValue(selectedPassState);
  const { activeStep, stepGoingforward } = useAtomValue(
    newImageRequestActiveStepState,
  );
  const fadeOut = activeStep > 1;

  return (
    <SatellitePassViz
      zoom={fadeOut}
      // targetLocation={targetLocation}
      selectedPass={selectedPass}
      initialZoom={!stepGoingforward}
    />
  );
}
