"use client";

import SpacecraftComponent from "@/components/space-components/SpacecraftComponent";
import { Container } from "@mui/system";

const components: {
  name: string;
  description: string;
  image: string;
  specifications?: Record<string, string>;
}[] = [
  {
    name: "Antenna Deployer Module",
    description:
      "Mechanism responsible for deploying the communication antenna once the spacecraft reaches orbit. Ensures reliable data transmission by extending the antenna into its operational position.",
    image:
      "https://artifact.aleasat.space/space-components/new-same-position/ADM.avif",
    specifications: {
      "Operating Frequency": "UHF (437 MHz)",
      Bandwidth: "20 kHz",
      "Antenna Type": "V-Dipole",
      "Deployment Mechanism": "Spring-loaded",
      "GNSS Antenna": "SANAV AM-9",
    },
  },
  {
    name: "Communication Board",
    description:
      "Handles the transmission and reception of data between the spacecraft and ground stations. Includes RF circuits and interfaces with antennas.",
    image:
      "https://artifact.aleasat.space/space-components/new-same-position/CB.avif",
    specifications: {
      "RF transciver": "Texas Instruments CC1110",
      "Power Amplifier": "Guerrilla GRF5504",
      TCXO: "Fox Electronics FT5HN",
      "GNSS Receiver": "Orion B16",
    },
  },
  {
    name: "Camera and Lens",
    description:
      "Captures visual data of Earth or space for imaging missions. Mounted with a lens and connected to the onboard computer for data processing.",
    image:
      "https://artifact.aleasat.space/space-components/new-same-position/len.avif",
    specifications: {
      Camera: "ArduCAM OV5642",
      "Array Size": "2592 x 1944 pixels",
      "Pixel Size": "1.4 μm",
      Lens: "Edmund optics",
      "Focal Length": "35 mm",
      "Field of View": "6° x 4.5°",
      Aperture: "f/2.8",
      Filter: "Edmund Optics UV/IR-Cut Filter",
      "Filter Passband": "400-700 nm",
    },
  },
  {
    name: "Solar Panel",
    description:
      "Provides electrical power to the spacecraft by converting solar energy into electricity. Essential for sustaining long-term operations in orbit.",
    image:
      "https://artifact.aleasat.space/space-components/new-same-position/solar-panel.avif",
    specifications: {
      Manufacturer: "EnduroSat",
      "Solar Cell Type": "Triple-junction InGaP/GaAs/Ge",
      Sensors: "Temperature sensor, Sun Sensor, Magnetorquer, and Gyroscope",
    },
  },
  {
    name: "Structure",
    description:
      "The physical framework of the spacecraft that houses and supports all subsystems and payloads. Designed for mechanical stability and efficient thermal management.",
    image:
      "https://artifact.aleasat.space/space-components/new-same-position/structure.avif",
    specifications: {
      Material: "Aluminum 6061",
    },
  },
  {
    name: "Kill Switch and RBF Pin",
    description:
      "A safety mechanism used during ground operations to prevent accidental activation of spacecraft systems. The Remove Before Flight (RBF) pin must be pulled prior to launch.",
    image:
      "https://artifact.aleasat.space/space-components/new-same-position/kill-switch.avif",
  },
  {
    name: "On-Board Computer",
    description:
      "The central processing unit of the spacecraft. Manages control, communication, payload operations, and interfaces with all other subsystems.",
    image:
      "https://artifact.aleasat.space/space-components/new-same-position/OBC.avif",
    specifications: {
      Microcontroller: "Texas Instruments TMS570LS0714",
      CPU: "ARM Cortex-R4F 32-Bit RISC CPU",
      Architecture: "Dual-core lockstep",
      "RAM Size": "128 KB",
      "Flash Size": "256 MB",
      "MRAM Size": "256 KB",
      IMU: "Bosch BMX160",
    },
  },
  {
    name: "Electrical Power System",
    description:
      "Manages power generation, storage, and distribution onboard. Regulates voltage and supplies power to all systems using solar panels and batteries.",
    image:
      "https://artifact.aleasat.space/space-components/new-same-position/EPS.avif",
    specifications: {
      Manufacturer: "ISISPACE",
      EPS: "Compact Electrical Power System 2",
      Capacity: "40 Wh",
      Voltage: "5V, 3.3V",
    },
  },
];

export default function AllSpacecraftComponents() {
  return (
    <Container maxWidth="xl">
      {components.map((component, index) => (
        <SpacecraftComponent
          key={index}
          name={component.name}
          description={component.description}
          image={component.image}
          specifications={component.specifications}
        />
      ))}
    </Container>
  );
}
