"use client";

import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import Grid from "@mui/material/Grid2";
import Paper from "@mui/material/Paper";
import Button from "@mui/material/Button";
import Image from "@aleasat/ui/components/Image";

interface SponsorItemProps {
  src: string;
  size: { xs: number; sm: number };
  taller?: boolean;
}

function SponsorItem({ src, size, taller = false }: SponsorItemProps) {
  return (
    <Grid size={size}>
      <Paper
        elevation={0}
        sx={{
          borderRadius: 0,
          position: "relative",
          width: "100%",
          height: { xs: 150, sm: 180 },
          textAlign: "center",
          background: "transparent",
          display: "flex",
          alignItems: "center",
          justifyContent: "center",
          minHeight: { xs: 150, sm: 180 },
        }}
      >
        <Box
          sx={{
            width: "100%",
            height: "100%",
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
            p: { xs: 1, sm: 2 },
          }}
        >
          <Image
            src={src}
            alt={src}
            sx={{
              height: { xs: 100, sm: 130 },
              width: { xs: 180, sm: 220 },
              objectFit: "contain",
            }}
          />
        </Box>
      </Paper>
    </Grid>
  );
}

export default function FeaturedSponsorships() {
  return (
    <Box
      sx={{
        position: "relative",
        background: "#000",
        py: { xs: 8, md: 12 },
        width: "100%",
      }}
    >
      <Box
        sx={{
          px: {
            xs: 4,
            sm: 6,
            md: 8,
          },
          maxWidth: 1200,
          mx: "auto",
          textAlign: "left",
        }}
      >
        <Typography
          component="h1"
          variant="h3"
          sx={{ 
            mb: 3, 
            fontWeight: "bold",
            color: "white",
            fontSize: { xs: "2rem", md: "3rem" }
          }}
        >
          FEATURED SPONSORSHIPS
        </Typography>
        <Typography 
          component="h2" 
          variant="h6" 
          sx={{ 
            mb: 8, 
            color: "white",
            opacity: 0.9 
          }}
        >
          This project would not be possible without our amazing sponsors!
        </Typography>
      </Box>
      
      <Box
        sx={{
          mx: "auto",
          maxWidth: 1000,
          px: { xs: 2, md: 4 },
        }}
      >
        <Grid
          container
          spacing={{ xs: 2, sm: 3, md: 4 }}
          alignItems="stretch"
          justifyContent="center"
          sx={{ 
            minHeight: { xs: 400, sm: 500 },
            "& > *": {
              display: "flex",
              alignItems: "stretch",
            }
          }}
        >
          {/* Top row - 3 sponsors */}
          <SponsorItem
            size={{ xs: 12, sm: 4 }}
            src="https://artifact.aleasat.space/sponsors/triumf.svg"
          />
          <SponsorItem
            size={{ xs: 12, sm: 4 }}
            src="https://artifact.aleasat.space/sponsors/AGI.png"
          />
          <SponsorItem
            size={{ xs: 12, sm: 4 }}
            src="https://artifact.aleasat.space/sponsors/rsl.svg"
          />
          
          {/* Middle row - 3 sponsors evenly spaced */}
          <SponsorItem
            size={{ xs: 12, sm: 4 }}
            src="https://artifact.aleasat.space/sponsors/sfsu.png"
          />
          <SponsorItem
            size={{ xs: 12, sm: 4 }}
            src="https://artifact.aleasat.space/sponsors/ebco.svg"
          />
          <SponsorItem
            size={{ xs: 12, sm: 4 }}
            src="https://artifact.aleasat.space/sponsors/csa.png"
          />
        </Grid>
        
        {/* Sponsors button */}
        <Box sx={{ mt: 6, display: "flex", justifyContent: "center" }}>
          <Button
            variant="outlined"
            sx={{
              color: "#b6bdd6",
              borderColor: "#b6bdd6",
              borderWidth: 2,
              borderRadius: "25px",
              px: { xs: 3, md: 4 },
              py: { xs: 1, md: 1.2 },
              textTransform: "none",
              transition: "all 0.3s ease",
              "&:hover": {
                color: "white",
                borderColor: "white",
              },
            }}
            href="/sponsor"
          >
            Sponsors
          </Button>
        </Box>
      </Box>
    </Box>
  );
}
