import { Box } from "@mui/material";
import dynamic from "next/dynamic";
import type { ComponentProps } from "react";

const TimeSeriesChartDisplay2 = dynamic(
  async () => await import("./RealTimeChart"),
  {
    ssr: false,
  },
);

export default function TimeSeriesChart(
  props: ComponentProps<typeof TimeSeriesChartDisplay2>,
) {
  return (
    <Box sx={{ position: "relative", width: "100%", height: "100%" }}>
      <TimeSeriesChartDisplay2 {...props} />
    </Box>
  );
}
