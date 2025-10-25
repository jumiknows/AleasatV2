import Image from "@aleasat/ui/components/Image";
import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import Typography from "@mui/material/Typography";

export default function SavingLivesSection() {
  return (
    <Box
      sx={{
        position: "relative",
        background: "url('/earth-from-space.png')",
        backgroundSize: "auto 80%",
        backgroundPosition: "center",
        backgroundRepeat: "no-repeat",
        py: { xs: "2rem", md: "4rem" },
      }}
    >
      <Container maxWidth="xl">
        <Box sx={{ textAlign: "center", mb: { xs: 4, md: 6 } }}>
          <Typography
            variant="h2"
            sx={{
              color: "white",
              fontWeight: "bold",
              letterSpacing: "0.05em",
              mb: 3,
              lineHeight: 1.1,
              textShadow: "0 0 20px rgba(74, 144, 226, 0.5)",
            }}
          >
            <Box
              component="span"
              sx={{
                background: "linear-gradient(45deg, #4A90E2, #7BB3F0)",
                backgroundClip: "text",
                WebkitBackgroundClip: "text",
                color: "transparent",
              }}
            >
              SAVING LIVES
            </Box>{" "}
            TO<br />
            SHAPE A SAFER WORLD
          </Typography>
          
          <Typography
            variant="body1"
            sx={{
              color: "white",
              maxWidth: "400px",
              mx: "auto",
              lineHeight: 1.6,
              fontWeight: 300,
            }}
          >
            We help disaster response by managing floods,
            wildfires, and earthquakes with satellite imagery data
          </Typography>
        </Box>

        <Box
          sx={{
            position: "relative",
            minHeight: { xs: "400px", md: "500px" },
          }}
        >
          <Box
            sx={{
              position: "absolute",
              left: "0%",
              top: "-20%",
              width: "280px",
              height: "280px",
              display: { xs: "none", md: "block" },
              transform: "rotate(15deg)",
              animation: "float 6s ease-in-out infinite",
              "@keyframes float": {
                "0%, 100%": { transform: "rotate(15deg) translateY(0px)" },
                "50%": { transform: "rotate(15deg) translateY(-10px)" },
              },
            }}
          >
            <Image
              src="/cubesat-3d.png"
              alt="ALEASAT CubeSat Left"
              sx={{
                width: "100%",
                height: "100%",
                objectFit: "contain",
                filter: "drop-shadow(0 0 20px rgba(74, 144, 226, 0.6))",
              }}
            />
          </Box>

          <Box
            sx={{
              position: "absolute",
              right: "0%",
              top: "-60%",
              width: "280px",
              height: "280px",
              display: { xs: "none", md: "block" },
              transform: "rotate(-15deg) scaleX(-1)",
              animation: "floatMirrored 6s ease-in-out infinite 3s",
              "@keyframes floatMirrored": {
                "0%, 100%": { transform: "rotate(-15deg) scaleX(-1) translateY(0px)" },
                "50%": { transform: "rotate(-15deg) scaleX(-1) translateY(-10px)" },
              },
            }}
          >
            <Image
              src="/cubesat-3d.png"
              alt="ALEASAT CubeSat Right"
              sx={{
                width: "100%",
                height: "100%",
                objectFit: "contain",
                filter: "drop-shadow(0 0 20px rgba(74, 144, 226, 0.6))",
              }}
            />
          </Box>
        </Box>
      </Container>
    </Box>
  );
}