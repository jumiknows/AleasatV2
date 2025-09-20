import express from "express";
import orbitPropagationController from "../controllers/orbitPropagationController.js";

const router: express.Router = express.Router();

/* GET programming languages. */
router.get("/ground-track", orbitPropagationController.getGroundTrack);
router.get("/passes", orbitPropagationController.findPasses);
router.get("/full-passes", orbitPropagationController.findFullPasses);

export default router;
