import { xml2js, xml2json } from "xml-js";

const ommEpochToJsDate = (ommEpoch: string): Date => {
  const year = ommEpoch.slice(0, 4);
  const day = ommEpoch.slice(5, 8);

  const yyyymmdd = new Date(Date.UTC(Number(year), 0, Number(day)))
    .toISOString()
    .slice(0, 10);
  return new Date(`${yyyymmdd}${ommEpoch.slice(8)}`);
};

const getLatestIssOem = async (): Promise<{
  info: { massKg: number; dragAreaMeterSquare: number; dragCoeff: number };
  stateVector: {
    epoch: Date;
    x: number;
    y: number;
    z: number;
    vx: number;
    vy: number;
    vz: number;
  };
}> => {
  const xml = await fetch(
    "https://nasa-public-data.s3.amazonaws.com/iss-coords/current/ISS_OEM/ISS.OEM_J2K_EPH.xml",
  );

  const json = xml2js(await xml.text(), { compact: true }) as any;

  /**
   * [
   *   { _text: 'Units are in kg and m^2' },
   *   { _text: 'MASS=473463.20' },
   *   { _text: 'DRAG_AREA=1514.10' },
   *   { _text: 'DRAG_COEFF=2.40' },
   *   { _text: 'SOLAR_RAD_AREA=0.00' },
   *   { _text: 'SOLAR_RAD_COEFF=0.00' },
   *   ...,
   *   ...,
   * ]
   */
  const comment = json.ndm.oem.body.segment.data.COMMENT;

  const info = {
    massKg: parseFloat(
      comment.find((c: any) => c._text.startsWith("MASS="))._text.split("=")[1],
    ),
    dragAreaMeterSquare: parseFloat(
      comment
        .find((c: any) => c._text.startsWith("DRAG_AREA="))
        ._text.split("=")[1],
    ),
    dragCoeff: parseFloat(
      comment
        .find((c: any) => c._text.startsWith("DRAG_COEFF="))
        ._text.split("=")[1],
    ),
  };

  /**
   * EPOCH Format - CCSDS Orbit Data Messages
   * https://public.ccsds.org/Pubs/502x0b3e1.pdf Section 7.5.10
   *
   * YYYY-DDDThh:mm:ss[.d→d][Z],
   * where ‘YYYY’ is the year; ‘MM’ is the two-digit month; ‘DDD’
   * is the three-digit day of year; ‘T’ is constant; ‘hh:mm:ss[.d→d]’ is the time in hours, minutes,
   * seconds, and optional fractional seconds; and ‘Z’ is an optional time code terminator (the
   * only permitted value is ‘Z’ for Zulu, i.e., UTC).
   */

  /**
   * {
   *   EPOCH: { _text: '2025-078T12:00:00.000Z' },
   *   X: { _attributes: { units: 'km' }, _text: '5583.3386615417603' },
   *   Y: { _attributes: { units: 'km' }, _text: '3835.2524648419899' },
   *   Z: { _attributes: { units: 'km' }, _text: '518.77784405604098' },
   *   X_DOT: { _attributes: { units: 'km/s' }, _text: '-3.08089854388952' },
   *   Y_DOT: { _attributes: { units: 'km/s' }, _text: '3.66062743759149' },
   *   Z_DOT: { _attributes: { units: 'km/s' }, _text: '5.98808521091469' }
   * }
   */
  const initialStateVector = json.ndm.oem.body.segment.data.stateVector[0];

  const stateVector = {
    epoch: ommEpochToJsDate(initialStateVector.EPOCH._text),
    x: parseFloat(initialStateVector.X._text),
    y: parseFloat(initialStateVector.Y._text),
    z: parseFloat(initialStateVector.Z._text),
    vx: parseFloat(initialStateVector.X_DOT._text),
    vy: parseFloat(initialStateVector.Y_DOT._text),
    vz: parseFloat(initialStateVector.Z_DOT._text),
  };

  return {
    info,
    stateVector,
  };
};

export { getLatestIssOem };
