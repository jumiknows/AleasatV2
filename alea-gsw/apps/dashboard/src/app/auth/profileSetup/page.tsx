"use client";
import ImageBox from "@aleasat/ui/components/Image";
import ErrorOutlineIcon from "@mui/icons-material/ErrorOutline";
import KeyboardArrowRightIcon from "@mui/icons-material/KeyboardArrowRight";
import {
  Box,
  Button,
  FormControl,
  FormControlLabel,
  InputAdornment,
  Radio,
  RadioGroup,
  TextField,
  Typography,
} from "@mui/material";
import Grid from "@mui/material/Grid2";
import type React from "react";
import { useEffect, useState } from "react";

const tempUsernames = ["test", "test1", "test2"];

export default function ProfileSetup() {
  const [formData, setFormData] = useState({
    firstName: "",
    lastName: "",
    username: "",
    phoneNumber: "",
    userType: "",
  });

  const [isFormValid, setIsFormValid] = useState(false);
  const [isUsernameTaken, setIsUsernameTaken] = useState<boolean | null>(null);
  const [isPhoneNumberValid, setIsPhoneNumberValid] = useState<boolean | null>(
    null,
  );

  useEffect(() => {
    const { firstName, lastName, username, phoneNumber, userType } = formData;
    setIsFormValid(
      firstName.trim() !== "" &&
        lastName.trim() !== "" &&
        username.trim() !== "" &&
        phoneNumber.trim() !== "" &&
        userType !== "" &&
        isUsernameTaken === false &&
        isPhoneNumberValid === true,
    );
  }, [formData, isUsernameTaken, isPhoneNumberValid]);

  useEffect(() => {
    const timeout = setTimeout(() => {
      const trimmed = formData.username.trim().toLowerCase();
      if (trimmed === "") {
        setIsUsernameTaken(null);
      } else {
        setIsUsernameTaken(tempUsernames.includes(trimmed));
      }
    }, 100);

    return () => clearTimeout(timeout);
  }, [formData.username]);

  useEffect(() => {
    const timeout = setTimeout(() => {
      const number = formData.phoneNumber.trim();
      const phoneRegex = /^\+?[\d\s\-().]{7,15}$/;
      if (number === "") {
        setIsPhoneNumberValid(null);
      } else {
        setIsPhoneNumberValid(phoneRegex.test(number));
      }
    }, 100);

    return () => clearTimeout(timeout);
  }, [formData.phoneNumber]);

  const handleInputChange =
    (field: string) => (event: React.ChangeEvent<HTMLInputElement>) => {
      setFormData({
        ...formData,
        [field]: event.target.value,
      });
    };

  const textFieldSx = {
    "& .MuiOutlinedInput-root": {
      height: "4.5rem",
      fontSize: "1.2rem",
      "& .MuiInputBase-input": {
        paddingLeft: "1.5rem",
      },
      "& fieldset": {
        borderRadius: 4,
        borderColor: "white",
      },
    },
  };

  return (
    <Box
      sx={{
        display: "flex",
        height: "100vh",
        alignItems: "flex-start",
        justifyContent: "center",
        paddingTop: "5%",
      }}
    >
      <Box
        sx={{
          flex: 1,
          py: "8%",
          pl: "8%",
          alignSelf: "flex-start",
          position: "relative",
        }}
      >
        <Typography variant="h2" fontWeight="bold" gutterBottom>
          PROFILE SETUP
        </Typography>
        <Typography variant="p">
          Provide a few details to complete your account.
        </Typography>

        <ImageBox
          src={"https://artifact.aleasat.space/dashboard/earth.png"}
          alt="earth"
          sx={{
            position: "absolute",
            top: "10rem",
            left: "-20rem",
            height: 900,
            zIndex: -1,
          }}
        />
      </Box>

      <Box sx={{ flex: 1, py: "8%", pr: "8%" }}>
        <form>
          <Grid container spacing={2} columnSpacing={12}>
            <Grid size={{ xs: 6 }} sx={{ position: "relative" }}>
              <Typography variant="h5">First Name</Typography>
            </Grid>
            <Grid size={{ xs: 6 }}>
              <Typography variant="h5">Last Name</Typography>
            </Grid>
            <Grid size={{ xs: 6 }}>
              <TextField
                placeholder="John"
                fullWidth
                variant="outlined"
                value={formData.firstName}
                onChange={handleInputChange("firstName")}
                sx={textFieldSx}
              />
            </Grid>
            <Grid size={{ xs: 6 }}>
              <TextField
                placeholder="Doe"
                fullWidth
                variant="outlined"
                value={formData.lastName}
                onChange={handleInputChange("lastName")}
                sx={textFieldSx}
              />
            </Grid>

            <Grid size={{ xs: 12 }} sx={{ mt: 2 }}>
              <Typography variant="h5">Username</Typography>
            </Grid>
            <Grid size={{ xs: 12 }}>
              <TextField
                placeholder="JohnDoe123__"
                fullWidth
                variant="outlined"
                value={formData.username}
                onChange={handleInputChange("username")}
                sx={textFieldSx}
                error={isUsernameTaken === true}
                helperText={
                  isUsernameTaken === true
                    ? "That username is already taken. Please choose another."
                    : isUsernameTaken === false
                      ? "Username is available"
                      : ""
                }
                slotProps={{
                  formHelperText: {
                    sx: {
                      color:
                        isUsernameTaken === false
                          ? "success.main"
                          : isUsernameTaken === true
                            ? "error.main"
                            : "inherit",
                      fontSize: "1rem",
                    },
                  },
                  input: {
                    startAdornment:
                      isUsernameTaken === true ? (
                        <InputAdornment position="start">
                          <ErrorOutlineIcon color="error" />
                        </InputAdornment>
                      ) : undefined,
                  },
                }}
              />
            </Grid>

            <Grid size={{ xs: 12 }} sx={{ mt: 2 }}>
              <Typography variant="h5">Phone Number</Typography>
            </Grid>
            <Grid size={{ xs: 12 }}>
              <TextField
                placeholder="(555) 123-4567"
                fullWidth
                variant="outlined"
                value={formData.phoneNumber}
                onChange={handleInputChange("phoneNumber")}
                sx={textFieldSx}
                error={isPhoneNumberValid === false}
                helperText={
                  isPhoneNumberValid === false
                    ? "Please enter a valid phone number."
                    : ""
                }
                slotProps={{
                  formHelperText: {
                    sx: {
                      color:
                        isPhoneNumberValid === false ? "error.main" : "inherit",
                      fontSize: "1rem",
                    },
                  },
                }}
              />
            </Grid>

            <Grid size={{ xs: 12 }} sx={{ mt: 2 }}>
              <FormControl>
                <Typography variant="h5" gutterBottom>
                  User Type
                </Typography>
                <RadioGroup
                  row
                  value={formData.userType}
                  onChange={(e) =>
                    setFormData({
                      ...formData,
                      userType: e.target.value,
                    })
                  }
                >
                  <FormControlLabel
                    value="public"
                    control={
                      <Radio
                        sx={{
                          width: "3rem",
                          height: "3rem",
                          "& svg": { fontSize: "2rem" },
                        }}
                      />
                    }
                    label="Public"
                  />
                  <FormControlLabel
                    value="private"
                    control={
                      <Radio
                        sx={{
                          width: "3rem",
                          height: "3rem",
                          "& svg": { fontSize: "2rem" },
                        }}
                      />
                    }
                    label="Private"
                  />
                </RadioGroup>
              </FormControl>
            </Grid>

            <Grid
              size={{ xs: 12 }}
              sx={{ mt: 8, display: "flex", justifyContent: "flex-end" }}
            >
              <Button
                disabled={!isFormValid}
                type="submit"
                variant="contained"
                size="large"
                endIcon={<KeyboardArrowRightIcon sx={{ mb: 0.2 }} />}
                sx={{
                  borderRadius: 3,
                  px: 8,
                  py: 1.5,
                  fontSize: "1.2rem",
                }}
              >
                Activate
              </Button>
            </Grid>
          </Grid>
        </form>
      </Box>
    </Box>
  );
}
