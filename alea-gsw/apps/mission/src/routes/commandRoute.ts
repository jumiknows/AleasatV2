import express from "express";
import commandController from "../controllers/commandController.js";

const router: express.Router = express.Router({ mergeParams: true });

router.patch("/:commandId", commandController.patchCommand);

export default router;
