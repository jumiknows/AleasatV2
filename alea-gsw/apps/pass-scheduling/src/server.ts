import "./instrumentation.js";

import fs from "fs";
import cors from "cors";
import express, { type ErrorRequestHandler } from "express";
import * as OpenApiValidator from "express-openapi-validator";
import swaggerUi, { type JsonObject } from "swagger-ui-express";
import groundStationRoute from "./routes/groundStationRoute.js";
import groundStationsRoute from "./routes/groundStationsRoute.js";
import missionQueueRoute from "./routes/missionQueueRoute.js";
import missionScheduleRoute from "./routes/missionScheduleRoute.js";
import webHooksRoute from "./routes/webhooksRoute.js";
import logger from "./utils/logger.js";

const app = express();
const port = process.env.PORT ?? 3000;

app.use(cors());
app.use(express.json());
app.use(express.text());
app.use(express.urlencoded({ extended: true }));

const openApiDocument = fs.readFileSync("./api/openapi.json", "utf-8");
app.use(
  "/api-docs",
  swaggerUi.serve,
  swaggerUi.setup(JSON.parse(openApiDocument) as JsonObject),
);
app.get("/openapi", (_req, res) => res.sendFile("../api/openapi.json"));
app.get("/healthz", (_req, res) => res.send("OK"));
app.use(
  OpenApiValidator.middleware({
    apiSpec: "./api/openapi.json",
    validateRequests: true, // (default)
    validateResponses: true, // false by default
    validateSecurity: false,
  }),
);

app.use("/webhooks", webHooksRoute);
app.use("/ground-station", groundStationRoute);
app.use("/ground-stations", groundStationsRoute);
app.use("/mission-queue", missionQueueRoute);
app.use("/mission-schedule", missionScheduleRoute);

app.use(((err, _req, res, _next) => {
  // format error
  res.status((err.status as number) || 500).json({
    message: err.message,
    errors: err.errors,
  });
}) as ErrorRequestHandler);

app.listen(port, () => logger.info("API Magic happening on port " + port));
