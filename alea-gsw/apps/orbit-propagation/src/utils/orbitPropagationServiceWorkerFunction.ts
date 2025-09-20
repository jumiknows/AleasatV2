import "../instrumentation.js";

import { context, propagation, trace } from "@opentelemetry/api";
import {
  eciToGeo,
  eciToLookAgnles,
  findCrossingPoint,
  findLocalMaxima,
} from "./orbitPropagationUtils.js";

const tracer = trace.getTracer("orbitPropagationServiceWorker");

export function workerGetGroundTrack(
  data: SharedArrayBuffer,
  t: number,
  // biome-ignore lint/complexity/noBannedTypes: contextCarrier is a generic object
  contextCarrier: {},
): number[][] {
  const activeContext = propagation.extract(context.active(), contextCarrier);
  const span = tracer.startSpan(
    "workerGetGroundTrack",
    { attributes: {} },
    activeContext,
  );
  trace.setSpan(activeContext, span);

  const dayData = new Float64Array(data);
  const stepIndex = dayData.findIndex(
    (utcGregorian) => Math.abs(utcGregorian - new Date(t).getTime()) <= 1000,
  );
  const groundTrack = [];
  for (let i = 0; i < 90; i++) {
    const index = stepIndex - 4 * 45 * 60 + i * 4 * 60;
    if (index < dayData.length && index >= 0)
      groundTrack.push({
        utcGregorian: dayData[index],
        x: dayData[index + 1],
        y: dayData[index + 2],
        z: dayData[index + 3],
      });
  }
  const geo = eciToGeo(groundTrack);

  span.end();
  return geo;
}

export function workerFindPasses(
  data: SharedArrayBuffer,
  lat: number,
  lng: number,
  minElevation: number,
  // biome-ignore lint/complexity/noBannedTypes: contextCarrier is a generic object
  contextCarrier: {},
): Array<{
  t: number;
  alt: number;
  az: number;
  lat: number;
  lng: number;
  height: number;
  x: number;
  y: number;
  z: number;
}> {
  const activeContext = propagation.extract(context.active(), contextCarrier);
  const span = tracer.startSpan(
    "workerFindPasses",
    { attributes: {} },
    activeContext,
  );
  trace.setSpan(activeContext, span);

  const lookAngles = eciToLookAgnles({
    ecis: new Float64Array(data),
    lat,
    lng,
    minElevation,
  });
  const maximas = findLocalMaxima(lookAngles).filter(
    (maxima) => maxima.alt >= minElevation,
  );

  span.end();
  return maximas;
}

export function workerFindFullPasses(
  db: Record<string, SharedArrayBuffer>,
  partialPasses: Awaited<ReturnType<typeof eciToLookAgnles>>,
  lat: number,
  lng: number,
  minElevation: number,
  // biome-ignore lint/complexity/noBannedTypes: contextCarrier is a generic object
  contextCarrier: {},
): Array<{
  rise: {
    t: number;
    x: number;
    y: number;
    z: number;
  };
  set: {
    t: number;
    x: number;
    y: number;
    z: number;
  };
  t: number;
  alt: number;
  az: number;
  lat: number;
  lng: number;
  height: number;
  x: number;
  y: number;
  z: number;
}> {
  const activeContext = propagation.extract(context.active(), contextCarrier);
  const span = tracer.startSpan(
    "workerFindFullPasses",
    { attributes: {} },
    activeContext,
  );
  trace.setSpan(activeContext, span);

  const typedDb: Record<string, Float64Array> = {};
  Object.keys(db).forEach((key) => {
    typedDb[key] = new Float64Array(db[key]);
  });

  const fullPasses = partialPasses
    .map((pass) => {
      const rise = findCrossingPoint(typedDb, pass, -1, minElevation, lat, lng);
      const set = findCrossingPoint(typedDb, pass, +1, minElevation, lat, lng);
      if (!rise || !set) return undefined;
      return { ...pass, rise, set };
    })
    .filter((pass) => pass !== undefined) as Array<{
    rise: {
      t: number;
      x: number;
      y: number;
      z: number;
    };
    set: {
      t: number;
      x: number;
      y: number;
      z: number;
    };
    t: number;
    alt: number;
    az: number;
    lat: number;
    lng: number;
    height: number;
    x: number;
    y: number;
    z: number;
  }>;
  span.end();
  return fullPasses;
}
