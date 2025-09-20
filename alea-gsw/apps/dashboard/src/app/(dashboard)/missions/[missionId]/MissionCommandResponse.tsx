import type { TransformedSpec } from "@aleasat/command-spec";
import { Paper, Typography } from "@mui/material";

function StringTypeResponse({
  response,
}: {
  resp: TransformedSpec[string]["args"][number] & { type: "string" };
  response: string;
}) {
  return <>{response}</>;
}

function NumberTypeResponse({
  response,
}: {
  resp: TransformedSpec[string]["args"][number] & { type: "number" };
  response: string;
}) {
  return <>{response}</>;
}

function DateTypeResponse({
  resp,
  response,
}: {
  resp: TransformedSpec[string]["args"][number] & { type: "date" };
  response: string;
}) {
  const date = resp.deserialize(response);

  return <>{date.toString()}</>;
}

function EmptyComponent({
  response,
}: {
  resp: TransformedSpec[string]["args"][number] & { type: "date" };
  response: string;
}) {
  return <>{response}</>;
}

const responseTypeComponentMapping = {
  string: StringTypeResponse,
  number: NumberTypeResponse,
  boolean: EmptyComponent,
  date: DateTypeResponse,
  buffer: EmptyComponent,
  array: EmptyComponent,
  matrix: EmptyComponent,
};
export default function MissionCommandResponse({
  resp,
  response,
}: {
  resp?: TransformedSpec[string]["args"][number];
  response?: string;
}) {
  if (!resp || !response)
    return (
      <Paper
        variant="outlined"
        sx={{
          padding: 2,
          marginY: 2,
        }}
      ></Paper>
    );

  const Component = responseTypeComponentMapping[resp.type];

  if (!Component) return <></>;

  return (
    <Paper
      variant="outlined"
      sx={{
        padding: 2,
        marginY: 2,
      }}
    >
      <Typography variant="h6" component="h2">
        {resp.name}:
      </Typography>
      {/* @ts-expect-error resp in not never */}
      <Component resp={resp} response={response} />
    </Paper>
  );
}
