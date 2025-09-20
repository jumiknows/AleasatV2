"use client";

import { CircularProgress, Typography } from "@mui/material";
import { useSetAtom } from "jotai";
import { useEffect } from "react";
import useSWRImmutable from "swr/immutable";
import { simDataState } from "./recoilState";

export default function SimFetcher() {
  const setSimData = useSetAtom(simDataState);

  // const { data, error, isLoading } = useSWRImmutable<
  //   {
  //     odyn_state: number[][];
  //     adyn_state: number[][];
  //     eps_state: number[][];
  //     sol_panels_state: number[][];
  //     start_time: string;
  //     sim_dt: number;
  //   },
  //   any,
  //   string
  // >(
  //   "http://localhost:8000/sim",
  //   async (url) =>
  //     await fetch(url, {
  //       method: "post",
  //       headers: {
  //         "Content-Type": "application/json",
  //       },
  //       body: JSON.stringify({
  //         sim_dt: 0.1,
  //         sim_duration: 60 * 90,
  //         tle: [
  //           "1 25544U 98067A   24285.20963619  .00088662  00000-0  15590-2 0  9999",
  //           "2 25544  51.6375 100.7254 0009189  58.4718 301.7167 15.49687368476546",
  //         ],
  //         adyn_state: [0, 0, 0, 1, 1.5, 0.78, 1.3],
  //       }),
  //     }).then(async (res) => await res.json()),
  // );

  const { data, error, isLoading } = useSWRImmutable<
    {
      odyn_state: number[][];
      adyn_state: number[][];
      eps_state: number[][];
      sol_panels_state: number[][];
      start_time: string;
      sim_dt: number;
    },
    any,
    string
  >(
    "https://artifact.aleasat.space/aleasim/sim-data.json",
    async (url) => await fetch(url).then(async (res) => await res.json()),
  );

  useEffect(() => {
    if (!data) return;
    setSimData(data);
  }, [data]);

  if (data) return <></>;
  if (isLoading)
    return (
      <CircularProgress
        style={{ width: "10vw", aspectRatio: "1/1", height: "unset" }}
      />
    );
  if (error) return <Typography>Failed to fetch sim data</Typography>;
}
