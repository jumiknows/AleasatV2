import Link from "@aleasat/ui/components/Link";
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Typography from "@mui/material/Typography";
import Image from "next/image";

export default function Telemetry({ imageExists }: { imageExists: boolean }) {
  return (
    <Box sx={{ position: "relative", width: "100%", aspectRatio: "32/9" }}>
      <Image
        src={
          imageExists
            ? "https://artifact.aleasat.space/telemetry_2.png"
            : "https://artifact.aleasat.space/hero2.png"
        }
        fill
        style={{ objectFit: "fill" }}
        alt="aleasat"
      />
      <Box
        sx={{
          position: "absolute",
          top: "50%",
          left: "80%",
          transform: "translate(-50%, -50%)",
        }}
      >
        <Box>
          <Typography
            variant="h3"
            sx={{
              width: "100%",
              transform: "translate(0%, -10%)",
              fontWeight: "500",
            }}
          >
            Telemetry and Data
          </Typography>
          <Button
            sx={{ top: "50%" }}
            variant="outlined"
            color="secondary"
            size="large"
            LinkComponent={Link}
            href="/telemetry"
          >
            View Data!
          </Button>
        </Box>
      </Box>
    </Box>
  );
}
