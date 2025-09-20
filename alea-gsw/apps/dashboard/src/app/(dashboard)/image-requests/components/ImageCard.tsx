import type { ImageRequest } from "@aleasat/api-lib";
import Link from "@aleasat/ui/components/Link";
import Card from "@mui/material/Card";
import CardContent from "@mui/material/CardContent";
import CardMedia from "@mui/material/CardMedia";
import Skeleton from "@mui/material/Skeleton";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import S3Image from "./S3Image";

const calendarOptions: Intl.DateTimeFormatOptions = {
  year: "numeric",
  month: "long",
  day: "numeric",
  hour: "2-digit",
  minute: "2-digit",
  second: "2-digit",
  hour12: false,
};

export default function ImageCard({
  imageRequest,
}: {
  imageRequest: ImageRequest;
}) {
  return (
    <Card
      sx={{
        height: "95%",
        borderColor:
          imageRequest.approved === null
            ? "yellow"
            : imageRequest.approved
              ? "green"
              : "red",
        borderRadius: "16px",
      }}
      variant="outlined"
    >
      <Link href={`/image-requests/${imageRequest.id}`}>
        <CardMedia sx={{ aspectRatio: "4/3", p: "13px" }}>
          {imageRequest.thumbnail?.imageId ? (
            <S3Image
              style={{ width: "100%", borderRadius: "16px" }}
              Bucket="imagerequest"
              Key={`${imageRequest.id}/${imageRequest.thumbnail?.imageId}`}
            />
          ) : (
            <Skeleton
              variant="rectangular"
              sx={{ width: "100%", height: "100%", borderRadius: "16px" }}
            />
          )}
        </CardMedia>
      </Link>
      <CardContent sx={{ p: "0 13px" }}>
        <Stack direction="row" justifyContent="space-between">
          <Typography fontWeight={700} fontSize={16}>
            {imageRequest.name}
          </Typography>
          <Typography
            textTransform={"uppercase"}
            fontWeight={700}
            fontSize={16}
          >
            {imageRequest.approved === null
              ? "Pending"
              : imageRequest.approved
                ? "Approved"
                : "Rejected"}
          </Typography>
        </Stack>
        <Typography textAlign="center" mt={1} fontSize={14}>
          {new Intl.DateTimeFormat("en-US", calendarOptions).format(new Date())}
        </Typography>
        <Typography textAlign="center" fontSize={14}>
          {"(12.234 N, -23.23 E)"}
        </Typography>
      </CardContent>
      {/* <CardActions> */}
      {/* <ImageCardDetails /> */}
      {/* </CardActions> */}
    </Card>
  );
}
