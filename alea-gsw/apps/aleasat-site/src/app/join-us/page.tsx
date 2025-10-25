import Hero from "@/components/Hero";
import JoinUsDescription from "@/components/join-us/JoinUsDescription";
import JoinUsSectionGrid from "@/components/join-us/JoinUsGridSection";
import Box from "@mui/material/Box";
import { Container } from "@mui/material";
import type { Metadata } from "next";
import Typography from "@mui/material/Typography";
import Grid from "@mui/material/Grid2";
import Link from "@mui/material/Link";
import Image from "@aleasat/ui/components/Image";

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
       <Container
        maxWidth={"lg"}
        sx={{
          display: "flex",
          flexDirection: { xs: "column-reverse", md: "row" },
          maxWidth: "1000px",
          margin: "auto",
          p: 3,
        }}
      >
        <Box sx={{ flex: 1, pr: { md: 3 }, mt: { xs: 3, md: 0 } }}>
          <Typography
            variant="h4"
            gutterBottom
            sx={{ mb: 5, fontWeight: "bold" }}
          >
            FLY YOUR SATELLITE
          </Typography>
          <Typography variant="body1">
            A recurring, hands-on programme designed and managed by the ESA
            Education Office in close collaboration with Universities from{" "}
            <Link
              href="https://www.esa.int/Education/Current_ESA_Member_States"
              target="_blank"
              sx={{
                textDecoration: "underline",
                color: "inherit",
                fontWeight: "bold",
              }}
            >
              ESA Member States
            </Link>
            , with the objective to complement academic education and inspire,
            engage, and better prepare university students for their future
            professions in the space sector.
          </Typography>
          <Link
            href="/"
            sx={{
              width: "fit-content",
              marginTop: "1rem",
              display: "flex",
              textDecoration: "none",
              "&:hover": {
                ".text": { textDecoration: "underline" },
                img: { transform: "translateX(30px)" },
              },
            }}
          >
            <Typography
              className="text"
              fontSize={{ xs: 20, sm: 25 }}
              fontWeight="bold"
              color="#82909B"
              data-cy="sponsor-learnmore"
            >
              Learn More{" "}
            </Typography>
          </Link>
        </Box>

        <Grid container spacing={1} flex={1} px={{ sm: 5 }} pb={{ sm: 5 }}>
          <Grid size={12}>
            <Image
              src="https://artifact.aleasat.space/sponsor-page/esa_columbus.jpg"
              alt="Barbour's Cut Main Image"
              sx={{ width: "100%" }}
            />
          </Grid>
          <Grid size={6}>
            <Image
              src="https://artifact.aleasat.space/sponsor-page/satellite_assembly_1.jpg"
              alt="ALEASAT Pillars"
              sx={{ width: "100%" }}
            />
          </Grid>
          <Grid size={6}>
            <Image
              src="https://artifact.aleasat.space/sponsor-page/satellite_assembly_2.png"
              alt="Additional Image"
              sx={{ width: "100%" }}
            />
          </Grid>
        </Grid>
      </Container>

      <Container
        maxWidth={"lg"}
        sx={{
          maxWidth: "1000px",
          display: "flex",
          flexDirection: { xs: "column", md: "row" },
          justifyContent: "space-between",
          alignItems: "center",
          margin: "auto",
          my: 5,
          height: "fit-content",
        }}
      >
        <Image
          src="https://artifact.aleasat.space/sponsor-page/aleasat_team.png"
          alt="sponsor-image"
          width={"100%"}
          sx={{ flex: 1, pr: { md: 3 } }}
        />

        <Box
          sx={{
            flex: 1,
            height: "100%",
            mt: { xs: 3, md: 0 },
            px: { md: 5 },
          }}
        >
          <Typography
            variant="h4"
            gutterBottom
            sx={{
              mb: 5,
              fontWeight: "bold",
            }}
          >
            MAKE A DIFFERENCE
          </Typography>
          <Typography variant="body1">
            We would love to talk about how we can work together! Please reach
            out to us at <b>captain@ubcorbit.com</b>, and we&apos;ll get in
            touch with you with you shortly. Thank you for considering
            supporting our ALEASAT project!
          </Typography>
        </Box>
      </Container>
    
    </Box>
  );
}
