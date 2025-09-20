"use client";

import {
  ArcGisBaseMapType,
  ArcGisMapServerImageryProvider,
  Cartesian3,
  // Ion,
  CesiumTerrainProvider,
  IonResource,
  JulianDate,
  PathGraphics,
  ReferenceFrame,
  SampledPositionProperty,
  TimeInterval,
  TimeIntervalCollection,
} from "cesium";
import {
  CameraFlyHome,
  Clock,
  Entity,
  Globe,
  ImageryLayer,
  ModelGraphics,
  Viewer,
} from "resium";
import "../widgets.css";

import { MenuItem, Select, TextField } from "@mui/material";
import { useMemo, useState } from "react";
import CameraInICRF from "./CameraInICRF";

export default function CesiumGlobeGmat({
  result,
}: {
  result: {
    utcGregorian: number;
    x: number;
    y: number;
    z: number;
  }[];
}) {
  const [track, setTrack] = useState("satellite");
  const [stepSize, setStepSize] = useState(1000);
  const [showPath, setShowPath] = useState(false);

  const { positionProperty } = useMemo(() => {
    if (!result) return { positionProperty: undefined };

    const positionProperty = new SampledPositionProperty(
      ReferenceFrame.INERTIAL,
    );

    const step = Math.floor(result.length / stepSize);

    result.forEach((pnt, index) => {
      if (index % step !== 0) return;
      const julianDate = JulianDate.fromDate(new Date(pnt.utcGregorian));

      positionProperty.addSample(
        julianDate,
        new Cartesian3(pnt.x * 1000, pnt.y * 1000, pnt.z * 1000),
      );
    });

    return { positionProperty };
  }, [result, stepSize]);

  if (!result) return <></>;

  // Ion.defaultAccessToken = process.env.NEXT_PUBLIC_CESIUM_ACCESS_TOKEN!;

  return (
    <>
      <Select
        label="Track"
        variant="outlined"
        defaultValue={"satellite"}
        onChange={(e) => setTrack(e.target.value)}
      >
        <MenuItem value={"satellite"}>Track Satellite</MenuItem>
        <MenuItem value={"earth"}>Track Earth</MenuItem>
        <MenuItem value={"icrf"}>ICRF</MenuItem>
      </Select>
      <Select
        label="Path Setting"
        variant="outlined"
        defaultValue={"false"}
        onChange={(e) => setShowPath(e.target.value === "true")}
      >
        <MenuItem value={"true"}>Show Path</MenuItem>
        <MenuItem value={"false"}>Don't Show Path</MenuItem>
      </Select>
      <TextField
        label="Step Size"
        variant="outlined"
        defaultValue={"1000"}
        onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
          const value = parseInt(e.target.value);
          if (isNaN(value) || value === 0) return;
          setStepSize(value);
        }}
      />
      <Viewer
        style={{ width: "100%", height: "100%" }}
        requestRenderMode
        maximumRenderTimeChange={0.1}
      >
        <ImageryLayer
          imageryProvider={ArcGisMapServerImageryProvider.fromBasemapType(
            ArcGisBaseMapType.SATELLITE,
          )}
        />
        <Globe
          enableLighting
          terrainProvider={CesiumTerrainProvider.fromUrl(
            IonResource.fromAssetId(1),
            {
              requestVertexNormals: true,
              requestWaterMask: false,
            },
          )}
        />
        <Clock
          startTime={JulianDate.fromDate(new Date(result[0].utcGregorian))}
          currentTime={JulianDate.fromDate(new Date(result[0].utcGregorian))}
          shouldAnimate
        />
        <Entity
          position={positionProperty}
          availability={
            new TimeIntervalCollection([
              new TimeInterval({
                start: JulianDate.fromDate(new Date(result[0].utcGregorian)),
                stop: JulianDate.fromDate(
                  new Date(result.at(-1)!.utcGregorian),
                ),
              }),
            ])
          }
          path={(showPath && new PathGraphics({ width: 1 })) || undefined}
          tracked={track === "satellite"}
        >
          <ModelGraphics
            uri="https://artifact.aleasat.space/model/satellite.glb"
            runAnimations={false}
            minimumPixelSize={256}
            maximumScale={100000}
          />
        </Entity>
        {track === "earth" && <CameraFlyHome duration={0} />}
        {track === "icrf" && <CameraInICRF />}
      </Viewer>
    </>
  );
}
