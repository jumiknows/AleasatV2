"use client";

import { useAtomValue } from "jotai";
import RealTimeChart from "./RealTimeChart";
import { simDataState } from "./recoilState";

export default function EpsChart() {
  const simData = useAtomValue(simDataState);

  if (simData === undefined) return <></>;

  const startTime = new Date(simData.start_time).getTime();

  const solarPanelHeaders = ["xn_panel", "yp_panel", "yn_panel", "zn_panel"];

  const data = simData.sol_panels_state.map((state, i) => {
    const pnt: Record<string, number> = {
      time: startTime + i * simData.sim_dt * 1000,
    };
    solarPanelHeaders.forEach((header, j) => {
      pnt[header] = state[j];
    });
    return pnt;
  });

  return <RealTimeChart data={data} dataKeys={solarPanelHeaders} />;
}
