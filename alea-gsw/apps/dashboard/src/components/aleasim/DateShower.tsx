"use client";

import { JulianDate } from "cesium";
import { useAtomValue, useSetAtom } from "jotai";
import { useEffect } from "react";
import { useCesium } from "resium";
import { simDataState, simTimeIndexState, simTimeState } from "./recoilState";

export default function DateShower() {
  const { viewer } = useCesium();

  const simData = useAtomValue(simDataState);
  const setTime = useSetAtom(simTimeState);
  const setTimeIndex = useSetAtom(simTimeIndexState);

  useEffect(() => {
    if (!viewer || !simData) return;

    const interval = setInterval(() => {
      if (!viewer.clock) return;
      const startTime = new Date(simData.start_time).getTime();
      const t = JulianDate.toDate(viewer.clock.currentTime).getTime();
      setTime(t);
      setTimeIndex(
        Math.max(Math.round((t - startTime) / 1000 / simData.sim_dt), 0),
      );
    }, 1000);

    return () => {
      clearInterval(interval);
    };
  }, [viewer]);

  return <></>;
}
