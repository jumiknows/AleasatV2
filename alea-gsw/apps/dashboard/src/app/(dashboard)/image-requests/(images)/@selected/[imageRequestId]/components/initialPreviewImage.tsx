import { auth } from "@/auth";
import { Configuration, ImageRequestApi } from "@aleasat/api-lib";
import S3Image from "../../../../components/S3Image";

export default async function InitialPreviewImage({
  imageRequestId,
  initialPreviewImage,
}: {
  imageRequestId: string;
  initialPreviewImage: {
    imageId: string | null;
    placeholderId: string | null;
  } | null;
}) {
  const session = await auth();
  if (session === null) return;

  let initialPreviewImageImageid = initialPreviewImage?.imageId;
  if (!initialPreviewImageImageid) {
    initialPreviewImageImageid = await new ImageRequestApi(
      new Configuration({
        accessToken: session.accessToken,
      }),
    )
      .getImageRequestInitialPreviewImage({ imageRequestId })
      .then((res) => res.data);
  }

  return (
    <S3Image
      style={{
        maxHeight: "85vh",
        maxWidth: "100%",
      }}
      Bucket="imagerequest"
      Key={`${imageRequestId}/${initialPreviewImageImageid}`}
    />
  );
}
