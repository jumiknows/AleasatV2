import dynamic from "next/dynamic";

export default dynamic(async () => await import("./TimeSeriesChartDisplay"), {
  ssr: false,
});
