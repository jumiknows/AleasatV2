import { auth } from "@/auth";
import Navbar from "@/components/Navbar";
import RootProvider from "@/components/RootProvider";
import ThemeRegistry from "@aleasat/ui/theme/ThemeRegistry";
import theme from "@aleasat/ui/theme/theme";
import AccountBoxIcon from "@mui/icons-material/AccountBox";
import AddPhotoAlternateIcon from "@mui/icons-material/AddPhotoAlternate";
import BurstModeIcon from "@mui/icons-material/BurstMode";
import DeveloperBoardIcon from "@mui/icons-material/DeveloperBoard";
import PlaylistAddIcon from "@mui/icons-material/PlaylistAdd";
import SettingsInputAntennaIcon from "@mui/icons-material/SettingsInputAntenna";
// import ListIcon from "@mui/icons-material/List";
import ViewInArIcon from "@mui/icons-material/ViewInAr";
import { ThemeProvider } from "@mui/material";
import Box from "@mui/material/Box";
import { GoogleAnalytics } from "@next/third-parties/google";
import type { Navigation } from "@toolpad/core";
import type { Metadata } from "next";
import { PublicEnvScript } from "next-runtime-env";

export const metadata: Metadata = {
  title: "ALEASAT Dashboard",
  description:
    "A dashboard of the user's current and recurring image requests.",
};

// const navigation: Navigation = [
//   {
//     kind: "header",
//     title: "Image Request",
//   },
//   {
//     segment: "image-requests",
//     title: "All image Requests",
//     icon: <BurstModeIcon />,
//   },
//   {
//     segment: "image-requests/new-request",
//     title: "New image Requests",
//     icon: <AddPhotoAlternateIcon />,
//   },
//   {
//     kind: "header",
//     title: "Mission Control",
//   },
//   // {
//   //   segment: "missions",
//   //   pattern: "missions/:missionId",
//   //   title: "Missions",
//   //   icon: <ListIcon />,
//   // },
//   {
//     segment: "missions/mission-planning",
//     title: "Mission Planning",
//     icon: <PlaylistAddIcon />,
//   },
//   {
//     kind: "header",
//     title: "ALEA Web Services",
//   },
//   {
//     segment: "launchpads",
//     title: "Launchpads",
//     icon: <DeveloperBoardIcon />,
//     pattern: "launchpads{/:segment}*",
//   },
//   {
//     kind: "header",
//     title: "ALEASIM",
//   },
//   {
//     segment: "aleasim",
//     title: "ALEASIM Viewer",
//     icon: <ViewInArIcon />,
//   },
//   {
//     kind: "header",
//     title: "Settings",
//   },
//   {
//     segment: "settings/profile",
//     title: "Profile",
//     icon: <AccountBoxIcon />,
//   },
//   {
//     segment: "settings/ground-station",
//     title: "Ground Stations",
//     icon: <SettingsInputAntennaIcon />,
//   },
//   {
//     segment: "dev-tools",
//     title: "Dev Tools",
//     icon: <DeveloperBoardIcon />,
//   },
// ];

export default async function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  const session = await auth();

  return (
    <html lang="en">
      <head>
        <PublicEnvScript />
      </head>
      <body suppressHydrationWarning={true}>
        <RootProvider session={session}>
          <Box
            className="scrollContainer"
            sx={{
              width: "100vw",
              height: "100vh",
              overflowX: "hidden",
              overflowY: "scroll",
            }}
          >
            <Navbar />
            {children}
          </Box>
        </RootProvider>
      </body>
      {process.env.NEXT_PUBLIC_G_TAG_ID && (
        <GoogleAnalytics gaId={process.env.NEXT_PUBLIC_G_TAG_ID} />
      )}
    </html>
  );
}
