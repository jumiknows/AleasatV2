"use client";

import { useAtom } from "jotai";
import SatellitePassList from "../../components/SatellitePassList";
import { selectedPassState, targetLocationState } from "../../recoilState";
import { satellitePassFormDataState } from "../recoilState";

export default function Page() {
  const [targetLocation] = useAtom(targetLocationState);
  const [satellitePassFormData] = useAtom(satellitePassFormDataState);
  const selectedPassValueSetter = useAtom(selectedPassState);

  return (
    <SatellitePassList
      targetLocation={targetLocation}
      timeRange={satellitePassFormData}
      selectedPassValueSetter={selectedPassValueSetter}
    />
  );
}
