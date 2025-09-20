import express from "express";
import groundStationController from "../controllers/groundStationController.js";

const router: express.Router = express.Router();

/* GET programming languages. */
router.get("/", groundStationController.getGroundStations);
router.post("/", groundStationController.addGroundStation);

export default router;
