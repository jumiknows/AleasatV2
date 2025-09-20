import { Configuration, MissionApi } from "@aleasat/mission-api";
import { getServiceAccountAccessToken } from "../utils/getServiceAccountAccessToken.js";

export const missionApi = new MissionApi(
  new Configuration({
    accessToken: getServiceAccountAccessToken,
  }),
);
