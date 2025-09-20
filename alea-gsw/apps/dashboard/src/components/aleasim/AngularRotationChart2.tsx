"use client";

import { useAtomValue } from "jotai";
import ReadTimeChartDisplay from "./RealTimeChart";
import { simDataState } from "./recoilState";

export default function AngularRatesChart() {
  const simData = useAtomValue(simDataState);

  if (simData === undefined) return <></>;

  const startTime = new Date(simData.start_time).getTime();

  const adynHeaders = ["q1", "q3", "q3", "q4"];

  const data = simData.adyn_state.map((state, i) => {
    const pnt: Record<string, number> = {
      time: startTime + i * simData.sim_dt * 1000,
    };
    adynHeaders.forEach((header, j) => {
      pnt[header] = state[j];
    });
    return pnt;
  });

  return <ReadTimeChartDisplay data={data} dataKeys={adynHeaders} />;
}
