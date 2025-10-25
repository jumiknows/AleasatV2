import type { Metadata } from "next";
import InnovationSection from "@/components/about/InnovationSection";
import SatelliteImagerySection from "@/components/about/SatelliteImagerySection";
import SavingLivesSection from "@/components/about/SavingLivesSection";

export const metadata: Metadata = {
  title: "About Us - ALEASAT",
  description: "Learn about ALEASAT's mission to save lives and shape a safer world through satellite imagery and disaster response technology.",
  openGraph: {
    title: "About Us - ALEASAT",
    description: "Learn about ALEASAT's mission to save lives and shape a safer world through satellite imagery and disaster response technology.",
    images: "https://artifact.aleasat.space/logos/aleasat-render-over-earth-96x96.png",
  },
};

export default function AboutPage() {
  return (
    <>
      <InnovationSection />
      <SatelliteImagerySection />
      <SavingLivesSection />
    </>
  );
}