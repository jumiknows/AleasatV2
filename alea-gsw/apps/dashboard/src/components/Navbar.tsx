import ImageBox from "@aleasat/ui/components/Image";
import Link from "@aleasat/ui/components/Link";
import DensityMediumIcon from "@mui/icons-material/DensityMedium";
import { Box, Paper } from "@mui/material";
import Grid from "@mui/material/Grid2";
import CustomizedMenus from "./Menu";

const links = [
  { label: "Request", link: "/" },
  { label: "Gallery", link: "/" },
  { label: "Ground Stations", link: "/" },
  { label: "Support", link: "/" },
];

export default function Navbar() {
  return (
    <Grid
      container
      sx={{
        position: "absolute",
        top: 0,
        width: "100%",
        zIndex: 100,
        mt: 3,
        justifyContent: "space-between",
        textAlign: "center",
      }}
    >
      <Grid size={2}>
        <ImageBox
          src={"https://artifact.aleasat.space/logos/ALEASAT_Logo.png"}
          alt=""
          height={"3rem"}
        />
      </Grid>
      <Grid size={4}>
        <Paper
          variant="glass"
          sx={{
            width: "100%",
            height: "100%",
            display: "flex",
            justifyContent: "space-around",
            alignItems: "center",
            px: 5,
            borderRadius: "1.2rem",
          }}
        >
          {links.map((item, idx) => {
            return (
              <Link
                key={idx}
                href={item.link}
                sx={{ textDecoration: "none", color: "primary.light" }}
              >
                {item.label}
              </Link>
            );
          })}
        </Paper>
      </Grid>
      <Grid size={{ xs: 0, md: 3 }} />
      <Grid size={2}>
        <CustomizedMenus />
      </Grid>
      <Grid size={1}>
        <DensityMediumIcon fontSize="large" sx={{ height: "100%" }} />
      </Grid>
    </Grid>
  );
}
