"use client";

import Link from "@aleasat/ui/components/Link";
import Button from "@mui/material/Button";
import Container from "@mui/material/Container";
import Stack from "@mui/material/Stack";
import { useAtom } from "jotai";
import { useRouter } from "next/navigation";
import { useState } from "react";
import { newImageRequestActiveStepState } from "../recoilState";

const stepsPath = [
  "FOVMap",
  "SatellitePassView",
  "RequestDetails",
  "ConfilmDetails",
].map((p) => "/image-requests/new-request/" + p);
const transitionTime = [
  [0, 0],
  [0, 0],
  [0, 0],
  [0, 0],
];

export default function StepperButton(props: {
  disable?: boolean;
  onClickNext?: () => void;
}) {
  const { disable = false, onClickNext } = props;

  const router = useRouter();
  const [activeStepTransition, setActiveStepTransition] = useAtom(
    newImageRequestActiveStepState,
  );
  const [fade, setFade] = useState(false);
  const { activeStep } = activeStepTransition;

  const handlerNext = () => {
    setActiveStepTransition((oldStep) => ({
      activeStep: oldStep.activeStep + 1,
      transitionTime: transitionTime[oldStep.activeStep][1],
      stepGoingforward: true,
    }));
    setFade(true);
    router.push(stepsPath[activeStep + 1]);
  };

  const handlerBack = () => {
    setActiveStepTransition((oldStep) => ({
      activeStep: oldStep.activeStep - 1,
      transitionTime: transitionTime[oldStep.activeStep][0],
      stepGoingforward: false,
    }));
    setFade(true);
    router.push(stepsPath[activeStep - 1]);
  };

  // useEffect(() => {
  //  const timeout = setTimeout(() => {
  //    router.push(stepsPath[activeStepTransition.activeStep]);
  //    // setFade(false);
  //  }, activeStepTransition.transitionTime);
  //  return () => clearTimeout(timeout);
  // }, [activeStepTransition]);

  return (
    <Container sx={{ display: "flex", flexDirection: "row", pt: 2 }}>
      <Stack
        direction="row"
        sx={{ width: "100%" }}
        justifyContent="space-between"
      >
        <Button
          variant="contained"
          disabled={activeStep === 0 || fade}
          LinkComponent={Link}
          onClick={handlerBack}
          sx={{ mr: 1 }}
        >
          Back
        </Button>
        <Button
          variant="contained"
          disabled={fade || disable}
          onClick={onClickNext ?? handlerNext}
        >
          {activeStep === stepsPath.length - 1 ? "Submit" : "Next"}
        </Button>
      </Stack>
    </Container>
  );
}
