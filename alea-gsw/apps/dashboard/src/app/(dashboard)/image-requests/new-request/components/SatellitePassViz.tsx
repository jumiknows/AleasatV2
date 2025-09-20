"use client";
import {
  Configuration,
  type GetGroundTrackRequest,
  OrbitPropagationApi,
} from "@aleasat/orbit-propagation-api";
import * as astrokit from "astrokit";
import { useAtomValue } from "jotai";
import { useSession } from "next-auth/react";
import { env } from "next-runtime-env";
import { useEffect, useRef, useState } from "react";
// React
import Globe, { type GlobeMethods } from "react-globe.gl";
import useSWR from "swr";
import * as three from "three";
import { DRACOLoader, GLTFLoader } from "three/examples/jsm/Addons.js";
import useResizeObserver from "use-resize-observer";
import { targetLocationState } from "../recoilState";
import type { Pass } from "./SatellitePassList";

const passInfoFetcher = async (
  params: GetGroundTrackRequest,
  accessToken: string,
) =>
  await new OrbitPropagationApi(
    new Configuration({
      accessToken,
    }),
  ).getGroundTrack(params);

const SatellitePassViz = (props: {
  zoom: boolean;
  selectedPass: Pass;
  initialZoom?: boolean;
}) => {
  const { zoom, selectedPass } = props;
  const targetLocation = useAtomValue(targetLocationState);

  const { ref, width = 1, height = 1 } = useResizeObserver<HTMLDivElement>();
  const session = useSession();
  const globeRef = useRef<GlobeMethods | undefined>(undefined);
  const [globeMaterial] = useState(new three.MeshPhongMaterial());
  const [satModel, setSatModel] = useState<three.Object3D | undefined>(
    undefined,
  );
  const { data, isLoading, error } = useSWR(
    selectedPass.culmination.t && session.data?.accessToken
      ? {
          t: selectedPass.culmination.t,
        }
      : null,
    async (key) => await passInfoFetcher(key, session.data!.accessToken),
  );

  useEffect(() => {
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

    globeRef.current?.pointOfView({
      ...targetLocation,
      altitude: props.initialZoom ? 550 / 6371 : 2.5,
    });
  }, []);

  useEffect(() => {
    if (!globeRef.current) return;
    if (zoom)
      globeRef.current.pointOfView(
        {
          lat: selectedPass.culmination.lat,
          lng: selectedPass.culmination.lng,
          altitude: selectedPass.culmination.height / 6371,
        },
        1500,
      );
    else
      globeRef.current.pointOfView({ ...targetLocation, altitude: 2.5 }, 1000);
  }, [zoom]);

  useEffect(() => {
    if (!globeRef.current) return;
    globeRef.current.pointOfView(targetLocation, 0);
  }, [targetLocation]);

  if (globeRef.current) {
    const ambientLight = (
      globeRef.current.scene().children as three.AmbientLight[]
    ).find((object) => object.isAmbientLight);
    if (ambientLight) ambientLight.intensity = 0;
  }

  useEffect(() => {
    const loader = new GLTFLoader();
    const dracoLoader = new DRACOLoader();
    dracoLoader.setDecoderPath("/decoder/");
    loader.setDRACOLoader(dracoLoader);
    loader.load(
      "https://artifact.aleasat.space/model/satellite.glb",
      (gltf) => {
        if (!globeRef.current) return;
        // globeRef.current.scene().add(gltf.scene);
        setSatModel((old) => {
          if (globeRef.current && old) globeRef.current.scene().remove(old);
          return gltf.scene;
        });
      },
    );
  }, []);

  useEffect(() => {
    if (!globeRef.current || !satModel) return;
    globeRef.current.scene().add(satModel);
  }, [globeRef.current]);

  useEffect(() => {
    if (!globeRef.current) return;
    const directionalLight = (
      globeRef.current.scene().children as three.DirectionalLight[]
    ).find((object) => object.isDirectionalLight);
    const coords = new astrokit.ITRFCoord(
      astrokit
        .qGCRS2ITRF(new Date(selectedPass.culmination.t))
        .rotate(astrokit.sun.posMOD(new Date(selectedPass.culmination.t))),
    );
    const sun = globeRef.current.getCoords(
      coords.latitude_deg(),
      coords.longitude_deg(),
      10000,
    );
    if (directionalLight) directionalLight.position.set(sun.x, sun.y, sun.z);

    if (satModel && selectedPass.id !== -1) {
      const satCords = globeRef.current.getCoords(
        selectedPass.culmination.lat,
        selectedPass.culmination.lng,
        selectedPass.culmination.height / 6371,
      );
      const ground = globeRef.current.getCoords(
        targetLocation.lat,
        targetLocation.lng,
        0,
      );
      satModel.position.set(satCords.x, satCords.y, satCords.z);
      satModel.lookAt(ground.x, ground.y, ground.z);
      satModel.scale.set(3, 3, 3);
    }
  }, [selectedPass.culmination.t, targetLocation, satModel]);

  console.log(
    isLoading || !data || error
      ? []
      : [
          data.groundTrack.map((value) => [...value, 500 / 6371]),
          [
            [
              selectedPass.culmination.lat,
              selectedPass.culmination.lng,
              selectedPass.culmination.height,
            ],
            [targetLocation.lat, targetLocation.lng, 0],
          ],
        ],
  );

  return (
    <div style={{ width: "100%", height: "100%" }} ref={ref}>
      <Globe
        ref={globeRef}
        animateIn={false}
        // globeImageUrl={"/earth-day.png"}
        globeMaterial={globeMaterial}
        backgroundColor="#000000"
        // backgroundColor="#202020"
        width={width}
        height={height}
        pathsData={
          isLoading || !data || error
            ? []
            : [
                data.groundTrack.map((value) => [...value, 500 / 6371]),
                [
                  [
                    selectedPass.culmination.lat,
                    selectedPass.culmination.lng,
                    selectedPass.culmination.height,
                  ],
                  [targetLocation.lat, targetLocation.lng, 0],
                ],
              ]
        }
        pathColor={() => ["rgba(255,255,255,0.6)", "rgba(255,255,255,0.6)"]}
        pathDashLength={0.01}
        pathDashGap={0.004}
        pathDashAnimateTime={1000}
        pathPointAlt={(pnt) => pnt[2] / 6371}
        pathTransitionDuration={0}
        pathResolution={1}
        labelsData={
          [
            // { lat: targetLocation.lat, lng: targetLocation.lng, text: "Target" },
          ]
        }
        labelColor={() => "rgb(255,255,255)"}
        labelSize={1.7}
        labelDotRadius={1}
        labelAltitude={0.001}
        labelsTransitionDuration={0}
        waitForGlobeReady={false}
      />
    </div>
  );
};

export default SatellitePassViz;
