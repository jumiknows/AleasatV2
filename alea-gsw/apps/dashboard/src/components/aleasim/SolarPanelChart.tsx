"use client";

import { useAtomValue } from "jotai";
import TimeSeriesChart from "./TimeSeriesChart";
import { simDataState } from "./recoilState";

export default function EpsChart() {
  const simData = useAtomValue(simDataState);

  if (simData === undefined) return <></>;

  const startTime = new Date(simData.start_time).getTime();

  const solarPanelHeaders = ["xn_panel", "yp_panel", "yn_panel", "zn_panel"];

  const solarPanelData: Record<string, Array<{ x: number; y: number }>> = {};

  solarPanelHeaders.forEach((header) => {
    solarPanelData[header] = [];
  });

  simData.sol_panels_state.forEach((state, i) => {
    const label = startTime + i * simData.sim_dt * 1000;
    solarPanelHeaders.forEach((header, j) => {
      solarPanelData[header].push({ x: label, y: state[j] });
    });
  });

  return (
    <TimeSeriesChart
      title="Solar Panel State"
      data={solarPanelData}
      dataKeys={solarPanelHeaders}
    />
  );
}
