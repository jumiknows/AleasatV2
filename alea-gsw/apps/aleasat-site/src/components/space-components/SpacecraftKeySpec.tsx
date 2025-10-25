import Image from "@aleasat/ui/components/Image";
import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import Typography from "@mui/material/Typography";

const tableData = [
  {
    key: "Payload",
    value: "Earth-imaging payload for disaster relief training purposes",
  },
  {
    key: "Ground Station(s)",
    value: "UHF band located at UBC Vancouver",
  },
  {
    key: "Dimensions",
    value: `Stowed: 100.0mm x 100.0mm x 113.5mm\nDeployed: 389.2mm x 253.9mm x 113.5mm`,
  },
  {
    key: "Mass",
    value: "1.035 kg",
  },
  {
    key: "Power consumption",
    value: "Idle: ±1.2W",
  },
  {
    key: "Solar panel",
    value: "4x Fixed 1U Solar Panel",
  },
  {
    key: "Mission Duration",
    value: "1-2 years",
  },
];

export default function SpacecraftKeySpec() {
  return (
    <Box sx={{ py: { xs: "4rem", md: "8rem" } }}>
      <Container maxWidth="md">
        <Box
          sx={{
            position: "relative",
            width: "100%",
            aspectRatio: "1200/776",
            mb: 6,
          }}
        >
          <Image
            src="https://artifact.aleasat.space/space-components/aleasat-drawing.svg"
            alt="Outline drawing of ALEASAT"
            sx={{
              width: "100%",
              height: "100%",
              objectFit: "contain",
              filter: "invert(1)",
            }}
          />
        </Box>
        
        <Typography
          variant="h2"
          sx={{
            textAlign: "center",
            mb: 2,
            fontWeight: "bold",
            letterSpacing: "0.05em",
          }}
        >
          ALEASAT
        </Typography>
        
        <Typography
          variant="h5"
          component="h2"
          sx={{
            textAlign: "center",
            mb: 6,
          }}
        >
          Key Specifications
        </Typography>
        <Box sx={{ display: "grid", gap: 3 }}>
          {tableData.map((row, index) => (
            <Box
              key={index}
              sx={{
                display: "flex",
                flexDirection: { xs: "column", sm: "row" },
                gap: { xs: 1, sm: 4 },
                py: 3,
                borderBottom: "1px solid rgba(255, 255, 255, 0.1)",
                "&:last-child": {
                  borderBottom: "none",
                },
              }}
            >
              <Typography
                variant="body1"
                sx={{
                  fontWeight: "bold",
                  minWidth: { sm: "200px" },
                  color: "white",
                }}
              >
                {row.key}
              </Typography>
              <Typography
                variant="body1"
                sx={{
                  color: "rgba(255, 255, 255, 0.8)",
                  whiteSpace: "pre-line",
                  lineHeight: 1.6,
                }}
              >
                {row.value}
              </Typography>
            </Box>
          ))}
        </Box>
      </Container>
    </Box>
  );
}
