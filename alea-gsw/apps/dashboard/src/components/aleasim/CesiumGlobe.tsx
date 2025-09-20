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
  Quaternion,
  ReferenceFrame,
  SampledPositionProperty,
  SampledProperty,
  TimeInterval,
  TimeIntervalCollection,
} from "cesium";
import {
  Clock,
  Entity,
  Globe,
  ImageryLayer,
  ModelGraphics,
  Viewer,
} from "resium";
import "./widgets.css";

import { useAtomValue } from "jotai";
import { useMemo } from "react";
import DateShower from "./DateShower";
import { simDataState } from "./recoilState";

export default function CesiumGlobe() {
  const simData = useAtomValue(simDataState);

  const { orientationProperty, positionProperty } = useMemo(() => {
    if (!simData)
      return { orientationProperty: undefined, positionProperty: undefined };
    const currentTime = new Date(simData.start_time).getTime();
    const pntInSec = 1 / simData.sim_dt;

    const orientationProperty = new SampledProperty(Quaternion);
    const positionProperty = new SampledPositionProperty(
      ReferenceFrame.INERTIAL,
    );

    simData.odyn_state.slice(1).forEach((pnt, index) => {
      if (index % pntInSec !== 0) return;

      const julianDate = JulianDate.fromDate(
        new Date(currentTime + index * simData.sim_dt * 1000),
      );

      positionProperty.addSample(
        julianDate,
        new Cartesian3(pnt[0], pnt[1], pnt[2]),
      );

      orientationProperty.addSample(
        julianDate,
        new Quaternion(
          simData.adyn_state[index][2],
          simData.adyn_state[index][1],
          simData.adyn_state[index][0],
          simData.adyn_state[index][3],
        ),
      );
    });

    return { orientationProperty, positionProperty };
  }, [simData]);

  if (!simData) return <></>;

  // Ion.defaultAccessToken = process.env.NEXT_PUBLIC_CESIUM_ACCESS_TOKEN!;
  const currentTime = new Date(simData.start_time).getTime();

  return (
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
        startTime={JulianDate.fromIso8601(simData.start_time)}
        currentTime={JulianDate.fromIso8601(simData.start_time)}
        shouldAnimate
      />
      <DateShower />
      <Entity
        position={positionProperty}
        availability={
          new TimeIntervalCollection([
            new TimeInterval({
              start: JulianDate.fromIso8601(simData.start_time),
              stop: JulianDate.fromDate(
                new Date(
                  currentTime +
                    simData.adyn_state.length * simData.sim_dt * 1000,
                ),
              ),
            }),
          ])
        }
        orientation={orientationProperty}
        path={new PathGraphics({ width: 3 })}
        tracked
      >
        <ModelGraphics
          uri="https://artifact.aleasat.space/model/satellite.glb"
          runAnimations={false}
          minimumPixelSize={256}
          maximumScale={100000}
        />
      </Entity>
    </Viewer>
  );
}
