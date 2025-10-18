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
      {/* Hero Section */}
      <Box sx={{ position: "relative" }}>
        <Hero
          type="image"
          src="/sponsor.jpg"
          title={`LAUNCHING DREAMS
SPONSORS MAKE IT POSSIBLE`}
        />
      </Box>

      {/* Main Content */}
      <Box
        sx={{
          position: "relative",
          zIndex: 2,
          background: "#000",
          minHeight: "100vh",
          py: { xs: 6, md: 10 },
          px: { xs: 3, md: 6 },
        }}
      >
        {/* Welcome Section */}
        <Container maxWidth="lg" sx={{ mb: 8 }}>
          <Typography
            variant="h3"
            sx={{
              color: "white",
              fontWeight: "bold",
              textAlign: "center",
              mb: 4,
              fontSize: { xs: 28, md: 42 },
            }}
          >
            PARTNER WITH ALEASAT
          </Typography>
          <Typography
            variant="h6"
            sx={{
              color: "#b6bdd6",
              textAlign: "center",
              lineHeight: 1.6,
              maxWidth: "800px",
              margin: "0 auto",
              mb: 6,
            }}
          >
            Join us in revolutionizing space technology and disaster relief. 
            Your sponsorship fuels innovation, educates the next generation of engineers,
            and creates real-world impact through satellite technology.
          </Typography>
        </Container>

        {/* Sponsorship Benefits Section */}
        <Container maxWidth="lg" sx={{ mb: 10 }}>
          <Typography
            variant="h4"
            sx={{
              color: "white",
              fontWeight: "bold",
              textAlign: "center",
              mb: 2,
            }}
          >
            SPONSORSHIP BENEFITS
          </Typography>
          <Typography
            variant="body1"
            sx={{
              color: "#b6bdd6",
              textAlign: "center",
              mb: 8,
              maxWidth: "600px",
              margin: "0 auto 4rem auto",
            }}
          >
            Discover the meaningful benefits of supporting the ALEASAT mission
          </Typography>

          <Grid container spacing={4}>
            {benefits.map((benefit) => (
              <Grid size={{ xs: 12, md: 4 }} key={benefit.title}>
                <Box
                  sx={{
                    textAlign: "center",
                    p: 4,
                    borderRadius: 2,
                    border: "1px solid #333",
                    background: "linear-gradient(145deg, #0a0a0a, #1a1a1a)",
                    height: "100%",
                    transition: "all 0.3s ease",
                    "&:hover": {
                      borderColor: "#4A90E2",
                      transform: "translateY(-5px)",
                    },
                  }}
                >
                  <Box
                    sx={{
                      width: 80,
                      height: 80,
                      borderRadius: "50%",
                      border: "2px solid #4A90E2",
                      display: "flex",
                      alignItems: "center",
                      justifyContent: "center",
                      margin: "0 auto 2rem auto",
                      background: "#000",
                    }}
                  >
                    <Image
                      src={benefit.iconPath}
                      alt={benefit.title}
                      width={40}
                      height={40}
                    />
                  </Box>
                  <Typography
                    variant="h6"
                    sx={{ color: "white", fontWeight: "bold", mb: 2 }}
                  >
                    {benefit.title}
                  </Typography>
                  <Typography
                    variant="body2"
                    sx={{ color: "#b6bdd6", lineHeight: 1.6 }}
                  >
                    {benefit.description}
                  </Typography>
                </Box>
              </Grid>
            ))}
          </Grid>
        </Container>

        {/* Call to Action Section */}
        <Container maxWidth="md">
          <Box
            sx={{
              textAlign: "center",
              p: 6,
              borderRadius: 3,
              background: "linear-gradient(45deg, #1a1a2e, #16213e)",
              border: "1px solid #4A90E2",
            }}
          >
            <Typography
              variant="h4"
              sx={{
                color: "white",
                fontWeight: "bold",
                mb: 3,
              }}
            >
              READY TO LAUNCH DREAMS?
            </Typography>
            <Typography
              variant="body1"
              sx={{
                color: "#b6bdd6",
                mb: 4,
                lineHeight: 1.6,
              }}
            >
              Contact us to discuss sponsorship opportunities and learn how we can work together 
              to advance space technology and disaster relief initiatives.
            </Typography>
            <Link
              href="mailto:captain@ubcorbit.com"
              sx={{
                display: "inline-block",
                background: "linear-gradient(45deg, #4A90E2, #357ABD)",
                color: "white",
                px: 4,
                py: 2,
                borderRadius: 2,
                textDecoration: "none",
                fontWeight: "bold",
                fontSize: "1.1rem",
                transition: "all 0.3s ease",
                "&:hover": {
                  background: "linear-gradient(45deg, #357ABD, #2E6BA8)",
                  transform: "translateY(-2px)",
                },
              }}
            >
              Get In Touch
            </Link>
          </Box>
        </Container>
      </Box>
    </Box>
  );
}
