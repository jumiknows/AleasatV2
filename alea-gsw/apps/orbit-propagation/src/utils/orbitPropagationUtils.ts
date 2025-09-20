import type { components } from "@aleasat/gmat-api";
import satellite from "satellite.js";

const radToDeg = 180 / Math.PI;

export const eciToLookAgnles = ({
  ecis,
  lat,
  lng,
  minElevation,
}: {
  ecis: Float64Array;
  lat: number;
  lng: number;
  minElevation: number;
}): Array<{
  t: number;
  alt: number;
  az: number;
  lat: number;
  lng: number;
  height: number;
  x: number;
  y: number;
  z: number;
}> => {
  const array = [];

  for (let i = 0; i < ecis.length / 4; i++) {
    const index = i * 4;
    const observerGd: satellite.GeodeticLocation = {
      latitude: satellite.degreesToRadians(lat),
      longitude: satellite.degreesToRadians(lng),
      height: 0,
    };
    const gmst = satellite.gstime(new Date(ecis[index]));
    const eci = { x: ecis[index + 1], y: ecis[index + 2], z: ecis[index + 3] };
    const positionEcf = satellite.eciToEcf(eci, gmst);
    const lookAngles = satellite.ecfToLookAngles(observerGd, positionEcf);
    const alt = lookAngles.elevation * radToDeg;
    if (alt < minElevation - 5) continue;
    const geodetic = satellite.eciToGeodetic(eci, gmst);

    array.push({
      ...eci,
      t: ecis[index],
      alt,
      az: lookAngles.azimuth * radToDeg,
      lat: satellite.degreesLat(geodetic.latitude),
      lng: satellite.degreesLong(geodetic.longitude),
      height: geodetic.height,
    });
  }

  return array;
};

export const eciToLookAgnle = ({
  eciArray,
  lat,
  lng,
}: {
  eciArray: Float64Array;
  lat: number;
  lng: number;
}): {
  t: number;
  alt: number;
  az: number;
  lat: number;
  lng: number;
  height: number;
  x: number;
  y: number;
  z: number;
} => {
  const observerGd: satellite.GeodeticLocation = {
    latitude: satellite.degreesToRadians(lat),
    longitude: satellite.degreesToRadians(lng),
    height: 0,
  };
  const gmst = satellite.gstime(new Date(eciArray[0]));
  const eci = { x: eciArray[1], y: eciArray[2], z: eciArray[3] };
  const positionEcf = satellite.eciToEcf(eci, gmst);
  const lookAngles = satellite.ecfToLookAngles(observerGd, positionEcf);
  const geodetic = satellite.eciToGeodetic(eci, gmst);

  return {
    ...eci,
    t: eciArray[0],
    alt: lookAngles.elevation * radToDeg,
    az: lookAngles.azimuth * radToDeg,
    lat: satellite.degreesLat(geodetic.latitude),
    lng: satellite.degreesLong(geodetic.longitude),
    height: geodetic.height,
  };
};

export const eciToGeo = (
  ecis: components["schemas"]["GmatOrbitPropagation"]["result"],
): number[][] =>
  ecis.map((eci) => {
    const gmst = satellite.gstime(new Date(eci.utcGregorian));
    const geodetic = satellite.eciToGeodetic(eci, gmst);
    return [
      satellite.degreesLat(geodetic.latitude),
      satellite.degreesLong(geodetic.longitude),
      geodetic.height,
    ];
  });

export const findLocalMaxima = <T>(arr: Array<T & { alt: number }>): T[] => {
  const maxes: T[] = [];
  for (let i = 1; i < arr.length - 1; ++i) {
    if (arr[i - 1].alt < arr[i].alt && arr[i].alt > arr[i + 1].alt)
      maxes.push(arr[i]);
  }
  return maxes;
};

export const getDayFromTime = (t: number | string | Date): string =>
  (t instanceof Date ? t : new Date(t)).toISOString().slice(0, 10);

export const getStepFromOffset = (
  db: Record<string, Float64Array>,
  origin: number,
  offset: number,
): Float64Array | undefined => {
  const originArray = db[getDayFromTime(origin)];
  const prevArray = db[getDayFromTime(origin - 24 * 60 * 60 * 1000)];
  const nextArray = db[getDayFromTime(origin + 24 * 60 * 60 * 1000)];
  const originIndex = originArray.indexOf(origin);
  const offsetIndex = originIndex + offset * 4;
  if (offsetIndex >= originArray.length) {
    if (!nextArray) return undefined;
    return nextArray.slice(
      offsetIndex - originArray.length,
      offsetIndex - originArray.length + 4,
    );
  }
  if (offsetIndex < 0) {
    if (!prevArray) return undefined;
    return prevArray.slice(offsetIndex, offsetIndex + 4);
  }
  return originArray.slice(offsetIndex, offsetIndex + 4);
};

export const findCrossingPoint = (
  db: Record<string, Float64Array>,
  pass: Awaited<ReturnType<typeof eciToLookAgnles>>[number],
  direction: number,
  crossingPoint: number,
  lat: number,
  lng: number,
): { t: number; x: number; y: number; z: number } | undefined => {
  let nextStep = getStepFromOffset(db, pass.t, direction);
  let nextStepLookangle =
    nextStep && eciToLookAgnle({ eciArray: nextStep, lat, lng });
  let currentStep = pass;
  let offset = direction;
  while (nextStepLookangle) {
    if (
      currentStep.alt > crossingPoint &&
      nextStepLookangle.alt < crossingPoint
    )
      return {
        t: currentStep.t,
        x: currentStep.x,
        y: currentStep.y,
        z: currentStep.z,
      };

    offset += direction;
    currentStep = nextStepLookangle;
    nextStep = getStepFromOffset(db, pass.t, offset);
    nextStepLookangle =
      nextStep && eciToLookAgnle({ eciArray: nextStep, lat, lng });
  }
  return undefined;
};
