import { Grid2, Stack, Typography } from "@mui/material";
import { title } from "process";

export default function TeamStatsSection() {
  const stats = [
    {
      title: "Members",
      value: "300+",
    },
    {
      title: "Subsystems",
      value: "10",
    },
    // {
    //   title: "Student Years",
    //   value: "Year 1 - Year 5",
    // },
    {
      title: "Nationalities",
      value: "10+",
    },
    {
      title: "Programs",
      value: "20+",
    },
  ];

  return (
    <>
      <Typography
        variant="h2"
        sx={{
          textAlign: "center",
          mt: 20,
        }}
      >
        Team Statistics
      </Typography>
      <Grid2
        container
        spacing={4}
        sx={{
          mt: 10,
          mb: 20,
        }}
      >
        {stats.map((stat, i) => (
          <Grid2 key={i} size={{ xs: 6, lg: 3 }}>
            <Stack
              spacing={1}
              alignItems="center"
              sx={{
                width: "100%",
                p: 2,
              }}
            >
              <Typography variant="h4">{stat.value}</Typography>
              <Typography variant="h4">{stat.title}</Typography>
            </Stack>
          </Grid2>
        ))}
      </Grid2>
    </>
  );
}
