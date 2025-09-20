import Hero from "@/components/Hero";
import Image from "@aleasat/ui/components/Image";
import { Container } from "@mui/material";
import Box from "@mui/material/Box";
import Grid from "@mui/material/Grid2";
import Link from "@mui/material/Link";
import Typography from "@mui/material/Typography";
import type { Metadata } from "next";

export const metadata: Metadata = {
  title: "Sponsors",
  description:
    "Support the future of space exploration by partnering with ALEASAT. Discover sponsorship benefits and brand exposure opportunities!",
  openGraph: {
    title: "Sponsors",
    description:
      "Support the future of space exploration by partnering with ALEASAT. Discover sponsorship benefits and brand exposure opportunities!",
    images: "https://artifact.aleasat.space/icon.png",
  },
};

const benefits: { title: string; description: string; iconPath: string }[] = [
  {
    title: "Brand Exposure",
    description:
      "Gain visibility by being showcased through UBC’s expanding innovative projects and dynamic teams.",
    iconPath: "https://artifact.aleasat.space/sponsor-page/earth.png",
  },
  {
    title: "Brand Association",
    description:
      "Positively shape their brand image by associating with a platform dedicated to aerospace education.",
    iconPath: "https://artifact.aleasat.space/sponsor-page/star.png",
  },
  {
    title: "Skill Development",
    description:
      "Apply classroom knowledge to build aerospace skills to foster better engineers for the future.",
    iconPath: "https://artifact.aleasat.space/sponsor-page/hat.png",
  },
];

export default function Sponsor() {
  return (
    <Box>
      <Box sx={{ position: "relative" }}>
        <Hero
          type="image"
          src="https://artifact.aleasat.space/sponsor-background.jpg"
          title="Our Sponsorship"
        />
        <Box
          sx={{
            padding: "auto",
            maxWidth: "100%",
            px: { xs: "5%", md: "10%" },
            pt: 15,
            position: "relative",
            zIndex: 2,
            background: "#000",
            pb: 25,
          }}
          id="next-section"
        >
          {/* New Sponsorship Benefits Section */}
          <Typography
            variant="h4"
            gutterBottom
            sx={{ mb: 5, fontWeight: "bold", textAlign: "center" }}
          >
            SPONSORSHIP BENEFITS
          </Typography>
          <Typography variant="body1" sx={{ mb: 8, textAlign: "center" }}>
            Discover the meaningful benefits why your support is essential for
            the ALEASAT mission&apos;s success.
          </Typography>

          <Box
            sx={{
              display: "flex",
              flexDirection: { xs: "column", md: "row" },
              gap: { xs: 8, md: 3 },
            }}
          >
            {benefits.map((benefit) => {
              return (
                <Box
                  key={benefit.title}
                  sx={{ flex: 1, position: "relative", textAlign: "center" }}
                >
                  {/* Icon Container */}
                  <Box
                    sx={{
                      width: 90,
                      aspectRatio: "1/1",
                      borderRadius: "50%",
                      border: "2px solid grey",
                      overflow: "hidden",
                      display: "flex",
                      justifyContent: "center",
                      alignItems: "center",
                      position: "absolute",
                      top: -45,
                      left: "50%",
                      backgroundColor: "black",
                      transform: "translateX(-50%)",
                      zIndex: 2,
                    }}
                  >
                    <Image
                      src={benefit.iconPath}
                      alt="Brand Exposure Image"
                      width={50}
                      height={50}
                    />
                  </Box>

                  {/* Main Box with Content */}
                  <Box
                    sx={{
                      py: 4,
                      px: { xs: 0.3, lg: 1 },
                      border: "0.5px solid grey",
                      borderRadius: 1,
                      zIndex: 1,
                    }}
                  >
                    <Typography variant="h6" sx={{ mt: 5 }}>
                      {benefit.title}
                    </Typography>
                    <Typography variant="body2">
                      {benefit.description}
                    </Typography>
                  </Box>
                </Box>
              );
            })}
          </Box>
        </Box>
      </Box>

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
