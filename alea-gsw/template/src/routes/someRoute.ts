import express from "express";
import someController from "../controllers/someController.js";

const router: express.Router = express.Router();

router.get("/:someId", someController.getSome);

export default router;
