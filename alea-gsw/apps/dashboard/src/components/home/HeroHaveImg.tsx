import type { ImageRequest } from "@aleasat/api-lib";
import Link from "@aleasat/ui/components/Link";
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import S3Image from "../../app/(dashboard)/image-requests/components/S3Image";

export const dynamic = "force-dynamic";

export default async function HeroWithImg({
  imageRequestIn,
}: {
  imageRequestIn: ImageRequest[];
}) {
  const imageRequest = imageRequestIn.at(-1);

  return (
    <Box
      sx={{
        height: "80vh",
        my: { xs: "3rem", md: "unset" },
        display: "flex",
        flexDirection: { xs: "column", md: "row" },
        justifyContent: { xs: "space-evenly", md: "space-around" },
        alignItems: "center",
        padding: "3em",
        backgroundImage: `url("https://artifact.aleasat.space/hero2.png")`,
        backgroundRepeat: "no-repeat",
        backgroundSize: "cover",
      }}
    >
      {imageRequest && (
        <Box>
          <S3Image
            style={{
              width: 600,
              height: "100%",
              objectFit: "cover",
              borderRadius: "30px",
              marginBottom: "1rem",
            }}
            Bucket="imagerequest"
            Key={`${imageRequest.id}/${imageRequest.initialPreviewImage?.imageId}`}
          />
          <Typography
            variant="h5"
            component="h4"
            style={{ textAlign: "center", fontWeight: "700" }}
          >
            Your most recent request
          </Typography>
        </Box>
      )}

      <Stack
        sx={{
          width: { sm: "30rem", lg: "35rem" },
          mx: { xs: 3, sm: 4, md: 5 },
          textAlign: "start",
        }}
      >
        <Box>
          <Typography
            fontSize={{ sm: 40, lg: 46 }}
            sx={{ width: "100%", fontWeight: "700" }}
          >
            Let&apos;s capture more imagery!
          </Typography>
          <Button
            variant="outlined"
            size="large"
            color="secondary"
            LinkComponent={Link}
            href="/image-requests/new-request"
            sx={{ my: 3 }}
          >
            Send Image Request
          </Button>
        </Box>
      </Stack>
    </Box>
  );

  /*

 return (
    <Box>
      <Box sx={{ position: "relative", width: "100%", height: "93vh" }}>
        <Image
          src={"https://artifact.aleasat.space/hero2.png"}
          fill
          style={{ objectFit: "cover", opacity:"0.3", backgroundColor:"black" }}
          alt="hero image"
        />
        <Box
          sx={{
            position: "absolute",
            top: "50%",
            left: "30%",
            transform: "translate(-50%, -50%)",
            width: "30%",
          }}
        >
        </Box>
        <Box
          sx={{
            position: "relative",
            top: "30%",
            left: "25%",
            transform: "translate(-50%, -50%)",
            width:"40%",
            height:"40%",
          }}
        >
          {imageRequest && (
            <S3Image
              style={{ width: "100%", height: "100%", objectFit: "cover", borderRadius: "30px" }}
              Bucket="imagerequest"
              Key={`${imageRequest.id}/${imageRequest.initialPreviewImage?.imageId}`}
            />
          )}
        
          <Typography variant="h5" component="h4" style={{textAlign:"center"}}>
          Your most recent request
          </Typography>
        </Box>
        <Box
          sx={{
            position: "relative",
            top: "30%",
            left: "80%",
            transform: "translate(-50%, -50%)",
            width:"25%",
          }}
        >
          <Typography style={{padding:"24px"}}variant="h4">Let’s capture more imagery!</Typography>
          <Button
            variant="outlined"
            size="large"
            color="secondary"
            LinkComponent={Link}
            href="/image-requests/new-request"
          >
            Send Image Request
          </Button>
        </Box>
      </Box>
    </Box>
  );
  */
}
