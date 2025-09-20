import { auth } from "@/auth";
import Box from "@mui/material/Box";
import Divider from "@mui/material/Divider";
import Stack from "@mui/material/Stack";
import TextField from "@mui/material/TextField";
import Typography from "@mui/material/Typography";

export default async function ProfileSetting() {
  const session = await auth();

  return (
    <Box>
      <Stack spacing={2} divider={<Divider flexItem />}>
        <Typography>Profile</Typography>
        <Stack>
          <TextField
            label="Name"
            variant="filled"
            rows={1}
            value={session?.user?.name}
            margin="normal"
            style={{ width: "50%" }}
            disabled
          />

          <TextField
            label="Email"
            variant="filled"
            rows={1}
            value={session?.user?.email}
            margin="normal"
            style={{ width: "50%" }}
            disabled
          />
        </Stack>
      </Stack>
    </Box>
  );
}
