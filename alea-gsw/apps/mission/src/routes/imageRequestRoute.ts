import express, { type Router } from "express";
import imageController from "../controllers/imageController.js";
import commandRoute from "./commandRoute.js";

const router: Router = express.Router();

router.get("/", imageController.getImageRequests);
router.get("/:id", imageController.getImageRequestById);
router.patch("/:id", imageController.updateImageRequest);
router.delete("/:id", imageController.deleteImageRequest);
router.post("/", imageController.createImageRequest);

export default router;
