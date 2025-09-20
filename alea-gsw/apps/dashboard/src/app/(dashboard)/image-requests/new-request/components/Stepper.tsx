"use client";

import Container from "@mui/material/Container";
import Step from "@mui/material/Step";
import StepLabel from "@mui/material/StepLabel";
import Stepper from "@mui/material/Stepper";
import { useAtomValue } from "jotai";
import { newImageRequestActiveStepState } from "../recoilState";

const stepsLabel = [
  "Select Target Location",
  "Select Time and Framing",
  "Request Details",
  "Confilm Details",
];

export default function NewImageRequestLayout() {
  const activeStepTransition = useAtomValue(newImageRequestActiveStepState);
  const { activeStep } = activeStepTransition;

  return (
    <Container>
      <Stepper activeStep={activeStep}>
        {stepsLabel.map((label) => {
          const stepProps: { completed?: boolean } = {};
          const labelProps: {
            optional?: React.ReactNode;
          } = {};
          return (
            <Step key={label} {...stepProps}>
              <StepLabel {...labelProps}>{label}</StepLabel>
            </Step>
          );
        })}
      </Stepper>
    </Container>
  );
}
