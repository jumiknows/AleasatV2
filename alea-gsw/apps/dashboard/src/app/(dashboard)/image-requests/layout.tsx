import { PageContainer } from "@toolpad/core";

export default function Layout({ children }: { children: React.ReactNode }) {
  return <PageContainer maxWidth={false}>{children}</PageContainer>;
}
