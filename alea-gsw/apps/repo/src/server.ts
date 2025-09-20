import "./instrumentation.js";

import cors from "cors";
import express, { type ErrorRequestHandler } from "express";
import * as OpenApiValidator from "express-openapi-validator";
import fs from "fs/promises";
import swaggerUi, { type JsonObject } from "swagger-ui-express";
import branchRoute from "./routes/branchRoute.js";
import commitRoute from "./routes/commitRoute.js";

// import { jwtDecode } from "jwt-decode";
// import { type UserInfoJwt } from "@aleasat/types";

const app = express();
const port = process.env.PORT ?? 3000;

app.use(cors());
app.use(express.json());
app.use(express.text());
app.use(express.urlencoded({ extended: true }));
const openApiDocument = await fs
  .readFile("./api/openapi.json", "utf-8")
  .then((data) => JSON.parse(data));
app.use(
  "/api-docs",
  swaggerUi.serve,
  swaggerUi.setup({
    ...openApiDocument,
    servers: [
      openApiDocument.servers.find(
        (server: { description: string }) =>
          server.description ===
          (process.env.DEPLOYMENT_ENVIRONMENT ?? "local"),
      ) ?? openApiDocument.servers[0],
    ],
  } as JsonObject),
);
app.get("/healthz", (_req, res) => res.send("OK"));

app.use(
  OpenApiValidator.middleware({
    apiSpec: "./api/openapi.json",
    validateRequests: true, // (default)
    validateResponses: false, // false by default
    validateSecurity: false,
  }),
);

app.use("/commits", commitRoute);
app.use("/branches", branchRoute);

app.use(((err, _req, res, _next) => {
  // format error
  res.status((err.status as number) || 500).json({
    message: err.message,
    errors: err.errors,
  });
}) as ErrorRequestHandler);

app.listen(port, () => console.log("API Magic happening on port " + port));
