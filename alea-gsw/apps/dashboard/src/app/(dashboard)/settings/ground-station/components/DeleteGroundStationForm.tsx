"use client";

import CloseIcon from "@mui/icons-material/Close";
import Button from "@mui/material/Button";
import Dialog from "@mui/material/Dialog";
import DialogActions from "@mui/material/DialogActions";
import DialogContent from "@mui/material/DialogContent";
import DialogContentText from "@mui/material/DialogContentText";
import DialogTitle from "@mui/material/DialogTitle";
import IconButton from "@mui/material/IconButton";
import Typography from "@mui/material/Typography";
import { useEffect } from "react";
import { useFormState, useFormStatus } from "react-dom";
import { deleteGroundStation } from "../actions";

function SubmitButton() {
  const { pending } = useFormStatus();

  return (
    <Button
      type="submit"
      color="error"
      variant="contained"
      fullWidth
      disabled={pending}
    >
      I understand, delete this ground station key
    </Button>
  );
}

export default function DeleteGroundStationForm({
  groundStationId,
  open,
  handleClose,
}: {
  groundStationId: number;

  open: boolean;
  handleClose?: () => void;
}) {
  const deleteGroundStationWithId = deleteGroundStation.bind(
    null,
    groundStationId,
  );
  const [state, formAction] = useFormState(deleteGroundStationWithId, {
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
      <DialogTitle>
        Are you sure you want to delete this ground station?
      </DialogTitle>
      <IconButton
        aria-label="close"
        onClick={handleClose}
        sx={{
          position: "absolute",
          right: 8,
          top: 8,
          color: (theme) => theme.palette.grey[500],
        }}
      >
        <CloseIcon />
      </IconButton>
      <DialogContent>
        <DialogContentText paragraph>
          This action CANNOT be undone. This will permanently delete the ground
          station and if you’d like to use it in the future, you will need to
          add it again.
        </DialogContentText>
        <Typography color="red">{state.errorMessage}</Typography>
      </DialogContent>
      <DialogActions>
        <SubmitButton />
      </DialogActions>
    </Dialog>
  );
}
