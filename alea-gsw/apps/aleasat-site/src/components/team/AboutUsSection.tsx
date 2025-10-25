import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import Typography from "@mui/material/Typography";

export default function AboutUsSection() {
  return (
    <Box sx={{ py: { xs: "4rem", md: "6rem" } }}>
      <Container maxWidth="lg">
        <Typography
          variant="h2"
          sx={{
            textAlign: "center",
            mb: 6,
            fontWeight: "bold",
            letterSpacing: "0.05em",
          }}
        >
          About the Team
        </Typography>
        
        <Box
          sx={{
            display: "grid",
            gap: 4,
            maxWidth: "800px",
            mx: "auto",
          }}
        >
          <Typography
            variant="body1"
            sx={{
              fontSize: "1.2rem",
              lineHeight: 1.8,
              color: "rgba(255, 255, 255, 0.9)",
              textAlign: "center",
            }}
          >
            ALEASAT is a student-led team of over 100 undergraduate and graduate
            students from UBC, SFU, and other institutions, united by a passion
            for space exploration and innovation.
          </Typography>
          
          <Typography
            variant="body1"
            sx={{
              fontSize: "1.1rem",
              lineHeight: 1.7,
              color: "rgba(255, 255, 255, 0.8)",
            }}
          >
            We design and build advanced CubeSats—miniaturized satellites—for real-time Earth observation,
            tackling real-world challenges through hands-on engineering and
            interdisciplinary collaboration. Beyond our technical goals, we are
            dedicated to inspiring the next generation of engineers and scientists
            through active community outreach, educational workshops, and
            mentorship.
          </Typography>
          
          <Typography
            variant="body1"
            sx={{
              fontSize: "1.1rem",
              lineHeight: 1.7,
              color: "rgba(255, 255, 255, 0.8)",
            }}
          >
            By combining cutting-edge space technology with a strong
            commitment to learning and engagement, ALEASAT empowers students to
            develop real-world skills, drive meaningful impact, and bring the
            dream of space closer to home.
          </Typography>
        </Box>
      </Container>
    </Box>
  );
}
