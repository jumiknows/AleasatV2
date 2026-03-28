import FacebookIcon from "@mui/icons-material/Facebook";
import InstagramIcon from "@mui/icons-material/Instagram";
import LinkedIn from "@mui/icons-material/LinkedIn";
import type { SxProps, Theme } from "@mui/material";
import Box from "@mui/material/Box";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import type { StaticImageData } from "next/image";
import Image from "../components/Image";
import Link from "../components/Link";

const Logo = ({ 
  src, 
  alt, 
  sx 
}: { 
  src: string | StaticImageData; 
  alt: string;
  sx?: SxProps<Theme>;
}) => {
  return (
    <Image
      src={src}
      alt={alt}
      sx={{
        width: { xs: 100, sm: 150 },
        height: { xs: 100, sm: 120 },
        transformOrigin: "bottom",
        transform: { xs: "scale(.8)", sm: "unset" },
        ...sx,
      }}
    />
  );
};

export default function Footer({
  links = [
    { label: "About", path: "/about" },
    { label: "Cubesat", path: "/cubesat" },
    { label: "Team", path: "/team" },
    { label: "Sponsor", path: "/sponsor" },
  ],
}: {
  links?: Array<{ label: string; path: string }>;
}) {
  const SHOW_RESOURCES = true;

  const sectionLabelSx: SxProps<Theme> = {
    color: "rgba(255,255,255,0.62)",
    fontSize: { xs: 12, md: 13 },
    letterSpacing: "0.09em",
    textTransform: "uppercase",
    fontWeight: 700,
    mb: 1,
    textAlign: { xs: "left", md: "left" },
  };

  const navLinkSx: SxProps<Theme> = {
    color: "white",
    textDecoration: "none",
    fontSize: { xs: 15, md: 17 },
    lineHeight: 1.25,
    px: { xs: 0.2, md: 1.2 },
    py: { xs: 0.35, md: 0.6 },
    borderRadius: "8px",
    transition: "all 0.2s ease",
    "&:hover": {
      backgroundColor: "rgba(255,255,255,0.09)",
      textDecoration: "none",
    },
  };

  const footerLinks = [
    { label: "FAQ", path: "/faq" },
    { label: "Privacy Policy", path: "/privacy" },
    { label: "Terms of Service", path: "/terms" },
    { label: "Contact Us", path: "/contact" },
  ];

  return (
    <footer>
      <Box
        sx={{
          background: "linear-gradient(180deg, #050505 0%, #000000 100%)",
          color: "white",
          py: { xs: 4, md: 5 },
          px: { xs: 2, sm: 3, md: 8 },
        }}
      >
        {/* Main content */}
        <Box
          sx={{
            display: "flex",
            flexDirection: { xs: "column", md: "row" },
            justifyContent: "space-between",
            alignItems: { xs: "stretch", md: "flex-start" },
            gap: { xs: 3.5, md: 3 },
            mb: 4,
            minHeight: "120px",
            maxWidth: "1280px",
            mx: "auto",
          }}
        >
          {/* Left side - Navigation sections stacked */}
          <Box
            sx={{
              display: "flex",
              flexDirection: "column",
              gap: 2.5,
              order: { xs: 1, md: 1 },
              flex: 1,
              minWidth: 0,
            }}
          >
            {/* Top row - Footer Links */}
            {SHOW_RESOURCES && (
              <Box>
                <Typography sx={sectionLabelSx}>Resources</Typography>
                <Box
                  sx={{
                    display: { xs: "grid", sm: "flex" },
                    gridTemplateColumns: { xs: "repeat(2, minmax(0, 1fr))", sm: "none" },
                    columnGap: { xs: 2, sm: 0 },
                    rowGap: { xs: 0.7, sm: 0 },
                    flexWrap: { sm: "wrap" },
                    gap: { sm: 1.5 },
                    justifyContent: { xs: "flex-start", md: "flex-start" },
                  }}
                >
                  {footerLinks.map(({ label, path }) => (
                    <Link key={label} href={path} sx={navLinkSx}>
                      {label}
                    </Link>
                  ))}
                </Box>
              </Box>
            )}

            {/* Bottom row - Main Navigation with separators */}
            <Box>
              <Typography sx={sectionLabelSx}>Explore</Typography>
              <Box
                sx={{
                  display: { xs: "grid", sm: "flex" },
                  gridTemplateColumns: { xs: "repeat(2, minmax(0, 1fr))", sm: "none" },
                  columnGap: { xs: 2, sm: 0 },
                  rowGap: { xs: 0.7, sm: 0 },
                  flexWrap: { sm: "wrap" },
                  gap: { sm: 1.5 },
                  justifyContent: { xs: "flex-start", md: "flex-start" },
                }}
              >
                {links.map(({ label, path }) => (
                  <Link key={label} href={path} sx={navLinkSx}>
                    {label}
                  </Link>
                ))}
              </Box>
            </Box>
          </Box>

          {/* Right side - University Logos with Social Media */}
          <Box
            sx={{
              display: "flex",
              gap: { xs: 2.5, md: 6 },
              flexDirection: "row",
              alignItems: "center",
              justifyContent: { xs: "space-between", md: "flex-start" },
              order: { xs: 2, md: 2 },
              width: { xs: "100%", md: "auto" },
            }}
          >
            {/* UBC */}
            <Stack alignItems="center" spacing={1}>
              <Logo
                src="https://artifact.aleasat.space/logos/UBC_Orbit_Logo.png"
                alt="UBC Orbit Logo"
                sx={{
                  width: { xs: 80, md: 110 },
                  height: { xs: 70, md: 90 },
                }}
              />
              <Box
                sx={{
                  display: "flex",
                  gap: 0.5,
                }}
              >
                <Link
                  href="https://www.facebook.com/ubcorbit/"
                  target="_blank"
                  data-testid="facebook-link"
                >
                  <FacebookIcon
                    sx={{
                      color: "gray",
                      fontSize: { xs: 20, md: 24 },
                      "&:hover": { color: "white" },
                    }}
                  />
                </Link>
                <Link
                  href="https://www.instagram.com/ubcorbit/"
                  target="_blank"
                  data-testid="instagram-link"
                >
                  <InstagramIcon
                    sx={{
                      color: "gray",
                      fontSize: { xs: 20, md: 24 },
                      "&:hover": { color: "white" },
                    }}
                  />
                </Link>
                <Link
                  href="https://www.linkedin.com/company/ubc-orbit/"
                  target="_blank"
                  data-testid="linkedin-link"
                >
                  <LinkedIn
                    sx={{
                      color: "gray",
                      fontSize: { xs: 20, md: 24 },
                      "&:hover": { color: "white" },
                    }}
                  />
                </Link>
              </Box>
            </Stack>

            {/* SFU */}
            <Stack alignItems="center" spacing={1}>
              <Logo
                src="https://artifact.aleasat.space/logos/SFUSAT_Hamburger_Logo.png"
                alt="SFU Sat Logo"
                sx={{
                  width: { xs: 80, md: 110 },
                  height: { xs: 70, md: 90 },
                }}
              />
              <Box
                sx={{
                  display: "flex",
                  gap: 0.5,
                }}
              >
                <Link
                  href="https://www.facebook.com/sfu.satellite/"
                  target="_blank"
                >
                  <FacebookIcon
                    sx={{
                      color: "gray",
                      fontSize: { xs: 20, md: 24 },
                      "&:hover": { color: "white" },
                    }}
                  />
                </Link>
                <Link
                  href="https://www.instagram.com/sfu.satellite/"
                  target="_blank"
                >
                  <InstagramIcon
                    sx={{
                      color: "gray",
                      fontSize: { xs: 20, md: 24 },
                      "&:hover": { color: "white" },
                    }}
                  />
                </Link>
                <Link
                  href="https://www.linkedin.com/company/sfu-satellite-design/?originalSubdomain=ca"
                  target="_blank"
                >
                  <LinkedIn
                    sx={{
                      color: "gray",
                      fontSize: { xs: 20, md: 24 },
                      "&:hover": { color: "white" },
                    }}
                  />
                </Link>
              </Box>
            </Stack>
          </Box>
        </Box>

        {/* Copyright */}
        <Box
          sx={{
            textAlign: "center",
            pt: 2,
            borderTop: "1px solid #333",
            maxWidth: "1280px",
            mx: "auto",
          }}
        >
          <Typography
            sx={{
              fontSize: { xs: 14, md: 16 },
              color: "gray",
            }}
          >
            © 2026 UBC Orbit Design Team & SFU Satellite Design Team
          </Typography>
        </Box>
      </Box>
    </footer>
  );
}
