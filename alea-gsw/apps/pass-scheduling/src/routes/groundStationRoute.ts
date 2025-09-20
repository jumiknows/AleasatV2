import express from "express";
import groundStationController from "../controllers/groundStationController.js";

const router: express.Router = express.Router();

/* GET programming languages. */
router.get("/:groundStationId", groundStationController.getGroundStation);
router.patch("/:groundStationId", groundStationController.patchGroundStation);
router.delete("/:groundStationId", groundStationController.deleteGroundStation);

export default router;
