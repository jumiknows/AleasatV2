"use client";

import { useAtomValue } from "jotai";
import TimeSeriesChart from "./TimeSeriesChart";
import { simDataState } from "./recoilState";

export default function AngularRatesChart() {
  const simData = useAtomValue(simDataState);

  if (simData === undefined) return <></>;

  const startTime = new Date(simData.start_time).getTime();

  const angularRates: Record<string, Array<{ x: number; y: number }>> = {};
  const adynHeaders = ["w1", "w2", "w3"];
  adynHeaders.forEach((header) => {
    angularRates[header] = [];
  });

  simData.adyn_state.forEach((state, i) => {
    const label = startTime + i * simData.sim_dt * 1000;
    adynHeaders.forEach((header, j) => {
      angularRates[header].push({ x: label, y: state[j + 4] });
    });
  });

  return (
    <TimeSeriesChart
      title="Angular Rate"
      data={angularRates}
      dataKeys={["w1", "w2", "w3"]}
    />
  );
}
