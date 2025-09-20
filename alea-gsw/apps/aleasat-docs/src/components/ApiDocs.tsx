"use client";

import { createScalarReferences } from "@scalar/api-reference";
import { useSession } from "next-auth/react";
import { useState } from "react";
import { useEffect } from "react";

interface Props {
  apiSpecs: string[];
}

export default function ApiDocs({ apiSpecs }: Props) {
  const session = useSession();
  const [referenceApp, setReferenceApp] = useState<null | ReturnType<
    typeof createScalarReferences
  >>(null);
  const [selectedServer, setSelectedServer] = useState<string | null>(null);

  useEffect(() => {
    const el = document.getElementById("app");

    const app = createScalarReferences(el, {
      authentication: {
        preferredSecurityScheme: "AccessToken",
        http: {
          bearer: {
            token: session.data?.accessToken,
          },
        },
      },

      spec: {
        //@ts-expect-error its just added
        sources: apiSpecs.map((spec) => ({
          title: JSON.parse(spec).info.title as string,
          content: spec,
        })),
      },

      defaultHttpClient: {
        targetKey: "node",
        clientKey: "fetch",
      },
      hiddenClients: [
        "libcurl",
        "clj_http",
        "httpclient",
        "restsharp",
        "native",
        "http1.1",
        "asynchttp",
        "nethttp",
        "okhttp",
        "unirest",
        "xhr",
        "axios",
        "jquery",
        "okhttp",
        "native",
        "request",
        "unirest",
        "axios",
        "nsurlsession",
        "cohttp",
        "curl",
        "guzzle",
        "http1",
        "http2",
        "webrequest",
        "restmethod",
        "undici",
        "httr",
        "wget",
        "httpie",
        "ofetch",
        "http",
      ],
    });

    setReferenceApp(app);

    return () => {
      app.unmount();
    };
  }, []);

  useEffect(() => {
    if (!referenceApp) return;

    referenceApp.updateConfig(
      {
        onServerChange: setSelectedServer,
        authentication: {
          preferredSecurityScheme: "AccessToken",
          http: {
            bearer: {
              token: session.data?.accessToken,
            },
          },
        },
      },
      true,
    );
  }, [session, referenceApp]);

  useEffect(() => {
    if (!referenceApp) return;

    referenceApp.updateSpec({
      //@ts-expect-error its just added
      sources: apiSpecs.map((spec) => {
        const parsed = JSON.parse(spec);

        if (parsed.servers) {
          const index = parsed.servers.findIndex(
            (server: { url: string }) => server.url === selectedServer,
          );
          if (index !== -1) {
            parsed.servers.unshift(parsed.servers.splice(index, 1)[0]);
          }
        }

        return {
          title: parsed.info.title as string,
          content: JSON.stringify(parsed),
        };
      }),
    });
  }, [selectedServer, referenceApp]);

  return (
    <>
      <div id="app"></div>
    </>
  );
}
