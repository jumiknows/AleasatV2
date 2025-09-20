import Image from "@aleasat/ui/components/Image";
import Link from "@aleasat/ui/components/Link";
import Box from "@mui/material/Box";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";

export default function Description() {
  return (
    <Box
      sx={{
        position: "relative",
        zIndex: 2,
        background: "#000",
        height: "100vh",
        py: { xs: "3rem", md: "unset" },
        display: "flex",
        flexDirection: { xs: "column", md: "row" },
        justifyContent: { xs: "space-evenly", md: "center" },
        alignItems: "center",
      }}
      id="next-section"
    >
      <Image
        data-testid="description-image"
        src="https://artifact.aleasat.space/description_image.png"
        alt="Description Image"
        sx={{
          width: { xs: 250, sm: 350, md: 390, lg: 450 },
          height: { xs: 250, sm: 350, md: 390, lg: 450 },
          my: "1rem",
          mx: 3,
          transform: "scaleX(-1)",
        }}
      />
      <Stack
        sx={{
          width: { sm: "30rem", lg: "35rem" },
          mx: { xs: 3, sm: 4, md: 5 },
        }}
      >
        <Typography data-testid="text-image" fontSize={{ sm: 20, lg: 26 }}>
          ALEASAT is a 1U Earth Observation CubeSat that can be used by radio
          amateurs to support disaster relief operations. With support from UBC,
          the Radio Amateurs of Canada, and the Education Office of the European
          Space Agency under the educational Fly Your Satellite! Programme, we
          are in the process of building and testing our satellite with an aim
          to launch it in a few years
        </Typography>
        <Link
          data-testid="learn-more-link"
          href="/"
          sx={{
            width: "fit-content",
            marginTop: "1rem",
            display: "flex",
            textDecoration: "none",
            "&:hover": {
              ".text": { textDecoration: "underline" },
              img: { transform: "translateX(30px)" },
            },
          }}
        >
          <Typography
            className="text"
            fontSize={{ xs: 20, sm: 25 }}
            fontWeight="bold"
            color="#82909B"
          >
            Learn More
          </Typography>
          <Image
            src="https://artifact.aleasat.space/arrow.svg"
            alt="arrow"
            width={{ xs: 50, md: 60 }}
            sx={{ ml: 3 }}
          />
        </Link>
      </Stack>
    </Box>
  );
}
