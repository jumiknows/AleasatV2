import Link from "@aleasat/ui/components/Link";
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Typography from "@mui/material/Typography";
import Image from "next/image";

export const dynamic = "force-dynamic";

export default async function HeroNoImg() {
  return (
    <Box>
      <Box sx={{ position: "relative", width: "100%", height: "93vh" }}>
        <Image
          src={"https://artifact.aleasat.space/hero.png"}
          fill
          style={{ objectFit: "cover" }}
          alt="hero image"
        />
        <Box
          sx={{
            position: "absolute",
            top: "50%",
            left: "30%",
            transform: "translate(-50%, -50%)",
            width: "30%",
          }}
        ></Box>
        <Box
          sx={{
            position: "absolute",
            top: "70%",
            left: "25%",
            transform: "translate(-50%, -50%)",
          }}
        ></Box>
        <Box
          sx={{
            position: "relative",
            top: "35%",
            left: "35%",
            transform: "translate(-50%, -50%)",
            width: "50%",
          }}
        >
          <Typography
            variant="h3"
            sx={{
              fontWeight: "500",
            }}
          >
            Get ready to capture images straight from space!
          </Typography>
          <Typography variant="h6" sx={{ width: "50%" }}>
            Send a request to our satellite to receive your first image
          </Typography>
          <Button
            sx={{
              position: "relative",
              top: "100%",
              left: "0%",
              transform: "translate(0%, 30%)",
            }}
            variant="outlined"
            size="large"
            color="secondary"
            LinkComponent={Link}
            href="/image-requests/new-request"
          >
            Send Image Request
          </Button>
        </Box>
      </Box>
    </Box>
  );
}
