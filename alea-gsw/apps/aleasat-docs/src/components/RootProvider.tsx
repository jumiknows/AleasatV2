"use client";

import Image from "@aleasat/ui/components/Image";
import theme from "@aleasat/ui/theme/theme";
import { AppRouterCacheProvider } from "@mui/material-nextjs/v13-appRouter";
import { AdapterDayjs } from "@mui/x-date-pickers/AdapterDayjs";
import { LocalizationProvider } from "@mui/x-date-pickers/LocalizationProvider";
import type { Navigation } from "@toolpad/core";
import { NextAppProvider } from "@toolpad/core/nextjs";
import { Provider, createStore } from "jotai";
import type { Session } from "next-auth";
import { SessionProvider, signIn, signOut } from "next-auth/react";
import { useReportWebVitals } from "next/web-vitals";
import type { ReactNode } from "react";
import ForceReAuth from "./ForceReAuth";

const store = createStore();

export default function RootProvider({
  children,
  session,
}: {
  children: ReactNode;
  session: Session | null | undefined;
}) {
  useReportWebVitals((metric) => {
    process.env.NEXT_PUBLIC_G_TAG_ID &&
      window.gtag("event", metric.name, {
        value: Math.round(
          // eslint-disable-next-line @typescript-eslint/no-unsafe-argument
          metric.name === "CLS" ? metric.value * 1000 : metric.value,
        ), // values must be integers
        event_label: metric.id, // id unique to current page load
        non_interaction: true, // avoids affecting bounce rate.
      });
  });

  return (
    <Provider store={store}>
      <SessionProvider refetchInterval={60} refetchOnWindowFocus>
        <ForceReAuth />
        <LocalizationProvider dateAdapter={AdapterDayjs}>
          <AppRouterCacheProvider options={{ enableCssLayer: true }}>
            <NextAppProvider
              branding={{
                logo: (
                  <Image
                    src="https://artifact.aleasat.space/logos/ALEASAT_Logo.png"
                    alt="AleaSat Logo"
                    sx={{
                      objectFit: "contain",
                      height: { xs: 50, sm: 70 },
                      width: { xs: 100, sm: 150 },
                    }}
                  />
                ),
                title: "Docs",
              }}
              theme={theme}
              session={session}
              authentication={{
                signIn: async () => await signIn("keycloak"),
                signOut,
              }}
            >
              {children}
            </NextAppProvider>
          </AppRouterCacheProvider>
        </LocalizationProvider>
      </SessionProvider>
    </Provider>
  );
}
