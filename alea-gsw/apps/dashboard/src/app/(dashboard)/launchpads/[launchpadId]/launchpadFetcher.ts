import { Configuration, LaunchpadApi } from "@aleasat/launchpad-api";
import { notFound } from "next/navigation";

export default async function launchpadFetcher(
  launchpadId: string,
  accessToken: string,
) {
  const launchpad = await new LaunchpadApi(new Configuration({ accessToken }))
    .getLaunchpad({ launchpadId })
    .catch((e) => {
      if (e.response.status === 404) {
        notFound();
      }
      throw e;
    });

  return launchpad;
}
