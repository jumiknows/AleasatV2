import { atom } from "jotai";

export const simDataState = atom<
  | {
      odyn_state: number[][];
      adyn_state: number[][];
      eps_state: number[][];
      sol_panels_state: number[][];
      start_time: string;
      sim_dt: number;
    }
  | undefined
>(undefined);

export const simIndexState = atom(0);

export const simTimeState = atom(0);

export const simTimeIndexState = atom(0);
