import { ATTR_SERVICE_NAME } from "@opentelemetry/semantic-conventions";

import { OTLPLogExporter } from "@opentelemetry/exporter-logs-otlp-grpc";
import { BatchLogRecordProcessor } from "@opentelemetry/sdk-logs";

import { OTLPTraceExporter } from "@opentelemetry/exporter-trace-otlp-grpc";
import {
  BatchSpanProcessor,
  SamplingDecision,
} from "@opentelemetry/sdk-trace-base";

import { OTLPMetricExporter } from "@opentelemetry/exporter-metrics-otlp-grpc";
import { PeriodicExportingMetricReader } from "@opentelemetry/sdk-metrics";

import { getNodeAutoInstrumentations } from "@opentelemetry/auto-instrumentations-node";

import { registerOTel } from "@vercel/otel";

import opentelemetry from "@opentelemetry/api";

registerOTel({
  attributes: {
    [ATTR_SERVICE_NAME]: "Aleasat Docs",
    "deployment.environment.name":
      process.env.DEPLOYMENT_ENVIRONMENT ?? "development",
  },
  spanProcessors: [new BatchSpanProcessor(new OTLPTraceExporter())],
  logRecordProcessor: new BatchLogRecordProcessor(new OTLPLogExporter()),
  metricReader: new PeriodicExportingMetricReader({
    exporter: new OTLPMetricExporter(),
  }),
  instrumentations: [
    getNodeAutoInstrumentations({
      "@opentelemetry/instrumentation-http": {
        ignoreIncomingPaths: ["/healthz"],
      },
    }),
  ],
  instrumentationConfig: {
    fetch: {
      propagateContextUrls: ["*"],
    },
  },
  traceSampler: {
    shouldSample: (
      context,
      _traceId,
      spanName,
      _spanKind,
      _attributes,
      _links,
    ) => {
      if (
        spanName === "GET /favicon.ico" ||
        spanName === "GET /healthz" ||
        // spanName === "GET /healthz/route" ||
        // spanName === "GET /favicon--route-entry" ||
        //@ts-expect-error it there fore next.js
        opentelemetry.trace.getSpan(context)?.name === "GET /favicon.ico" ||
        //@ts-expect-error it there fore next.js
        opentelemetry.trace.getSpan(context)?.name === "GET /healthz"
      ) {
        return {
          decision: SamplingDecision.RECORD,
        };
      }

      return {
        decision: SamplingDecision.RECORD_AND_SAMPLED,
      };
    },
    toString: () => "Aleasat Docs Sampler",
  },
});
