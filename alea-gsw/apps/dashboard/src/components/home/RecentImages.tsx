import type { ImageRequest } from "@aleasat/api-lib";
import Link from "@aleasat/ui/components/Link";
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Container from "@mui/material/Container";
import Grid from "@mui/material/Grid"; // Grid version 1
import Typography from "@mui/material/Typography";
import Image from "next/image";
import ImageCard from "../../app/(dashboard)/image-requests/components/ImageCard";

export const dynamic = "force-dynamic";

export default async function RecentImages({
  imageRequestIn,
}: {
  imageRequestIn: ImageRequest[];
}) {
  const imageRequests = imageRequestIn.slice(-3);

  return (
    <Container sx={{ p: "20vh 0", textAlign: "center" }}>
      <Box sx={{ minHeight: "20vh", p: "5vh 0" }}>
        {imageRequests.length > 0 ? (
          <Grid
            container
            spacing={8}
            alignItems="center"
            justifyContent="center"
          >
            {imageRequests.map((imageRequest, i) => (
              <Grid item xs={4} key={i}>
                <ImageCard imageRequest={imageRequest} />
              </Grid>
            ))}
          </Grid>
        ) : (
          <Box>
            <Image
              src={"https://artifact.aleasat.space/MINIALEASAT.png"}
              style={{ objectFit: "cover" }}
              width={200}
              height={200}
              alt="hero image"
            />
            <Typography variant="h4">
              OOPS It looks like you have not requested any images yet.
            </Typography>
          </Box>
        )}
      </Box>
      <Button
        variant="outlined"
        color="secondary"
        size="large"
        disabled={imageRequests.length === 0}
        LinkComponent={Link}
        href="/image-requests"
      >
        View More Requests
      </Button>
    </Container>
  );
}
