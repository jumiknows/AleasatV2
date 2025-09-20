import express from "express";
import webhooksController from "../controllers/webhooksController.js";

const router: express.Router = express.Router();

router.post(
  "/new-gmat-orbit-propagation",
  webhooksController.webhooksNewGmatOrbitPropagation,
);

export default router;
