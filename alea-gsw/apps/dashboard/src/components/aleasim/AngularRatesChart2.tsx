"use client";

import { useAtomValue } from "jotai";
import RealTimeChart from "./RealTimeChart";
import { simDataState } from "./recoilState";

export default function AngularRatesChart() {
  const simData = useAtomValue(simDataState);

  if (simData === undefined) return <></>;

  const startTime = new Date(simData.start_time).getTime();

  const adynHeaders = ["w1", "w2", "w3"];

  const data = simData.adyn_state.map((state, i) => {
    const pnt: Record<string, number> = {
      time: startTime + i * simData.sim_dt * 1000,
    };
    adynHeaders.forEach((header, j) => {
      pnt[header] = state[j + 4];
    });
    return pnt;
  });

  return <RealTimeChart data={data} dataKeys={["w1", "w2", "w3"]} />;
}
