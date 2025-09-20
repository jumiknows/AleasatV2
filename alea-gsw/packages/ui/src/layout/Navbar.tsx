"use client";

import { Button, Typography, useMediaQuery, useTheme } from "@mui/material";
import AppBar from "@mui/material/AppBar";
import Box from "@mui/material/Box";
import Drawer from "@mui/material/Drawer";
import IconButton from "@mui/material/IconButton";
import List from "@mui/material/List";
import ListItem from "@mui/material/ListItem";
import ListItemText from "@mui/material/ListItemText";
import Toolbar from "@mui/material/Toolbar";

import { usePathname } from "next/navigation";
import { type ReactNode, useState } from "react";

import Hamburger from "../components/Hamburger";
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

  const JoinBtn = () => (
    <Button
      variant="contained"
      href="/join-us"
      sx={{ bgcolor: "#333136", color: "white" }}
    >
      Join Us!
    </Button>
  );

  const NavDrawer = () => (
    <Box
      sx={{
        pt: 8,
        width: 200,
        background: "#000",
        height: "100%",
        zIndex: 10,
      }}
      role="presentation"
      onClick={() => setDrawerOpen(false)}
    >
      <List>
        {links?.map(({ label, path }) => (
          <ListItem key={label} component={Link} href={path}>
            <ListItemText
              primary={label}
              sx={{
                textAlign: "start",
                color: "white",
                my: 1,
                fontWeight: pathname === path ? "bold" : "normal",
              }}
            />
          </ListItem>
        ))}
        <ListItem>
          <JoinBtn />
        </ListItem>
      </List>
    </Box>
  );

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
          justifyContent: "space-between",
          height: "100%",
        }}
      >
        <Box
          sx={{
            height: "100%",
            display: "flex",
            alignItems: "center",
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
              }}
            />
          </Link>
        </Box>
        {isMobile ? (
          <>
            <IconButton
              sx={{ zIndex: 99, position: "relative" }}
              edge="end"
              aria-label="menu"
              onClick={() => setDrawerOpen(!drawerOpen)}
            >
              <Hamburger active={drawerOpen} />
            </IconButton>
            <Drawer
              sx={{ positiove: "relative", zIndex: 10 }}
              anchor="right"
              open={drawerOpen}
              onClose={() => setDrawerOpen(!drawerOpen)}
            >
              <NavDrawer />
            </Drawer>
          </>
        ) : (
          <Box
            sx={{
              display: "flex",
              gap: "5rem",
              alignItems: "center",
            }}
          >
            {links?.map(({ label, path }) => (
              <Link
                key={label}
                href={path}
                data-cy={`navbar-${label}`}
                data-link={label}
                sx={{
                  textDecoration: "none",
                  color: "white",
                  position: "relative",
                  "&:hover": {
                    color: "primary.main",
                    "&:before": {
                      transform: "scaleX(1)",
                      transformOrigin: "left",
                    },
                  },
                  "&:before": {
                    content: '""',
                    position: "absolute",
                    borderRadius: 2,
                    width: "100%",
                    height: "3px",
                    bottom: "-6px",
                    left: 1,
                    backgroundColor: "primary.main",
                    transform: "scaleX(0)",
                    transformOrigin: "left",
                    transition: "transform 0.2s ease-in-out",
                  },
                }}
              >
                <Typography
                  color="inherit"
                  sx={{ fontWeight: pathname === path ? "bold" : "normal" }}
                >
                  {label}
                </Typography>
              </Link>
            ))}
            <JoinBtn />
          </Box>
        )}
        {children}
      </Toolbar>
    </AppBar>
  );
}
