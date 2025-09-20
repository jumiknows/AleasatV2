import { Configuration, RepoApi } from "@aleasat/repo-api";
import { Autocomplete, CircularProgress, TextField } from "@mui/material";
import { useSession } from "next-auth/react";
import { type Dispatch, type SetStateAction, useEffect } from "react";
import useSWR from "swr";

export default function BranchList({
  branch,
  setBranch,
}: {
  branch: string | undefined;
  setBranch: Dispatch<SetStateAction<string | undefined>>;
}) {
  const session = useSession();

  const {
    data: branches,
    error,
    isLoading,
  } = useSWR(
    session.data && ["RepoApi.getBranches", session.data.accessToken],
    async ([, accessToken]) =>
      await new RepoApi(new Configuration({ accessToken }))
        .getBranches()
        .catch((e) => {
          console.error(e);
          return undefined;
        }),
  );

  useEffect(() => {
    if (!branches) return;
    setBranch((oldBranch) => {
      if (!oldBranch) return branches[0];
      return oldBranch;
    });
  }, [branches]);

  return (
    <Autocomplete
      options={branches?.slice(1) ?? []}
      loading={isLoading}
      getOptionLabel={(option) =>
        option.startsWith("remotes/origin/") ? option.slice(15) : option
      }
      disableClearable
      value={branch ?? ""}
      defaultValue={branch ?? ""}
      onChange={(_, value) => {
        setBranch(value);
      }}
      renderInput={(params) => (
        <TextField
          {...params}
          label="Branch"
          slotProps={{
            input: {
              ...params.InputProps,
              endAdornment: (
                <>
                  {isLoading ? (
                    <CircularProgress color="inherit" size={20} />
                  ) : null}
                  {params.InputProps.endAdornment}
                </>
              ),
            },
          }}
          error={!!error}
          helperText={error?.message}
        />
      )}
    />
  );
}
