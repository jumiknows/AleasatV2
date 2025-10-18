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
      {/* Hero Section with Team Photo */}
      <Box
        sx={{
          position: "relative",
          height: "60vh",
          minHeight: "400px",
          overflow: "hidden",
          display: "flex",
          alignItems: "center",
          justifyContent: "center",
        }}
      >
        <Box
          sx={{
            position: "absolute",
            top: 0,
            left: 0,
            width: "100%",
            height: "100%",
            backgroundImage: "url('https://artifact.aleasat.space/team-photo/team-photo.avif')",
            backgroundSize: "cover",
            backgroundPosition: "center",
            "&::after": {
              content: '""',
              position: "absolute",
              top: 0,
              left: 0,
              width: "100%",
              height: "100%",
              background: "rgba(0, 0, 0, 0.4)",
            },
          }}
        />
        <Container maxWidth="lg" sx={{ position: "relative", zIndex: 1 }}>
          <Typography
            variant="h1"
            sx={{
              textAlign: "center",
              color: "white",
              fontWeight: "bold",
              fontSize: { xs: "3rem", sm: "4rem", md: "5rem" },
              textShadow: "0 4px 20px rgba(0,0,0,0.7)",
              mb: 2,
            }}
          >
            Our Team
          </Typography>
          <Typography
            variant="h5"
            sx={{
              textAlign: "center",
              color: "rgba(255, 255, 255, 0.9)",
              maxWidth: "600px",
              mx: "auto",
              textShadow: "0 2px 10px rgba(0,0,0,0.7)",
            }}
          >
            100+ passionate students united by space exploration and innovation
          </Typography>
        </Container>
      </Box>

      {/* Main Content */}
      <Box sx={{ backgroundColor: "#000" }}>
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
