import { auth } from "@/auth";
import { Configuration, LaunchpadApi } from "@aleasat/launchpad-api";
import {
  Button,
  Card,
  CardActions,
  CardContent,
  Container,
  Grid2,
  Typography,
} from "@mui/material";
import Link from "next/link";
import AddLaunchpadButtonPageContainer from "./AddLaunchpadButtonPageContainer";

export default async function LaunchpadsPage() {
  const session = await auth();
  if (!session) {
    return null;
  }

  const launchpads = await new LaunchpadApi(
    new Configuration({ accessToken: session.accessToken }),
  )
    .getLaunchpads({ cache: "no-cache" })
    .catch((e) => {
      console.error(e);
      return [];
    });

  return (
    <AddLaunchpadButtonPageContainer>
      <Container maxWidth="lg">
        <Grid2 container spacing={2}>
          {launchpads.map((launchpad) => (
            <Grid2 key={launchpad.id} size={12}>
              <Card
                sx={{
                  width: "100%",
                  height: "100%",
                }}
              >
                <CardContent>
                  <Typography>{launchpad.name}</Typography>
                  <Typography>
                    Status: {launchpad.userSessionId ? "In use" : "Available"}
                  </Typography>
                  <Typography>
                    Firmware: {launchpad.fwCommit.slice(0, 8)}
                  </Typography>
                </CardContent>
                <CardActions>
                  <Link href={`/launchpads/${launchpad.id}`}>
                    <Button size="small">Details</Button>
                  </Link>
                </CardActions>
              </Card>
            </Grid2>
          ))}
        </Grid2>
      </Container>
    </AddLaunchpadButtonPageContainer>
  );
}
