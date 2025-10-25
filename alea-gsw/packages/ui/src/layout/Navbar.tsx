"use client";

import { useMediaQuery, useTheme } from "@mui/material";
import AppBar from "@mui/material/AppBar";
import Box from "@mui/material/Box";
import IconButton from "@mui/material/IconButton";
import Toolbar from "@mui/material/Toolbar";
import Typography from "@mui/material/Typography";
import MenuIcon from "@mui/icons-material/Menu";
import { usePathname } from "next/navigation";
import { type ReactNode, useState } from "react";

import HiddenMenu from "../components/HiddenMenu";
import Image from "../components/Image";
import Link from "../components/Link";

const NavLink = ({ href, label, isActive }: { href: string; label: string; isActive: boolean }) => (
  <Link
    href={href}
    data-cy={`navbar-${label.toLowerCase().replace(/\s+/g, "-")}`}
    sx={{
      textDecoration: "none",
      color: isActive ? "white" : "rgba(255, 255, 255, 0.8)",
      position: "relative",
      px: 2,
      py: 1,
      borderRadius: "8px",
      backgroundColor: isActive ? "rgba(74, 144, 226, 0.2)" : "transparent",
      transition: "all 0.3s ease",
      "&:hover": {
        color: "white",
        backgroundColor: "rgba(74, 144, 226, 0.15)",
      },
      "&::after": isActive ? {
        content: '""',
        position: "absolute",
        bottom: -2,
        left: "50%",
        width: "80%",
        height: "2px",
        background: "linear-gradient(45deg, #4A90E2, #357ABD)",
        transform: "translateX(-50%)",
        borderRadius: "2px",
      } : {},
    }}
  >
    <Typography
      sx={{
        fontWeight: isActive ? 600 : 500,
        fontSize: "1rem",
      }}
    >
      {label}
    </Typography>
  </Link>
);

export default function Navbar({
  children,
  links,
}: {
  children?: ReactNode;
  links?: Array<{ label: string; path: string }>;
}) {
  const theme = useTheme();
  const isMobile = useMediaQuery(theme.breakpoints.down("md"));
  const [drawerOpen, setDrawerOpen] = useState(false);
  const pathname = usePathname();

  return (
    <AppBar
      position="sticky"
      sx={{ height: 70, background: "transparent", top: 0, zIndex: 1100 }}
      data-cy="navbar"
    >
      <Toolbar
        sx={{
          position: "relative",
          py: 3,
          display: "flex",
          alignItems: "center",
          height: "100%",
        }}
      >
        {/* Logo and Navigation Items - Left Side */}
        <Box
          sx={{
            display: "flex",
            alignItems: "center",
            flex: 1,
          }}
        >
          <Link href="/">
            <Image
              src="https://artifact.aleasat.space/logos/ALEASAT_Logo.png"
              alt="AleaSat Logo"
              sx={{
                objectFit: "contain",
                height: { xs: 50, sm: 70 },
                width: { xs: 100, sm: 150 },
                mr: 10,
              }}
            />
          </Link>
          
          {/* Navigation Items */}
          {!isMobile && (
            <Box
              sx={{
                display: "flex",
                gap: 4,
                alignItems: "center",
              }}
            >
              {links?.map(({ label, path }) => (
                <NavLink
                  key={label}
                  href={path}
                  label={label}
                  isActive={pathname === path}
                />
              ))}
              
              <NavLink
                href="/join-us"
                label="Join Us"
                isActive={pathname === "/join-us"}
              />
            </Box>
          )}
        </Box>
        
        {/* Right side menu */}
        <Box sx={{ display: "flex", alignItems: "center", gap: 2 }}>
          {!isMobile && children}
          <IconButton
            sx={{ color: "white" }}
            onClick={() => setDrawerOpen(!drawerOpen)}
            aria-label="menu"
          >
            <MenuIcon />
          </IconButton>
        </Box>
        
        {/* Hidden Menu */}
        {drawerOpen && (
          <HiddenMenu 
            links={links} 
            onClose={() => setDrawerOpen(false)} 
          />
        )}
      </Toolbar>
    </AppBar>
  );
}
