import Footer from "@aleasat/ui/layout/Footer";
import { DashboardLayout } from "@toolpad/core/DashboardLayout";
import type * as React from "react";

export default function DashboardPagesLayout(props: {
  children: React.ReactNode;
}) {
  return <DashboardLayout hideNavigation>{props.children}</DashboardLayout>;
}
