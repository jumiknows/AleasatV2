"use client";
import { Box, Button, Paper, Typography } from "@mui/material";

const imageData: {
  title: string;
  date: string;
  time: string;
  location: string;
  coords: string;
  image: string;
}[] = [
  {
    title: "The Image Name",
    date: "January 9, 2023",
    time: "7:35pm",
    location: "Vancouver, BC",
    coords: "(56.09 N, 127.12 E)",
    image: "https://artifact.aleasat.space/team-photo/team-photo.avif",
  },
  {
    title: "The Image Name 2",
    date: "January 9, 2023",
    time: "7:35pm",
    location: "Vancouver, BC",
    coords: "(56.09 N, 127.12 E)",
    image: "https://artifact.aleasat.space/team-photo/team-photo.avif",
  },
  {
    title: "The Image Name 3",
    date: "January 9, 2023",
    time: "7:35pm",
    location: "Vancouver, BC",
    coords: "(56.09 N, 127.12 E)",
    image: "https://artifact.aleasat.space/team-photo/team-photo.avif ",
  },
];

const Gallery = () => {
  return (
    <Box
      sx={{
        position: "relative",
        width: "100%",
        display: "flex",
        justifyContent: "center",
      }}
    >
      <Box
        sx={{
          position: "relative",
          width: "100%",
          maxWidth: "90rem",
          backgroundColor: "#3a3a3a",
          clipPath:
            "polygon(0% 5%, 62% 5%, 65% 0%, 100% 0%, 100% 90%, 94% 100%, 0% 100%, 0% 5%)",
          padding: "0.05rem",
          boxSizing: "border-box",
          borderRadius: "1rem",
        }}
      >
        <Box
          sx={{
            backgroundColor: "#121212",
            clipPath:
              "polygon(0% 5%, 62% 5%, 65% 0%, 100% 0%, 100% 90%, 94% 100%, 0% 100%, 0% 5%)",
            borderRadius: "1rem",
            color: "#fff",
            padding: "1.5rem",
            boxSizing: "border-box",
          }}
        >
          <Typography
            variant="h3"
            fontWeight="bold"
            gutterBottom
            sx={{ mt: "2rem" }}
          >
            GALLERY
          </Typography>

          <Box
            sx={{
              width: "100%",
              height: "0.05rem",
              backgroundColor: "#3a3a3a",
              marginBottom: "1rem",
            }}
          />

          <Box
            sx={{
              backgroundColor: "#1A1A1A",
              p: "0.5rem",
              borderRadius: "0.5rem",
            }}
          >
            <Typography variant="p" gutterBottom>
              View the latest satellite images captured by ALEASAT. Click any
              image for full details.
            </Typography>
          </Box>

          <Box
            sx={{
              display: "flex",
              justifyContent: "space-around",
              flexWrap: "wrap",
              gap: "1rem",
              mt: "1.5rem",
            }}
          >
            {imageData.length > 0 ? (
              imageData.map((img, idx) => (
                <Box
                  key={idx}
                  sx={{
                    borderRadius: "1rem",
                    overflow: "hidden",
                    width: "100%",
                    height: "19rem",
                    maxWidth: "20rem",
                    flexGrow: 1,
                    position: "relative",
                  }}
                >
                  <Box
                    sx={{
                      backgroundImage: `url(${img.image})`,
                      backgroundSize: "cover",
                      backgroundPosition: "center",
                      height: "100%",
                      width: "100%",
                    }}
                  />
                  <Paper
                    variant="glass"
                    sx={{
                      position: "absolute",
                      bottom: 0,
                      width: "100%",
                      p: "0.75rem",
                      boxSizing: "border-box",
                      borderRadius: "1rem",
                    }}
                  >
                    <Typography variant="p" fontWeight="bold">
                      {img.title}
                    </Typography>
                    <Typography variant="body2">
                      {img.location} {img.coords}
                    </Typography>
                    <Typography
                      variant="caption"
                      sx={{
                        display: "block",
                        textAlign: "right",
                        mt: "0.5rem",
                      }}
                    >
                      {img.date} {img.time}
                    </Typography>
                  </Paper>
                </Box>
              ))
            ) : (
              <>Empty</>
            )}
          </Box>

          <Box mt={"3rem"} display="flex" justifyContent="center">
            <Button
              variant="contained"
              color="primary"
              sx={{
                px: "2.5rem",
                py: "1rem",
                borderRadius: "1rem",
              }}
            >
              View full gallery
            </Button>
          </Box>
        </Box>
      </Box>
    </Box>
  );
};

export default Gallery;
