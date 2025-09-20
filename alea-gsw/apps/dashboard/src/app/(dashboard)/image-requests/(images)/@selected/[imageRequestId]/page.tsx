import { auth } from "@/auth";
import { Configuration, ImageRequestApi } from "@aleasat/api-lib";
import Box from "@mui/material/Box";
import Card from "@mui/material/Card";
import CardContent from "@mui/material/CardContent";
import CardMedia from "@mui/material/CardMedia";
import { notFound } from "next/navigation";
import ImageRequestDetails from "./components/ImageRequestDetails";
import ImageRequestImage from "./components/ImageRequestImages";

export default async function ImageRequestViewer({
  params,
}: {
  params: { imageRequestId: string };
}) {
  const session = await auth();
  if (session === null) return;

  const imageRequest = await new ImageRequestApi(
    new Configuration({
      accessToken: session.accessToken,
    }),
  ).getImageRequest(
    { imageRequestId: params.imageRequestId },
    {
      validateStatus: (status) =>
        (status >= 200 && status < 300) || status === 404,
    },
  );

  if (imageRequest.status === 404) notFound();
  return (
    <Box height={"92vh"} pt="3vh" pb={5}>
      <Card sx={{ borderRadius: 0, height: "100%" }}>
        <CardMedia
          sx={{
            height: "65%",
            pb: 3,
            background: "#080808",
          }}
        >
          <ImageRequestImage
            imageRequestId={imageRequest.data.id}
            initialPreviewImage={imageRequest.data.initialPreviewImage}
            finalPreviewImage={imageRequest.data.finalPreviewImage}
            image={imageRequest.data.image}
          />
        </CardMedia>
        <CardContent
          sx={{
            pt: 1,
            border: "1px solid #fff",
            height: "35%",
          }}
        >
          <ImageRequestDetails imageRequest={imageRequest.data} />
        </CardContent>
      </Card>
    </Box>
  );
}
