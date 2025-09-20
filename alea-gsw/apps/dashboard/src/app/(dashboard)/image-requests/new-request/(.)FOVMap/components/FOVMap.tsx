"use client";

import {
  LayersControl,
  MapContainer,
  ScaleControl,
  TileLayer,
} from "react-leaflet";
import "leaflet/dist/leaflet.css";
import { useAtom } from "jotai";
import { env } from "next-runtime-env";
import { type ElementRef, useEffect, useRef } from "react";
import { targetLocationState } from "../../recoilState";

const Tile = ({
  id,
  maxZoom,
}: {
  id: string;
  minZoom: number;
  maxZoom: number;
}) => (
  <TileLayer
    url="https://api.mapbox.com/styles/v1/{id}/tiles/{z}/{x}/{y}?access_token={accessToken}"
    attribution='Map data &copy; <a href="https://www.openstreetmap.org/">OpenStreetMap</a> contributors, <a href="https://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>, Imagery © <a href="https://www.mapbox.com/">Mapbox</a>'
    minZoom={maxZoom}
    maxZoom={maxZoom}
    id={id}
    tileSize={512}
    zoomOffset={-1}
    accessToken={env("NEXT_PUBLIC_MAPBOX_ACCESS_TOKEN")}
  />
);

export default function FOVMap({
  initialZoom,
  minZoom,
  maxZoom,
  children,
}: {
  initialZoom: number;
  minZoom: number;
  maxZoom: number;
  children: React.ReactNode;
}) {
  const [targetLocation] = useAtom(targetLocationState);
  const mapRef = useRef<ElementRef<typeof MapContainer>>(null);

  useEffect(() => {
    if (mapRef.current) {
      mapRef.current.setView(targetLocation);
    }
  }, [targetLocation, targetLocation.lat, targetLocation.lng]);

  return (
    <MapContainer
      ref={mapRef}
      center={targetLocation}
      zoom={initialZoom}
      scrollWheelZoom={"center"}
      zoomControl={true}
      worldCopyJump={true}
      style={{ height: "100%", aspectRatio: "34/24" }}
    >
      <LayersControl position="topright">
        <LayersControl.BaseLayer name="Streets">
          <Tile id="mapbox/streets-v11" maxZoom={maxZoom} minZoom={minZoom} />
        </LayersControl.BaseLayer>
        <LayersControl.BaseLayer name="Satellite" checked>
          <Tile
            id="mapbox/satellite-streets-v11"
            maxZoom={maxZoom}
            minZoom={minZoom}
          />
        </LayersControl.BaseLayer>
      </LayersControl>

      <ScaleControl position="bottomleft" />

      {children}
    </MapContainer>
  );
}
