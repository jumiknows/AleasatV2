"use client";

import Button from "@mui/material/Button";
import { useState } from "react";
import DeleteGroundStationForm from "./DeleteGroundStationForm";

export default function DeleteGroundStationButton({
  groundStationId,
}: {
  groundStationId: number;
}) {
  const [formOpen, setFormOpen] = useState(false);
  const [formKey, setFormKey] = useState(0);

  const handleClickOpen = () => {
    setFormOpen(true);
  };

  const handleClose = () => {
    setFormOpen(false);
    setFormKey((oldFormKey) => oldFormKey + 1);
  };

  return (
    <>
      <Button variant="contained" color="error" onClick={handleClickOpen}>
        Delete
      </Button>
      <DeleteGroundStationForm
        groundStationId={groundStationId}
        open={formOpen}
        handleClose={handleClose}
        key={formKey}
      />
    </>
  );
}
