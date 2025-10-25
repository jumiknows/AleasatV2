import { Box, Divider, Grid2, Typography } from "@mui/material";
import Image from "next/image";

interface SpacecraftComponentProps {
  name: string;
  description: string;
  image: string;
  specifications?: Record<string, string>;
}

export default function SpacecraftComponent({
  name,
  description,
  image,
  specifications,
}: SpacecraftComponentProps) {
  return (
    <Grid2
      container
      spacing={4}
      sx={{ my: { xs: 8, md: 12 } }}
      justifyContent="center"
      alignItems="center"
    >
      <Grid2 size={{ xs: 12, sm: 4 }} sx={{ overflow: "hidden" }}>
        <Box sx={{ position: "relative", width: "100%", aspectRatio: "1/1" }}>
          <Box
            sx={{
              position: "absolute",
              top: "50%",
              left: "50%",
              transform: "translate(-50%, -50%)",
              width: "160%",
              aspectRatio: "16/9",
            }}
          >
            <img
              src={image}
              alt={name}
              style={{
                width: "100%",
                height: "100%",
                objectFit: "cover",
                filter: "brightness(1.2)",
              }}
            />
            <img
              src="https://artifact.aleasat.space/space-components/new-same-position/base2.avif"
              alt="Base"
              style={{
                position: "absolute",
                top: 0,
                left: 0,
                width: "100%",
                height: "100%",
                objectFit: "cover",
                opacity: 0.2,
                filter: "saturate(0)",
              }}
            />
          </Box>
        </Box>
      </Grid2>
      
      <Grid2 size={{ xs: 12, sm: 8 }}>
        <Typography 
          variant="h2" 
          sx={{ 
            fontSize: "2rem",
            fontWeight: "bold",
            mb: 2,
          }}
        >
          {name}
        </Typography>
        
        <Typography 
          variant="body1" 
          sx={{ 
            fontSize: "1.2rem",
            mb: 4,
          }}
        >
          {description}
        </Typography>
        
        {specifications && (
          <>
            <Typography variant="body1" sx={{ mb: 1 }}>
              Specifications:
            </Typography>
            <Box sx={{ display: "grid", gap: 1 }}>
              {Object.entries(specifications).map(([key, value]) => (
                <Typography key={key} variant="body1">
                  <strong>{key}:</strong> {value}
                </Typography>
              ))}
            </Box>
          </>
        )}
      </Grid2>
    </Grid2>
  );
}
