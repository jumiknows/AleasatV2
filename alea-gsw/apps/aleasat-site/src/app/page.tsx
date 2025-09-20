import Hero from "@/components/Hero";
import About from "@/components/home/About";
import BecomeRadioOperator from "@/components/home/BecomeRadioOperator";
import FeaturedSponsorships from "@/components/home/FeaturedSponsorships";
import HomeHeroSection from "@/components/home/HomeHeroSection";
import Plan from "@/components/home/Plan";
import Quote from "@/components/home/Quote";
import Sponsors from "@/components/home/sponsors/Sponsors";
import TheTeam from "@/components/home/TheTeam";
import Box from "@mui/material/Box";

export default function Home() {
  return (
    <Box>
      <Box position="relative">
        {/* <HomeHeroSection /> */}
        <Hero
          type="video"
          src="https://artifact.aleasat.space/heroSection.mp4"
          title="Explore ALEASAT"
        />
      </Box>
      <About />
      <Plan />
      <TheTeam />
      <BecomeRadioOperator />
      <FeaturedSponsorships />
      {/* <Quote /> */}
    </Box>
  );
}
