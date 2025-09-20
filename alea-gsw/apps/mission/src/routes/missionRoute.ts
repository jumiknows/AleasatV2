import express from "express";
import missionController from "../controllers/missionController.js";
import commandRoute from "./commandRoute.js";

const router: express.Router = express.Router();
router.use("/:missionId/commands", commandRoute);

router.get("/", missionController.getMissions);
router.get("/:missionId", missionController.getMission);
router.patch("/:missionId", missionController.patchMission);
router.post("/", missionController.createMission);

export default router;
