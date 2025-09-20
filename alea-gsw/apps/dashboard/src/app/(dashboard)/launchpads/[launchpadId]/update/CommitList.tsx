import { Configuration, RepoApi } from "@aleasat/repo-api";
import { Autocomplete, CircularProgress, TextField } from "@mui/material";
import { useSession } from "next-auth/react";
import { type Dispatch, type SetStateAction, useEffect } from "react";
import useSWR from "swr";

export default function CommitList({
  branch,
  commit,
  setCommit,
}: {
  branch: string | undefined;
  commit: string | undefined;
  setCommit: Dispatch<SetStateAction<string | undefined>>;
}) {
  const session = useSession();

  const {
    data: commits,
    error,
    isLoading,
  } = useSWR(
    session.data && branch
      ? ["RepoApi.getBranchCommits", session.data.accessToken, branch]
      : undefined,
    async ([_method, accessToken, branchName]) =>
      await new RepoApi(new Configuration({ accessToken }))
        .getBranchCommits({ branchName })
        .catch((e) => {
          e.status = e.response.status;
          throw e;
        }),
  );

  useEffect(() => {
    if (!commits) return;
    setCommit((oldCommit) => {
      if (!oldCommit) return commits[0].hash;
      if (!commits.find((commit) => commit.hash === oldCommit))
        return commits[0].hash;
      return oldCommit;
    });
  }, [commits]);

  return (
    <div>
      <Autocomplete
        options={commits?.map((commit) => commit.hash) ?? []}
        loading={isLoading}
        getOptionLabel={(option) => {
          const commit = commits?.find(
            (commit) => commit.hash === option,
          )?.message;
          return commit ? `${option.slice(0, 8)} ${commit}` : "";
        }}
        disableClearable
        onChange={(_, value) => {
          setCommit(value);
        }}
        value={commit ?? ""}
        defaultValue={commit ?? ""}
        renderInput={(params) => (
          <TextField
            {...params}
            label="Commit"
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
    </div>
  );
}
