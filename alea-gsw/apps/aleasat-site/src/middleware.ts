import { auth as middleware } from "@/auth";
import { NextResponse } from "next/server";

export default middleware((request) => {
  if (
    !request.auth &&
    !request.nextUrl.pathname.startsWith("/auth") &&
    !request.nextUrl.pathname.startsWith("/api/auth") &&
    process.env.ENABLE_AUTH === "true"
  ) {
    return NextResponse.redirect(
      new URL(
        `${process.env.AUTH_URL}/api/auth/signin?callbackUrl=${process.env.AUTH_URL}${request.nextUrl.pathname}`,
      ),
    );
  }
});

export const config = {
  matcher: ["/((?!api|_next/static|_next/image|favicon.ico).*)"],
};
