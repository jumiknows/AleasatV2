"use client";

import {
  Configuration,
  ImageRequestApi,
  type ImageRequestApiGetImageRequestPreviewRequest,
} from "@aleasat/api-lib";
import Box from "@mui/material/Box";
import Skeleton from "@mui/material/Skeleton";
import { useAtomValue } from "jotai";
import { useSession } from "next-auth/react";
import { env } from "next-runtime-env";
import Image from "next/image";
import useSWR from "swr";
import { SatVizRotationState, selectedPassState } from "../../recoilState";

const imageFetcher = async (
  params: ImageRequestApiGetImageRequestPreviewRequest,
  accessToken: string,
) =>
  await new ImageRequestApi(
    new Configuration({
      accessToken,
    }),
  )
    .getImageRequestPreview(params, {
      headers: { Authorization: "Bearer " + accessToken },
    })
    .then((res) => res.data.base64);

export default function PreviewImage() {
  const session = useSession();
  const selectedPass = useAtomValue(selectedPassState);
  const SatVizRotation = useAtomValue(SatVizRotationState);

  const { data, isLoading, error } = useSWR(
    session.data?.accessToken && selectedPass && SatVizRotation
      ? {
          satLat: selectedPass.culmination.lat,
          satLng: selectedPass.culmination.lng,
          satAlt: selectedPass.culmination.height,
          satRotX: SatVizRotation.x,
          satRotY: SatVizRotation.y,
          satRotZ: SatVizRotation.z,
        }
      : null,
    async (key) => await imageFetcher(key, session.data!.accessToken),
  );

  return (
    <Box
      sx={{
        width: "100%",
        aspectRatio: "34/24",
        position: "relative",
      }}
    >
      {isLoading || !data || error ? (
        <Skeleton
          variant="rectangular"
          animation="pulse"
          style={{ width: "100%", height: "100%" }}
        />
      ) : (
        <Image
          src={`data:image/jpeg;base64,${data}`}
          alt="Preview of the satellite image"
          fill
        />
      )}
    </Box>
  );
}
