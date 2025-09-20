// React
import { useState } from "react";

// Material UI
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Container from "@mui/material/Container";
import Typography from "@mui/material/Typography";
import {
  DatePicker,
  type DatePickerProps,
} from "@mui/x-date-pickers/DatePicker";

// Utils
import dayjs, { type Dayjs } from "dayjs";
import { useAtom } from "jotai";
import { satellitePassFormDataState } from "../recoilState";

const DATE_FIELD_PROPS: DatePickerProps<Dayjs>["slotProps"] = {
  textField: {
    size: "small",
    sx: {
      maxWidth: 150,
    },
  },
};

const SatellitePassForm = () => {
  const [satellitePassFormData, setSatellitePassFormData] = useAtom(
    satellitePassFormDataState,
  );
  const [tempFormData, setTempFormData] = useState(satellitePassFormData);

  const MIN_DATE = dayjs.utc();
  const MAX_DATE = MIN_DATE.add(11, "day");

  return (
    <Container>
      <Box
        width={1}
        height={1}
        pl={1}
        pr={1}
        pb={1}
        display="flex"
        flexDirection="row"
        alignItems={"center"}
        justifyContent={"left"}
      >
        <Box p={1}>
          <Typography variant={"subtitle1"} fontWeight={"bold"} sx={{ pr: 8 }}>
            Select time and angle:
          </Typography>
        </Box>
        <Box p={1}>
          <DatePicker
            label="From"
            value={dayjs.utc(tempFormData.startDate)}
            minDate={MIN_DATE}
            maxDate={dayjs.utc(tempFormData.endDate)}
            onChange={(newValue) => {
              if (newValue)
                setTempFormData((data) => ({
                  ...data,
                  startDate: newValue.toISOString(),
                }));
            }}
            slotProps={DATE_FIELD_PROPS}
          />
        </Box>
        <Box p={1}>
          <DatePicker
            label="To"
            value={dayjs(tempFormData.endDate)}
            minDate={dayjs(tempFormData.startDate)}
            maxDate={MAX_DATE}
            onChange={(newValue) => {
              if (newValue)
                setTempFormData((data) => ({
                  ...data,
                  endDate: newValue.toISOString(),
                }));
            }}
            slotProps={DATE_FIELD_PROPS}
          />
        </Box>
        <Box p={1}>
          <Button
            style={{ width: "100%" }}
            variant="contained"
            onClick={() => {
              setSatellitePassFormData((data) => ({
                ...data,
                ...tempFormData,
              }));
            }}
          >
            Find Timeslot
          </Button>
        </Box>
      </Box>
    </Container>
  );
};

export default SatellitePassForm;
