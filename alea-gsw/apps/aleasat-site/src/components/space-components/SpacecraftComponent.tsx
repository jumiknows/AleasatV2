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
    <>
      <Grid2
        container
        spacing={2}
        sx={{ my: 20 }}
        justifyContent="center"
        alignItems="center"
      >
        <Grid2 size={{ xs: 12, sm: 3 }} sx={{ overflow: "hidden" }}>
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
              <Image
                src={image}
                alt={name}
                fill
                sizes="(min-width: 1536px) 614.4px, (min-width: 600px) 40vw, 100vw"
                style={{
                  filter: "brightness(1.2)",
                }}
              />
              <Image
                src={
                  "https://artifact.aleasat.space/space-components/new-same-position/base2.avif"
                }
                alt={name}
                fill
                sizes="(min-width: 1536px) 614.4px, (min-width: 600px) 40vw, 100vw"
                style={{
                  opacity: 0.2,
                  filter: "saturate(0)",
                }}
              />
            </Box>
          </Box>
        </Grid2>
        <Grid2 size={{ xs: 12, sm: 9 }}>
          <Typography variant="h2" sx={{ fontSize: "2rem", mb: 2 }}>
            {name}
          </Typography>
          <Typography variant="body1" sx={{ fontSize: "1.2rem", mb: 6 }}>
            {description}
          </Typography>
          {specifications && (
            <>
              <Typography variant="body1">Specifications:</Typography>
              <Divider sx={{ my: 0.5, borderColor: "#fff" }} />
              <Grid2
                container
                spacing={{
                  xs: 0.5,
                  sm: 1,
                }}
              >
                {Object.entries(specifications).map(([key, value]) => (
                  <Grid2 key={key} size={{ xs: 12, md: 6 }}>
                    <Typography variant="body1">
                      {key}: {value}
                    </Typography>
                  </Grid2>
                ))}
              </Grid2>
            </>
          )}
        </Grid2>
      </Grid2>
    </>
  );
}
