import { useMemo, useState } from "react";

import { useAtom } from "jotai";
import type { LatLngExpression } from "leaflet";
import { Polygon, useMapEvents } from "react-leaflet";
import { targetLocationState } from "../../recoilState";
import { rectBounds } from "./mapUtils";

const FOV_WIDTH = 62208;
const FOV_HEIGHT = 46656;

const THE_WORLD: LatLngExpression[] = [
  [90, -180],
  [90, 180],
  [-90, 180],
  [-90, -180],
];

const FOVPolygon = () => {
  const [FOVMapFOV, setFOVMapFOV] = useState<[number, number]>([
    49.158359, -122.928838,
  ]);
  const setTargetLocation = useAtom(targetLocationState)[1];

  const map = useMapEvents({
    move: () => {
      const center = map.getCenter();
      const precision = 10 ** 6;
      const lat = Math.round(center.lat * precision) / precision;
      const lng = Math.round(center.lng * precision) / precision;
      setFOVMapFOV([lat, lng]);
    },
    dragend: () => {
      setTargetLocation({ lat: FOVMapFOV[0], lng: FOVMapFOV[1] });
    },
  });

  const fovPositions = useMemo(() => {
    return rectBounds(FOVMapFOV, FOV_WIDTH, FOV_HEIGHT, 0);
  }, [FOVMapFOV]);

  return (
    <>
      <Polygon
        positions={[THE_WORLD, fovPositions]}
        color="black"
        opacity={0.6}
        fillColor="black"
        fillOpacity={0.6}
        interactive={false}
      />

      <Polygon
        positions={fovPositions}
        color="#ff0000"
        fillOpacity={0}
        interactive={false}
      />
    </>
  );
};

export default FOVPolygon;
