"use client";

import { keyframes } from "@emotion/react";
import KeyboardDoubleArrowDownIcon from "@mui/icons-material/KeyboardDoubleArrowDown";
import Box from "@mui/material/Box";

const fadeMoveDown = keyframes`
  0% { transform: translateY(-20px); opacity: 0; }
  50% { opacity: 1; }
  100% { transform: translateY(20px); opacity: 0; }
`;

export default function ScrollDownSection() {
  const handleScroll = () => {
    const nextSection = document.getElementById("next-section");
    if (nextSection) {
      document.body.style.overflow = "hidden"; // Temporarily disable scrolling

      nextSection.scrollIntoView({
        behavior: "smooth",
        block: "start",
      });

      setTimeout(() => {
        document.body.style.overflow = "auto"; // Re-enable scrolling
      }, 1000); // Adjust timeout to match your smooth scroll duration
    }
  };

  return (
    <Box
      position="relative"
      data-testid="scroll-down-arrow"
      sx={{
        cursor: "pointer",
        textAlign: "center",
        animation: `${fadeMoveDown} 2s ease-in-out infinite`,
        marginTop: "10px", // Space between the text and the arrow
        "&:hover": {
          animation: "none",
        },
        "&:active": {
          animation: "none",
        },
      }}
      onClick={handleScroll}
    >
      <KeyboardDoubleArrowDownIcon
        sx={{
          fontSize: "clamp(30px, 3vw, 50px)", // Adjusts between 40px and 70px based on viewport width
          color: "#FFF",
          textShadow: "0px 0px 3px rgba(0, 0, 0, 0.4)",
          "&:active": {
            color: "rgba(255, 255, 255, 0.2)",
          },
        }}
      />
    </Box>
  );
}
