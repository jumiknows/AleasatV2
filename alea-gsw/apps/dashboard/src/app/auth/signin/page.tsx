import ImageBox from "@aleasat/ui/components/Image";
import Link from "@aleasat/ui/components/Link";
import GoogleIcon from "@mui/icons-material/Google";
import { Box, Button, Divider, Typography } from "@mui/material";
import Grid from "@mui/material/Grid2";
import signInKeycloak from "./signInKeycloak";

export default function SignInPage(props: {
  searchParams: { callbackUrl: string | undefined };
}) {
  return (
    <Grid
      container
      sx={{
        position: "relative",
        alignItems: "center",
        height: "100%",
        width: "100%",
        overflow: "hidden",
      }}
    >
      <ImageBox
        src={"https://artifact.aleasat.space/dashboard/earth.png"}
        alt="earth"
        sx={{ position: "absolute", left: "-20rem", height: 900 }}
      />
      <Grid size={6} />
      <Grid size={6}>
        <Box
          sx={{
            display: "flex",
            flexDirection: "column",
            position: "relative",
            rowGap: 2,
            width: "84%",
          }}
        >
          <Typography variant="h1">LAUNCH YOUR MISSION WITH US!</Typography>
          <Divider orientation="horizontal" sx={{ borderColor: "white" }} />
          <form
            action={async () => {
              "use server";
              try {
                const params = await props.searchParams;
                await signInKeycloak(params.callbackUrl);
              } catch (error) {
                // Signin can fail for a number of reasons, such as the user
                // not existing, or the user not having the correct role.
                // In some cases, you may want to redirect to a custom error
                // if (error instanceof AuthError) {
                // return redirect(`${SIGNIN_ERROR_URL}?error=${error.type}`);
                // }

                // Otherwise if a redirects happens Next.js can handle it
                // so you can just re-thrown the error and let Next.js handle it.
                // Docs:
                // https://nextjs.org/docs/app/api-reference/functions/redirect#server-component
                console.log(error);
                throw error;
              }
            }}
          >
            <Button
              type="submit"
              variant="contained"
              size="large"
              startIcon={<GoogleIcon />}
              sx={{ height: "4rem", width: "23rem", borderRadius: "1rem" }}
            >
              <Typography variant="h6" textTransform={"none"}>
                Sign up with Google
              </Typography>
            </Button>
          </form>
          <Typography variant="p">
            By signing up, you agree to our{" "}
            <Link href="/">Terms of Service</Link> <br />
            and <Link href="/">Privacy Policy</Link>.
          </Typography>
        </Box>
      </Grid>
    </Grid>
  );
}
