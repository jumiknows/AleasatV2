import {
  Box,
  Container,
  Paper,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableRow,
  Typography,
} from "@mui/material";
import Image from "next/image";
import React from "react";

const tableData = [
  {
    key: "Payload",
    value: "Earth-imaging payload for disaster relief training purposes",
  },
  {
    key: "Ground Station(s)",
    value: "UHF band located at UBC Vancouver",
  },
  {
    key: "Dimensions",
    value: `Stowed: 100.0mm x 100.0mm x 113.5mm\nDeployed: 389.2mm x 253.9mm x 113.5mm`,
  },
  {
    key: "Mass",
    value: "1.035 kg",
  },
  {
    key: "Power consumption",
    value: "Idle: ±1.2W",
  },
  {
    key: "Solar panel",
    value: "4x Fixed 1U Solar Panel",
  },
  {
    key: "Mission Duration",
    value: "1-2 years",
  },
];

export default function SpacecraftKeySpec() {
  return (
    <>
      <Container maxWidth="md">
        <Box
          sx={{
            position: "relative",
            width: "100%",
            aspectRatio: "1200/776",
          }}
        >
          <Image
            src="https://artifact.aleasat.space/space-components/aleasat-drawing.svg"
            alt="Outline drawing of ALEASAT"
            fill
            sizes="(min-width: 900px) 900px, 100vw"
            style={{
              filter: "invert(1)",
            }}
          />
        </Box>
        <Typography
          variant="h2"
          component="p"
          sx={{
            textAlign: "center",
            mb: 5,
          }}
        >
          ALEASAT
        </Typography>
        <Typography
          variant="h5"
          component="h2"
          sx={{
            textAlign: "center",
          }}
        >
          Key Specifications
        </Typography>
        <Container maxWidth="md">
          <TableContainer
            component={Paper}
            sx={{
              backgroundColor: "transparent",
              backgroundImage: "none",
            }}
          >
            <Table aria-label="simple table">
              <TableBody>
                {tableData.map((row, index) => (
                  <TableRow
                    key={index}
                    sx={{ "&:last-child td, &:last-child th": { border: 0 } }}
                  >
                    <TableCell>{row.key}</TableCell>
                    <TableCell>
                      {row.value.split("\n").map((line, i) => (
                        <React.Fragment key={i}>
                          {line}
                          <br />
                        </React.Fragment>
                      ))}
                    </TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </TableContainer>
        </Container>
      </Container>
    </>
  );
}
