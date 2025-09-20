"use client";

import ThemeRegistry from "@aleasat/ui/theme/ThemeRegistry";
import { Provider, createStore } from "jotai";
import { useReportWebVitals } from "next/web-vitals";
import type { ReactNode } from "react";

const store = createStore();

export default function RootProvider({ children }: { children: ReactNode }) {
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
    <ThemeRegistry>
      <Provider store={store}>{children}</Provider>
    </ThemeRegistry>
  );
}
