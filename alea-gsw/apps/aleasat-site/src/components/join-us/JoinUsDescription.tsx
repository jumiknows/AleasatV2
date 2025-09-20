import { Box, Card, CardContent, Grid, Typography } from "@mui/material";
import Image from "next/image";

export default function JoinUsDescription() {
  return (
    <Box
      sx={{
        backgroundColor: "#000",
        color: "#fff",
        display: "flex",
        justifyContent: "center",
        alignItems: "center",
        mt: "10%",
        mx: { xs: "8%", md: "12%" },
        mb: { xs: "0%", md: "8%" },
      }}
    >
      <Grid container spacing={1}>
        {/* Left side with images */}
        <Grid item xs={12} md={6}>
          <Box
            sx={{
              position: "relative",
              display: "flex",
              flexDirection: { xs: "column", md: "row" },
              width: "100%",
              gap: { xs: 2, md: 4 },
            }}
          >
            {/* First image (UBC Orbit) */}
            <Box
              sx={{
                position: "relative",
                width: { xs: "70%", md: "60%" },
                height: "auto",
                transform: { md: "translateY(5%)" },
                zIndex: 1,
              }}
            >
              <Image
                src="https://artifact.aleasat.space/join-us-images/join-us-orbit-image.png"
                alt="UBC Orbit Team"
                layout="responsive"
                width={420}
                height={420}
                style={{
                  borderRadius: "20px",
                  boxShadow: "0px 4px 10px rgba(0, 0, 0, 1.0)",
                }}
              />
            </Box>
            {/* Second image (SFU SAT) */}
            <Box
              sx={{
                position: "relative",
                width: { xs: "50%", md: "40%" },
                height: "auto",
                transform: {
                  xs: "translateY(-80%) translateX(100%)",
                  md: "translateY(50%) translateX(-50%)",
                },
              }}
            >
              <Image
                src="https://artifact.aleasat.space/join-us-images/join-us-sat-image.png"
                alt="SFU SAT Team"
                layout="responsive"
                width={311}
                height={313}
                style={{ borderRadius: "20px" }}
              />
            </Box>
          </Box>
        </Grid>

        {/* Right side with text and logos */}
        <Grid item xs={12} md={6}>
          <Box
            sx={{
              position: "relative",
              display: "flex",
              flexDirection: "column",
              gap: { xs: 3, md: 8 },
            }}
          >
            {/* UBC Orbit logo and description */}
            <Box
              sx={{
                position: "relative",
                mt: { xs: "-60%", md: "-20%" },
                mb: "-6%",
              }}
            >
              <Box
                sx={{
                  backgroundColor: "#000",
                  position: "relative",
                  transform: {
                    xs: "translateX(70%) translateY(85%)",
                    md: "translateX(30%) translateY(45%)",
                  },
                  width: { xs: "40%", md: "33%" },
                  height: "auto",
                  zIndex: 2,
                }}
              >
                <Image
                  src="https://artifact.aleasat.space/logos/UBC_Orbit_Logo.png"
                  alt="UBC Orbit Logo"
                  layout="responsive"
                  width={255}
                  height={144}
                />
              </Box>
              <Card
                sx={{
                  textAlign: "center",
                  backgroundColor: "#000",
                  border: 1,
                  borderRadius: 0,
                  borderColor: "#8E959B",
                  boxShadow: "none",
                  marginTop: { xs: "30px", md: "0px" },
                  width: "100%",
                  height: "100%",
                }}
              >
                <CardContent
                  sx={{
                    backgroundColor: "#000",
                    width: "100%",
                    height: "100%",
                    textAlign: "center",
                  }}
                >
                  <Typography
                    variant="body1"
                    sx={{
                      px: { xs: "4%", md: "8%" },
                      pt: { xs: "12%", md: "10%" },
                      pb: { xs: "0%", md: "4%" },
                      fontSize: { xs: "13px", md: "23px" },
                      textAlign: { xs: "center", md: "left" },
                    }}
                  >
                    The UBC Orbit team consists of 90+ students and is dedicated
                    to providing students with an opportunity to design and
                    develop satellites.
                  </Typography>
                </CardContent>
              </Card>
            </Box>

            {/* SFU SAT logo and description */}
            <Box sx={{ position: "relative", mt: "-6%" }}>
              <Box
                sx={{
                  backgroundColor: "#000",
                  position: "relative",
                  transform: {
                    xs: "translateX(75%) translateY(100%)",
                    md: "translateX(170%) translateY(50%)",
                  },
                  width: { xs: "40%", md: "33%" },
                  height: "auto",
                  zIndex: 2,
                }}
              >
                <Image
                  src="https://artifact.aleasat.space/logos/SFUSAT_Hamburger_Logo.png" // Replace with correct path
                  alt="SFU SAT Logo"
                  layout="responsive"
                  width={238}
                  height={121}
                />
              </Box>
              <Card
                sx={{
                  backgroundColor: "#000",
                  border: 1,
                  borderRadius: 0,
                  borderColor: "#8E959B",
                  boxShadow: "none",
                  marginTop: { xs: "30px", md: "0px" }, // to ensure it aligns well on mobile
                }}
              >
                <CardContent
                  sx={{
                    backgroundColor: "#000",
                    textAlign: "center",
                  }}
                >
                  <Typography
                    variant="body1"
                    sx={{
                      px: { xs: "4%", md: "8%" },
                      pt: { xs: "12%", md: "10%" },
                      pb: { xs: "0%", md: "4%" },
                      fontSize: { xs: "13px", md: "23px" },
                      textAlign: { xs: "center", md: "left" },
                    }}
                  >
                    The SFU Satellite Design team provides education and
                    training for future engineers who are interested in the
                    space industry.
                  </Typography>
                </CardContent>
              </Card>
            </Box>
          </Box>
        </Grid>
      </Grid>
    </Box>
  );
}
