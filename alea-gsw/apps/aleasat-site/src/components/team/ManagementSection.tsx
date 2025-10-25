import Image from "@aleasat/ui/components/Image";
import { type MemberData, members } from "@/data/members";
import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import Grid2 from "@mui/material/Grid2";
import Typography from "@mui/material/Typography";

const PersonCard = ({ person }: { person: MemberData }) => (
  <Box
    sx={{
      textAlign: "center",
      transition: "all 0.3s ease",
      "&:hover": {
        transform: "translateY(-8px)",
      },
    }}
  >
    <Box
      sx={{
        position: "relative",
        width: "100%",
        aspectRatio: "3/4",
        borderRadius: "16px",
        overflow: "hidden",
        mb: 3,
        border: "2px solid rgba(255, 255, 255, 0.1)",
        transition: "all 0.3s ease",
        "&:hover": {
          border: "2px solid rgba(74, 144, 226, 0.4)",
          boxShadow: "0 8px 30px rgba(74, 144, 226, 0.2)",
        },
      }}
    >
      <img
        src={`https://artifact.aleasat.space/team-photo/${person.image!}`}
        alt={`${person.firstname} ${person.lastname}`}
        style={{
          width: "100%",
          height: "100%",
          objectFit: "cover",
        }}
      />
    </Box>
    <Typography
      variant="h6"
      sx={{
        fontWeight: "bold",
        mb: 1,
        color: "white",
      }}
    >
      {person.firstname} {person.lastname}
    </Typography>
    <Typography
      variant="body1"
      sx={{
        color: "rgba(255, 255, 255, 0.7)",
        fontWeight: 400,
      }}
    >
      {person.role}
    </Typography>
  </Box>
);

export default function ManagementSection() {
  const captains = members.CAPTAIN as MemberData[];
  const advisors = members.ADVISOR as MemberData[];

  return (
    <Box sx={{ py: { xs: "4rem", md: "6rem" } }}>
      <Container maxWidth="lg">
        {/* Advisors Section */}
        <Box sx={{ mb: 8 }}>
          <Typography
            variant="h2"
            sx={{
              textAlign: "center",
              mb: 4,
              fontWeight: "bold",
              letterSpacing: "0.05em",
            }}
          >
            Our Advisors
          </Typography>
          <Typography
            variant="body1"
            sx={{
              textAlign: "center",
              mb: 6,
              fontSize: "1.1rem",
              color: "rgba(255, 255, 255, 0.8)",
              maxWidth: "600px",
              mx: "auto",
            }}
          >
            Our advisors provide expert guidance and support across all stages
            of the satellite's development—offering critical design feedback and
            helping shape our long-term vision.
          </Typography>
          <Grid2 container spacing={4} justifyContent="center">
            {advisors.map((advisor, i) => (
              <Grid2 key={i} size={{ xs: 12, sm: 6, md: 4 }}>
                <PersonCard person={advisor} />
              </Grid2>
            ))}
          </Grid2>
        </Box>

        {/* Leadership Section */}
        <Box>
          <Typography
            variant="h2"
            sx={{
              textAlign: "center",
              mb: 4,
              fontWeight: "bold",
              letterSpacing: "0.05em",
            }}
          >
            Our Leadership
          </Typography>
          <Typography
            variant="body1"
            sx={{
              textAlign: "center",
              mb: 6,
              fontSize: "1.1rem",
              color: "rgba(255, 255, 255, 0.8)",
              maxWidth: "600px",
              mx: "auto",
            }}
          >
            The Leaders lead and coordinate subteams, ensuring tasks stay on
            track and aligned with project timelines. Their leadership is key to
            ALEASAT's mission success.
          </Typography>
          <Grid2 container spacing={4} justifyContent="center">
            {captains.map((captain, i) => (
              <Grid2 key={i} size={{ xs: 12, sm: 6, md: 4 }}>
                <PersonCard person={captain} />
              </Grid2>
            ))}
          </Grid2>
        </Box>
      </Container>
    </Box>
  );
}
