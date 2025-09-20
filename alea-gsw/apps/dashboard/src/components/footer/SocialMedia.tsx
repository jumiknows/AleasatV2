import Image from "@aleasat/ui/components/Image";
import Link from "@aleasat/ui/components/Link";
import FacebookIcon from "@mui/icons-material/Facebook";
import InstagramIcon from "@mui/icons-material/Instagram";
import LinkedIn from "@mui/icons-material/LinkedIn";
import Box from "@mui/material/Box";
import Stack from "@mui/material/Stack";
import type { StaticImageData } from "next/image";

const Logo = ({ src, alt }: { src: string | StaticImageData; alt: string }) => {
  return (
    <Image
      src={src}
      alt={alt}
      sx={{
        width: { xs: 100, sm: 130 },
        height: { xs: 100, sm: 120 },
        transformOrigin: "bottom",
        transform: { xs: "scale(.8)", sm: "unset" },
      }}
    />
  );
};

const SocialMedia = (props: { isMobile: boolean }) => {
  const { isMobile } = props;
  return (
    <Box
      sx={{
        width: "100%",
        height: "100%",
        justifyContent: { xs: "center", sm: "space-evenly", md: "end" },
        alignItems: "center",
        display: isMobile
          ? { xs: "flex", md: "none" }
          : { xs: "none", md: "flex" },
      }}
    >
      {/* UBC */}
      <Stack>
        <Logo
          src="https://artifact.aleasat.space/logos/UBC_Orbit_Logo.png"
          alt="UBC Orbit Logo"
        />
        <Box
          sx={{
            display: "flex",
            justifyContent: "space-between",
          }}
        >
          <Link
            href="https://www.facebook.com/ubcorbit/"
            target="_blank"
            data-testid="facebook-link"
          >
            <FacebookIcon
              sx={{ color: "gray", fontSize: { xs: 27, sm: 35 } }}
            />
          </Link>
          <Link
            href="https://www.instagram.com/ubcorbit/"
            target="_blank"
            data-testid="instagram-link"
          >
            <InstagramIcon
              sx={{ color: "gray", fontSize: { xs: 27, sm: 35 } }}
            />
          </Link>
          <Link
            href="https://www.linkedin.com/company/ubc-orbit/"
            target="_blank"
            data-testid="linkedin-link"
          >
            <LinkedIn sx={{ color: "gray", fontSize: { xs: 27, sm: 35 } }} />
          </Link>
        </Box>
      </Stack>
      {/* SFU */}
      <Stack marginLeft={{ md: "5rem" }}>
        <Logo
          src="https://artifact.aleasat.space/logos/SFUSAT_Hamburger_Logo.png"
          alt="SFU Sat Logo"
        />
        <Box
          sx={{
            display: "flex",
            justifyContent: "space-between",
          }}
        >
          <Link href="https://www.facebook.com/sfu.satellite/" target="_blank">
            <FacebookIcon
              sx={{ color: "gray", fontSize: { xs: 27, sm: 35 } }}
            />
          </Link>
          <Link href="https://www.instagram.com/sfu.satellite/" target="_blank">
            <InstagramIcon
              sx={{ color: "gray", fontSize: { xs: 27, sm: 35 } }}
            />
          </Link>
          <Link
            href="https://www.linkedin.com/company/sfu-satellite-design/?originalSubdomain=ca"
            target="_blank"
          >
            <LinkedIn sx={{ color: "gray", fontSize: { xs: 27, sm: 35 } }} />
          </Link>
        </Box>
      </Stack>
    </Box>
  );
};

export default SocialMedia;
