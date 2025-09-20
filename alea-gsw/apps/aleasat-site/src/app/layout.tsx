import RootProvider from "@/components/providers";
import Footer from "@aleasat/ui/layout/Footer";
import Navbar from "@aleasat/ui/layout/Navbar";
import Box from "@mui/material/Box";
import { GoogleAnalytics } from "@next/third-parties/google";
import type { Metadata } from "next";
import { PublicEnvScript } from "next-runtime-env";

export const metadata: Metadata = {
  metadataBase: new URL("https://www.aleasat.space"),
  title: "ALEASAT",
  description: "ALEASAT Home Page",
  openGraph: {
    title: "ALEASAT",
    description: "ALEASAT Home Page",
    images:
      "https://artifact.aleasat.space/logos/aleasat-render-over-earth-96x96.png",
  },
};

const links = [
  {
    label: "Spacecraft",
    path: "/spaceship",
  },
  {
    label: "Team",
    path: "/team",
  },
  {
    label: "Sponsor",
    path: "/sponsor",
  },
];

export default async function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <head>
        <PublicEnvScript />
      </head>
      <body suppressHydrationWarning={true}>
        <RootProvider>
          <Navbar links={links} />
          <Box>{children}</Box>
          <Footer data-testid="footer" links={links} />
        </RootProvider>
      </body>
      {process.env.NEXT_PUBLIC_G_TAG_ID && (
        <GoogleAnalytics gaId={process.env.NEXT_PUBLIC_G_TAG_ID} />
      )}
    </html>
  );
}
