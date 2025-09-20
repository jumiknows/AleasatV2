"use client";

import { Configuration, LaunchpadApi } from "@aleasat/launchpad-api";
import { Alert, Box, Paper, Typography } from "@mui/material";
import type { Session } from "next-auth";
import { notFound } from "next/navigation";
import React, { useState, useRef, useEffect } from "react";
import useSWRImmutable from "swr/immutable";

export default function FirmwareUpdateLog({
  jobName,
  session,
}: {
  jobName: string;
  session: Session;
}) {
  const innerRef = useRef<HTMLDivElement>(null);
  const [messageHistory, setMessageHistory] = useState("");
  const [scrollToBottom, setScrollToBottom] = useState(true);

  useEffect(() => {
    if (!innerRef.current) return;

    if (scrollToBottom) {
      innerRef.current.scrollTop =
        innerRef.current.scrollHeight - innerRef.current.clientHeight;
    }
  }, [messageHistory]);

  const { error, isLoading } = useSWRImmutable(
    [jobName, session.accessToken],
    async ([jobName, accessToken]) => {
      return await new LaunchpadApi(
        new Configuration({
          accessToken,
        }),
      )
        .getLaunchpadFirmwareLogRaw({
          jobName,
          launchpadId: "62b884a2-e8f8-403e-9ec9-2c88c6d23330",
        })
        .then(async (res) => {
          if (!res.raw.ok) setMessageHistory(await res.raw.text());
          const reader = res.raw.body!.getReader();

          let done = false;
          let value: Uint8Array | undefined;
          while (!done) {
            ({ value, done } = await reader.read());
            if (value)
              setMessageHistory(
                (prev) => prev + new TextDecoder().decode(value),
              );
          }
        })
        .catch((e) => {
          e.status = e.response.status;
          throw e;
        });
    },
    {
      onErrorRetry: (error, _, config, revalidate, opts): void => {
        if (error.status === 404) {
          return;
        }

        const maxRetryCount = config.errorRetryCount;
        const currentRetryCount = opts.retryCount;

        // Exponential backoff
        const timeout =
          ~~(
            (Math.random() + 0.5) *
            (1 << (currentRetryCount < 8 ? currentRetryCount : 8))
          ) * config.errorRetryInterval;

        if (
          typeof maxRetryCount !== "undefined" &&
          currentRetryCount > maxRetryCount
        ) {
          return;
        }

        setTimeout(revalidate, timeout, opts);
      },
      errorRetryCount: 0,
    },
  );

  if (!messageHistory && error?.status === 404) {
    notFound();
  }

  return (
    <>
      <Typography variant="h5">Firmware Update Log</Typography>
      <Box sx={{ marginY: 2 }}>
        {!messageHistory && error ? (
          <Alert severity="error">{error.message}</Alert>
        ) : isLoading ? (
          messageHistory ? (
            <Alert severity="info">Running</Alert>
          ) : (
            <Alert severity="warning">Pending</Alert>
          )
        ) : (
          <Alert severity="success">Update Success</Alert>
        )}
      </Box>
      <Paper
        sx={{
          padding: 2,
        }}
        variant="outlined"
      >
        <Box
          sx={{
            height: "60vh",
            overflowY: "scroll",
            overflowX: "hidden",
          }}
          ref={innerRef}
          onScroll={(e) => {
            setScrollToBottom(
              e.currentTarget.scrollTop + e.currentTarget.offsetHeight ===
                e.currentTarget.scrollHeight,
            );
          }}
        >
          <Typography sx={{ whiteSpace: "pre-line" }}>
            {messageHistory}
          </Typography>
        </Box>
      </Paper>
    </>
  );
}
