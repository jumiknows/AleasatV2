import { auth } from "@/auth";
import RootProvider from "@/components/RootProvider";
import Box from "@mui/material/Box";
import { GoogleAnalytics } from "@next/third-parties/google";
import type { Metadata } from "next";
import { PublicEnvScript } from "next-runtime-env";

export const metadata: Metadata = {
  title: "ALEASAT Docs",
  description:
    "The documentation for the ALEASAT API, including the OpenAPI specification.",
  openGraph: {
    images:
      "https://artifact.aleasat.space/logos/aleasat-render-over-earth-96x96.png",
  },
};

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
              overflowX: "hidden",
              height: "100vh",
              overflowY: "hidden",
            }}
          >
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
