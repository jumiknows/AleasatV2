import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Typography from "@mui/material/Typography";
import Image from "@aleasat/ui/components/Image";

export default function Plan() {
  return (
    <Box
      sx={{
        position: "relative",
        zIndex: 2,
        background: "#000",
        minHeight: "50vh",
        py: { xs: "2rem", md: "3rem" },
        display: "flex",
        flexDirection: { xs: "column", md: "row" },
        justifyContent: "space-between",
        alignItems: "center",
        px: { xs: 4, md: 10, lg: 14 },
        gap: { xs: 3, md: 4, lg: 6 },
      }}
      id="plan-section"
    >
      {/* Left side - Text content */}
      <Box
        sx={{
          flex: 1,
          maxWidth: { xs: "100%", md: "50%", lg: "45%" },
          textAlign: { xs: "center", md: "left" },
          pr: { md: 4 },
        }}
      >
        <Typography
          variant="h2"
          sx={{
            color: "white",
            fontWeight: "bold",
            fontSize: { xs: 25, md: 35, lg: 45 },
            letterSpacing: "0.1em",
            mb: { xs: 2, md: 3 },
            textAlign: { xs: "center", md: "left" },
          }}
        >
          PLAN
        </Typography>

        <Typography
          variant="h6"
          sx={{
            color: "white",
            lineHeight: 1.4,
            mb: { xs: 3, md: 4 },
            textAlign: { xs: "center", md: "left" },
            maxWidth: { md: "90%" },
          }}
        >
          A 10x10x10cm CubeSat that combines state of the art
          engineering to handle the harsh conditions of launch and
          space, communication systems and sophisticated
          embedded hardware and software solutions.
        </Typography>

        <Box
          sx={{
            display: "flex",
            gap: 2,
            justifyContent: { xs: "center", md: "flex-start" },
            flexWrap: "wrap",
          }}
        >
          <Button
            variant="outlined"
            sx={{
              color: "#b6bdd6",
              borderColor: "#b6bdd6",
              borderWidth: 2,
              borderRadius: "25px",
              px: { xs: 3, md: 4 },
              py: { xs: 1, md: 1.2 },
              textTransform: "none",
              transition: "all 0.3s ease",
              "&:hover": {
                color: "white",
                borderColor: "white",
              },
            }}
            href="/cubesat"
          >
            Cubesat
          </Button>

          <Button
            variant="outlined"
            sx={{
              color: "#b6bdd6",
              borderColor: "#b6bdd6",
              borderWidth: 2,
              borderRadius: "25px",
              px: { xs: 3, md: 4 },
              py: { xs: 1, md: 1.2 },
              textTransform: "none",
              transition: "all 0.3s ease",
              "&:hover": {
                color: "white",
                borderColor: "white",
              },
            }}
            href="/telemetry"
          >
            Telemetry
          </Button>
        </Box>
      </Box>

      {/* Right side - CubeSat image */}
      <Box
        sx={{
          flex: 1,
          display: "flex",
          justifyContent: { xs: "center", md: "flex-end" },
          alignItems: "center",
          maxWidth: { xs: "100%", md: "50%", lg: "55%" },
          position: "relative",
        }}
      >
        <Image
          // src="https://artifact.aleasat.space/cubesat-3d.png"
          src="/cubesat-3d.png"
          alt="ALEASAT CubeSat 3D Model"
          sx={{
            width: { xs: 320, sm: 400, md: 450, lg: 520 },
            height: { xs: 320, sm: 400, md: 450, lg: 520 },
            objectFit: "contain",
            filter: "brightness(1.1)",
            transform: "scaleX(-1)", // Mirror image (horizontal flip)
          }}
        />
      </Box>
    </Box>
  );
}
