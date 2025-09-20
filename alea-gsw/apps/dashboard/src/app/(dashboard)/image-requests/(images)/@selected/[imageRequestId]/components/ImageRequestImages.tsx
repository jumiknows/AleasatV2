import { Typography } from "@mui/material";
import CircularProgress from "@mui/material/CircularProgress";
import Stack from "@mui/material/Stack";
import { Suspense } from "react";
import S3Image from "../../../../components/S3Image";
import InitialPreviewImage from "./initialPreviewImage";

export default function ImageRequestImages({
  imageRequestId,
  initialPreviewImage,
  image,
}: {
  imageRequestId: string;
  image: {
    imageId: string | null;
    placeholderId: string | null;
  } | null;
  initialPreviewImage: {
    imageId: string | null;
    placeholderId: string | null;
  } | null;
  finalPreviewImage: {
    imageId: string | null;
    placeholderId: string | null;
  } | null;
}) {
  return (
    <Stack sx={{ overflowY: "scroll", height: "100%" }}>
      {(image?.imageId ?? false) && (
        <S3Image
          style={{
            maxWidth: "100%",
          }}
          Bucket="imagerequest"
          Key={`${imageRequestId}/${image?.imageId}`}
        />
      )}
      <Suspense
        fallback={
          <Stack
            sx={{
              height: "100%",
              aspectRatio: "1/1",
              justifyContent: "center",
            }}
          >
            <CircularProgress />
            <Typography>Rendering preview, please check later</Typography>
          </Stack>
        }
      >
        <InitialPreviewImage
          imageRequestId={imageRequestId}
          initialPreviewImage={initialPreviewImage}
        />
      </Suspense>
    </Stack>
  );
}
