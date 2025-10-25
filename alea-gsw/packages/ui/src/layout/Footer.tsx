import FacebookIcon from "@mui/icons-material/Facebook";
import InstagramIcon from "@mui/icons-material/Instagram";
import LinkedIn from "@mui/icons-material/LinkedIn";
import type { SxProps, Theme } from "@mui/material";
import Box from "@mui/material/Box";
import Grid from "@mui/material/Grid2";
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
          background: "#000",
          color: "white",
          py: { xs: 3, md: 4 },
          px: { xs: 2, md: 8 },
        }}
      >
        {/* Main content */}
        <Box
          sx={{
            display: "flex",
            flexDirection: { xs: "column", md: "row" },
            justifyContent: "space-between",
            alignItems: { xs: "flex-start", md: "flex-start" },
            gap: { xs: 4, md: 2 },
            mb: 3,
            minHeight: "120px",
          }}
        >
          {/* Left side - Navigation sections stacked */}
          <Box
            sx={{
              display: "flex",
              flexDirection: "column",
              gap: 3,
              order: { xs: 1, md: 1 },
              flex: 1,
            }}
          >
            {/* Top row - Footer Links */}
            <Box
              sx={{
                display: "flex",
                flexDirection: { xs: "column", sm: "row" },
                gap: { xs: 2, sm: 6 },
                alignItems: { xs: "flex-start", md: "flex-start" },
              }}
            >
              {footerLinks.map(({ label, path }) => (
                <Link
                  key={label}
                  href={path}
                  sx={{
                    color: "white",
                    textDecoration: "none",
                    fontSize: { xs: 16, md: 18 },
                    "&:hover": {
                      textDecoration: "underline",
                    },
                  }}
                >
                  {label}
                </Link>
              ))}
            </Box>

            {/* Bottom row - Main Navigation with separators */}
            <Box
              sx={{
                display: "flex",
                flexDirection: { xs: "column", sm: "row" },
                gap: { xs: 2, sm: 0 },
                alignItems: { xs: "flex-start", md: "flex-start" },
              }}
            >
              {links.map(({ label, path }, index) => (
                <Box key={label} sx={{ display: "flex", alignItems: "center" }}>
                  <Link
                    href={path}
                    sx={{
                      color: "white",
                      textDecoration: "none",
                      fontSize: { xs: 16, md: 18 },
                      "&:hover": {
                        textDecoration: "underline",
                      },
                    }}
                  >
                    {label}
                  </Link>
                  {index < links.length - 1 && (
                    <Typography
                      sx={{
                        mx: 3,
                        color: "gray",
                        display: { xs: "none", sm: "block" },
                      }}
                    >
                      |
                    </Typography>
                  )}
                </Box>
              ))}
            </Box>
          </Box>

          {/* Right side - University Logos with Social Media */}
          <Box
            sx={{
              display: "flex",
              gap: { xs: 4, md: 6 },
              flexDirection: { xs: "column", sm: "row" },
              alignItems: "center",
              order: { xs: 2, md: 2 },
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
          }}
        >
          <Typography
            sx={{
              fontSize: { xs: 14, md: 16 },
              color: "gray",
            }}
          >
            © 2025 UBC Orbit Design Team & SFU Satellite Design Team
          </Typography>
        </Box>
      </Box>
    </footer>
  );
}
