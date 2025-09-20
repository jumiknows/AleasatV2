import childProcess from "child_process";
import process from "node:process";
import path, { dirname } from "path";
import { fileURLToPath } from "url";
import util from "util";
import type { SatelliteState } from "@aleasat/job-queue-api";
import { trace } from "@opentelemetry/api";
import { mkdir, readFile, writeFile } from "fs/promises";
import logger from "./logger.js";

type CartesianState = {
  /** @enum {string} */
  type: "cartesian";
  x: number;
  y: number;
  z: number;
  vx: number;
  vy: number;
  vz: number;
};
type KeplerianState = {
  /** @enum {string} */
  type: "keplerian";
  sma: number;
  ecc: number;
  inc: number;
  raan: number;
  aop: number;
  ta: number;
};

type GmatOrbitPropagation = {
  stateElement: CartesianState | KeplerianState;
  stateId: string;
  result: {
    utcGregorian: number;
    x: number;
    y: number;
    z: number;
  }[];
};

const exec = util.promisify(childProcess.exec);

const tracer = trace.getTracer("runOrbitPropagation");

const cacheDir = path.join(
  dirname(fileURLToPath(import.meta.url)),
  "../../cache",
);
await mkdir(cacheDir, { recursive: true });

export const runOrbitPropagation = async (
  satelliteState: SatelliteState,
): Promise<string | null> => {
  return await tracer.startActiveSpan("runOrbitPropagation", async (span) => {
    const gmatScript = generateScriptFile(satelliteState);
    await writeFile("cache/gmat.script", gmatScript);

    const start = process.hrtime();

    const gmatError = await tracer.startActiveSpan("gmat", async (span) => {
      let errored = false;
      await exec(
        `GmatConsole -x -r ${path.join(
          dirname(fileURLToPath(import.meta.url)),
          "../..",
        )}/cache/gmat.script`,
      ).catch((error) => {
        logger.error(error, "GMAT execution error");
        span.recordException(error);
        errored = true;
      });

      span.end();
      return errored;
    });

    const end = process.hrtime(start);
    logger.info(`GMAT execution time: ${end[0]}s ${end[1] / 1e6}ms`);

    if (gmatError) {
      span.end();
      return null;
    }

    const results = await parseGmatResult(satelliteState);

    span.end();

    return results;
  });
};

const heading: Array<keyof GmatOrbitPropagation["result"][number]> = [
  "utcGregorian",
  "x",
  "y",
  "z",
];

const parseGmatResult = async (
  satelliteState: SatelliteState,
): Promise<string> => {
  return await tracer.startActiveSpan("parseGmatResult", async (span) => {
    const resultCsv = await readFile("cache/result.csv", "utf-8");
    const result = [] as GmatOrbitPropagation["result"];
    resultCsv
      .split("\n")
      .slice(1, -1)
      .forEach((line) => {
        // @ts-expect-error temp result
        const lineResult: GmatOrbitPropagation["result"][number] = {};
        line.split(",").forEach((value, index) => {
          lineResult[heading[index]] =
            index === 0 ? new Date(value).getTime() : parseFloat(value);
        });
        result.push(lineResult);
      });

    span.end();

    return JSON.stringify({
      stateId: satelliteState.stateId,
      stateElement: satelliteState.stateElement,
      result,
    });
  });
};

const generateGmatScriptSatSection = (
  satelliteState: SatelliteState,
): string => {
  let sat = `Create Spacecraft Sat;
GMAT Sat.DateFormat = ${satelliteState.epochFormat};
GMAT Sat.Epoch = '${satelliteState.epoch}';
GMAT Sat.CoordinateSystem = ${satelliteState.coordinateSystem};
GMAT Sat.DryMass = 473463.2;
GMAT Sat.Cd = 2.4;
GMAT Sat.Cr = 1.8;
GMAT Sat.DragArea = 1514.1;
GMAT Sat.SRPArea = 0;\n`;

  if (satelliteState.stateElement.type === "cartesian") {
    for (let i = 0; i < 3; i++) {
      sat += `GMAT Sat.${String.fromCharCode(88 + i)} = ${
        satelliteState.stateElement[
          String.fromCharCode(120 + i) as "x" | "y" | "z"
        ]
      };\n`;
    }
    for (let i = 0; i < 3; i++) {
      sat += `GMAT Sat.V${String.fromCharCode(88 + i)} = ${
        satelliteState.stateElement[
          ("v" + String.fromCharCode(120 + i)) as "vx" | "vy" | "vz"
        ]
      };\n`;
    }
  } else if (satelliteState.stateElement.type === "keplerian") {
    sat += `GMAT Sat.SMA = ${satelliteState.stateElement.sma};\n`;
    sat += `GMAT Sat.ECC = ${satelliteState.stateElement.ecc};\n`;
    sat += `GMAT Sat.INC = ${satelliteState.stateElement.inc};\n`;
    sat += `GMAT Sat.RAAN = ${satelliteState.stateElement.raan};\n`;
    sat += `GMAT Sat.AOP = ${satelliteState.stateElement.aop};\n`;
    sat += `GMAT Sat.TA = ${satelliteState.stateElement.ta};\n`;
  }

  return sat;
};

const generateScriptFile = (satelliteState: SatelliteState): string => {
  return `${generateGmatScriptSatSection(satelliteState)}
Create ForceModel Propagator1_ForceModel;
GMAT Propagator1_ForceModel.CentralBody = Earth;
GMAT Propagator1_ForceModel.PrimaryBodies = {Earth};
GMAT Propagator1_ForceModel.PointMasses = {Jupiter, Luna, Mars, Mercury, Neptune, Pluto, Saturn, Sun, Uranus, Venus};
GMAT Propagator1_ForceModel.SRP = On;
GMAT Propagator1_ForceModel.RelativisticCorrection = Off;
GMAT Propagator1_ForceModel.ErrorControl = RSSStep;
GMAT Propagator1_ForceModel.GravityField.Earth.Degree = 20;
GMAT Propagator1_ForceModel.GravityField.Earth.Order = 20;
GMAT Propagator1_ForceModel.GravityField.Earth.StmLimit = 100;
GMAT Propagator1_ForceModel.GravityField.Earth.PotentialFile = 'EGM2008.grv';
GMAT Propagator1_ForceModel.GravityField.Earth.TideModel = 'SolidAndPole';
GMAT Propagator1_ForceModel.SRP.Flux = 1367;
GMAT Propagator1_ForceModel.SRP.SRPModel = Spherical;
GMAT Propagator1_ForceModel.SRP.Nominal_Sun = 149597870.691;
GMAT Propagator1_ForceModel.Drag.AtmosphereModel = MSISE90;
GMAT Propagator1_ForceModel.Drag.HistoricWeatherSource = 'ConstantFluxAndGeoMag';
GMAT Propagator1_ForceModel.Drag.PredictedWeatherSource = 'ConstantFluxAndGeoMag';
GMAT Propagator1_ForceModel.Drag.CSSISpaceWeatherFile = 'SpaceWeather-All-v1.2.txt';
GMAT Propagator1_ForceModel.Drag.SchattenFile = 'SchattenPredict.txt';
GMAT Propagator1_ForceModel.Drag.F107 = 150;
GMAT Propagator1_ForceModel.Drag.F107A = 150;
GMAT Propagator1_ForceModel.Drag.MagneticIndex = 3;
GMAT Propagator1_ForceModel.Drag.SchattenErrorModel = 'Nominal';
GMAT Propagator1_ForceModel.Drag.SchattenTimingModel = 'NominalCycle';
GMAT Propagator1_ForceModel.Drag.DragModel = 'Spherical';

Create Propagator Propagator1;
GMAT Propagator1.FM = Propagator1_ForceModel;
GMAT Propagator1.Type = RungeKutta89;
GMAT Propagator1.InitialStepSize = 60;
GMAT Propagator1.Accuracy = 9.999999999999999e-12;
GMAT Propagator1.MinStep = 0.001;
GMAT Propagator1.MaxStep = 1;
GMAT Propagator1.MaxStepAttempts = 50;
GMAT Propagator1.StopIfAccuracyIsViolated = true;

Create ReportFile ReportFile1;
GMAT ReportFile1.SolverIterations = Current;
GMAT ReportFile1.Precision = 10;
GMAT ReportFile1.Add = {Sat.UTCGregorian, Sat.EarthMJ2000Eq.X, Sat.EarthMJ2000Eq.Y, Sat.EarthMJ2000Eq.Z};
GMAT ReportFile1.Filename = '${path.join(
    dirname(fileURLToPath(import.meta.url)),
    "../..",
  )}/cache/result.csv';
GMAT ReportFile1.WriteHeaders = true;
GMAT ReportFile1.Delimiter = ',';
GMAT ReportFile1.WriteReport = true;
GMAT ReportFile1.FixedWidth = false;

BeginMissionSequence;
Propagate Propagator1(Sat) {Sat.ElapsedDays = 7};`;
};
