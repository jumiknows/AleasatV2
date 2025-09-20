import express from "express";
import missionExecutionController from "../controllers/missionExecutionController.js";

const router: express.Router = express.Router();

router.post("/schedule", missionExecutionController.scheduleMissionExecution);

export default router;
