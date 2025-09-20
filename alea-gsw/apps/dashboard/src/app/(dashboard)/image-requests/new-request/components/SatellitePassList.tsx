"use client";

import {
  Configuration,
  type FindPassesRequest,
  OrbitPropagationApi,
} from "@aleasat/orbit-propagation-api";
import SatelliteIcon from "@mui/icons-material/Satellite";
import Box from "@mui/material/Box";
import CircularProgress from "@mui/material/CircularProgress";
import List from "@mui/material/List";
import ListItemButton from "@mui/material/ListItemButton";
import ListItemIcon from "@mui/material/ListItemIcon";
import ListItemText from "@mui/material/ListItemText";
import ListSubheader from "@mui/material/ListSubheader";
import Paper from "@mui/material/Paper";
import Typography from "@mui/material/Typography";
import dayjs from "dayjs";
import { useSession } from "next-auth/react";
import { env } from "next-runtime-env";
import { type Dispatch, type SetStateAction, useEffect } from "react";
import useSWR from "swr";

export interface Pass<T = Date> {
  id: number;
  rise: { t: T };
  culmination: {
    t: T;
    alt: number;
    az: number;
    lat: number;
    lng: number;
    height: number;
  };
  set: { t: T };
}

const passesFetcher = async (params: FindPassesRequest, accessToken: string) =>
  await new OrbitPropagationApi(
    new Configuration({
      accessToken,
    }),
  ).findPasses(params);

export default function Page(props: {
  targetLocation?: { lat: number; lng: number };
  timeRange: {
    startDate: string;
    endDate: string;
  };
  selectedPassValueSetter: [Pass, Dispatch<SetStateAction<Pass>>];
}) {
  const { targetLocation, timeRange, selectedPassValueSetter } = props;
  const [selectedPass, setSelectedPass] = selectedPassValueSetter;
  const session = useSession();
  const { data, isLoading, error } = useSWR(
    session.data?.accessToken && targetLocation
      ? {
          latitude: targetLocation.lat,
          longitude: targetLocation.lng,
          minElevation: 30.0,
          start: new Date(timeRange.startDate),
          end: new Date(timeRange.endDate),
        }
      : null,
    async (key) => await passesFetcher(key, session.data!.accessToken),
  );

  useEffect(() => {
    if (data && data.passes.length > 0 && selectedPass.id === -1)
      setSelectedPass(data.passes[0]);
  }, [data, selectedPass]);

  if (isLoading || !data || error)
    return (
      <Box
        width={1}
        height={1}
        display={"flex"}
        flexDirection={"row"}
        justifyContent={"center"}
        alignItems={"center"}
      >
        <CircularProgress />
        {error && <Typography>Error! Retrying</Typography>}
      </Box>
    );

  const passes = data.passes.map((pass) => {
    return {
      id: pass.id,
      latitude: pass.culmination.lat,
      longitude: data.longitude,
      rise: { t: dayjs(pass.rise.t) },
      culmination: {
        t: dayjs(pass.culmination.t),
        alt: pass.culmination.alt,
        az: pass.culmination.az,
        lat: pass.culmination.lat,
        lng: pass.culmination.lng,
        height: pass.culmination.height,
      },
      set: { t: dayjs(pass.set.t) },
    };
  });

  passes.sort((a, b) => {
    if (a.rise.t.isBefore(b.rise.t)) {
      return -1;
    } else if (a.rise.t.isAfter(b.rise.t)) {
      return 1;
    } else {
      return 0;
    }
  });

  // Group passes by date
  const passMap = new Map<dayjs.Dayjs, Pass<dayjs.Dayjs>[]>();
  let currentDate;
  for (const pass of passes) {
    const newDate = pass.rise.t;
    if (!currentDate?.isSame(newDate, "day")) {
      currentDate = newDate.startOf("day");
      passMap.set(currentDate, []);
    }

    passMap.get(currentDate)?.push(pass);
  }

  return (
    <Paper variant="outlined" style={{ backgroundColor: "#000" }}>
      <Box flexBasis={0} flexGrow={1} overflow="auto">
        <List subheader={<li />}>
          {Array.from(passMap.entries()).map(([date, passes], dateIndex) => (
            <li key={`date-${dateIndex}`}>
              <ul style={{ padding: 0 }}>
                <ListSubheader sx={{ lineHeight: "36px" }}>
                  {date.format("YYYY-MM-DD")}
                </ListSubheader>
                {passes.map((pass, passIndex) => {
                  return (
                    <ListItemButton
                      sx={{ pt: 0, pb: 0 }}
                      key={`pass-${dateIndex}-${passIndex}`}
                      selected={pass.id === selectedPass.id}
                      onClick={() =>
                        setSelectedPass(
                          data.passes.find((o) => o.id === pass.id)!,
                        )
                      }
                    >
                      <ListItemIcon sx={{ minWidth: 0, pr: 2 }}>
                        <SatelliteIcon />
                      </ListItemIcon>
                      <ListItemText
                        disableTypography={true}
                        primary={
                          <Box
                            display={"flex"}
                            alignItems={"center"}
                            m={0}
                            p={0}
                          >
                            <Typography fontWeight={"bold"}>
                              {pass.culmination.t.format("HH:mm:ss")}
                            </Typography>
                            <Typography
                              variant={"overline"}
                              color={"text.secondary"}
                              ml={1}
                            >
                              (GMT{pass.culmination.t.format("ZZ")})
                            </Typography>
                          </Box>
                        }
                        secondary={
                          <Box
                            display={"flex"}
                            alignItems={"center"}
                            m={0}
                            p={0}
                          >
                            <Typography
                              variant={"caption"}
                              color={"text.primary"}
                              width={"100px"}
                            >
                              Elevation: {Math.round(pass.culmination.alt)}&deg;
                            </Typography>
                            <Typography
                              variant={"caption"}
                              color={"text.primary"}
                              flexGrow={1}
                            >
                              Heading: {Math.round(pass.culmination.az)}&deg;
                            </Typography>
                          </Box>
                        }
                      />
                    </ListItemButton>
                  );
                })}
              </ul>
            </li>
          ))}
        </List>
      </Box>
    </Paper>
  );
}
