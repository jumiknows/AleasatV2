import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Typography from "@mui/material/Typography";

export default function About() {
  return (
    <Box
      sx={{
        position: "relative",
        zIndex: 2,
        background: "#000",
        minHeight: "50vh",
        py: { xs: "2rem", md: "3rem" },
        display: "flex",
        flexDirection: "column",
        justifyContent: "center",
        alignItems: "center",
        textAlign: "center",
        px: { xs: 3, md: 6 },
      }}
      id="about-section"
    >
      <Typography
        variant="h2"
        sx={{
          color: "white",
          fontWeight: "bold",
          letterSpacing: "0.1em",
          mb: { xs: 2, md: 3 },
        }}
      >
        ABOUT
      </Typography>

      <Box
        sx={{
          maxWidth: { xs: "100%", sm: "600px", md: "800px" },
          mb: { xs: 3, md: 4 },
        }}
      >
        <Typography
          variant="h6"
          sx={{
            color: "white",
            lineHeight: 1.3,
            mb: 1,
          }}
        >
          Natural disasters kill an average of 60,000 people per year globally.
        </Typography>
        <Typography
          variant="h6"
          sx={{
            color: "white",
            lineHeight: 1.3,
          }}
        >
          We provide natural disaster relief operation via satellite imagery.
        </Typography>
      </Box>

      <Button
        variant="outlined"
        sx={{
          color: "#b6bdd6",
          borderColor: "#b6bdd6",
          borderWidth: 2,
          borderRadius: "25px",
          px: { xs: 5, md: 5.5 },
          py: { xs: 1, md: 1.5 },
          textTransform: "none",
          transition: "all 0.3s ease",
          "&:hover": {
            color: "white",
            borderColor: "white",
          },
        }}
        href="/about"
      >
        About Us
      </Button>
    </Box>
  );
}
