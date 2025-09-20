"use client";

import { Configuration, GroundStationApi } from "@aleasat/pass-scheduling-api";
import CircularProgress from "@mui/material/CircularProgress";
import { useSession } from "next-auth/react";
import { env } from "next-runtime-env";
import useSWR from "swr";

import Autocomplete from "@mui/material/Autocomplete";
import TextField from "@mui/material/TextField";
import { useAtom } from "jotai";
import type { Session } from "next-auth";
import { useState } from "react";
import { AROGroundStationSelectedState } from "../../recoilState";

const groundStationsFetcher = async (session: Session) =>
  await new GroundStationApi(
    new Configuration({
      accessToken: session.accessToken,
    }),
  ).getGroundStations({
    ownerId: session.user.id,
  });

export default function AROGroundStationSelect() {
  const session = useSession();
  const {
    data: groundStations,
    isLoading,
    error,
  } = useSWR(session.data, async (key) => await groundStationsFetcher(key));

  const [AROGroundStationSelected, setAROGroundStationSelected] = useAtom(
    AROGroundStationSelectedState,
  );
  const [open, setOpen] = useState(false);

  return (
    <Autocomplete
      open={open}
      onOpen={() => setOpen(true)}
      onClose={() => setOpen(false)}
      isOptionEqualToValue={(option, value) => option.id === value.id}
      getOptionLabel={(option) => option.name}
      options={groundStations ?? []}
      loading={isLoading}
      fullWidth
      defaultValue={AROGroundStationSelected}
      noOptionsText={error ? "Error: please try again later" : "No options"}
      onChange={(_event, value, _reason, _details) => {
        if (value !== null) setAROGroundStationSelected(value);
        else
          setAROGroundStationSelected({
            id: -1,
            name: "",
            lat: 0,
            lng: 0,
          });
      }}
      renderInput={(params) => (
        <TextField
          {...params}
          error={error}
          label="Extra Ground Station (Optional)"
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
}
