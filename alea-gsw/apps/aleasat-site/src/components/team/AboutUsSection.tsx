import { Container, Typography } from "@mui/material";

export default function AboutUsSection() {
  return (
    <>
      <Container maxWidth="md">
        <Typography variant="h2" sx={{ py: 4 }}>
          About the Team
        </Typography>
        <Typography
          sx={{
            mb: 8,
          }}
        >
          ALEASAT is a student-led team of over 100 undergraduate and graduate
          students from UBC, SFU, and other institutions, united by a passion
          for space exploration and innovation. We design and build advanced
          CubeSats—miniaturized satellites—for real-time Earth observation,
          tackling real-world challenges through hands-on engineering and
          interdisciplinary collaboration. Beyond our technical goals, we are
          dedicated to inspiring the next generation of engineers and scientists
          through active community outreach, educational workshops, and
          mentorship. By combining cutting-edge space technology with a strong
          commitment to learning and engagement, ALEASAT empowers students to
          develop real-world skills, drive meaningful impact, and bring the
          dream of space closer to home.
        </Typography>
      </Container>
    </>
  );
}
