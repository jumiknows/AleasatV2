"use client";

import {
  Configuration,
  type FindPassesRequest,
  OrbitPropagationApi,
} from "@aleasat/orbit-propagation-api";
import Autocomplete from "@mui/material/Autocomplete";
import CircularProgress from "@mui/material/CircularProgress";
import TextField from "@mui/material/TextField";
import { useAtom, useAtomValue } from "jotai";
import { useSession } from "next-auth/react";
import { env } from "next-runtime-env";
import { useEffect, useState } from "react";
import useSWR from "swr";
import {
  AROGroundStationPassState,
  AROGroundStationSelectedState,
  selectedPassState,
} from "../../recoilState";

const passesFetcher = async (params: FindPassesRequest, accessToken: string) =>
  await new OrbitPropagationApi(
    new Configuration({
      accessToken,
    }),
  ).findPasses(params);

export default function AROGroundStationPassSelect() {
  const session = useSession();

  const AROGroundStationSelected = useAtomValue(AROGroundStationSelectedState);

  const selectedPass = useAtomValue(selectedPassState);
  const [AROGroundStationPass, setAROGroundStationPass] = useAtom(
    AROGroundStationPassState,
  );
  const {
    data: passes,
    isLoading,
    error,
  } = useSWR(
    session.data?.accessToken && AROGroundStationSelected.id !== -1
      ? {
          latitude: AROGroundStationSelected.lat,
          longitude: AROGroundStationSelected.lng,
          minElevation: 30.0,
          start: selectedPass.set.t,
          end: new Date(
            new Date(selectedPass.set.t).getTime() + 1000 * 60 * 60 * 24 * 10,
          ),
        }
      : null,
    async (key) => await passesFetcher(key, session.data!.accessToken),
  );

  const [open, setOpen] = useState(false);

  useEffect(() => {
    if (AROGroundStationSelected.id === -1) {
      setAROGroundStationPass({
        ...AROGroundStationPass,
        id: -1,
      });
    }
  }, [AROGroundStationSelected]);

  return (
    <Autocomplete
      open={open}
      onOpen={() => setOpen(true)}
      onClose={() => setOpen(false)}
      isOptionEqualToValue={(option, value) => option.id === value?.id}
      getOptionLabel={(option) =>
        `${new Date(option.rise.t).toLocaleString()} to ${new Date(
          option.set.t,
        ).toLocaleString()}`
      }
      noOptionsText={error ? "Error: please try again later" : "No options"}
      options={passes?.passes ?? []}
      loading={isLoading}
      fullWidth
      value={AROGroundStationPass.id !== -1 ? AROGroundStationPass : null}
      onChange={(_event, value, _reason, _details) => {
        if (value !== null) setAROGroundStationPass(value);
        else
          setAROGroundStationPass({
            ...AROGroundStationPass,
            id: -1,
          });
      }}
      renderInput={(params) => (
        <TextField
          {...params}
          label="Downlink Time (Optional)"
          InputProps={{
            ...params.InputProps,
            endAdornment: (
              <>
                {isLoading && <CircularProgress color="inherit" size={20} />}
                {params.InputProps.endAdornment}
              </>
            ),
          }}
        />
      )}
    />
  );
  // <TextField
  //   label="Ground Station"
  //   select
  //   onChange={handleChange}
  //   value={AROGroundStationSelected.id}
  //   fullWidth
  //   size="small"
  // >
  //   <MenuItem value="">None</MenuItem>
  //   {groundStations?.map((groundStation, i) => (
  //     <MenuItem value={groundStation.id} key={i}>
  //       {groundStation.name}
  //     </MenuItem>
  //   ))}
  // </TextField>
}
