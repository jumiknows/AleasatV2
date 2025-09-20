import CancelIcon from "@mui/icons-material/Cancel";
import HelpIcon from "@mui/icons-material/Help";
import Box from "@mui/material/Box";
import Container from "@mui/material/Container";
import Divider from "@mui/material/Divider";
import IconButton from "@mui/material/IconButton";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import { useAtom } from "jotai";
import { openSatVizHelpState } from "../recoilState";

export default function HelpButton() {
  const [open, setOpen] = useAtom(openSatVizHelpState);

  return (
    <>
      {open && (
        <Container
          sx={{
            width: "100%",
            height: "100%",
            backgroundColor: "rgba(0,0,0,0.6)",
            position: "absolute",
            top: "50%",
            left: "50%",
            transform: "translate(-50%, -50%)",
          }}
        >
          <div style={{ height: "100%" }} onClick={() => setOpen(false)}>
            <Stack justifyContent="center" sx={{ height: "100%" }}>
              <Stack justifyContent="center" direction="row" spacing={4}>
                <Stack>
                  <svg
                    fill="#FFFFFF"
                    width="100px"
                    height="100px"
                    viewBox="0 0 381.669 381.669"
                  >
                    <polygon
                      xmlns="http://www.w3.org/2000/svg"
                      points="54.496 205.83 108.352 205.83 108.352 175.839 54.496 175.839 54.496 155.361 0 190.834 54.496 226.308"
                      transform="matrix(1, 0, 0, 1, 0, -3.552713678800501e-15)"
                    />
                    <g
                      xmlns="http://www.w3.org/2000/svg"
                      transform="matrix(0.4888260066509247, 0, 0, 0.4888260066509247, 80.73329162597655, 75.50572967529297)"
                    >
                      <path d="M237.298,99.533c-0.301-0.34-0.562-0.677-0.916-0.999c-3.937-3.535-4.043-2.491,0.266-6.463 c3.192-2.929,7.063-5.222,10.574-7.755c9.286-6.711,15.398-15.699,19.529-26.356C276.02,34.033,248.707,17.503,235,3.344 c-9.904-10.247-25.496,5.382-15.604,15.604c7.643,7.912,17.489,14.328,24.14,23.123c7.453,9.848-3.901,20.712-11.68,26.194 c-12.026,8.473-22.423,19.727-20.02,31.794c-53.971,5.042-103.87,34.623-103.87,86.767V333.2c0,64.664,52.603,117.27,117.27,117.27 c64.663,0,117.27-52.605,117.27-117.27V186.817C342.51,129.888,289.543,102.317,237.298,99.533z M130.044,186.817 c0-38.707,42.017-61.117,85.535-64.841v135.005c-39.697-1.998-71.928-11.042-85.535-15.457V186.817z M320.433,333.194 c0,52.5-42.705,95.199-95.192,95.199c-52.488,0-95.196-42.699-95.196-95.199V264.73c19.713,5.958,56.817,14.995,100.676,14.995 c28.088,0,58.93-3.759,89.713-14.352V333.194z M320.433,241.896c-27.916,10.675-56.424,14.849-82.78,15.415v-135.66 c42.569,2.553,82.78,22.969,82.78,65.175V241.896z M206.072,133.429v111.973c-17.153,3.027-67.583-11.094-67.583-11.094 C131.049,155.812,160.429,142.005,206.072,133.429z" />
                    </g>
                    <polygon
                      xmlns="http://www.w3.org/2000/svg"
                      points="327.813 175.839 381.669 175.839 381.669 205.83 327.813 205.83 327.813 226.308 273.317 190.835 327.813 155.361"
                      style={{ transformOrigin: "327.493px 190.835px" }}
                      transform="matrix(-1, 0, 0, -1, 0, 0)"
                    />
                  </svg>
                  <Typography textAlign="center">Move</Typography>
                </Stack>
                <Divider orientation="vertical" flexItem />
                <Stack>
                  <Stack
                    direction="row"
                    alignItems="center"
                    justifyContent="center"
                    spacing={2}
                  >
                    <svg
                      xmlns="http://www.w3.org/2000/svg"
                      id="svg2"
                      viewBox="0 0 259.72 186.47"
                      version="1.1"
                      height="50px"
                    >
                      <g id="imagebot_1" transform="translate(-13.455 -10.173)">
                        <g
                          id="imagebot_8"
                          transform="matrix(1.484 0 0 1.5027 -28.769 -921.73)"
                        >
                          <rect
                            id="imagebot_9"
                            style={{
                              strokeLinejoin: "round",
                              stroke: "#fff",
                              strokeWidth: 8.2,
                              fillOpacity: 0,
                            }}
                            rx="18.298"
                            ry="18.298"
                            height="102.77"
                            width="153.84"
                            y="624.24"
                            x="32.863"
                          />
                        </g>
                        <title id="title12" />
                        <text
                          id="imagebot_2"
                          font-size="93"
                          transform="matrix(1.1583 0 0 1.1583 14.511 6.2637)"
                          style={{
                            fontSize: "93px",
                            fontWeight: "bold",
                            fontFamily: "Sans-serif",
                            textAnchor: "middle",
                            strokeWidth: 0,
                            fill: "#fff",
                          }}
                          y="105.77733"
                          x="102.68533"
                          font-weight="bold"
                        >
                          Ctrl
                        </text>
                      </g>
                    </svg>
                    <svg
                      xmlns="http://www.w3.org/2000/svg"
                      width="24"
                      height="24"
                      viewBox="0 0 24 24"
                      fill="#fff"
                    >
                      <path d="M24 10h-10v-10h-4v10h-10v4h10v10h4v-10h10z" />
                    </svg>
                    <svg
                      fill="#FFFFFF"
                      width="100px"
                      height="100px"
                      viewBox="0 0 381.669 381.669"
                    >
                      <polygon
                        xmlns="http://www.w3.org/2000/svg"
                        points="54.496 205.83 108.352 205.83 108.352 175.839 54.496 175.839 54.496 155.361 0 190.834 54.496 226.308"
                        transform="matrix(1, 0, 0, 1, 0, -3.552713678800501e-15)"
                      />
                      <g
                        xmlns="http://www.w3.org/2000/svg"
                        transform="matrix(0.4888260066509247, 0, 0, 0.4888260066509247, 80.73329162597655, 75.50572967529297)"
                      >
                        <path d="M237.298,99.533c-0.301-0.34-0.562-0.677-0.916-0.999c-3.937-3.535-4.043-2.491,0.266-6.463 c3.192-2.929,7.063-5.222,10.574-7.755c9.286-6.711,15.398-15.699,19.529-26.356C276.02,34.033,248.707,17.503,235,3.344 c-9.904-10.247-25.496,5.382-15.604,15.604c7.643,7.912,17.489,14.328,24.14,23.123c7.453,9.848-3.901,20.712-11.68,26.194 c-12.026,8.473-22.423,19.727-20.02,31.794c-53.971,5.042-103.87,34.623-103.87,86.767V333.2c0,64.664,52.603,117.27,117.27,117.27 c64.663,0,117.27-52.605,117.27-117.27V186.817C342.51,129.888,289.543,102.317,237.298,99.533z M130.044,186.817 c0-38.707,42.017-61.117,85.535-64.841v135.005c-39.697-1.998-71.928-11.042-85.535-15.457V186.817z M320.433,333.194 c0,52.5-42.705,95.199-95.192,95.199c-52.488,0-95.196-42.699-95.196-95.199V264.73c19.713,5.958,56.817,14.995,100.676,14.995 c28.088,0,58.93-3.759,89.713-14.352V333.194z M320.433,241.896c-27.916,10.675-56.424,14.849-82.78,15.415v-135.66 c42.569,2.553,82.78,22.969,82.78,65.175V241.896z M206.072,133.429v111.973c-17.153,3.027-67.583-11.094-67.583-11.094 C131.049,155.812,160.429,142.005,206.072,133.429z" />
                      </g>
                      <polygon
                        xmlns="http://www.w3.org/2000/svg"
                        points="327.813 175.839 381.669 175.839 381.669 205.83 327.813 205.83 327.813 226.308 273.317 190.835 327.813 155.361"
                        style={{ transformOrigin: "327.493px 190.835px" }}
                        transform="matrix(-1, 0, 0, -1, 0, 0)"
                      />
                    </svg>
                  </Stack>
                  <Typography textAlign="center">Rotate</Typography>
                </Stack>
              </Stack>
            </Stack>
          </div>
        </Container>
      )}
      <Box
        sx={{
          position: "absolute",
          top: "0px",
          right: "0px",
        }}
      >
        <IconButton aria-label="Help" onClick={() => setOpen((o) => !o)}>
          {open ? (
            <CancelIcon fontSize="large" />
          ) : (
            <HelpIcon fontSize="large" />
          )}
        </IconButton>
      </Box>
    </>
  );
}
