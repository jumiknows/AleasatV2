import { auth } from "@/auth";
import { Configuration, ImageRequestApi } from "@aleasat/api-lib";
import Grid from "@mui/material/Grid"; // Grid version 1
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import ImageCard from "../components/ImageCard";
import SearchBar from "../components/SearchBar";

export const dynamic = "force-dynamic";

export default async function MyImage() {
  const session = await auth();
  if (session === null) return;

  const imageRequests = await new ImageRequestApi(
    new Configuration({
      accessToken: session.accessToken,
    }),
  )
    .getImageRequests(
      { owner: session.user.id },
      { headers: { Authorization: "Bearer " + session.accessToken } },
    )
    .then((res) => res.data)
    .catch((err) => {
      console.error(err);
      return [];
    });

  return (
    <Stack spacing={2}>
      <Stack direction="row" justifyContent="space-between">
        <Typography variant="h4" fontWeight="bold">
          Completed Requests
        </Typography>
        <SearchBar />
      </Stack>
      <Grid container>
        {imageRequests
          .filter((imageRequest) => imageRequest.approved === true)
          .map((imageRequest, i) => (
            <Grid item mr={4} xs={3} key={i}>
              <ImageCard imageRequest={imageRequest} />
            </Grid>
          ))}
      </Grid>
      <Typography variant="h4" fontWeight="bold">
        Pending Requests
      </Typography>
      <Grid container>
        {imageRequests
          .filter((imageRequest) => imageRequest.approved === null)
          .map((imageRequest, i) => (
            <Grid item mr={4} xs={3} key={i}>
              <ImageCard imageRequest={imageRequest} />
            </Grid>
          ))}
      </Grid>
      <Typography variant="h4" fontWeight="bold">
        Reported Incidents and Rejected Requests
      </Typography>
      <Grid container>
        {imageRequests
          .filter((imageRequest) => imageRequest.approved === false)
          .map((imageRequest, i) => (
            <Grid item mr={4} xs={3} key={i}>
              <ImageCard imageRequest={imageRequest} />
            </Grid>
          ))}
      </Grid>
    </Stack>
  );
}
