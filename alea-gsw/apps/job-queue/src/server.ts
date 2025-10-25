import "./instrumentation.js";

import fs from "fs";
import cors from "cors";
import express, { type ErrorRequestHandler } from "express";
import * as OpenApiValidator from "express-openapi-validator";
import swaggerUi, { type JsonObject } from "swagger-ui-express";
import { Queues } from "./bullmq/queues.js";
import gmatJobQueueRoutes from "./routes/gmatJobQueueRoutes.js";
import jobQueueRoutes from "./routes/jobQueueRoutes.js";
import orbitPropJobQueueRoutes from "./routes/orbitPropJobQueueRoutes.js";

// import { jwtDecode } from "jwt-decode";
// import { type UserInfoJwt } from "@aleasat/types";

const app = express();
const port = process.env.PORT ?? 3000;

Queues.init();

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
    validateResponses: false, // false by default
    validateSecurity: false,
  }),
);

app.use("/", jobQueueRoutes);
app.use("/", gmatJobQueueRoutes);
app.use("/", orbitPropJobQueueRoutes);

app.use(((err, _req, res, _next) => {
  // format error
  res.status((err.status as number) || 500).json({
    message: err.message,
    errors: err.errors,
  });
}) as ErrorRequestHandler);

app.listen(port, () => console.log("API Magic happening on port " + port));
