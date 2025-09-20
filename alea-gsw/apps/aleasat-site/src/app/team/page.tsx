import Hero from "@/components/Hero";
import AboutUsSection from "@/components/team/AboutUsSection";
import ManagementSection from "@/components/team/ManagementSection";
import OurMembersSection from "@/components/team/OurMembersSection";
import SubsystemListSection from "@/components/team/SubsystemListSection";
import TeamStatsSection from "@/components/team/TeamStatsSection";
import { Box, Container, Typography } from "@mui/material";
import type { Metadata } from "next";

export const metadata: Metadata = {
  title: "Team",
  description: "Meet the passionate and skilled team behind ALEASAT.",
  openGraph: {
    title: "Team",
    description: "Meet the passionate and skilled team behind ALEASAT.",
    images: "https://artifact.aleasat.space/icon.png",
  },
};

export default function AboutUsPage() {
  return (
    <>
      <Hero
        type="image"
        src="https://artifact.aleasat.space/team-photo/team-photo.avif"
        title="Our Team"
      />
      <Box
        sx={{
          position: "relative",
          backgroundColor: "#000",
        }}
      >
        <Container maxWidth="xl">
          <AboutUsSection />
          <ManagementSection />
          <TeamStatsSection />
          <SubsystemListSection />
          <OurMembersSection />
        </Container>
      </Box>
    </>
  );
}
