"use client";

import Button from "@mui/material/Button";
import Dialog from "@mui/material/Dialog";
import DialogActions from "@mui/material/DialogActions";
import DialogContent from "@mui/material/DialogContent";
import DialogContentText from "@mui/material/DialogContentText";
import DialogTitle from "@mui/material/DialogTitle";
import Stack from "@mui/material/Stack";
import { useAtomValue } from "jotai";
import { useRouter } from "next/navigation";
import { useEffect, useState } from "react";
import { useFormState } from "react-dom";
import {
  AROGroundStationPassState,
  AROGroundStationSelectedState,
  ImageRequestNameAndDescriptionState,
  SatVizRotationState,
  selectedPassState,
} from "../../recoilState";
import { updateWorkingImageRequest } from "../actions";
import ValidationStatus from "./VaildationStatus";

export default function ReviewDetials(props: {
  open: boolean;
  handleClose: () => void;
  setErrorMessage: (errorMessage: string) => void;
}) {
  const router = useRouter();
  const { open, handleClose, setErrorMessage } = props;
  const selectedPass = useAtomValue(selectedPassState);
  const SatVizRotation = useAtomValue(SatVizRotationState);
  const AROGroundStationSelected = useAtomValue(AROGroundStationSelectedState);
  const AROGroundStationPass = useAtomValue(AROGroundStationPassState);
  const imageRequestNameAndDescription = useAtomValue(
    ImageRequestNameAndDescriptionState,
  );
  const [updateWorkingImageRequestState, updateWorkingImageRequestAction] =
    useFormState(updateWorkingImageRequest, {
      loading: true,
      success: true,
      imageRequestId: "",
    });
  const [submited, setSubmited] = useState(false);

  const submit = () => {
    setSubmited(true);
    updateWorkingImageRequestAction({
      name: imageRequestNameAndDescription.name,
      description: imageRequestNameAndDescription.description,
      satelliteLocationLat: selectedPass.culmination.lat,
      satelliteLocationLng: selectedPass.culmination.lng,
      satelliteLocationT: new Date(selectedPass.culmination.t).toISOString(),
      satelliteRotationX: SatVizRotation.x,
      satelliteRotationY: SatVizRotation.y,
      satelliteRotationZ: SatVizRotation.z,
      AROgroundStationId: AROGroundStationSelected.id,
      AROgroundStationPassRise: new Date(
        AROGroundStationPass.rise.t,
      ).toISOString(),
      AROgroundStationPassSet: new Date(
        AROGroundStationPass.set.t,
      ).toISOString(),
    });
  };

  useEffect(() => {
    if (
      !updateWorkingImageRequestState.success &&
      updateWorkingImageRequestState.nextError
    ) {
      setErrorMessage(
        `${updateWorkingImageRequestState.nextError}, please try again later.`,
      );
      handleClose();
    }
    if (
      updateWorkingImageRequestState.success &&
      !updateWorkingImageRequestState.loading
    ) {
      router.push(
        `/image-requests/${updateWorkingImageRequestState.imageRequestId}`,
      );
    }
  }, [updateWorkingImageRequestState]);

  return (
    <Dialog open={open} onClose={submited ? undefined : handleClose}>
      <DialogTitle>Submit</DialogTitle>
      <DialogContent>
        <DialogContentText paragraph>
          Are you sure you want to submit the image request?
        </DialogContentText>
        <Stack spacing={2}>
          <ValidationStatus
            loading={updateWorkingImageRequestState.loading}
            success={
              updateWorkingImageRequestState.success ||
              (updateWorkingImageRequestState.imageRequestVaildationError?.pass
                .insufficientMemory ??
                false)
            }
            error={
              !updateWorkingImageRequestState.success &&
              !updateWorkingImageRequestState.nextError &&
              updateWorkingImageRequestState.imageRequestVaildationError?.pass
                .insufficientMemory
                ? "Insufficient memory, please choose another time"
                : ""
            }
            submited={submited}
            subject="Satellite"
          />
          <ValidationStatus
            loading={updateWorkingImageRequestState.loading}
            success={
              updateWorkingImageRequestState.success ||
              (updateWorkingImageRequestState.imageRequestVaildationError?.image
                .invalidFraming ??
                false)
            }
            error={
              !updateWorkingImageRequestState.success &&
              !updateWorkingImageRequestState.nextError &&
              updateWorkingImageRequestState.imageRequestVaildationError?.image
                .invalidFraming
                ? "Invalid, please adjust the framing"
                : ""
            }
            submited={submited}
            subject="Frameing"
          />
          <ValidationStatus
            loading={updateWorkingImageRequestState.loading}
            success={
              updateWorkingImageRequestState.success ||
              (updateWorkingImageRequestState.imageRequestVaildationError?.image
                .commandTimeCollide ??
                false)
            }
            error={
              !updateWorkingImageRequestState.success &&
              !updateWorkingImageRequestState.nextError &&
              updateWorkingImageRequestState.imageRequestVaildationError?.image
                .commandTimeCollide
                ? "Time Collision with other mission, please choose another time"
                : ""
            }
            submited={submited}
            subject="Capturing"
          />
          <ValidationStatus
            loading={updateWorkingImageRequestState.loading}
            success={
              updateWorkingImageRequestState.success ||
              (updateWorkingImageRequestState.imageRequestVaildationError?.ARO
                .passTimeCollide ??
                false)
            }
            error={
              !updateWorkingImageRequestState.success &&
              !updateWorkingImageRequestState.nextError &&
              updateWorkingImageRequestState.imageRequestVaildationError?.ARO
                .passTimeCollide
                ? "Time Collision with other mission, please choose another time"
                : ""
            }
            submited={submited}
            subject="Extra Ground Station"
            skip={AROGroundStationSelected.id === -1}
          />
        </Stack>
      </DialogContent>
      <DialogActions>
        <Button
          type="submit"
          color="error"
          variant="contained"
          fullWidth
          disabled={submited}
          onClick={submit}
        >
          Submit
        </Button>
      </DialogActions>
    </Dialog>
  );
}
