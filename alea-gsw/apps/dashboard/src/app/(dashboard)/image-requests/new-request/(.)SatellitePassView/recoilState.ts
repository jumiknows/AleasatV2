import dayjs from "dayjs";
import utc from "dayjs/plugin/utc";
import { atom } from "jotai";

dayjs.extend(utc);

const DEFAULT_DATE_WINDOW = 5;
const DEFAULT_START_DATE = dayjs.utc().startOf("day");
const DEFAULT_END_DATE = DEFAULT_START_DATE.add(
  DEFAULT_DATE_WINDOW,
  "day",
).subtract(1, "second");

export const satellitePassFormDataState = atom({
  startDate: DEFAULT_START_DATE.toISOString(),
  endDate: DEFAULT_END_DATE.toISOString(),
});

export const openSatVizHelpState = atom(true);
