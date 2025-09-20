"use client";

import Button from "@mui/material/Button";
import { useState } from "react";
import AddGroundStationForm from "./AddGroundStationForm";

export default function AddGroundStationButton() {
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
      <Button variant="contained" onClick={handleClickOpen}>
        Add Ground Station
      </Button>
      <AddGroundStationForm
        open={formOpen}
        handleClose={handleClose}
        key={formKey}
      />
    </>
  );
}
