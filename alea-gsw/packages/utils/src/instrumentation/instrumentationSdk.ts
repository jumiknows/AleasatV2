import { NodeSDK } from "@opentelemetry/sdk-node";

import { Resource } from "@opentelemetry/resources";
import { ATTR_SERVICE_NAME } from "@opentelemetry/semantic-conventions";

import { OTLPLogExporter } from "@opentelemetry/exporter-logs-otlp-grpc";
import { BatchLogRecordProcessor } from "@opentelemetry/sdk-logs";

import { OTLPTraceExporter } from "@opentelemetry/exporter-trace-otlp-grpc";
import { BatchSpanProcessor } from "@opentelemetry/sdk-trace-base";

import { OTLPMetricExporter } from "@opentelemetry/exporter-metrics-otlp-grpc";
import { PeriodicExportingMetricReader } from "@opentelemetry/sdk-metrics";

import { getNodeAutoInstrumentations } from "@opentelemetry/auto-instrumentations-node";
import prismaInstrumentationPkg from "@prisma/instrumentation";
const { PrismaInstrumentation } = prismaInstrumentationPkg;

export const startInstrumentation = (serviceName: string): void => {
  const sdk = new NodeSDK({
    resource: new Resource({
      [ATTR_SERVICE_NAME]: serviceName,
      "deployment.environment.name":
        process.env.DEPLOYMENT_ENVIRONMENT ?? "development",
    }),
    spanProcessors: [new BatchSpanProcessor(new OTLPTraceExporter())],
    logRecordProcessors: [new BatchLogRecordProcessor(new OTLPLogExporter())],
    metricReader: new PeriodicExportingMetricReader({
      exporter: new OTLPMetricExporter(),
    }),
    instrumentations: [
      getNodeAutoInstrumentations({
        "@opentelemetry/instrumentation-http": {
          ignoreIncomingPaths: ["/healthz"],
        },
      }),
      new PrismaInstrumentation(),
    ],
  });

  sdk.start();
};
