import Hero from "@/components/Hero";
import Image from "@aleasat/ui/components/Image";
import { Container } from "@mui/material";
import Box from "@mui/material/Box";
import Grid from "@mui/material/Grid2";
import Link from "@mui/material/Link";
import Typography from "@mui/material/Typography";
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

export default function JoinUs() {
  return (
    <Box>
      {/* Hero Section – same layout as Sponsors */}
      <Box sx={{ position: "relative", minHeight: "100vh" }}>
        <Hero type="image" src="/sponsor.jpg" title="" />
        {/* Gradient */}
        <Box
          sx={{
            position: "absolute",
            inset: 0,
            pointerEvents: "none",
            background:
              "linear-gradient(to bottom, rgba(0,0,0,0) 40%, rgba(0,0,0,0.9) 100%)",
            zIndex: 1,
          }}
        />
        <Box
          sx={{
            position: "absolute",
            inset: 0,
            display: "flex",
            flexDirection: "column",
            alignItems: "center",
            justifyContent: "center",
            textAlign: "center",
            zIndex: 2,
            px: 2,
          }}
        >
          {/* Main Title */}
          <Typography
            variant="h2"
            sx={{
              color: "white",
              fontWeight: 700,
              fontSize: { xs: "2.5rem", md: "3.5rem" },
              lineHeight: 1.1,
              mb: 1,
            }}
          >
            YOU GOT WHAT IT TAKES?
          </Typography>

          {/* Subtitle (empty for now cause just copied from sponsors page */}
          <Typography
            variant="h2"
            sx={{
              color: "white",
              opacity: 1.0,
              fontSize: { xs: "1.2rem", md: "2.0rem" },
              fontWeight: 500,
            }}
          />
        </Box>
      </Box>

      {/* Main Content */}
      <Box
        sx={{
          position: "relative",
          zIndex: 2,
          background: "#000",
          minHeight: "100vh",
          py: { xs: 6, md: 3 },
          px: { xs: 3, md: 6 },
        }}
      >
        {/* OUR MISSION image section */}
        <Container maxWidth={false} disableGutters sx={{ mb: 8 }}>
          <Box
            sx={{
              position: "relative",
              width: "100vw",        // forcing full width cause fixing margins didnt work
              left: "50%",
              right: "50%",
              marginLeft: "-50vw",  
              marginRight: "-50vw",
              overflow: "hidden",
              minHeight: "65vh",
              backgroundImage: "url('/OurMissionBackground.png')",
              backgroundSize: "cover",
              backgroundPosition: "center",
              backgroundRepeat: "no-repeat",
              
            }}
          >
            {/* Top+bottom black gradient over image */}
            <Box
              sx={{
                position: "absolute",
                inset: 0,
                background:
                  "linear-gradient(to bottom, rgba(0,0,0,0.95) 0%, rgba(0,0,0,0) 50%, rgba(0,0,0,0) 50%, rgba(0,0,0,0.95) 100%)",
                zIndex: 1,
              }}
            />

            <Box
              sx={{
                position: "relative",
                zIndex: 2,
                height: "100%",
                display: "flex",
                flexDirection: "column",
                justifyContent: "center",
                alignItems: "flex-start",
                px: { xs: 3, md: 8 },
                py: { xs: 8, md: 10 },
              }}
            >

              <Typography
                variant="h3"
                sx={{
                  color: "white",
                  fontWeight: "bold",
                  textAlign: "left",
                  mb:10,
                  fontSize: { xs: 20, md: 40 },
                }}
              >
                OUR MISSION
              </Typography>

              <Typography
                variant="body1"
                sx={{
                  color: "#e5e7eb",
                  fontSize: { xs: "1.2rem", md: "1.3rem" },
                  maxWidth: "700px",
                  lineHeight: 1.6,
                }}
              >
                We provide natural disaster relief support by giving amateur
                radio operators and the public access to high-quality imagery
                captured directly from our satellite mission.
              </Typography>
            </Box>
          </Box>
        </Container>

{/* EXPERIENCE SECTION */}
<Container
  maxWidth={false}
  disableGutters
  sx={{ 
    mb: 10, 
    display: "flex", 
    justifyContent: "center",
    px: 2 
  }}
>
  <Box
    sx={{
      width: "100%",
      maxWidth: 1150, 
      display: "flex",
      flexDirection: { xs: "column", md: "row" },
      alignItems: "center", 
    }}
  >
    {/* Left image container */}
    <Box
      sx={{
        flex: { xs: 1, md: 1.2 }, 
        height: { xs: 300, md: 550 }, 
        width: "100%",
        borderRadius: 3,
        overflow: "hidden",
        position: "relative",
        zIndex: 1,
      }}
    >
      <Image
        src="/TableSetup.jpg"
        alt="ALEASAT team"
        style={{
          width: "100%",
          height: "100%",
          objectFit: "cover",
          display: "block",
        }}
      />
    </Box>

{/* Right Card */}
<Box
      sx={{
        flex: { xs: 1, md: 1.5 },
        background: "rgba(29, 29, 29, 0.6)", 
        // 2. blur effect
        backdropFilter: "blur(12px)",
        border: "1px solid rgba(255, 255, 255, 0.1)",

        borderRadius: 2,
        p: { xs: 4, md: 4 },
        boxShadow: "0px 10px 40px rgba(0,0,0,0.5)", 
        
        mt: { xs: -4, md: 0 },   
        ml: { xs: 0, md: -8 },   
        position: "relative",
        zIndex: 2,
        // REMOVED maxWidth here so it fills the remaining space
      }}
    >
      <Typography
        variant="h3"
        sx={{
          color: "white",
          fontWeight: "800",
          fontFamily: 'h3', 
          letterSpacing: "0.05em",
          textTransform: "uppercase",
          fontSize: { xs: "2rem", md: "2.5rem" },
          mb: 1,
        }}
      >
        Experience
      </Typography>

      <Typography variant="body1" sx={{ color: "#a1a1aa", mb: 5, fontSize: "1.1rem" }}>
        By joining our team you will learn:
      </Typography>

      {/* Leadership Skills block */}
      <Box sx={{ display: "flex", gap: 3, mb: 4 }}>
        <Box sx={{ minWidth: 50, pt: 0.5 }}>
           <Image 
             src="/!leadership.png" 
             alt="Leadership" 
             width={160} 
             height={100} 
           />
        </Box>
        <Box>
          <Typography variant="h6" sx={{ color: "white", fontWeight: "bold", mb: 0.5 }}>
            Leadership Skills
          </Typography>
          <Typography variant="body2" sx={{ color: "#cbd5f5", lineHeight: 1.7, fontSize: "1.0rem"}}>
            • Take initiative and own projects.<br />
            • Learn project management and communication.<br />
            • Collaborate with people who share your interests.
          </Typography>
        </Box>
      </Box>

      {/* Technical Skills block */}
      <Box sx={{ display: "flex", gap: 3 }}>
        <Box sx={{ minWidth: 50, pt: 0.5 }}>
           <Image 
             src="/!technical.png" 
             alt="Technical" 
             width={160} 
             height={100} 
           />
        </Box>
        <Box>
          <Typography variant="h6" sx={{ color: "white", fontWeight: "bold", mb: 0.5 }}>
            Technical Skills
          </Typography>
          <Typography variant="body2" sx={{ color: "#cbd5f5", lineHeight: 1.7, fontSize: "1.0rem" }}>
            • Apply coursework to real hardware and software.<br />
            • Learn engineering best practices and review processes.<br />
            • Work with cutting-edge aerospace technology.
          </Typography>
        </Box>
      </Box>

    </Box>
  </Box>
</Container>

      {/* TRAVEL OPPORTUNITIES SECTION */}
      <Box sx={{ mb: 15 }}>
        
        <Container maxWidth="lg">
          {/* 1. Section Header */}
          <Typography
            variant="h3"
            sx={{
              color: "white",
              fontWeight: "800",
              fontFamily: 'h3',
              letterSpacing: "0.05em",
              textTransform: "uppercase",
              mb: 4,
            }}
          >
            TRAVEL OPPORTUNITIES
          </Typography>

          <Box
            sx={{
              width: "100%",
              height: { xs: 300, md: 500 },
              position: "relative",
              mb: 5,
              borderRadius: 1,
              overflow: "hidden",
              border: "1px solid rgba(255, 255, 255, 0.1)",
            }}
          >
            <Image
              src="/map.png"
              alt="World Map of ALEASAT Travels"
              style={{ objectFit: "cover" }}
            />
          </Box>

          <Box sx={{ mb: -8}}>
            <Grid container spacing={4}>
              <Grid size={{ xs: 12, md: 6}} sx={{ display: "flex", flexDirection: "column" }}>
                <Box sx={{ mb: 6 }}>
                  <Typography
                    variant="h3"
                    sx={{
                      color: "white",
                      fontWeight: "400",
                      fontFamily: 'h3',
                      textTransform: "uppercase",
                      mb: 1,
                      fontSize: { xs: "2rem", md: "2.0rem" },
                    }}
                  >
                    EUROPEAN SPACE AGENCY
                  </Typography>
                  <Typography variant="h6" sx={{ color: "#cbd5f5", mb: 2 }}>
                    Noordwijk, Netherlands
                  </Typography>
                  <Typography variant="body1" sx={{ color: "white", maxWidth: 700 }}>
                    Members of ALEASAT flew to Noordwijk, Netherlands to pitch their
                    concept for the Fly Your Satellite! programme!
                  </Typography>
                </Box>

                {/* travel1 */}
                <Box sx={{ position: "relative", height: 350, borderRadius: 1, overflow: "hidden", width: "100%" }}>
                  <Image
                    src="/travel1.png"
                    alt="ESA Trip 1"
                    style={{ objectFit: "cover" }}
                  />
                </Box>
              </Grid>

              {/*travel2 */}
              <Grid size={{ xs: 12, md: 6 }}>
                <Box sx={{ position: "relative", height: "100%", minHeight: { xs: 400, md: 600 }, borderRadius: 1, overflow: "hidden" }}>
                  <Image
                    src="/travel2.png"
                    alt="ESA Trip 2"
                    style={{ objectFit: "cover" }}
                  />
                </Box>
              </Grid>
            </Grid>
          </Box>

{/* 4. CANADIAN SPACE AGENCY */}
<Box sx={{ mb: 1 }}>
            <Grid container spacing={4}>
              
              {/* travel3 */}
              <Grid size={{ xs: 12, md: 6 }}>
                <Box 
                  sx={{ 
                    position: "relative", 
                    height: { xs: 400, md: 650 }, 
                    borderRadius: 1, 
                    overflow: "hidden" 
                  }}
                >
                  <Image
                    src="/travel3.png"
                    alt="CSA Trip 1"
                    style={{ objectFit: "cover" }}
                  />
                </Box>
              </Grid>

              {/* travel4 */}
              <Grid size={{ xs: 12, md: 6 }} sx={{ display: "flex", flexDirection: "column" }}>
                <Box sx={{ mb: 4 }}>
                  <Typography
                    variant="h3"
                    sx={{
                      color: "white",
                      fontWeight: "400",
                      fontFamily: 'h3',
                      textTransform: "uppercase",
                      mb: 1,
                      fontSize: { xs: "2rem", md: "2.0rem" },
                    }}
                  >
                    CANADIAN SPACE AGENCY
                  </Typography>
                  <Typography variant="h6" sx={{ color: "#cbd5f5", mb: 2}}>
                    Ottawa, Ontario
                  </Typography>
                  <Typography variant="body1" sx={{ color: "white", maxWidth: 700 }}>
                    Members of ALEASAT flew to Ottawa for vibration testing of ALEASAT's structure.
                  </Typography>
                </Box>

                {/* travel4 */}
                <Box 
                  sx={{ 
                    position: "relative", 
                    height: 460, 
                    borderRadius: 0, 
                    overflow: "hidden", 
                    width: "100%"
                  }}
                >
                  <Image
                    src="/travel4.png"
                    alt="CSA Trip 2"
                    style={{ objectFit: "cover" }}
                  />
                </Box>
              </Grid>

            </Grid>
          </Box>

{/* 5. PARIS CONVENTION CENTRE */}
<Box sx={{ mb: 5 }}>
            <Grid container spacing={4}>
              
              {/*paris1.png */}
              <Grid size={{ xs: 12, md: 6}} sx={{ display: "flex", flexDirection: "column" }}>
              
                <Box sx={{ mb: 4 }}>
                  <Typography
                    variant="h3"
                    sx={{
                      color: "white",
                      fontWeight: "400",
                      fontFamily: 'h3', 
                      textTransform: "uppercase",
                      mb: 1,
                      fontSize: { xs: "2rem", md: "2.0rem" },
                    }}
                  >
                    PARIS CONVENTION CENTRE
                  </Typography>
                  <Typography variant="h6" sx={{ color: "#cbd5f5", mb: 2 }}>
                    Paris, France
                  </Typography>
                  <Typography variant="body1" sx={{ color: "#a1a1aa", maxWidth: 700 }}>
                    Members of ALEASAT flew to Paris to present a paper "Low-cost
                    Manufacturing and Testing of Altitude Control Systems for
                    CubeSats."
                  </Typography>
                </Box>

                {/* Bottom Left Image (paris1) */}
                <Box sx={{ position: "relative", height: 350, borderRadius: 1, overflow: "hidden", width: "100%" }}>
                  <Image
                    src="/paris1.png"
                    alt="Paris Trip 1"
                    style={{ objectFit: "cover" }}
                  />
                </Box>
              </Grid>

              {/* Right image (paris2) */}
              <Grid size={{ xs: 12, md: 6 }}>
                <Box sx={{ position: "relative", height: { xs: 400, md: 450 }, borderRadius: 1, overflow: "hidden" }}>
                  <Image
                    src="/paris2.png"
                    alt="Paris Trip 2"
                    style={{ objectFit: "cover" }}
                  />
                </Box>
              </Grid>

            </Grid>
          </Box>
        </Container>

{/* 6. APPLY NOWW */}
<Container maxWidth="sm" sx={{ mt: 15, mb: 5}}>
          <Box
            sx={{
              textAlign: "center",
              p: 4,
              background: "transparent",
            }}
          >
            <Typography
              variant="h3"
              sx={{
                color: "white",
                fontWeight: "800",
                fontFamily: 'h3', 
                textTransform: "uppercase",
                letterSpacing: "0.05em",
                mb: 4,
              }}
            >
              APPLY NOW!
            </Typography>

            {/* replaced link with text for now */}
            <Box
              sx={{
                display: "inline-block",
                background: "rgba(255, 255, 255, 0.05)", // Subtle dark background
                border: "1px solid rgba(255, 255, 255, 0.1)", // Subtle border
                borderRadius: 3,
                px: 4,
                py: 2,
              }}
            >
              <Typography
                variant="body1"
                sx={{
                  color: "#a1a1aa", // Muted text will change once recruitment opens
                  fontSize: "1.1rem",
                  fontWeight: 500,
                }}
              >
                Please wait for the next recruitment period
              </Typography>
            </Box>
          </Box>
        </Container>
      </Box>
    </Box>
  </Box>
  );
}
