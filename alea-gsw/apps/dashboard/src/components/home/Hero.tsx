import { auth } from "@/auth";
import { Configuration, ImageRequestApi } from "@aleasat/api-lib";
import Box from "@mui/material/Box";
import HeroWithImg from "./HeroHaveImg";
import HeroNoImg from "./HeroNoImg";
import RecentImages from "./RecentImages";
import Telemetry from "./Telemetry";

export const dynamic = "force-dynamic";

export default async function Hero() {
  const session = await auth().catch((error) => {
    console.log("Unable to authenticate.", error);
  });

  const imageRequests = session
    ? await new ImageRequestApi(
        new Configuration({
          accessToken: session.accessToken,
        }),
      )
        .getImageRequests(
          { owner: session.user.id },
          { headers: { Authorization: "Bearer " + session.accessToken } },
        )
        .then((res) => res.data.slice(-3))
        .catch((error) => {
          console.log("Unable to retrieve image requests.", error);
          return [];
        })
    : [];
  return (
    <Box>
      {imageRequests.length > 0 ? (
        <HeroWithImg imageRequestIn={imageRequests}></HeroWithImg>
      ) : (
        <HeroNoImg />
      )}
      <RecentImages imageRequestIn={imageRequests} />
      <Telemetry imageExists={imageRequests.length > 0} />
    </Box>
  );
}
