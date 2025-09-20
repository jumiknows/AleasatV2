import "server-only";

import Box from "@mui/material/Box";
import Stepper from "./components/Stepper";

export default async function NewImageRequestLayout({
  children, // will be a page or nested layout
}: {
  children: React.ReactNode;
}) {
  return (
    <Box style={{ height: "90vh", marginTop: "3vh" }}>
      <Stepper />
      {children}
    </Box>
  );
}
