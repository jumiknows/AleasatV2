import type { SatelliteState } from "@aleasat/job-queue-api";
import { runOrbitPropagation } from "../utils/runOrbitPropagation.js";

const runGmatOrbitPropagation = async (
  params: SatelliteState,
): Promise<string | null> => {
  return await runOrbitPropagation(params);
};

export default { runGmatOrbitPropagation };
