"use client";

import {
  Cartesian3,
  type JulianDate,
  Matrix4,
  type Scene,
  Transforms,
} from "cesium";
import { useEffect } from "react";
import { useCesium } from "resium";

export default function CameraInICRF() {
  const { scene, viewer } = useCesium();

  function icrf(_scene: Scene, time: JulianDate) {
    const icrfToFixed = Transforms.computeIcrfToFixedMatrix(time)!;
    const camera = viewer!.camera;
    const offset = Cartesian3.clone(camera.position);
    const transform = Matrix4.fromRotationTranslation(icrfToFixed);
    camera.lookAtTransform(transform, offset);
  }

  useEffect(() => {
    scene?.postUpdate.addEventListener(icrf);
    return () => {
      scene?.postUpdate.removeEventListener(icrf);
    };
  }, [scene]);

  return null;
}
