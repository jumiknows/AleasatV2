import { atom } from "jotai";

export const newImageRequestActiveStepState = atom({
  activeStep: 0,
  transitionTime: 0,
  stepGoingforward: true,
});

export const newImageRequestStatePopulatedState = atom(false);

export const targetLocationState = atom({ lat: 49.158359, lng: -122.928838 });

export const selectedPassState = atom({
  id: -1,
  rise: { t: new Date() },
  culmination: {
    t: new Date(),
    alt: 0,
    az: 0,
    lat: 0,
    lng: 0,
    height: 0,
  },
  set: { t: new Date() },
});

export const SatVizRotationState = atom({ x: 0, y: 0, z: 0 });

export const AROGroundStationSelectedState = atom({
  id: -1,
  name: "",
  lat: 0,
  lng: 0,
});

export const AROGroundStationPassState = atom({
  id: -1,
  rise: {
    t: new Date(),
  },
  culmination: {
    t: new Date(),
    alt: 0,
    az: 0,
    lat: 0,
    lng: 0,
    height: 0,
  },
  set: {
    t: new Date(),
  },
});

export const ImageRequestNameAndDescriptionState = atom({
  name: "",
  description: "",
});
