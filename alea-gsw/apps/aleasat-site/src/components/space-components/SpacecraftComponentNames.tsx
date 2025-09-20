import Typography from "@mui/material/Typography";
import { Stack, type SxProps } from "@mui/system";

export default function SpacecraftComponentNames() {
  const componentTextSxProps: SxProps = {
    fontSize: {
      xs: "1rem",
      sm: "h4.fontSize",
      lg: "h3.fontSize",
    },
    fontWeight: 200,
  };

  return (
    <>
      <Stack
        sx={{
          width: {
            xs: "50vw",
            md: "30vw",
          },
          ml: {
            xs: "56%",
            md: "70%",
          },
          "@media (max-aspect-ratio: 16/9)": {
            height: "47.5vh",
            pb: "5vh",
          },
          "@media (min-aspect-ratio: 16/9)": {
            height: "26.5vw",
            pb: "2vw",
          },
        }}
        direction="column"
        justifyContent="space-evenly"
      >
        <Typography sx={componentTextSxProps}>
          Antenna Deployer Module
        </Typography>
        <Typography sx={componentTextSxProps}>Comms Board</Typography>
        <Typography sx={componentTextSxProps}>Reaction Wheel</Typography>
        <Typography sx={componentTextSxProps}>Camera</Typography>
      </Stack>
      <Stack
        sx={{
          width: {
            xs: "50vw",
            md: "30vw",
          },
          ml: {
            xs: "56%",
            md: "70%",
          },
          "@media (max-aspect-ratio: 16/9)": {
            height: "30vh",
          },
          "@media (min-aspect-ratio: 16/9)": {
            height: "16.5vw",
          },
        }}
        direction="column"
        justifyContent="space-between"
      >
        <Typography
          sx={{
            ...componentTextSxProps,
            "@media (max-aspect-ratio: 16/9)": {
              pb: "7vh",
            },
            "@media (min-aspect-ratio: 16/9)": {
              pb: "3vw",
            },
          }}
        >
          Solar Pannel & structure
        </Typography>
        <Typography sx={componentTextSxProps}>On-Board Computer</Typography>
        <Typography sx={componentTextSxProps}>
          Electrical Power Supply
        </Typography>
      </Stack>
    </>
  );
}
