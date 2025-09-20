import Hero from "@/components/Hero";
import JoinUsDescription from "@/components/join-us/JoinUsDescription";
import JoinUsSectionGrid from "@/components/join-us/JoinUsGridSection";
import Box from "@mui/material/Box";
import type { Metadata } from "next";

export const metadata: Metadata = {
  title: "Join Us",
  description:
    "Become part of our mission! Discover exciting opportunities to collaborate, support, or join ALEASAT in advancing space technology and innovation.",
  openGraph: {
    title: "Join Us",
    description:
      "Become part of our mission! Discover exciting opportunities to collaborate, support, or join ALEASAT in advancing space technology and innovation.",
    images: "https://artifact.aleasat.space/icon.png",
  },
};

export default function JoinUs() {
  return (
    <Box>
      <Box position="relative">
        <Hero
          type="image"
          src="https://artifact.aleasat.space/sponsor-background.jpg"
          title="Join Us"
        />
      </Box>
      <JoinUsDescription />
      <JoinUsSectionGrid />
    </Box>
  );
}
