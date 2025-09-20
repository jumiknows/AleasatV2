import CheckIcon from "@mui/icons-material/Check";
import PriorityHighIcon from "@mui/icons-material/PriorityHigh";
import RemoveIcon from "@mui/icons-material/Remove";
import Avatar from "@mui/material/Avatar";
import Box from "@mui/material/Box";
import CircularProgress from "@mui/material/CircularProgress";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";

export default function ValidationStatus({
  loading,
  success,
  subject,
  skip = false,
  submited,
  error = "",
}: {
  loading: boolean;
  success: boolean;
  subject: string;
  skip?: boolean;
  submited: boolean;
  error?: string;
}) {
  return (
    <Stack
      direction="row"
      alignItems="center"
      spacing={4}
      style={{
        visibility: submited ? "visible" : "hidden",
      }}
    >
      <Box sx={{ m: 1, position: "relative" }}>
        <Avatar
          style={{
            width: "30px",
            height: "30px",
            backgroundColor: skip ? "grey" : success ? "green" : "red",
            visibility: !submited || (loading && !skip) ? "hidden" : "visible",
          }}
        >
          {skip ? (
            <RemoveIcon
              sx={{
                color: "white",
              }}
            />
          ) : success ? (
            <CheckIcon
              sx={{
                color: "white",
              }}
            />
          ) : (
            <PriorityHighIcon
              sx={{
                color: "white",
              }}
            />
          )}
        </Avatar>
        <CircularProgress
          size={38}
          style={{
            color: "green",
            position: "absolute",
            top: -4,
            left: -4,
            zIndex: 1,
            visibility: skip || !submited || !loading ? "hidden" : "visible",
          }}
        />
      </Box>
      {skip ? (
        <Typography>{subject}: Skipped</Typography>
      ) : loading ? (
        <Typography>{subject}: Validating...</Typography>
      ) : (
        <Typography>
          {subject}: {success ? "Success" : error}
        </Typography>
      )}
    </Stack>
  );
}
