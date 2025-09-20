import Button from "@mui/material/Button";
import Typography from "@mui/material/Typography";
import { motion } from "framer-motion";
import type { ReactNode } from "react";

interface TagProps {
  onClick: () => void;
  children: ReactNode;
  isActive: boolean;
  category: string;
}

export default function Tag({
  children,
  isActive,
  onClick,
  category,
}: TagProps) {
  const filterName = category ? category.toLowerCase() : "";

  return (
    <Button
      onClick={onClick}
      className="my-button"
      disableRipple
      data-cy={`filter-${filterName}`}
      data-active={isActive}
      sx={{
        borderRadius: "10px",
      }}
      id={children ? children.toString() : ""}
    >
      <Typography
        color="white"
        zIndex={1}
        position="relative"
        fontSize={{ xs: 13, sm: 18, md: 20 }}
      >
        {children}
      </Typography>
      {isActive && (
        <motion.span
          layoutId="tag"
          transition={{ duration: 0.3, type: "spring" }}
          style={{
            height: "100%",
            position: "absolute",
            inset: 0,
            zIndex: 0,
            border: "1px solid white",
            borderRadius: "10px",
          }}
        />
      )}
    </Button>
  );
}
