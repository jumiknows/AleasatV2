import express from "express";
import launchpadController from "../controllers/launchpadController.js";

const router: express.Router = express.Router({ mergeParams: true });

router.get("/launchpads", launchpadController.getLaunchpads);

router.post("/launchpads", launchpadController.createLaunchpad);

router.get("/launchpads/:launchpadId", launchpadController.getLaunchpad);

router.patch("/launchpads/:launchpadId", launchpadController.patchLaunchpad);

router.delete("/launchpads/:launchpadId", launchpadController.deleteLaunchpad);

router.get(
  "/launchpads/:launchpadId/userSession",
  launchpadController.getLaunchpadUserSession,
);
router.post(
  "/launchpads/:launchpadId/userSession",
  launchpadController.createLaunchpadUserSession,
);
router.put(
  "/launchpads/:launchpadId/firmware",
  launchpadController.updateLaunchpadFirmware,
);

router.get(
  "/launchpads/:launchpadId/firmware/log",
  launchpadController.getLaunchpadFirmwareLog,
);

export default router;
