import { auth } from "@/auth";
import {
  Configuration,
  GroundStationApi,
  ResponseError,
} from "@aleasat/pass-scheduling-api";
import Box from "@mui/material/Box";
import Card from "@mui/material/Card";
import CardContent from "@mui/material/CardContent";
import Divider from "@mui/material/Divider";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import AddGroundStationButton from "./components/AddGroundStationButton";
import DeleteGroundStationButton from "./components/DeleteGroundStationButton";
import EditGroundStationButton from "./components/EditGroundStationButton";

export default async function GroundStationSetting() {
  const session = await auth();

  if (session === null) return;

  const res = await new GroundStationApi(
    new Configuration({
      accessToken: session.accessToken,
    }),
  )
    .getGroundStationsRaw({
      ownerId: session.user.id,
    })
    .catch((error) => {
      if (error instanceof ResponseError)
        return { raw: error.response, value: () => [] };
      throw error;
    });

  const error = res.raw.status !== 200;
  const groundStations = await res.value();

  return (
    <Box>
      <Stack spacing={2} divider={<Divider flexItem />}>
        <Stack direction="row" spacing={2} justifyContent="space-between">
          <Typography>Ground Stations</Typography>
          <AddGroundStationButton />
        </Stack>
        <Stack spacing={5}>
          {error ? (
            <Typography>Internal Server Error</Typography>
          ) : (
            groundStations.map((groundStation, i) => (
              <Card variant="outlined" key={i}>
                <CardContent>
                  <Stack direction="row" justifyContent="space-between">
                    <div>
                      <Typography>{groundStation.name}</Typography>
                      <Typography variant="body2" color="text.secondary">
                        {groundStation.description}
                      </Typography>
                      <Typography>
                        Lat: {groundStation.lat} Lng: {groundStation.lng}
                      </Typography>
                      <Typography>
                        Minimum Elevation: {groundStation.minElevation}
                      </Typography>
                    </div>
                    <Stack spacing={2}>
                      <EditGroundStationButton groundStation={groundStation} />
                      <DeleteGroundStationButton
                        groundStationId={groundStation.id}
                      />
                    </Stack>
                  </Stack>
                </CardContent>
              </Card>
            ))
          )}
          {groundStations.length === 0 && (
            <Card variant="outlined">
              <CardContent>
                <Typography>There is no ground station configed</Typography>
              </CardContent>
            </Card>
          )}
        </Stack>
      </Stack>
    </Box>
  );
}
