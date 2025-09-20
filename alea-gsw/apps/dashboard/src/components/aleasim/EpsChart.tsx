"use client";

import { useAtomValue } from "jotai";
import TimeSeriesChart from "./TimeSeriesChart";
import { simDataState } from "./recoilState";

export default function EpsChart() {
  const simData = useAtomValue(simDataState);

  if (simData === undefined) return <></>;

  const startTime = new Date(simData.start_time).getTime();

  const epsHeaders = [
    "battery_soc_percent",
    "battery_soc_Ah",
    "watt_hour_estimate",
    "solar_current",
    "load_current",
    "eps_voltage",
    "power_net",
    "power_in",
    "power_out",
  ];

  const epsData: Record<string, Array<{ x: number; y: number }>> = {};

  epsHeaders.forEach((header) => {
    epsData[header] = [];
  });

  simData.eps_state.forEach((state, i) => {
    const label = startTime + i * simData.sim_dt * 1000;
    epsHeaders.forEach((header, j) => {
      epsData[header].push({ x: label, y: state[j] });
    });
  });

  return (
    <TimeSeriesChart title="EPS State" data={epsData} dataKeys={epsHeaders} />
  );
}
