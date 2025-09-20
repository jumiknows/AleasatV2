import AllSpacecraftComponents from "@/components/space-components/AllSpacecraftComponents";
import SpacecraftHeroSection from "@/components/space-components/SpacecraftHeroSection";
import SpacecraftKeySpec from "@/components/space-components/SpacecraftKeySpec";
import type { Metadata } from "next";

export const metadata: Metadata = {
  title: "Spacecraft | ALEASAT",
  description:
    "Explore the cutting-edge technology behind our spacecraft, from solar panels to hardware components, and learn how each element contributes to space exploration.",
  openGraph: {
    title: "Spacecraft",
    description:
      "Explore the cutting-edge technology behind our spacecraft, from solar panels to hardware components, and learn how each element contributes to space exploration.",
    images: "https://artifact.aleasat.space/icon.png",
  },
};

export default function Spaceship() {
  return (
    <>
      <SpacecraftHeroSection />
      <AllSpacecraftComponents />
      <SpacecraftKeySpec />
    </>
  );
}
