import express from "express";
import branchController from "../controllers/branchController.js";

const router: express.Router = express.Router();

router.get("/", branchController.getBranches);
router.get("/:branchName/commits", branchController.getBranchCommits);

export default router;
