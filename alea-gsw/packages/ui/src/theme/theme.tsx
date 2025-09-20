import type { LinkProps } from "@mui/material/Link";
import { createTheme, responsiveFontSizes } from "@mui/material/styles";
import Link from "next/link";
import React from "react";
import { audiowide, pathway } from "./fonts";

// eslint-disable-next-line react/display-name
const LinkBehavior = React.forwardRef<
  HTMLAnchorElement,
  React.ComponentProps<typeof Link>
>((props, ref) => {
  return <Link ref={ref} {...props} />;
});

declare module "@mui/material/styles" {
  interface TypographyVariants {
    p: React.CSSProperties;
  }

  // allow configuration using `createTheme()`
  interface TypographyVariantsOptions {
    p?: React.CSSProperties;
  }
}

// Update the Typography's variant prop options
declare module "@mui/material/Typography" {
  interface TypographyPropsVariantOverrides {
    p: true;
  }
}

// Update the Paper's variant prop options
declare module "@mui/material/Paper" {
  interface PaperPropsVariantOverrides {
    glass: true;
  }
}

let theme = createTheme({
  palette: {
    mode: "dark",
    background: {
      default: "#000",
    },
    primary: {
      main: "#67ACF0",
      light: "#C3D6E5",
      dark: "#4788C7",
      contrastText: "#fff",
    },
    secondary: {
      main: "#D0D0D0",
      contrastText: "#000",
    },
  },
  typography: {
    allVariants: { color: "#fff" },
    h1: {
      fontFamily: audiowide.style.fontFamily,
      fontSize: "3.812rem",
      fontWeight: "bold",
    },
    h2: { fontFamily: audiowide.style.fontFamily, fontSize: "3.062rem" },
    h3: { fontFamily: audiowide.style.fontFamily, fontSize: "2.438rem" },
    h4: {
      fontFamily: pathway.style.fontFamily,
      fontSize: "1.938rem",
      fontWeight: "bold",
    },
    h5: { fontFamily: pathway.style.fontFamily, fontSize: "1.562rem" },
    h6: { fontFamily: pathway.style.fontFamily, fontSize: "1.250rem" },
    p: { fontFamily: pathway.style.fontFamily, fontSize: "1rem" },
  },
  components: {
    MuiLink: {
      defaultProps: {
        component: LinkBehavior,
      } as LinkProps,
    },
    MuiButtonBase: {
      defaultProps: {
        LinkComponent: LinkBehavior,
      },
    },
    MuiButton: {
      styleOverrides: {
        iconSizeMedium: {
          "& > *:first-child": {
            fontSize: 35,
          },
        },
      },
    },
    MuiPaper: {
      styleOverrides: {
        root: {
          variants: [
            {
              props: { variant: "glass" },
              style: {
                backgroundColor: "rgba(30, 30, 30, 0.8)",
                border: "1.2px solid #56616A",
                backdropFilter: "blur(20)",
              },
            },
          ],
        },
      },
    },
    MuiCssBaseline: {
      styleOverrides: {
        body: {
          "*::-webkit-scrollbar": {
            width: "8px",
            backgroundColor: "rgba(0, 0, 0, 0)",
          },
          "*::-webkit-scrollbar-track": {
            backgroundColor: "rgba(0, 0, 0, 0)",
          },
          "*::-webkit-scrollbar-thumb": {
            borderRadius: "10px",
            WebkitBoxShadow: "inset 0 0 6px rgba(0, 0, 0, 0.1)",
            backgroundColor: "#888888",
          },
        },
      },
    },
  },
});
theme = responsiveFontSizes(theme);
export default theme;
