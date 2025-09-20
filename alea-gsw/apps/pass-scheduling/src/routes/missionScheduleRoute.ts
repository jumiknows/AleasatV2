import express from "express";
import missionScheduleController from "../controllers/missionScheduleController.js";

const router: express.Router = express.Router();

router.get("/missions", missionScheduleController.getScheduledMissions);
router.get(
  "/missions/:missionId",
  missionScheduleController.getScheduledMission,
);
router.get("/spaces", missionScheduleController.getScheduleSpaces);

export default router;
