import { Container } from "@mui/material";
import { Box } from "@mui/system";
import { useEffect, useRef, useState } from "react";

interface HomeHeroProps {
  children?: (scrollPosition: number) => React.ReactNode;
}

export default function HomeHeroBackground({
  children = () => null,
}: HomeHeroProps) {
  const boxRef = useRef<HTMLDivElement | null>(null);
  const [scrollPosition, setScrollPosition] = useState(0);
  useEffect(() => {
    const handleScroll = () => {
      if (boxRef.current) {
        const { clientHeight } = boxRef.current;
        const offsetTop = -boxRef.current.getBoundingClientRect().top;
        const scrollPercent = Math.min(
          Math.max(offsetTop / (clientHeight - window.innerHeight), 0),
          1,
        );
        setScrollPosition(scrollPercent);
      }
    };
    handleScroll();

    window.addEventListener("scroll", handleScroll);

    return () => {
      window.removeEventListener("scroll", handleScroll);
    };
  }, []);

  return (
    <>
      <Box
        ref={boxRef}
        sx={{
          position: "relative",
          width: "100%",
          height: "300vh",
          bgcolor: "#000",
        }}
      >
        <Box sx={{ position: "sticky", top: 0 }}>
          <Box
            sx={{
              position: "relative",
              height: "100vh",
            }}
          >
            <Box
              sx={{
                position: "relative",
                "@media (max-aspect-ratio: 16/9)": {
                  height: {
                    xs: "unset",
                    sm: "100vh",
                  },
                  aspectRatio: {
                    xs: "16/9",
                    sm: "unset",
                  },
                },
                "@media (min-aspect-ratio: 16/9)": {
                  aspectRatio: "16/9",
                },
                width: "100%",
                overflow: "hidden",
              }}
            >
              {Array.from({ length: 100 }, (_, i) => (
                <Box
                  key={i}
                  sx={{
                    "@media (max-aspect-ratio: 16/9)": {
                      height: {
                        xs: "unset",
                        sm: "100%",
                      },
                      width: {
                        xs: "147%",
                        sm: "unset",
                      },
                    },
                    "@media (min-aspect-ratio: 16/9)": {
                      width: {
                        xs: "147%",
                        sm: "100%",
                      },
                    },
                    aspectRatio: "16/9",
                    transform: {
                      xs: `translate(-66%, -40%)`,
                      sm: `translate(-50%, -50%)`,
                    },
                    top: "50%",
                    left: "50%",
                    position: "absolute",
                  }}
                  style={{
                    visibility:
                      Math.floor(scrollPosition * 99) === i
                        ? "visible"
                        : "hidden",
                  }}
                >
                  <img
                    alt=""
                    src={`https://artifact.aleasat.space/hero-video/${String(i * 4 + 1).padStart(4, "0")}.avif`}
                    style={{
                      height: "100%",
                      width: "100%",
                    }}
                  />
                </Box>
              ))}
            </Box>
          </Box>
          <Container
            sx={{
              position: "absolute",
              top: {
                xs: "unset",
                sm: 0,
              },
              bottom: {
                xs: 0,
                sm: "unset",
              },
              pb: {
                xs: 8,
                sm: 0,
              },
              left: {
                xs: 0,
                sm: 10,
                md: 50,
              },
              width: {
                xs: "100%",
                sm: "40%",
              },
            }}
          >
            {children(scrollPosition)}
          </Container>
        </Box>
      </Box>
    </>
  );
}
