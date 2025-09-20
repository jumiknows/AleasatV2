"use client";

import MyLocationIcon from "@mui/icons-material/MyLocation";
import Box from "@mui/material/Box";
import IconButton from "@mui/material/IconButton";
import * as astrokit from "astrokit";
import { useAtom, useAtomValue } from "jotai";
import React, { useEffect, useRef, useState } from "react";
import Globe, { type GlobeMethods } from "react-globe.gl";
import * as three from "three";
import useResizeObserver from "use-resize-observer";
import {
  SatVizRotationState,
  selectedPassState,
  targetLocationState,
} from "../../recoilState";
import { AleaFlyControls } from "./AleaFlyControls";
import HelpButton from "./HelpButton";

function lon2tile(lon: number, zoom: number) {
  return Math.floor(((lon + 180) / 360) * Math.pow(2, zoom));
}
function lat2tile(lat: number, zoom: number) {
  return Math.floor(
    ((1 -
      Math.log(
        Math.tan((lat * Math.PI) / 180) + 1 / Math.cos((lat * Math.PI) / 180),
      ) /
        Math.PI) /
      2) *
      Math.pow(2, zoom),
  );
}
function tile2long(x: number, z: number) {
  return (x / Math.pow(2, z)) * 360 - 180;
}
function tile2lat(y: number, z: number) {
  const n = Math.PI - (2 * Math.PI * y) / Math.pow(2, z);
  return (180 / Math.PI) * Math.atan(0.5 * (Math.exp(n) - Math.exp(-n)));
}

export default function SatViz() {
  const { ref, width = 1, height = 1 } = useResizeObserver<HTMLDivElement>();
  const globeEl = useRef<GlobeMethods | undefined>(undefined);
  const [tilesData, setTilesData] = useState<object[]>([]);
  const [tileHeight, setTileHeight] = useState(2);
  const [satVizRotation, setSatVizRotation] = useAtom(SatVizRotationState);
  const targetLocation = useAtomValue(targetLocationState);
  const [selectedPass] = useAtom(selectedPassState);
  const [globeMaterial] = useState(new three.MeshPhongMaterial());

  const [showTips, setShowTips] = useState(true);
  useEffect(() => {
    if (!showTips) return;
    const timeout = setTimeout(() => setShowTips(false), 1000);
    return () => clearTimeout(timeout);
  }, [showTips]);

  const zoomLevel = 8; // degrees
  const lookAtTarget = () => {
    if (!globeEl.current) return;
    const sat = globeEl.current.getCoords(
      selectedPass.culmination.lat,
      selectedPass.culmination.lng,
      selectedPass.culmination.height / 6371,
    );
    globeEl.current.camera().position.set(sat.x, sat.y, sat.z);
    const ground = globeEl.current.getCoords(
      targetLocation.lat,
      targetLocation.lng,
      0,
    );
    globeEl.current.camera().lookAt(ground.x, ground.y, ground.z);
    console.log(targetLocation.lat, targetLocation.lng);
    handleCameraRotations();
  };

  useEffect(() => {
    lookAtTarget();
    if (!globeEl.current) return;
    const ambientLight = (
      globeEl.current.scene().children as three.AmbientLight[]
    ).find((object) => object.isAmbientLight);
    if (ambientLight) ambientLight.intensity = 0;
    const directionalLight = (
      globeEl.current.scene().children as three.DirectionalLight[]
    ).find((object) => object.isDirectionalLight);
    const coords = new astrokit.ITRFCoord(
      astrokit
        .qGCRS2ITRF(new Date(selectedPass.culmination.t))
        .rotate(astrokit.sun.posMOD(new Date(selectedPass.culmination.t))),
    );
    const sun = globeEl.current.getCoords(
      coords.latitude_deg(),
      coords.longitude_deg(),
      10000,
    );
    if (directionalLight) directionalLight.position.set(sun.x, sun.y, sun.z);
  }, [selectedPass]);

  const handleCameraRotations = async () => {
    if (!globeEl.current) return;
    await new Promise((resolve) => setTimeout(resolve, 0));
    const r = globeEl.current.camera().rotation;
    setSatVizRotation({ x: r.x, y: r.y, z: r.z });
    const raycaster = new three.Raycaster();
    raycaster.setFromCamera(new three.Vector2(0, 0), globeEl.current.camera());
    const intersects = raycaster.intersectObjects(
      globeEl.current.scene().children,
    );
    const currentLookingLocation = intersects.map((i) =>
      globeEl.current?.toGeoCoords(i.point),
    )[0];

    if (!currentLookingLocation) return;
    const currentTileHeight =
      (360 *
        Math.cos(
          (tile2lat(
            lat2tile(currentLookingLocation.lat, zoomLevel) + 0.5,
            zoomLevel,
          ) *
            Math.PI) /
            180,
        )) /
      2 ** zoomLevel;
    setTileHeight(currentTileHeight);
    const tempTilesDate = [];
    const x = lon2tile(currentLookingLocation.lng, zoomLevel);
    const y = lat2tile(currentLookingLocation.lat, zoomLevel);
    for (let i = -2; i < 3; i++) {
      for (let j = -2; j < 3; j++) {
        const material = new three.MeshPhongMaterial();
        const texture = new three.TextureLoader().load(
          `https://api.mapbox.com/styles/v1/mapbox/satellite-v9/tiles/${zoomLevel}/${
            x + j
          }/${
            y - i
          }?access_token=pk.eyJ1IjoiMDEwMDEwMDAiLCJhIjoiY2xwMHh0dnd6MGN4djJqdDIwNXAzdm00YyJ9.rm-SbH66VXhfQSjEZDZn4g`,
        );
        const darkTexture = new three.TextureLoader().load(
          `https://api.mapbox.com/styles/v1/mapbox/dark-v11/tiles/${zoomLevel}/${
            x + j
          }/${
            y - i
          }?access_token=pk.eyJ1IjoiMDEwMDEwMDAiLCJhIjoiY2xwMHh0dnd6MGN4djJqdDIwNXAzdm00YyJ9.rm-SbH66VXhfQSjEZDZn4g`,
        );
        texture.colorSpace = three.SRGBColorSpace;
        darkTexture.colorSpace = three.SRGBColorSpace;
        material.map = texture;
        material.emissive = new three.Color(255, 255, 255);
        material.emissiveMap = darkTexture;
        material.emissiveIntensity = 0.001;
        tempTilesDate.push({
          lng: tile2long(x + j + 0.5, zoomLevel),
          lat: tile2lat(y + 0.5, zoomLevel) + i * currentTileHeight,
          material,
        });
      }
    }
    setTilesData(tempTilesDate);
  };

  useEffect(() => {
    if (!globeEl.current) return;
    globeEl.current.controls().object = new three.Camera();
    (globeEl.current.camera() as three.PerspectiveCamera).fov = 6;

    // eslint-disable-next-line no-new
    new AleaFlyControls(
      globeEl.current.camera() as three.PerspectiveCamera,
      globeEl.current.renderer().domElement,
      selectedPass.culmination.height,
      handleCameraRotations,
    );

    lookAtTarget();
    if (satVizRotation.x || satVizRotation.y || satVizRotation.z) {
      globeEl.current
        .camera()
        .rotation.set(satVizRotation.x, satVizRotation.y, satVizRotation.z);
    }
    handleCameraRotations();

    const texture = new three.TextureLoader().load(
      "https://artifact.aleasat.space/earth-day.jpg",
    );
    const night = new three.TextureLoader().load(
      "https://artifact.aleasat.space/earth-night.jpg",
    );
    texture.colorSpace = three.SRGBColorSpace;
    night.colorSpace = three.SRGBColorSpace;
    globeMaterial.map = texture;
    globeMaterial.emissive = new three.Color(255, 255, 255);
    globeMaterial.emissiveMap = night;
    globeMaterial.emissiveIntensity = 0.001;
  }, []);

  return (
    <div
      style={{ height: "100%", aspectRatio: "4/3", position: "relative" }}
      ref={ref}
    >
      <Globe
        ref={globeEl}
        animateIn={false}
        // globeImageUrl={"https://artifact.aleasat.space/earth-day.jpg"}
        globeMaterial={globeMaterial}
        width={width}
        height={height}
        tilesData={tilesData}
        tileHeight={tileHeight}
        tileWidth={360 / 2 ** zoomLevel}
        tileMaterial="material"
        tileAltitude={0.000001}
        // tileCurvatureResolution={10}
        tilesTransitionDuration={0}
      />
      <Box
        sx={{
          position: "absolute",
          bottom: "0px",
          right: "0px",
        }}
      >
        <IconButton aria-label="Help" onClick={() => lookAtTarget()}>
          <MyLocationIcon fontSize="large" />
        </IconButton>
      </Box>
      <HelpButton />
    </div>
  );
}
