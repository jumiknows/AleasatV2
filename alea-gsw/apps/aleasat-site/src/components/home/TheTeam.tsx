import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Typography from "@mui/material/Typography";
import Image from "@aleasat/ui/components/Image";

export default function TheTeam() {
  return (
    <Box
      sx={{
        position: "relative",
        zIndex: 2,
        background: "#000",
        minHeight: "100vh",
        py: { xs: "2rem", md: "4rem" },
        display: "flex",
        justifyContent: "center",
        alignItems: "center",
        px: { xs: 2, md: 6, lg: 10 },
      }}
      id="team-section"
    >
      <Box
        sx={{
          position: "relative",
          maxWidth: "1200px",
          width: "100%",
          borderRadius: 3,
          overflow: "hidden",
          boxShadow: "0 20px 40px rgba(0, 0, 0, 0.3)",
          minHeight: { xs: "auto", md: "500px" },
          display: { xs: "flex", md: "block" },
          flexDirection: { xs: "column", md: "row" },
        }}
      >
        {/* Background image */}
        <Box
          sx={{
            position: { xs: "relative", md: "absolute" },
            top: { xs: "auto", md: 0 },
            left: { xs: "auto", md: 0 },
            width: { xs: "100%", md: "100%" },
            height: { xs: "250px", md: "100%" },
            zIndex: 1,
          }}
        >
          <Image
            // src="https://artifact.aleasat.space/team-working.jpg"
            src="/TheTeam.png"
            alt="ALEASAT Team Working on Electronics"
            sx={{
              width: { xs: "100%", md: "60%" },
              height: "100%",
              objectFit: "cover",
              transform: "scaleX(-1)",
            }}
          />
        </Box>

        {/* Content with dark background */}
        <Box
          sx={{
            position: { xs: "relative", md: "absolute" },
            top: { xs: "auto", md: "50%" },
            right: { xs: "auto", md: "1%" },
            transform: { 
              xs: "none", 
              md: "translateY(-50%)" 
            },
            width: { xs: "100%", md: "50%" },
            mt: { xs: 3, md: 0 },
            background: "rgba(34, 34, 34, 0.9)",
            border: "3px solid",
            borderColor: "#777474af",
            p: { xs: 3, md: 4, lg: 5 },
            zIndex: 2,
            display: "flex",
            flexDirection: "column",
            justifyContent: "center",
          }}
        >
          <Typography
            variant="h2"
            sx={{
              color: "white",
              fontWeight: "bold",
              letterSpacing: "0.05em",
              mb: { xs: 2, md: 3 },
              lineHeight: 1.1,
            }}
          >
            THE TEAM
          </Typography>

          <Typography
            sx={{
              color: "white",
              lineHeight: 1.6,
              mb: { xs: 3, md: 4 },
              opacity: 0.95,
            }}
          >
            A team of 100+ individuals that combine their passion of space and engineering to build a disaster relief satellite subject to rigorous testing. It is a culmination of several subsystems working together, managed by two very highly talented leaders.
          </Typography>

          <Button
            variant="outlined"
            sx={{
              color: "#b6bdd6",
              borderColor: "#b6bdd6",
              borderWidth: 1.5,
              borderRadius: "20px",
              px: { xs: 3, md: 4 },
              py: { xs: 1, md: 1.2 },
              textTransform: "none",
              alignSelf: "flex-start",
              transition: "all 0.3s ease",
              "&:hover": {
                color: "white",
                borderColor: "white",
                backgroundColor: "rgba(255, 255, 255, 0.08)",
              },
            }}
            href="/team"
          >
            Learn More
          </Button>
        </Box>
      </Box>
    </Box>
  );
}
