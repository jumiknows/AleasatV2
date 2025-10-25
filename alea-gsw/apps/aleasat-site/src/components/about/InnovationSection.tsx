import Image from "@aleasat/ui/components/Image";
import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import Typography from "@mui/material/Typography";

export default function InnovationSection() {
  return (
    <Box
      sx={{
        position: "relative",
        background: "rgba(30, 30, 30, 0.8)",
        backdropFilter: "blur(20px)",
        overflow: "hidden",
        py: { xs: "4rem", md: "8rem" },
      }}
    >
      <Box
        sx={{
          position: "absolute",
          inset: 0,
          background: `
            radial-gradient(4px 4px at 20px 30px, rgba(255, 255, 255, 0.8), transparent),
            radial-gradient(3px 3px at 40px 70px, rgba(255, 255, 255, 0.6), transparent),
            radial-gradient(3px 3px at 90px 40px, rgba(255, 255, 255, 0.7), transparent),
            radial-gradient(2px 2px at 130px 80px, rgba(255, 255, 255, 0.5), transparent),
            radial-gradient(5px 5px at 160px 30px, rgba(255, 255, 255, 0.9), transparent),
            radial-gradient(3px 3px at 200px 90px, rgba(255, 255, 255, 0.4), transparent),
            radial-gradient(2px 2px at 250px 50px, rgba(255, 255, 255, 0.6), transparent),
            radial-gradient(4px 4px at 300px 120px, rgba(255, 255, 255, 0.8), transparent)
          `,
          backgroundRepeat: "repeat",
          backgroundSize: "350px 150px",
          filter: "blur(0.5px)",
          opacity: 0.3,
        }}
      />

      <Box
        sx={{
          position: "absolute",
          top: "30%",
          left: "50%",
          transform: "translate(-50%, -50%)",
          width: "1200px",
          height: "300px",
          border: "1px solid rgba(192, 192, 192, 0.2)",
          borderRadius: "50%",
        }}
      />
      <Box
        sx={{
          position: "absolute",
          top: "30%",
          left: "50%",
          transform: "translate(-50%, -50%)",
          width: "1000px",
          height: "240px",
          border: "1px solid rgba(192, 192, 192, 0.36)",
          borderRadius: "50%",
        }}
      />
      <Box
        sx={{
          position: "absolute",
          top: "30%",
          left: "50%",
          transform: "translate(-50%, -50%)",
          width: "800px",
          height: "180px",
          border: "1px solid rgba(192, 192, 192, 0.5)",
          borderRadius: "50%",
        }}
      />

      <Container maxWidth="xl">
        <Box
          sx={{
            display: "flex",
            justifyContent: "center",
            alignItems: "center",
            gap: { xs: 2, md: 4 },
            mb: { xs: 4, md: 6 },
            flexWrap: "wrap",
          }}
        >
          {/* UBC Orbit Team photo */}
          <Box
            sx={{
              width: "500px",
              height: "300px",
              borderRadius: "30px",
              overflow: "hidden",
            }}
          >
            <Image
              src="/ubcorbit.png"
              alt="UBC Orbit Team"
              sx={{
                width: "100%",
                height: "100%",
                objectFit: "cover",
              }}
            />
          </Box>

          <Box
            sx={{
              width: "500px",
              height: "300px",
              borderRadius: "30px",
              overflow: "hidden",
            }}
          >
            <Image
              src="/sfusat.png"
              alt="SFU Satellite Design Team"
              sx={{
                width: "100%",
                height: "100%",
                objectFit: "cover",
              }}
            />
          </Box>
        </Box>

        <Box sx={{ textAlign: "center", maxWidth: "800px", mx: "auto" }}>
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
            <Box
              component="span"
              sx={{
                background: "linear-gradient(45deg, #ffffffff, #ffffffff)",
                backgroundClip: "text",
                WebkitBackgroundClip: "text",
                color: "transparent",
              }}
            >
              INNOVATION
            </Box>{" "}
            THAT<br />
            MAKES AN IMPACT
          </Typography>
          
                    <Typography
            variant="body1"
            sx={{
              color: "#b6bdd6",
              maxWidth: "600px",
              mx: "auto",
              lineHeight: 1.6,
              fontWeight: 300,
              mb: 4,
            }}
          >
            UBC Orbit Team and SFU Satellite Design Team work to
            create an advanced satellite technology
          </Typography>

          <Box
            sx={{
              display: "flex",
              justifyContent: "center",
              mt: { xs: 3, md: 4 },
            }}
          >
            <Box
              sx={{
                width: "20px",
                height: "20px",
                borderRight: "3px solid rgba(128, 128, 128, 0.8)",
                borderBottom: "3px solid rgba(128, 128, 128, 0.8)",
                transform: "rotate(45deg)",
              }}
            />
          </Box>
        </Box>
      </Container>
    </Box>
  );
}