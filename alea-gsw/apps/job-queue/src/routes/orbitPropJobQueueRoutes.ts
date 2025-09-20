import express from "express";
import orbitPropJobQueueController from "../controllers/orbitPropJobQueueController.js";

const router: express.Router = express.Router({ mergeParams: true });

router.post(
  "/jobs/orbit-propagation",
  orbitPropJobQueueController.createOrbitPropJob,
);
router.get(
  "/jobs/orbit-propagation/:jobId",
  orbitPropJobQueueController.getOrbitPropJob,
);

export default router;
