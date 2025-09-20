"use client";

import {
  Configuration,
  LaunchpadApi,
  type WriteLaunchpad,
} from "@aleasat/launchpad-api";
import { Button, Grid2, TextField } from "@mui/material";
import { PageContainer } from "@toolpad/core";
import { useSession } from "next-auth/react";
import {
  type Control,
  Controller,
  type FieldValues,
  type Path,
  useForm,
} from "react-hook-form";
import useSWRMutation from "swr/mutation";

function MuiFormTextInput<T extends FieldValues, V>({
  name,
  control,
}: {
  name: Path<T>;
  control: Control<T, V>;
}) {
  return (
    <Controller
      name={name}
      control={control}
      render={({ field, fieldState }) => (
        <TextField
          {...field}
          error={!!fieldState.error}
          helperText={fieldState.error?.type}
          label={name}
          fullWidth
        />
      )}
      rules={{ required: true }}
    />
  );
}

export default function NewLaunchpadPage() {
  const session = useSession();
  const { control, handleSubmit } = useForm<WriteLaunchpad>({
    defaultValues: {
      serviceName: "",
      name: "",
      node: "",
      port: "",
      fwCommit: "",
    },
  });

  const { trigger } = useSWRMutation(
    ["LaunchpadApi.createLaunchpad", session.data?.accessToken],
    async ([, accessToken], { arg: launchpad }: { arg: WriteLaunchpad }) =>
      await new LaunchpadApi(
        new Configuration({ accessToken }),
      ).createLaunchpad({
        writeLaunchpad: launchpad,
      }),
  );

  if (!session.data?.user.groups.includes("aleasat-lead")) {
    return null;
  }

  return (
    <PageContainer maxWidth={false} title="Add Launchpad">
      <form
        onSubmit={handleSubmit(async (launchpad) => await trigger(launchpad))}
      >
        <Grid2 container spacing={2}>
          <Grid2 size={6}>
            <MuiFormTextInput name="name" control={control} />
          </Grid2>
          <Grid2 size={6}>
            <MuiFormTextInput name="node" control={control} />
          </Grid2>
          <Grid2 size={6}>
            <MuiFormTextInput name="port" control={control} />
          </Grid2>
          <Grid2 size={6}>
            <MuiFormTextInput name="serviceName" control={control} />
          </Grid2>
          <Grid2 size={6}>
            <MuiFormTextInput name="fwCommit" control={control} />
          </Grid2>
          <Grid2 size={12} container sx={{ justifyContent: "center" }}>
            <Button type="submit" variant="outlined">
              Submit
            </Button>
          </Grid2>
        </Grid2>
      </form>
    </PageContainer>
  );
}
