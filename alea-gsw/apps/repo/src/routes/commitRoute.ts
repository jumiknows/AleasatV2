import express from "express";
import commitController from "../controllers/commitController.js";

const router: express.Router = express.Router();

router.get("/", commitController.getCommits);
router.get("/:commitHash/file", commitController.getFile);
router.get("/:commitHash/obc-config", commitController.getObcConfig);
router.get("/:commitHash/obc-proto", commitController.getObcProto);
router.get("/:commitHash/obc-grpc-service", commitController.getObcGrpcService);

export default router;
