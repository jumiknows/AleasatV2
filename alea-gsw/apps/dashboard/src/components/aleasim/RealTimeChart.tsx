"use client";

import { Paper } from "@mui/material";

import {
  // CartesianGrid,
  Legend,
  Line,
  LineChart,
  ResponsiveContainer,
  Tooltip,
  XAxis,
  YAxis,
} from "recharts";

import { useAtomValue } from "jotai";
import { simDataState, simTimeIndexState } from "./recoilState";

const colors = [
  "rgb(54, 162, 235)", // blue
  "rgb(255, 99, 132)", // red
  "rgb(255, 159, 64)", // orange
  "rgb(255, 205, 86)", // yellow
  "rgb(75, 192, 192)", // green
  "rgb(153, 102, 255)", // purple
  "rgb(201, 203, 207)", // grey
];

export default function RealTimeChart({
  data,
  dataKeys,
  timeKey = "time",
}: {
  data: Record<string, number>[];
  dataKeys: string[];
  timeKey?: string;
}) {
  const timeIndex = useAtomValue(simTimeIndexState);
  const simData = useAtomValue(simDataState);

  if (simData === undefined) return <></>;

  return (
    <Paper sx={{ width: "100%", height: "100%" }}>
      <ResponsiveContainer width="100%" height="100%">
        <LineChart
          data={data
            .slice(Math.max(timeIndex - 60 / simData.sim_dt, 0), timeIndex)
            .filter((_, i) => i % (1 / simData.sim_dt) === 0)}
        >
          {/* <CartesianGrid strokeDasharray="3 3" /> */}
          <XAxis
            dataKey={timeKey}
            tickFormatter={(tickItem) =>
              new Date(tickItem as number).toTimeString().slice(0, 8)
            }
          />
          <YAxis />
          <Tooltip />
          <Legend />
          {dataKeys.map((key, i) => (
            <Line
              type="monotone"
              dataKey={key}
              key={key}
              isAnimationActive={false}
              dot={false}
              stroke={colors[i % colors.length]}
              strokeWidth={2}
            />
          ))}
        </LineChart>
      </ResponsiveContainer>
    </Paper>
  );
}
