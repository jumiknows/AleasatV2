"use client";

import Box, { type BoxProps } from "@mui/material/Box";
import NextImage from "next/image";
import type { StaticImageData } from "next/image";
import { type RefObject, forwardRef } from "react";

// Define the custom props for the Image component
interface ImageProps extends BoxProps {
  ref?: RefObject<HTMLDivElement>;
  src: string | StaticImageData;
  alt: string;
  cover?: boolean;
}

export default function ImageBox({
  ref,
  src,
  alt,
  cover = false,
  ...props
}: ImageProps) {
  return (
    <Box position="relative" {...props} ref={ref}>
      <NextImage
        src={src}
        alt={alt}
        width={1000}
        height={1000}
        sizes="100vw"
        style={{
          width: "100%",
          height: "100%",
          objectFit: cover ? "cover" : "contain",
        }}
      />
    </Box>
  );
}
