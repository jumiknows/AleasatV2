"use client";

import { Button, Typography, useMediaQuery, useTheme } from "@mui/material";
import AppBar from "@mui/material/AppBar";
import Box from "@mui/material/Box";
import IconButton from "@mui/material/IconButton";
import List from "@mui/material/List";
import ListItem from "@mui/material/ListItem";
import ListItemText from "@mui/material/ListItemText";
import Toolbar from "@mui/material/Toolbar";
import MenuIcon from "@mui/icons-material/Menu";

import { usePathname } from "next/navigation";
import { type ReactNode, useState } from "react";

import HiddenMenu from "../components/HiddenMenu";
import Image from "../components/Image";
import Link from "../components/Link";

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
      position="static"
      sx={{ height: 70, background: "#000" }}
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
          
          {/* Navigation Items - Right beside logo */}
          {!isMobile && (
            <Box
              sx={{
                display: "flex",
                gap: "4rem",
                alignItems: "center",
              }}
            >
              {links?.map(({ label, path }) => {
                const isSelected = pathname === path;
                
                if (isSelected) {
                  return (
                    <Button
                      key={label}
                      href={path}
                      variant="contained"
                      data-cy={`navbar-${label}`}
                      data-link={label}
                      sx={{
                        background: "linear-gradient(45deg, #4A90E2, #357ABD)",
                        color: "white",
                        borderRadius: "18px",
                        typography: "body1",
                        textTransform: "none",
                        transition: "all 0.3s ease",
                        px: 3,
                        py: 1,
                        "&:hover": {
                          background: "linear-gradient(45deg, #5BA3F5, #4A90E2)",
                          textTransform: "none",
                        },
                      }}
                    >
                      {label}
                    </Button>
                  );
                }
                
                return (
                  <Link
                    key={label}
                    href={path}
                    data-cy={`navbar-${label}`}
                    data-link={label}
                    sx={{
                      textDecoration: "none",
                      color: "#b6bdd6",
                      position: "relative",
                      textTransform: "none",
                      "&:hover": {
                        color: "white",
                        fontWeight: "bold",
                        textTransform: "none",
                      },
                    }}
                  >
                    <Typography
                      color="inherit"
                      sx={{ 
                        fontWeight: "normal",
                        transition: "all 0.3s ease",
                        textTransform: "none",
                      }}
                    >
                      {label}
                    </Typography>
                  </Link>
                );
              })}
              
              {/* Join Us as regular nav item */}
              {(() => {
                const isSelected = pathname === "/join-us";
                
                if (isSelected) {
                  return (
                    <Button
                      href="/join-us"
                      variant="contained"
                      data-cy="navbar-join-us"
                      sx={{
                        background: "linear-gradient(45deg, #4A90E2, #357ABD)",
                        color: "white",
                        borderRadius: "15px",
                        typography: "body1",
                        textTransform: "none",
                        boxShadow: "0 4px 20px rgba(74, 144, 226, 0.3)",
                        transition: "all 0.3s ease",
                        px: 3,
                        py: 1,
                        "&:hover": {
                          background: "linear-gradient(45deg, #5BA3F5, #4A90E2)",
                          boxShadow: "0 4px 15px rgba(74, 144, 226, 0.4)",
                          textTransform: "none",
                        },
                      }}
                    >
                      Join Us
                    </Button>
                  );
                }
                
                return (
                  <Link
                    href="/join-us"
                    data-cy="navbar-join-us"
                    sx={{
                      textDecoration: "none",
                      color: "#b6bdd6",
                      position: "relative",
                      textTransform: "none",
                      "&:hover": {
                        color: "white",
                        fontWeight: "bold",
                        textTransform: "none",
                      },
                    }}
                  >
                    <Typography
                      color="inherit"
                      sx={{ 
                        fontWeight: "normal",
                        transition: "all 0.3s ease",
                        textTransform: "none",
                      }}
                    >
                      Join Us
                    </Typography>
                  </Link>
                );
              })()}
            </Box>
          )}
        </Box>
        
        {/* Mobile Menu and Icons */}
        {isMobile ? (
          <>
            <IconButton
              sx={{ zIndex: 99, position: "relative" }}
              edge="end"
              aria-label="menu"
              onClick={() => setDrawerOpen(!drawerOpen)}
            >
              <MenuIcon sx={{ color: "white" }} />
            </IconButton>
            {drawerOpen && (
              <HiddenMenu 
                links={links} 
                onClose={() => setDrawerOpen(false)} 
              />
            )}
          </>
        ) : (
          /* Icons on the right for desktop - including hamburger menu */
          <Box sx={{ display: "flex", alignItems: "center", gap: 2 }}>
            {children}
            <IconButton
              sx={{ color: "white" }}
              onClick={() => setDrawerOpen(!drawerOpen)}
            >
              <MenuIcon />
            </IconButton>
            {drawerOpen && (
              <HiddenMenu 
                links={links} 
                onClose={() => setDrawerOpen(false)} 
              />
            )}
          </Box>
        )}
      </Toolbar>
    </AppBar>
  );
}
