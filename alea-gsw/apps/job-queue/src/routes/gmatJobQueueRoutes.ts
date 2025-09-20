import express from "express";
import gmatJobQueueController from "../controllers/gmatJobQueueController.js";

const router: express.Router = express.Router({ mergeParams: true });

router.post("/jobs/gmat", gmatJobQueueController.createGMATJob);
router.get("/jobs/gmat/:jobId", gmatJobQueueController.getGMATJob);

export default router;
