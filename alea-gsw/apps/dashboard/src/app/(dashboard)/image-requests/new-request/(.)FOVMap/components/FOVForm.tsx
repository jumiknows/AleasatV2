"use client";

// React
import { useEffect, useState } from "react";

import Autocomplete, { createFilterOptions } from "@mui/material/Autocomplete";
// Material UI
import Box from "@mui/material/Box";
import CircularProgress from "@mui/material/CircularProgress";
import TextField from "@mui/material/TextField";
import { useSetAtom } from "jotai";
import { targetLocationState } from "../../recoilState";

const FOVForm = () => {
  const setTargetLocation = useSetAtom(targetLocationState);
  const [open, setOpen] = useState(false);
  const [loading, setLoading] = useState(false);
  const [cities, setCities] = useState<
    Array<{ n: string; t: number; g: number; c: string }>
  >([]);

  useEffect(() => {
    if (open && !loading && cities.length === 0) {
      setLoading(true);
      const loadCities = async () => {
        // https://simplemaps.com/data/world-cities
        const c = await fetch("https://artifact.aleasat.space/cities.json")
          .then(
            async (res) =>
              (await res.json()) as Array<{
                n: string;
                t: number;
                g: number;
                c: string;
              }>,
          )
          .catch(() => []);
        setCities(c);
        setLoading(false);
      };
      loadCities();
    }
  }, [loading, open, cities]);

  return (
    <Box
      width={1}
      height={1}
      display="flex"
      flexDirection="row"
      alignItems={"center"}
      justifyContent={"center"}
    >
      <Box p={2}>
        <Autocomplete
          id="asynchronous-demo"
          sx={{ width: 300 }}
          open={open}
          onOpen={() => setOpen(true)}
          onClose={() => setOpen(false)}
          isOptionEqualToValue={(option, value) => option.n === value.n}
          getOptionLabel={(option) => `${option.n}, ${option.c}`}
          // groupBy={(option) => option.c}
          options={cities}
          loading={loading}
          filterOptions={createFilterOptions({
            ignoreCase: true,
            limit: 10,
          })}
          onChange={(_, value) => {
            if (value !== null)
              setTargetLocation({ lat: value.t, lng: value.g });
          }}
          renderInput={(params) => (
            <TextField
              {...params}
              variant="outlined"
              size="small"
              label="Location Search"
              name="searchText"
              InputProps={{
                ...params.InputProps,
                endAdornment: (
                  <>
                    {loading ? (
                      <CircularProgress color="inherit" size={20} />
                    ) : null}
                    {params.InputProps.endAdornment}
                  </>
                ),
              }}
            />
          )}
        />
      </Box>
    </Box>
  );
};

export default FOVForm;
