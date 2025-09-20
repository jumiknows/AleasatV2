"use client";
import { Box, Container, Typography } from "@mui/material";
import { motion } from "framer-motion";
import Image from "next/image";
export default function SubteamDescription(subteam: {
  name: string;
  description: string;
  imageUrl: string;
  iconUrl: string;
  sx?: object;
}) {
  return (
    <Container
      sx={{
        position: "relative",
        width: "100%",
        padding: 0,
        ...subteam.sx,
      }}
      maxWidth="md"
      component={motion.div}
      initial={{ opacity: 0.1 }}
      animate={{ opacity: 1 }}
      exit={{ opacity: 0.1 }}
      transition={{ duration: 0.3, ease: "easeInOut" }}
    >
      <Box
        sx={{
          border: "solid 2px rgba(130, 136, 155, 0.4)",
          width: { xs: "100%", sm: "60%" },
          left: { xs: "0%", sm: "0" },
          aspectRatio: "3/2",
          position: "relative",
        }}
      >
        <Image
          src={
            subteam.imageUrl
              ? subteam.imageUrl
              : "https://artifact.aleasat.space/team-photo/team-photo.jpg"
          }
          alt={`Picture of ${subteam.name} team`}
          fill
          style={{ objectFit: "cover" }}
        />
      </Box>
      <Box
        sx={{
          width: "100%",
          aspectRatio: { xs: "2/1", sm: "12/1" },
          position: "relative",
        }}
      ></Box>
      <Box
        sx={{
          width: { xs: "80%", sm: "auto" },
          height: "auto",
          py: { xs: 1, sm: 2 },
          position: "absolute",
          border: "solid 1.5px rgba(130, 136, 155, 0.4)",
          top: { xs: "48%", sm: "20%" },
          left: { xs: "10%", sm: "45%" },
          backgroundColor: "rgba(30,30,30,0.8)",
          backdropFilter: "blur(10px)",
        }}
      >
        <Box sx={{ width: "100%", position: "relative" }}>
          <Box
            sx={{
              position: "absolute",
              transform: "translate(10%, -70%)",
              width: "20%",
              aspectRatio: "1/1",
              borderRadius: "50%",
              left: { xs: "40%", sm: "70%" },
              backgroundColor: "rgba(30, 30, 30, 1)",
              backdropFilter: "blur(10px)",
              border: "solid 1.5px rgba(130, 136, 155, 0.4)",
            }}
          >
            <Box
              sx={{
                top: "50%",
                left: "50%",
                transform: "translate(-50%, -50%)",
                width: "80%",
                aspectRatio: "1/1",
                position: "relative",
              }}
            >
              <Image
                src={
                  subteam.iconUrl
                    ? subteam.iconUrl
                    : "https://artifact.aleasat.space/icon.png"
                }
                alt={`Icon of ${subteam.name} team`}
                fill
              />
            </Box>
          </Box>
        </Box>
        <Box
          sx={{
            width: "100%",
            height: "100%",
            padding: "0 1rem",
          }}
        >
          <Box sx={{ width: "100%", mt: 2 }}>
            <Typography
              variant="h4"
              component="h2"
              sx={{
                fontSize: { xs: "5vw", sm: "4vw", md: "h2.fontSize" },
                fontWeight: "bold",
                my: { xs: 1, sm: 2 },
                textAlign: { xs: "center", sm: "start" },
              }}
            >
              {subteam.name}
            </Typography>
          </Box>
          <Typography
            sx={{
              fontSize: { xs: "2.6vw", sm: "1.75vw", md: "body1.fontSize" },
              textAlign: { xs: "center", sm: "start" },
              textWrap: "pretty",
            }}
          >
            {subteam.description}
          </Typography>
        </Box>
      </Box>
    </Container>
  );
}
