import childProcess from "child_process";
import util from "util";
import { mkdir, rm, writeFile } from "fs/promises";
import { type components } from "@aleasat/gmat-api";
import path, { dirname } from "path";
import { fileURLToPath } from "url";
import satellite from "satellite.js";

const exec = util.promisify(childProcess.exec);

const inputData = [
  [
    "ab0d2242-bca1-49b5-a78d-0c968d036947",
    {
      x: 4679.806329900379,
      y: -4973.093291658213,
      z: -0.0003913415953835786,
      vx: -0.7196019153772706,
      vy: -0.6734699468508896,
      vz: 7.575258113565158,
      type: "cartesian",
    },
    "BDSAT-2",
    1725505707773,
    "2024-09-05 03:08:27+00",
    "1 55098U 23001CT  24249.13087701  .00080280  00000+0  19129-2 0  9992",
    "2 55098  97.4181 313.2597 0010450 224.7689 135.2715 15.41237375 93135",
  ],
  [
    "26602791-f3b4-44c8-8b73-816a87fe7d1b",
    {
      x: 4853.826161232455,
      y: -4802.624263362153,
      z: 0.0004244654487310191,
      vx: -0.6954053427493632,
      vy: -0.6982928820636494,
      vz: 7.575344715066744,
      type: "cartesian",
    },
    "BDSAT-2",
    1725679585386,
    "2024-09-07 03:26:25+00",
    "1 55098U 23001CT  24251.14334938  .00081605  00000+0  19234-2 0  9992",
    "2 55098  97.4173 315.3038 0010463 219.9377 140.1101 15.41560804 93447",
  ],
  [
    "b97b9793-dab3-4476-902c-7c90644b3b53",
    {
      x: 6543.898205020467,
      y: -1896.645562390436,
      z: -0.003265895313423825,
      vx: -0.2883529239944826,
      vy: -0.9429800884729767,
      vz: 7.584584596083555,
      type: "cartesian",
    },
    "BDSAT-2",
    1728098942769,
    "2024-10-05 03:29:02+00",
    "1 55098U 23001CT  24279.14517094  .00129532  00000-0  25974-2 0  9999",
    "2 55098  97.4120 343.8366 0009946 126.6311 233.5854 15.46175438 97766",
  ],
] as const;

const generateGmatScriptSatSection = (
  satelliteState: components["schemas"]["SatelliteState"],
  config = {
    dryMass: 1.2,
    dragArea: 0.022,
    srpArea: 0.022,
  },
): string => {
  let sat = `Create Spacecraft Sat;
GMAT Sat.DateFormat = ${satelliteState.epochFormat};
GMAT Sat.Epoch = '${satelliteState.epoch}';
GMAT Sat.CoordinateSystem = ${satelliteState.coordinateSystem};
GMAT Sat.DryMass = ${config.dryMass};
GMAT Sat.Cd = 2.2;
GMAT Sat.Cr = 1.8;
GMAT Sat.DragArea = ${config.dragArea};
GMAT Sat.SRPArea = ${config.srpArea};\n`;

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

  return sat;
};

const generateScriptFile = (
  reportFileName = "result.csv",
  time = "ElapsedDays = 31",
): string => {
  return `Create ForceModel Propagator1_ForceModel;
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
GMAT Propagator1.MinStep = 0;
GMAT Propagator1.MaxStep = 60;
GMAT Propagator1.MaxStepAttempts = 50;
GMAT Propagator1.StopIfAccuracyIsViolated = true;

Create ReportFile ReportFile1;
GMAT ReportFile1.SolverIterations = Current;
GMAT ReportFile1.Precision = 10;
GMAT ReportFile1.Add = {Sat.UTCGregorian, Sat.EarthMJ2000Eq.X, Sat.EarthMJ2000Eq.Y, Sat.EarthMJ2000Eq.Z};
GMAT ReportFile1.Filename = '${path.join(
    dirname(fileURLToPath(import.meta.url)),
    "..",
  )}/cache/${reportFileName}';
GMAT ReportFile1.WriteHeaders = true;
GMAT ReportFile1.Delimiter = ',';
GMAT ReportFile1.WriteReport = true;
GMAT ReportFile1.FixedWidth = false;

BeginMissionSequence;
Propagate Propagator1(Sat) {Sat.${time}};`;
};

const runOrbitPropagation = async (
  satelliteState: components["schemas"]["SatelliteState"],
  id: string,
  seconds: number,
  config?: {
    dryMass: number;
    dragArea: number;
    srpArea: number;
  },
): Promise<void> => {
  const gmatScript =
    generateGmatScriptSatSection(satelliteState, config) +
    "\n" +
    generateScriptFile(`result-${id}.csv`, `ElapsedSecs = ${seconds}`);
  await writeFile(`cache/gamt-${id}.script`, gmatScript);

  await exec(
    `GmatConsole -x -r ${path.join(
      dirname(fileURLToPath(import.meta.url)),
      "..",
    )}/cache/gamt-${id}.script`,
  ).catch((error) => {
    console.error(error);
    error = true;
  });

  // await parseGmatResult(satelliteState);
};

await rm("cache", { force: true, recursive: true });
await mkdir("cache", { recursive: true });

const dryMassStart = 1;
const dryMassStep = 0.1;
const dryMassEnd = 1.1;

const dragAreaStart = 0.01;
const dragAreaStep = 0.01;
const dragAreaEnd = 0.1;

const srpAreaStart = 0;
const srpAreaStep = 0.0001;
const srpAreaEnd = 0;

const compareIndex = 2;

const runs: Array<{
  tag: string;
  config: {
    dryMass: number;
    dragArea: number;
    srpArea: number;
  };
}> = [
  // {
  //   tag: `dryMass-1.3-dragArea-0.325-srpArea-0.325`,
  //   config: {
  //     dryMass: 1.3,
  //     dragArea: 0.325,
  //     srpArea: 0.325,
  //   },
  // },
];

for (
  let dryMass = dryMassStart;
  dryMass <= dryMassEnd;
  dryMass += dryMassStep
) {
  for (
    let dragArea = dragAreaStart;
    dragArea <= dragAreaEnd;
    dragArea += dragAreaStep
  ) {
    for (
      let srpArea = srpAreaStart;
      srpArea <= srpAreaEnd;
      srpArea += srpAreaStep
    ) {
      const tag = `dryMass-${dryMass}-dragArea-${dragArea}-srpArea-${srpArea}`;

      runs.push({
        tag,
        config: {
          dryMass,
          dragArea,
          srpArea,
        },
      });
    }
  }
}

const minResult = {
  config: { dryMass: 0, dragArea: 0, srpArea: 0 },
  result: { x: 0, y: 0, z: 0 },
  diff: { x: Infinity, y: Infinity, z: Infinity },
  rms: Infinity,
};

let cpu = 0;
const maxCpu = 28;

const result = await Promise.all(
  runs.map(async ({ tag, config }, i) => {
    // eslint-disable-next-line no-unmodified-loop-condition
    while (cpu >= maxCpu) {
      await new Promise((resolve) => setTimeout(resolve, 1000));
    }
    cpu++;

    console.log(`Run ${i + 1}/${runs.length}\nTag: ${tag}`);
    await runOrbitPropagation(
      {
        epochFormat: "UTCGregorian",
        epoch:
          // To this format: 09 Aug 2024 03:45:21.893
          // 09 Aug 2024
          new Date(inputData[0][3]).toUTCString().slice(5, 17) +
          // 03:45:21.893
          new Date(inputData[0][3]).toISOString().slice(11, -1),
        coordinateSystem: "EarthMJ2000Eq",
        stateElement: inputData[0][1],
        stateId: inputData[0][0],
      },
      tag,
      Math.ceil((inputData[compareIndex][3] - inputData[0][3]) / 1000),
      config,
    );

    cpu--;

    const result = await exec(`tail -n 2 cache/result-${tag}.csv`).then((res) =>
      res.stdout.split("\n").map((line) => line.split(",")),
    );

    const step = result.find((line) =>
      line[0].startsWith(
        new Date(inputData[compareIndex][3]).toUTCString().slice(5, 17) +
          new Date(inputData[compareIndex][3]).toISOString().slice(11, -5),
      ),
    )!;

    const run = {
      config,
      result: {
        x: parseFloat(step[1]),
        y: parseFloat(step[2]),
        z: parseFloat(step[3]),
      },
      diff: {
        x: Math.abs(parseFloat(step[1]) - inputData[compareIndex][1].x),
        y: Math.abs(parseFloat(step[2]) - inputData[compareIndex][1].y),
        z: Math.abs(parseFloat(step[3]) - inputData[compareIndex][1].z),
      },
      rms: Math.sqrt(
        Math.abs(parseFloat(step[1]) - inputData[compareIndex][1].x) ** 2 +
          Math.abs(parseFloat(step[2]) - inputData[compareIndex][1].y) ** 2 +
          Math.abs(parseFloat(step[3]) - inputData[compareIndex][1].z) ** 2,
      ),
    };

    if (run.rms < minResult.rms) {
      minResult.config = run.config;
      minResult.result = run.result;
      minResult.diff = run.diff;
      minResult.rms = run.rms;
    }

    return run;
  }),
);

console.log(result);
console.log(minResult);

const satrec = satellite.twoline2satrec(inputData[0][5], inputData[0][6]);
const positionAndVelocity = satellite.propagate(
  satrec,
  new Date(inputData[compareIndex][3]),
);

if (typeof positionAndVelocity.position !== "boolean")
  console.log({
    x: positionAndVelocity.position.x,
    y: positionAndVelocity.position.y,
    z: positionAndVelocity.position.z,
    diff: {
      x: Math.abs(
        positionAndVelocity.position.x - inputData[compareIndex][1].x,
      ),
      y: Math.abs(
        positionAndVelocity.position.y - inputData[compareIndex][1].y,
      ),
      z: Math.abs(
        positionAndVelocity.position.z - inputData[compareIndex][1].z,
      ),
    },
    rms: Math.sqrt(
      Math.abs(positionAndVelocity.position.x - inputData[compareIndex][1].x) **
        2 +
        Math.abs(
          positionAndVelocity.position.y - inputData[compareIndex][1].y,
        ) **
          2 +
        Math.abs(
          positionAndVelocity.position.z - inputData[compareIndex][1].z,
        ) **
          2,
    ),
  });

writeFile("cache/final-result.json", JSON.stringify(result, null, 2));
