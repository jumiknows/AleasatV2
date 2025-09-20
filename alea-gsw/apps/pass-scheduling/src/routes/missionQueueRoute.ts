import express from "express";
import missionQueueController from "../controllers/missionQueueController.js";

const router: express.Router = express.Router();

/* GET programming languages. */
router.post("/", missionQueueController.queueMission);
router.get("/", missionQueueController.getQueuedMissions);

export default router;
