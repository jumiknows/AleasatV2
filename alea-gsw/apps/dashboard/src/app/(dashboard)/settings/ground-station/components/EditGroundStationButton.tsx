"use client";

import Button from "@mui/material/Button";
import { useState } from "react";
import EditGroundStationForm from "./EditGroundStationForm";

export default function EditGroundStationButton({
  groundStation,
}: {
  groundStation: {
    id: number;
    name: string;
    description: string;
    lat: number;
    lng: number;
    minElevation: number;
  };
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
      <Button variant="outlined" color="success" onClick={handleClickOpen}>
        Edit
      </Button>
      <EditGroundStationForm
        groundStation={groundStation}
        open={formOpen}
        handleClose={handleClose}
        key={formKey}
      />
    </>
  );
}
