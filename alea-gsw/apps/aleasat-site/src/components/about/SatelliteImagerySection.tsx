import Image from "@aleasat/ui/components/Image";
import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import Typography from "@mui/material/Typography";

export default function SatelliteImagerySection() {
  return (
    <Box
      sx={{
        position: "relative",
        background: "#000000",
        overflow: "hidden",
        py: { xs: "4rem", md: "8rem" },
      }}
    >
      <Container maxWidth="xl">
        <Box
          sx={{
            display: "flex",
            alignItems: "center",
            gap: { xs: 4, md: 8 },
            flexDirection: { xs: "column", md: "row" },
          }}
        >
          <Box sx={{ flex: 1, textAlign: { xs: "center", md: "left" } }}>
            <Typography
              variant="h2"
              sx={{
                color: "white",
                fontWeight: "bold",
                letterSpacing: "0.05em",
                mb: 3,
                lineHeight: 1.1,
              }}
            >
              EXPLORE OUR<br />
              <Box
                component="span"
                sx={{
                  background: "linear-gradient(45deg, #4A90E2, #7BB3F0)",
                  backgroundClip: "text",
                  WebkitBackgroundClip: "text",
                  color: "transparent",
                }}
              >
                SATELLITE IMAGERY
              </Box>
            </Typography>
            
            <Typography
              variant="body1"
              sx={{
                color: "#ffffffff",
                maxWidth: "500px",
                mx: { xs: "auto", md: 0 },
                lineHeight: 1.6,
                fontWeight: 300,
              }}
            >
              We request imagery of a specific location on Earth, and then downlink
              that imagery directly from the satellite with an on-board camera
            </Typography>
          </Box>

          <Box
            sx={{
              flex: 1,
              display: "flex",
              justifyContent: "center",
              position: "relative",
            }}
          >
            <Box
              sx={{
                width: { xs: "350px", md: "450px" },
                height: { xs: "300px", md: "350px" },
                borderRadius: "12px",
                overflow: "hidden",
                boxShadow: "0 20px 60px rgba(0, 0, 0, 0.5)",
              }}
            >
              <Image
                src="/Satellite-map-view.png"
                alt="Satellite Map View"
                sx={{
                  width: "100%",
                  height: "100%",
                  objectFit: "cover",
                }}
              />
            </Box>
          </Box>
        </Box>

        {/* Bottom half with map1.png */}
        <Box
          sx={{
            display: "flex",
            justifyContent: "center",
            mt: { xs: 6, md: 8 },
            pt: { xs: 4, md: 6 },
          }}
        >
          {/* <Box
            sx={{
              maxWidth: { xs: "90%", sm: "80%", md: "70%" },
              borderRadius: "12px",
              overflow: "hidden",
              boxShadow: "0 20px 60px rgba(0, 0, 0, 0.5)",
            }}
          >
            <Image
              src="/map1.png"
              alt="Satellite Map"
              sx={{
                width: "50%",
                height: "auto",
                objectFit: "cover",
                margin: "0 auto",
                display: "block",
              }}
            />
          </Box> */}
        </Box>
      </Container>
    </Box>
  );
}