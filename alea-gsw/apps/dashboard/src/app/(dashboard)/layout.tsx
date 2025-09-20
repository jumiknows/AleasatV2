import Footer from "@/components/footer";
import { Box } from "@mui/material";
import type * as React from "react";

export default function DashboardPagesLayout(props: {
  children: React.ReactNode;
}) {
  const links = [
    {
      label: "About",
      path: "/",
    },
    {
      label: "CUBESAT",
      path: "/",
    },
    {
      label: "Team",
      path: "/",
    },
    {
      label: "Sponsor",
      path: "/",
    },
  ];

  const infoLinks = [
    {
      label: "FAQ",
      path: "/",
    },
    {
      label: "Privacy Policy",
      path: "/",
    },
    {
      label: "Terms of Service",
      path: "/",
    },
    {
      label: "Contact Us",
      path: "/",
    },
  ];

  return (
    <Box>
      {/* <DashboardLayout>
        {props.children}
      </DashboardLayout> */}
      {props.children}
      <Footer links={links} infoLinks={infoLinks} />
    </Box>
  );
}
