"use client";

import { Paper } from "@mui/material";
import {
  CategoryScale,
  Chart as ChartJS,
  Colors,
  Decimation,
  Legend,
  LineElement,
  LinearScale,
  PointElement,
  TimeScale,
  Title,
  Tooltip,
} from "chart.js";
import { Line } from "react-chartjs-2";
import "chartjs-adapter-dayjs-4";

import annotationPlugin from "chartjs-plugin-annotation";
import zoomPlugin from "chartjs-plugin-zoom";

ChartJS.register(
  zoomPlugin,
  annotationPlugin,
  Decimation,
  TimeScale,
  CategoryScale,
  LinearScale,
  Colors,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
);

export default function LineChartDisplay({
  data,
  dataKeys,
  title,
}: {
  data: Record<string, Array<{ x: number; y: number }>>;
  dataKeys: string[];
  title: string;
}) {
  return (
    <Paper sx={{ width: "100%", height: "100%" }}>
      <Line
        options={{
          responsive: true,
          animation: false,
          maintainAspectRatio: false,
          normalized: true,
          parsing: false,
          plugins: {
            legend: {
              position: "top",
            },
            title: {
              display: true,
              text: title,
            },
            zoom: {
              pan: {
                enabled: true,
                mode: "x",
              },
              zoom: {
                wheel: {
                  enabled: true,
                },
                pinch: {
                  enabled: true,
                },
                mode: "x",
              },
            },
            decimation: {
              enabled: true,
              algorithm: "lttb",
              samples: 100,
              threshold: 1,
            },
          },
          scales: {
            x: {
              type: "time",
              ticks: {
                source: "auto",
                maxRotation: 0,

                // Disabled rotation for performance
                autoSkip: true,
              },
            },
          },
          interaction: {
            mode: "nearest",
            axis: "x",
            intersect: false,
          },
        }}
        data={{
          datasets: dataKeys.map((key) => ({
            label: key,
            data: data[key],
          })),
        }}
      />
    </Paper>
  );
}
