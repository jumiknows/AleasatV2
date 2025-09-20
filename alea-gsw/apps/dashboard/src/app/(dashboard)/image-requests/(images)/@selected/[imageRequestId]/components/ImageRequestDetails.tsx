import Container from "@mui/material/Container";
import Typography from "@mui/material/Typography";

import type { ImageRequest } from "@aleasat/api-lib";

export default function ImageRequestDetails({
  imageRequest,
}: {
  imageRequest: ImageRequest;
}) {
  return (
    <Container sx={{ width: "100%" }}>
      <Typography variant="h4" fontWeight="bold" lineHeight={1.5}>
        {imageRequest.name}
      </Typography>
      <Typography>January 9, 2023: 19.28:34</Typography>
      <Typography>Vancouver, BC, Canada (-56.09 N, 127.12 E)</Typography>
      <br />
      <Typography>{imageRequest.description}</Typography>
    </Container>
  );
}
