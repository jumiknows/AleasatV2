import { type MemberData, members } from "@/data/members";
import { Box, Container, Grid2, Stack, Typography } from "@mui/material";
import Image from "next/image";

export default function ManagementSection() {
  const captains = members.CAPTAIN as MemberData[];
  const advisors = members.ADVISOR as MemberData[];

  return (
    <>
      <Container maxWidth="md">
        <Box sx={{ my: 4 }}>
          <Typography variant="h2" sx={{ textAlign: "center", mb: 2 }}>
            Our Advisors
          </Typography>
          <Typography sx={{ mb: 2 }}>
            Our advisors provide expert guidance and support across all stages
            of the satellite’s development—offering critical design feedback and
            helping shape our long-term vision.
          </Typography>
          <Grid2 container spacing={4} justifyContent="center">
            {advisors.map((advisor, i) => (
              <Grid2 key={i} size={{ xs: 6, sm: 4 }}>
                <Box
                  sx={{
                    position: "relative",
                    width: "100%",
                    aspectRatio: "1/1",
                    overflow: "hidden",
                  }}
                >
                  <Box
                    sx={{
                      position: "relative",
                      width: "100%",
                      aspectRatio: "2/3",
                    }}
                  >
                    <Image
                      src={
                        "https://artifact.aleasat.space/team-photo/" +
                        advisor.image!
                      }
                      fill
                      alt={`Image of ${advisor.firstname}`}
                      style={{
                        objectFit: "cover",
                      }}
                    />
                  </Box>
                </Box>
                <Typography variant="h5" sx={{ textAlign: "center" }}>
                  {advisor.firstname} {advisor.lastname}
                </Typography>
                <Typography
                  variant="h6"
                  sx={{ textAlign: "center", fontWeight: "300" }}
                >
                  {advisor.role}
                </Typography>
              </Grid2>
            ))}
          </Grid2>
        </Box>
        <Box sx={{ my: 4 }}>
          <Typography variant="h2" sx={{ textAlign: "center", mb: 2 }}>
            Our Leaderships
          </Typography>
          <Typography sx={{ mb: 2 }}>
            The Leaders lead and coordinate subteams, ensuring tasks stay on
            track and aligned with project timelines. Their leadership is key to
            ALEASAT's mission success.
          </Typography>
          <Grid2 container spacing={4} justifyContent="center">
            {captains.map((captain, i) => (
              <Grid2 key={i} size={{ xs: 6, sm: 4 }}>
                <Box
                  sx={{
                    position: "relative",
                    width: "100%",
                    aspectRatio: "1/1",
                    overflow: "hidden",
                  }}
                >
                  <Box
                    sx={{
                      position: "relative",
                      width: "100%",
                      aspectRatio: "2/3",
                    }}
                  >
                    <Image
                      src={
                        "https://artifact.aleasat.space/team-photo/" +
                        captain.image!
                      }
                      fill
                      alt={`Image of ${captain.firstname}`}
                      style={{
                        objectFit: "cover",
                      }}
                    />
                  </Box>
                </Box>
                <Typography variant="h5" sx={{ textAlign: "center" }}>
                  {captain.firstname} {captain.lastname}
                </Typography>
                <Typography
                  variant="h6"
                  sx={{ textAlign: "center", fontWeight: "300" }}
                >
                  {captain.role}
                </Typography>
              </Grid2>
            ))}
          </Grid2>
        </Box>
      </Container>
    </>
  );
}
