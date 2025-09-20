import "../instrumentation.js";

import workerpool from "workerpool";

import {
  workerFindFullPasses,
  workerFindPasses,
  workerGetGroundTrack,
} from "./orbitPropagationServiceWorkerFunction.js";

workerpool.worker({
  workerFindPasses,
  workerGetGroundTrack,
  workerFindFullPasses,
});
