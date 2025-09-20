"use client";

import AngularRatesChart2 from "@/components/aleasim/AngularRatesChart2";
import AngularRotationChart2 from "@/components/aleasim/AngularRotationChart2";
import SimFetcher from "@/components/aleasim/SimFetcher";
import SolarPanelChart2 from "@/components/aleasim/SolarPanelChart2";
import { Grid2 } from "@mui/material";
import dynamic from "next/dynamic";

import AngularRatesChart from "@/components/aleasim/AngularRatesChart";
import AngularRotationChart from "@/components/aleasim/AngularRotationChart";
import EpsChart from "@/components/aleasim/EpsChart";
import SolarPanelChart from "@/components/aleasim/SolarPanelChart";

const CesiumGlobe = dynamic(
  async () => await import("@/components/aleasim/CesiumGlobe"),
  {
    ssr: false,
  },
);

export default function Page() {
  return (
    <>
      <SimFetcher />
      <Grid2 container spacing={2}>
        <Grid2 size={6} sx={{ height: "80vh" }}>
          <CesiumGlobe />
        </Grid2>
        <Grid2 container size={6} sx={{ height: "80vh", overflowY: "scroll" }}>
          <Grid2 size={12} sx={{ height: "40vh" }}>
            <AngularRotationChart2 />
          </Grid2>
          <Grid2 size={12} sx={{ height: "40vh" }}>
            <AngularRatesChart2 />
          </Grid2>
          <Grid2 size={12} sx={{ height: "40vh" }}>
            <SolarPanelChart2 />
          </Grid2>
        </Grid2>
        <Grid2 container size={6} spacing={2}>
          <Grid2 size={12} sx={{ height: "40vh" }}>
            <AngularRotationChart />
          </Grid2>
          <Grid2 size={12} sx={{ height: "40vh" }}>
            <AngularRatesChart />
          </Grid2>
          <Grid2 size={12} sx={{ height: "40vh" }}>
            <SolarPanelChart />
          </Grid2>
          <Grid2 size={12} sx={{ height: "40vh" }}>
            <EpsChart />
          </Grid2>
        </Grid2>
      </Grid2>
    </>
  );
}
