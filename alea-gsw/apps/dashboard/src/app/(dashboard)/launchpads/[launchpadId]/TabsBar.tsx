"use client";

import { Tab, Tabs } from "@mui/material";
import Link from "next/link";
import { usePathname } from "next/navigation";

export default function TabsBar({ launchpadId }: { launchpadId: string }) {
  const pathname = usePathname();

  const tabs = [
    { path: "", label: "Details" },
    { path: "/update", label: "Update" },
  ];
  const path = `/launchpads/${launchpadId}`;

  const index = tabs.findIndex((tab) => pathname === `${path}${tab.path}`);

  return (
    <Tabs
      value={index === -1 ? false : index}
      sx={{ borderBottom: 1, borderColor: "divider", marginBottom: 5 }}
    >
      {tabs.map((tab) => (
        <Tab
          key={tab.path}
          label={tab.label}
          href={`${path}${tab.path}`}
          LinkComponent={Link}
        />
      ))}
    </Tabs>
  );
}
