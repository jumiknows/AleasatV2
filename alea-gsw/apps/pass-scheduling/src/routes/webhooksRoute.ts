import express from "express";
import webhooksController from "../controllers/webhooksController.js";

const router: express.Router = express.Router();

router.post(
  "/new-orbit-propagation",
  webhooksController.webhooksNewOrbitPropagation,
);

export default router;
