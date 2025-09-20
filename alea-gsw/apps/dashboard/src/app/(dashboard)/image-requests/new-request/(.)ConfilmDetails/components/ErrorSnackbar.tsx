import Alert from "@mui/material/Alert";
import Snackbar from "@mui/material/Snackbar";

export default function ErrorSnackbar(props: {
  errorMessage: string;
  handleClose: () => void;
}) {
  const { errorMessage, handleClose } = props;

  return (
    <Snackbar
      open={errorMessage !== ""}
      autoHideDuration={6000}
      onClose={handleClose}
      anchorOrigin={{ vertical: "bottom", horizontal: "center" }}
    >
      <Alert
        onClose={handleClose}
        severity="error"
        variant="filled"
        sx={{ width: "100%" }}
      >
        {errorMessage}
      </Alert>
    </Snackbar>
  );
}
