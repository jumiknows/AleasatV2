import HomeHeroSection from "@/components/home/HomeHeroSection";
import Quote from "@/components/home/Quote";
import Sponsors from "@/components/home/sponsors/Sponsors";
import Box from "@mui/material/Box";

export default function Home() {
  return (
    <Box>
      <Box position="relative">
        <HomeHeroSection />
      </Box>
      <Sponsors />
      <Quote />
    </Box>
  );
}
