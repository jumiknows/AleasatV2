import type { components } from "@aleasat/gmat-api";
import { runOrbitPropagation } from "../utils/runOrbitPropagation.js";

const runGmatOrbitPropagation = async (
  params: components["schemas"]["SatelliteState"],
): Promise<string | null> => {
  return await runOrbitPropagation(params);
};

export default { runGmatOrbitPropagation };
