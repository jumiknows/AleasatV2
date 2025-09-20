const MERIDIAN_LENGTH = 20003930; // metres
const EQUATOR_PARALLEL_LENGTH = 40075000; // metres

function rotatePoint(
  point: [number, number],
  thetaDeg: number,
): [number, number] {
  const thetaRad = (thetaDeg * Math.PI) / 180;
  const xRot = Math.cos(thetaRad) * point[0] - Math.sin(thetaRad) * point[1];
  const yRot = Math.sin(thetaRad) * point[0] + Math.cos(thetaRad) * point[1];

  return [xRot, yRot];
}

function xyToLatlng(
  originLatlng: [number, number],
  deltaXy: [number, number],
): [number, number] {
  const dx = deltaXy[0];
  const dy = deltaXy[1];

  const lat1 = originLatlng[0];
  const lng1 = originLatlng[1];

  const latScaler = 180 / MERIDIAN_LENGTH;
  const lngScaler =
    360 / (EQUATOR_PARALLEL_LENGTH * Math.cos((lat1 * Math.PI) / 180));

  const lat2 = dy * latScaler + lat1;
  const lng2 = dx * lngScaler + lng1;

  return [lat2, lng2];
}

function rectBounds(
  center: [number, number],
  width: number,
  height: number,
  rotation: number,
) {
  const x = width / 2;
  const y = height / 2;

  const AXy: [number, number] = [-x, y];
  const BXy: [number, number] = [x, y];
  const CXy: [number, number] = [x, -y];
  const DXy: [number, number] = [-x, -y];

  const AXyRot = rotatePoint(AXy, -rotation);
  const BXyRot = rotatePoint(BXy, -rotation);
  const CXyRot = rotatePoint(CXy, -rotation);
  const DXyRot = rotatePoint(DXy, -rotation);

  const A = xyToLatlng(center, AXyRot);
  const B = xyToLatlng(center, BXyRot);
  const C = xyToLatlng(center, CXyRot);
  const D = xyToLatlng(center, DXyRot);

  const bounds = [A, B, C, D];

  return bounds;
}

function destination(
  start: [number, number],
  distance: number,
  heading: number,
) {
  const endXy = rotatePoint([0, distance], -heading);
  const end = xyToLatlng(start, endXy);

  return end;
}

export { rectBounds, destination };
