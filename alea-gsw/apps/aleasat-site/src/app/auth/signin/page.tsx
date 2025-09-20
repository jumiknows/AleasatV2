import { auth } from "@/auth";
import { cookies } from "next/headers";
import { redirect } from "next/navigation";
import SignInBox, { type signinErrors } from "./SigninBox";

export default async function SignInPage({
  searchParams,
}: {
  searchParams: {
    callbackUrl: string | undefined;
    error: keyof typeof signinErrors | undefined;
  };
}) {
  const session = await auth();
  const keycloakLogin =
    (await cookies()).get("keycloakLogin")?.value === "true";

  if (process.env.ENABLE_AUTH !== "true" || session !== null) {
    redirect(searchParams.callbackUrl ?? "/");
  }

  return (
    <SignInBox
      callbackUrl={searchParams.callbackUrl}
      error={searchParams.error}
      keycloakLogin={keycloakLogin}
    />
  );
}
