"use client";

import Button from "@mui/material/Button";
import Dialog from "@mui/material/Dialog";
import DialogActions from "@mui/material/DialogActions";
import DialogContent from "@mui/material/DialogContent";
import DialogContentText from "@mui/material/DialogContentText";
import DialogTitle from "@mui/material/DialogTitle";
import Stack from "@mui/material/Stack";
import TextField from "@mui/material/TextField";
import Typography from "@mui/material/Typography";
import { useEffect } from "react";
import { useFormState, useFormStatus } from "react-dom";
import { addGroundStation } from "../actions";

function SubmitButton() {
  const { pending } = useFormStatus();

  return (
    <Button type="submit" disabled={pending}>
      Add
    </Button>
  );
}

export default function AddGroundStationForm({
  open,
  handleClose,
}: {
  open: boolean;
  handleClose?: () => void;
}) {
  const [state, formAction] = useFormState(addGroundStation, {
    errorMessage: "",
    success: false,
  });
  useEffect(() => {
    if (state.success) handleClose!();
  }, [state.success]);

  return (
    <Dialog
      open={open}
      onClose={handleClose}
      PaperProps={{
        component: "form",
        action: formAction,
      }}
    >
      <DialogTitle>Add New Ground Station</DialogTitle>
      <DialogContent>
        <DialogContentText>
          To subscribe to this website, please enter your email address here. We
          will send updates occasionally.
        </DialogContentText>
        <TextField
          required
          margin="dense"
          name="name"
          label="Name"
          variant="standard"
        />
        <TextField
          required
          margin="dense"
          name="description"
          label="Description"
          fullWidth
          variant="standard"
        />
        <Stack direction="row" spacing={4}>
          <TextField
            required
            margin="dense"
            name="latitude"
            label="Latitude"
            type="number"
            inputProps={{
              maxLength: 4,
              step: 0.1,
            }}
            variant="standard"
          />
          <TextField
            required
            margin="dense"
            name="longitude"
            label="Longitude"
            type="number"
            inputProps={{
              maxLength: 4,
              step: 0.1,
            }}
            variant="standard"
          />
        </Stack>
        <TextField
          required
          margin="dense"
          name="minElevation"
          label="Minimum Elevation"
          type="number"
          inputProps={{
            maxLength: 4,
            step: 0.1,
          }}
          variant="standard"
        />
      </DialogContent>
      <DialogActions>
        <Typography color="red">{state.errorMessage}</Typography>
        <Button onClick={handleClose}>Cancel</Button>
        <SubmitButton />
      </DialogActions>
    </Dialog>
  );
}
