import {
  MissionScheduleApi,
  Configuration as PassSchedulingConfiguration,
} from "@aleasat/pass-scheduling-api";

import {
  MissionApi,
  Configuration as MissionConfiguration,
  ResponseError,
} from "@aleasat/mission-api";

import { notFound } from "next/navigation";
import { cache } from "react";

export const getMission = cache(
  async ({
    missionId,
    accessToken,
  }: {
    missionId: string;
    accessToken: string;
  }) => {
    const missionApi = new MissionApi(
      new MissionConfiguration({
        accessToken,
      }),
    );

    const mission = await missionApi
      .getMission({ missionId })
      .catch((error) => {
        if (error instanceof ResponseError && error.response.status === 404)
          return undefined;
        throw error;
      });

    if (!mission) notFound();

    return mission;
  },
);

export const getMissionSchedule = cache(
  async ({
    missionId,
    accessToken,
  }: {
    missionId: string;
    accessToken: string;
  }) => {
    const missionScheduleApi = new MissionScheduleApi(
      new PassSchedulingConfiguration({
        accessToken,
      }),
    );

    const missionSchedule = await missionScheduleApi
      .getScheduledMission(
        { missionId },
        {
          headers: { Authorization: `Bearer ${accessToken}` },
        },
      )
      .catch((error) => {
        if (error instanceof ResponseError && error.response.status === 404)
          return undefined;
        throw error;
      });

    return missionSchedule;
  },
);
