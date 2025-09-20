import express from "express";
import jobQueueController from "../controllers/jobQueueController.js";

const router: express.Router = express.Router({ mergeParams: true });

router.delete("/jobs/:jobId", jobQueueController.deleteJob);

export default router;
