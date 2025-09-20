"use client";

import Box from "@mui/material/Box";
import Grid from "@mui/material/Grid";
import { useState } from "react";
import StepperButton from "../components/StepperButton";
import ErrorSnackbar from "./components/ErrorSnackbar";
import ReviewDetials from "./components/ReviewDetails";
import SubmitDialog from "./components/SubmitDialog";

export default function FinalizeDetails() {
  const [showSubmitDialog, setShowSubmitDialog] = useState(false);
  const [errorSnackbarMessage, setErrorSnackbarMessage] = useState("");
  const [submitDialogKey, setSubmitDialogKey] = useState(0);

  const handleClose = () => {
    setShowSubmitDialog(false);
    setSubmitDialogKey((o) => o + 1);
  };

  return (
    <>
      <Grid
        container
        flexDirection={"column"}
        key={0}
        style={{ height: "85%" }}
      >
        <Grid item style={{ height: "8vh" }}></Grid>
        <Grid
          item
          container
          style={{
            width: "100%",
            height: "80%",
            aspectRatio: "34/24",
            justifyContent: "center",
          }}
        >
          <Box style={{ height: "100%", aspectRatio: "4/3" }}>
            <ReviewDetials />
          </Box>
        </Grid>
      </Grid>
      <SubmitDialog
        open={showSubmitDialog}
        handleClose={handleClose}
        key={submitDialogKey}
        setErrorMessage={setErrorSnackbarMessage}
      />
      <ErrorSnackbar
        errorMessage={errorSnackbarMessage}
        handleClose={() => setErrorSnackbarMessage("")}
      />
      <StepperButton onClickNext={() => setShowSubmitDialog(true)} />
    </>
  );
}
