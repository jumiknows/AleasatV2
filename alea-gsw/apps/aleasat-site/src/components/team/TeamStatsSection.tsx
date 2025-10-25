import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import Grid2 from "@mui/material/Grid2";
import Typography from "@mui/material/Typography";

export default function TeamStatsSection() {
  const stats = [
    {
      title: "Team Members",
      value: "300+",
    },
    {
      title: "Subsystems",
      value: "10",
    },
    {
      title: "Nationalities",
      value: "10+",
    },
    {
      title: "Programs",
      value: "20+",
    },
  ];

  return (
    <Box sx={{ py: { xs: "4rem", md: "6rem" } }}>
      <Container maxWidth="lg">
        <Typography
          variant="h2"
          sx={{
            textAlign: "center",
            mb: 8,
            fontWeight: "bold",
            letterSpacing: "0.05em",
          }}
        >
          Team Statistics
        </Typography>
        
        <Grid2 container spacing={4}>
          {stats.map((stat, i) => (
            <Grid2 key={i} size={{ xs: 6, md: 3 }}>
              <Box
                sx={{
                  textAlign: "center",
                  p: 4,
                  borderRadius: "12px",
                  border: "1px solid rgba(255, 255, 255, 0.1)",
                  transition: "all 0.3s ease",
                  "&:hover": {
                    border: "1px solid rgba(74, 144, 226, 0.3)",
                    transform: "translateY(-4px)",
                  },
                }}
              >
                <Typography
                  variant="h3"
                  sx={{
                    fontWeight: "bold",
                    mb: 2,
                    background: "linear-gradient(45deg, #4A90E2, #7BB3F0)",
                    backgroundClip: "text",
                    WebkitBackgroundClip: "text",
                    color: "transparent",
                  }}
                >
                  {stat.value}
                </Typography>
                <Typography
                  variant="body1"
                  sx={{
                    color: "rgba(255, 255, 255, 0.8)",
                    fontWeight: 500,
                  }}
                >
                  {stat.title}
                </Typography>
              </Box>
            </Grid2>
          ))}
        </Grid2>
      </Container>
    </Box>
  );
}
