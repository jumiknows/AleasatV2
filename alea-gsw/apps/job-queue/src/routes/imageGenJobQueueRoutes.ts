import express from "express";
import imageGenJobQueueController from "../controllers/imageGenJobQueueController.js";

const router: express.Router = express.Router({ mergeParams: true });

router.post(
  "/jobs/image-generation",
  imageGenJobQueueController.createImageGenJob,
);
router.get(
  "/jobs/image-generation/:jobId",
  imageGenJobQueueController.getImageGenJob,
);

export default router;
