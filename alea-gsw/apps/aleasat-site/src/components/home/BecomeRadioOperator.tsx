import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Typography from "@mui/material/Typography";

export default function BecomeRadioOperator() {
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
      id="radio-operator-section"
    >
      <Typography
        variant="h2"
        sx={{
          color: "white",
          fontWeight: "bold",
          letterSpacing: "0.05em",
          mb: { xs: 3, md: 4 },
          lineHeight: 1.1,
          textTransform: "uppercase",
        }}
      >
        BECOME A RADIO OPERATOR?
      </Typography>

        <Typography
          variant="h6"
          sx={{
            color: "white",
            lineHeight: 1.6,
            mb: { xs: 4, md: 6 },
            opacity: 0.9,
            maxWidth: "900px",
          }}
        >
          You will request imagery of a specific location on Earth, and then downlink that imagery directly from the satellite.
        </Typography>

        <Button
          variant="contained"
          sx={{
            background: "linear-gradient(45deg, #4A90E2, #357ABD)",
            color: "white",
            borderRadius: "15px",
            fontSize: { xs: "1rem", md: "1.125rem" },
            textTransform: "none",
            boxShadow: "0 4px 20px rgba(74, 144, 226, 0.3)",
            transition: "all 0.3s ease",
            "&:hover": {
              background: "linear-gradient(45deg, #357ABD, #2E6BA8)",
              boxShadow: "0 6px 25px rgba(74, 144, 226, 0.4)",
            },
          }}
          href="/join-us"
        >
          Sign Up
        </Button>
    </Box>
  );
}
