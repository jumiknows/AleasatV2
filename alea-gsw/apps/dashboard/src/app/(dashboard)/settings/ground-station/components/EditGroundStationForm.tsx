"use client";

import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import CircularProgress from "@mui/material/CircularProgress";
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
import { editGroundStation } from "../actions";

function SubmitButton() {
  const { pending } = useFormStatus();

  return (
    <Box sx={{ m: 1, position: "relative" }}>
      <Button type="submit" disabled={pending}>
        Update
      </Button>
      {pending && (
        <CircularProgress
          size={24}
          sx={{
            color: "green",
            position: "absolute",
            top: "50%",
            left: "50%",
            marginTop: "-12px",
            marginLeft: "-12px",
          }}
        />
      )}
    </Box>
  );
}

export default function EditGroundStationForm({
  groundStation,
  open,
  handleClose,
}: {
  groundStation: {
    id: number;
    name: string;
    description: string;
    lat: number;
    lng: number;
    minElevation: number;
  };
  open: boolean;
  handleClose?: () => void;
}) {
  const editGroundStationWithId = editGroundStation.bind(
    null,
    groundStation.id,
  );
  const [state, formAction] = useFormState(editGroundStationWithId, {
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
      <DialogTitle>Edit Ground Station</DialogTitle>
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
          defaultValue={groundStation.name}
        />
        <TextField
          required
          margin="dense"
          name="description"
          label="Description"
          fullWidth
          variant="standard"
          defaultValue={groundStation.description}
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
            defaultValue={groundStation.lat}
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
            defaultValue={groundStation.lng}
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
          defaultValue={groundStation.minElevation}
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
