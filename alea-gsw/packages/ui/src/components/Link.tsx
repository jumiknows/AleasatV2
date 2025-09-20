"use client";

import MuiLink from "@mui/material/Link";
import NextLink from "next/link";
import type { ComponentProps, RefObject } from "react";

export default function Link(
  props: ComponentProps<typeof MuiLink> &
    ComponentProps<typeof NextLink> & {
      ref?: RefObject<HTMLAnchorElement>;
    },
) {
  return (
    <MuiLink
      {...props}
      component={NextLink}
      href={props.href}
      ref={props.ref}
    />
  );
}
