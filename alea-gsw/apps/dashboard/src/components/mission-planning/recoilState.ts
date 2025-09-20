import { commits } from "@aleasat/command-spec";
import type { components } from "@aleasat/mission-api";
import { atom } from "jotai";

export const newMissionState = atom<components["schemas"]["WriteMission"]>({
  fwVersion: commits[1],
  commands: [
    // {
    //   sequenceNumber: 1,
    //   commandId: 2,
    //   commandName: "GET_TIME",
    //   timeOffset: 0,
    //   arguments: [],
    // },
  ],
});

export const newMissionCommandsState = atom([
  { id: 1, commandName: "", commandId: -1, args: [""] },
]);
