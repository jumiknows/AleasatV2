import Link from "@aleasat/ui/components/Link";
import { Divider, type SxProps, type Theme } from "@mui/material";
import Box from "@mui/material/Box";
import Grid from "@mui/material/Grid2";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import SocialMedia from "./SocialMedia";

const NavLinks = ({
  links,
}: {
  sx?: SxProps<Theme>;
  links?: Array<{ label: string; path: string }>;
}) => {
  return (
    <Box
      sx={{
        display: "flex",
        justifyContent: "space-between",
        width: { xs: "100%", md: "70%", lg: "50%" },
        justifySelf: { xs: "center", md: "start" },
        flexDirection: { xs: "column", md: "row" },
        alignItems: { xs: "center", md: "flex-start" },
        gap: { xs: 2, md: 0 },
      }}
    >
      {links?.map(({ label, path }) => (
        <Link data-testid="footer" key={label} href={path}>
          <Typography variant="p" color="white">
            {label}
          </Typography>
        </Link>
      ))}
    </Box>
  );
};

// component for nav to faq, privacy policy etc.
const NavInfoLinks = ({
  links,
}: {
  sx?: SxProps<Theme>;
  links?: Array<{ label: string; path: string }>;
}) => {
  return (
    <Stack
      direction={{ xs: "column", md: "row" }}
      divider={
        <Divider
          orientation={"vertical"}
          flexItem
          sx={{ borderColor: "#fff", display: { xs: "none", md: "block" } }}
        />
      }
      spacing={{ xs: 1, md: 1 }}
      sx={{
        justifyContent: "space-between",
        width: { xs: "100%", md: "95%", lg: "80%", xl: "50%" },
        alignItems: { xs: "center", md: "flex-start" },
      }}
    >
      {links?.map(({ label, path }) => (
        <Link data-testid="footer" key={label} href={path} display="contents">
          <Typography
            sx={{
              color: "white",
              fontSize: { xs: "0.7rem", md: "1rem" },
              textAlign: { xs: "center", md: "left" },
            }}
          >
            {label}
          </Typography>
        </Link>
      ))}
    </Stack>
  );
};

export default function Footer({
  links,
  infoLinks,
}: {
  links?: Array<{ label: string; path: string }>;
  infoLinks?: Array<{ label: string; path: string }>;
}) {
  return (
    <Box component={"footer"} sx={{ height: "15rem" }}>
      <Grid
        container
        sx={{
          position: "relative",
          background: "#000",
          width: "100%",
          pt: { xs: "5rem", md: "1rem" },
          pb: { xs: "5rem", md: "1rem" },
          height: { md: "fit-content" },
          alignItems: "center",
          px: "5rem",
        }}
      >
        <Grid size={{ xs: 12, md: 6 }} sx={{ height: "100%" }}>
          <Stack
            sx={{
              height: "100%",
            }}
            spacing={5}
          >
            <NavLinks links={links} />
            <Stack spacing={1}>
              <NavInfoLinks links={infoLinks} />
              <SocialMedia isMobile={true} />
              <Typography
                sx={{
                  textAlign: { xs: "center", md: "start" },
                  fontSize: { xs: 12, md: 14, lg: "0.7rem" },
                  width: { xs: "100%", md: "unset" },
                }}
                color="#C3D6E5"
              >
                © 2024 UBC Orbit Design Team & SFU Satellite Design Team
              </Typography>
            </Stack>
          </Stack>
        </Grid>
        <Grid
          size={6}
          sx={{ height: "100%", display: { xs: "none", md: "unset" } }}
        >
          <SocialMedia isMobile={false} />
        </Grid>
      </Grid>
    </Box>
  );
}
