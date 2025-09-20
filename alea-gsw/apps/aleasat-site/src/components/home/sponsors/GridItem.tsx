import Image from "@aleasat/ui/components/Image";
import Grid, { type Grid2Props } from "@mui/material/Grid2";
import Paper from "@mui/material/Paper";

interface GridItemProps extends Grid2Props {
  src: string;
  taller?: boolean;
}

export default function GridItem({
  src,
  taller = false,
  ...rest
}: GridItemProps) {
  return (
    <Grid {...rest}>
      <Paper
        elevation={0}
        sx={{
          borderRadius: 0,
          position: "relative",
          width: "100%",
          height: "100%",
          textAlign: "center",
          background: "#6A868C26",
        }}
      >
        <Image
          src={src}
          alt={src}
          sx={{
            height: taller ? { xs: 200, sm: 207 } : 100,
            p: { xs: 0.1, sm: 1 },
          }}
        />
      </Paper>
    </Grid>
  );
}
