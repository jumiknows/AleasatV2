import { Box, Typography } from "@mui/material";
import { Container } from "@mui/system";
import Image from "next/image";

export default async function Page() {
  return (
    <Container
      sx={{
        display: "flex",
        justifyContent: "space-between",
        alignItems: "center",
      }}
    >
      <Typography variant="h3">Profile Setup</Typography>

      <Box
        sx={{
          position: "fixed",
          bottom: "-300px",
          left: "-310px",
          zIndex: -1,
        }}
      >
        <img
          src="https://artifact.aleasat.space/dashboard/earth.png"
          width="1100"
          alt="image of earth"
          style={{ objectFit: "contain" }}
        />
      </Box>

      <Typography
        sx={{
          mt: "20px",
        }}
        variant="h3"
      >
        {" "}
        Form goes here
      </Typography>
    </Container>
  );
}
