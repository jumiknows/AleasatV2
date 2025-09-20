/* eslint-disable */
import Long from "long";
import type { CallContext, CallOptions } from "nice-grpc-common";
import _m0 from "protobufjs/minimal.js";
import { Timestamp } from "./google/protobuf/timestamp.js";

export const protobufPackage = "";

export enum Code {
  NONE = 0,
  SUCCESS = 1,
  SUCCESS_SCHED = 2,
  ERROR = 3,
  CMD_DNE = 4,
  NOT_IMPl = 5,
  UNRECOGNIZED = -1,
}

export function codeFromJSON(object: any): Code {
  switch (object) {
    case 0:
    case "NONE":
      return Code.NONE;
    case 1:
    case "SUCCESS":
      return Code.SUCCESS;
    case 2:
    case "SUCCESS_SCHED":
      return Code.SUCCESS_SCHED;
    case 3:
    case "ERROR":
      return Code.ERROR;
    case 4:
    case "CMD_DNE":
      return Code.CMD_DNE;
    case 5:
    case "NOT_IMPl":
      return Code.NOT_IMPl;
    case -1:
    case "UNRECOGNIZED":
    default:
      return Code.UNRECOGNIZED;
  }
}

export function codeToJSON(object: Code): string {
  switch (object) {
    case Code.NONE:
      return "NONE";
    case Code.SUCCESS:
      return "SUCCESS";
    case Code.SUCCESS_SCHED:
      return "SUCCESS_SCHED";
    case Code.ERROR:
      return "ERROR";
    case Code.CMD_DNE:
      return "CMD_DNE";
    case Code.NOT_IMPl:
      return "NOT_IMPl";
    case Code.UNRECOGNIZED:
    default:
      return "UNRECOGNIZED";
  }
}

export interface resetCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface resetCommandOBCResponse {
}

export interface pingCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface pingCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: pingCommandOBCResponseData | undefined;
}

export interface pingCommandOBCResponseData {
}

export interface getTimeCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface getTimeCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: getTimeCommandOBCResponseData | undefined;
}

export interface getTimeCommandOBCResponseData {
  timestamp: Date | undefined;
}

export interface setTimeCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  timestamp: Date | undefined;
}

export interface setTimeCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: setTimeCommandOBCResponseData | undefined;
}

export interface setTimeCommandOBCResponseData {
}

export interface getMinStackSpaceCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface getMinStackSpaceCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: getMinStackSpaceCommandOBCResponseData | undefined;
}

export interface getMinStackSpaceCommandOBCResponseData {
  minStackSpace: Uint8Array;
}

export interface captureRtosTraceCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  length: number;
}

export interface captureRtosTraceCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: captureRtosTraceCommandOBCResponseData | undefined;
}

export interface captureRtosTraceCommandOBCResponseData {
  trace: Uint8Array;
}

export interface rtosStateCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface rtosStateCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: rtosStateCommandOBCResponseData | undefined;
}

export interface rtosStateCommandOBCResponseData {
}

export interface lowPowerCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  enable: boolean;
}

export interface lowPowerCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: lowPowerCommandOBCResponseData | undefined;
}

export interface lowPowerCommandOBCResponseData {
}

export interface sysTimeCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface sysTimeCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: sysTimeCommandOBCResponseData | undefined;
}

export interface sysTimeCommandOBCResponseData {
  sysTimeUs: number;
}

export interface cpuUsageCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  task: number;
}

export interface cpuUsageCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: cpuUsageCommandOBCResponseData | undefined;
}

export interface cpuUsageCommandOBCResponseData {
  usagePct: number;
}

export interface taskRuntimeCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  task: number;
}

export interface taskRuntimeCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: taskRuntimeCommandOBCResponseData | undefined;
}

export interface taskRuntimeCommandOBCResponseData {
  runtime: number;
}

export interface gpioExpResetCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface gpioExpResetCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpioExpResetCommandOBCResponseData | undefined;
}

export interface gpioExpResetCommandOBCResponseData {
}

export interface i2cResetCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface i2cResetCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: i2cResetCommandOBCResponseData | undefined;
}

export interface i2cResetCommandOBCResponseData {
}

export interface heartbeatCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  enable: boolean;
}

export interface heartbeatCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: heartbeatCommandOBCResponseData | undefined;
}

export interface heartbeatCommandOBCResponseData {
}

export interface gpsRestartCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  startMode: number;
}

export interface gpsRestartCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsRestartCommandOBCResponseData | undefined;
}

export interface gpsRestartCommandOBCResponseData {
}

export interface fwInfoCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface fwInfoCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: fwInfoCommandOBCResponseData | undefined;
}

export interface fwInfoCommandOBCResponseData {
  fwVersion: number[];
  fwHash: number;
  flashAddress: number;
  platform: number;
  target: number;
  flags: number;
  size: number;
  crc32: number;
}

export interface wmmInitCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface wmmInitCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: wmmInitCommandOBCResponseData | undefined;
}

export interface wmmInitCommandOBCResponseData {
}

export interface wmmIsInitCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface wmmIsInitCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: wmmIsInitCommandOBCResponseData | undefined;
}

export interface wmmIsInitCommandOBCResponseData {
  isInit: boolean;
}

export interface wmmGetMagRefCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  lon: number;
  lat: number;
  alt: number;
  year: number;
  month: number;
  day: number;
}

export interface wmmGetMagRefCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: wmmGetMagRefCommandOBCResponseData | undefined;
}

export interface wmmGetMagRefCommandOBCResponseData {
  magRef: number[];
}

export interface gpsQuerySwVerCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface gpsQuerySwVerCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsQuerySwVerCommandOBCResponseData | undefined;
}

export interface gpsQuerySwVerCommandOBCResponseData {
  gpsSoftwareVersion: number[];
}

export interface gpsSetFactorySettingsCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface gpsSetFactorySettingsCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsSetFactorySettingsCommandOBCResponseData | undefined;
}

export interface gpsSetFactorySettingsCommandOBCResponseData {
}

export interface gpsConfigurePowerModeCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  writeSettings: number;
  enablePowerSave: boolean;
}

export interface gpsConfigurePowerModeCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsConfigurePowerModeCommandOBCResponseData | undefined;
}

export interface gpsConfigurePowerModeCommandOBCResponseData {
}

export interface gpsQueryPowerModeCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface gpsQueryPowerModeCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsQueryPowerModeCommandOBCResponseData | undefined;
}

export interface gpsQueryPowerModeCommandOBCResponseData {
  gpsInPowerSaveMode: boolean;
}

export interface gpsQuerySwCrcCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface gpsQuerySwCrcCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsQuerySwCrcCommandOBCResponseData | undefined;
}

export interface gpsQuerySwCrcCommandOBCResponseData {
  softwareCrc: number;
}

export interface gpsConfigurePositionUpateRateCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  writeSettings: number;
  positionUpdateRate: number;
}

export interface gpsConfigurePositionUpateRateCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsConfigurePositionUpateRateCommandOBCResponseData | undefined;
}

export interface gpsConfigurePositionUpateRateCommandOBCResponseData {
}

export interface gpsQueryPositionUpdateRateCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface gpsQueryPositionUpdateRateCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsQueryPositionUpdateRateCommandOBCResponseData | undefined;
}

export interface gpsQueryPositionUpdateRateCommandOBCResponseData {
  updateRate: number;
}

export interface gpsConfigureNmeaMessgesCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  writeSettings: number;
  ggaInterval: number;
  gsaInterval: number;
  gsvInterval: number;
  gllInterval: number;
  rmcInterval: number;
  vtgInterval: number;
  zdaInterval: number;
}

export interface gpsConfigureNmeaMessgesCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsConfigureNmeaMessgesCommandOBCResponseData | undefined;
}

export interface gpsConfigureNmeaMessgesCommandOBCResponseData {
}

export interface gpsQueryNmeaMessgesCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface gpsQueryNmeaMessgesCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsQueryNmeaMessgesCommandOBCResponseData | undefined;
}

export interface gpsQueryNmeaMessgesCommandOBCResponseData {
  ggaInterval: number;
  gsaInterval: number;
  gsvInterval: number;
  gllInterval: number;
  rmcInterval: number;
  vtgInterval: number;
  zdaInterval: number;
}

export interface gpsConfigureSerialPortCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  baudRate: number;
}

export interface gpsConfigureSerialPortCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsConfigureSerialPortCommandOBCResponseData | undefined;
}

export interface gpsConfigureSerialPortCommandOBCResponseData {
}

export interface gpsConfigureNavMsgIntervalCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  writeSettings: number;
  navigationMsgInt: number;
}

export interface gpsConfigureNavMsgIntervalCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: gpsConfigureNavMsgIntervalCommandOBCResponseData | undefined;
}

export interface gpsConfigureNavMsgIntervalCommandOBCResponseData {
}

export interface getUptimeCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface getUptimeCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: getUptimeCommandOBCResponseData | undefined;
}

export interface getUptimeCommandOBCResponseData {
  seconds: number;
}

export interface testEchoCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  number: number;
  array: uint32Matrix | undefined;
  arrayf: doubleMatrix | undefined;
  message: string;
}

export interface testEchoCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEchoCommandOBCResponseData | undefined;
}

export interface testEchoCommandOBCResponseData {
  number: number;
  array: uint32Matrix | undefined;
  arrayf: doubleMatrix | undefined;
  message: string;
}

export interface testHangCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  durationUs: number;
}

export interface testHangCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testHangCommandOBCResponseData | undefined;
}

export interface testHangCommandOBCResponseData {
}

export interface testCommsTxRxCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testCommsTxRxCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCommsTxRxCommandOBCResponseData | undefined;
}

export interface testCommsTxRxCommandOBCResponseData {
  commsErr: number;
  command: number;
}

export interface testCommsStress1CommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testCommsStress1CommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCommsStress1CommandOBCResponseData | undefined;
}

export interface testCommsStress1CommandOBCResponseData {
  pass: number;
  fail: number;
}

export interface testCommsFlashAppCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testCommsFlashAppCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCommsFlashAppCommandOBCResponseData | undefined;
}

export interface testCommsFlashAppCommandOBCResponseData {
  commsErr: number;
}

export interface testCommsRebootCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testCommsRebootCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCommsRebootCommandOBCResponseData | undefined;
}

export interface testCommsRebootCommandOBCResponseData {
}

export interface testCommsGetTelemCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testCommsGetTelemCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCommsGetTelemCommandOBCResponseData | undefined;
}

export interface testCommsGetTelemCommandOBCResponseData {
  uptime: number;
  uart0RxCount: number;
  uart1RxCount: number;
  rxMode: number;
  txMode: number;
  adc: number[];
  rssi: number;
  lastLqi: number;
  lastFrequest: number;
  packetsSent: number;
  csCount: number;
  packetsGood: number;
  packetsRejectedChecksum: number;
  packetsRejectedOther: number;
  custom0: number;
  custom1: number;
}

export interface testEpsReadFloatCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  readCmd: number;
}

export interface testEpsReadFloatCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsReadFloatCommandOBCResponseData | undefined;
}

export interface testEpsReadFloatCommandOBCResponseData {
  epsErr: number;
  data: number;
}

export interface testEpsReadFloatRawCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  readCmd: number;
}

export interface testEpsReadFloatRawCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsReadFloatRawCommandOBCResponseData | undefined;
}

export interface testEpsReadFloatRawCommandOBCResponseData {
  epsErr: number;
  data: number;
}

export interface testEpsReadIntCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  readCmd: number;
}

export interface testEpsReadIntCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsReadIntCommandOBCResponseData | undefined;
}

export interface testEpsReadIntCommandOBCResponseData {
  epsErr: number;
  data: number;
}

export interface testEpsWriteCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  writeCmd: number;
  state: number;
}

export interface testEpsWriteCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsWriteCommandOBCResponseData | undefined;
}

export interface testEpsWriteCommandOBCResponseData {
  epsErr: number;
}

export interface testEpsReadSanityCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testEpsReadSanityCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsReadSanityCommandOBCResponseData | undefined;
}

export interface testEpsReadSanityCommandOBCResponseData {
  successCounter: number;
}

export interface testEpsWritereadSanityCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testEpsWritereadSanityCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsWritereadSanityCommandOBCResponseData | undefined;
}

export interface testEpsWritereadSanityCommandOBCResponseData {
  successCounter: number;
}

export interface testEpsMeasureBusesCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testEpsMeasureBusesCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsMeasureBusesCommandOBCResponseData | undefined;
}

export interface testEpsMeasureBusesCommandOBCResponseData {
  epsErr: number;
  battV: number;
  battA: number;
  bcrV: number;
  bcrA: number;
  bus3v3V: number;
  bus3v3A: number;
  bus5vV: number;
  bus5vA: number;
  lup3v3V: number;
  lup5vV: number;
}

export interface testEpsMeasureTempsCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testEpsMeasureTempsCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsMeasureTempsCommandOBCResponseData | undefined;
}

export interface testEpsMeasureTempsCommandOBCResponseData {
  epsErr: number;
  cellTemps: number[];
  mcuTemp: number;
}

export interface testEpsReadCountersCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testEpsReadCountersCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsReadCountersCommandOBCResponseData | undefined;
}

export interface testEpsReadCountersCommandOBCResponseData {
  epsErr: number;
  uptime: number;
  powerOnCycles: number;
  underVoltage: number;
  shortCircuit: number;
  overTemp: number;
}

export interface testEpsReadStatusCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testEpsReadStatusCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testEpsReadStatusCommandOBCResponseData | undefined;
}

export interface testEpsReadStatusCommandOBCResponseData {
  epsErr: number;
  outCond1: number;
  outCond2: number;
}

export interface testMagCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  magnetorquer: number;
}

export interface testMagCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testMagCommandOBCResponseData | undefined;
}

export interface testMagCommandOBCResponseData {
}

export interface testMagAllCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testMagAllCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testMagAllCommandOBCResponseData | undefined;
}

export interface testMagAllCommandOBCResponseData {
}

export interface testMagInitCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testMagInitCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testMagInitCommandOBCResponseData | undefined;
}

export interface testMagInitCommandOBCResponseData {
}

export interface testImuCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testImuCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testImuCommandOBCResponseData | undefined;
}

export interface testImuCommandOBCResponseData {
}

export interface testPanelGyroCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  gyro: number;
}

export interface testPanelGyroCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testPanelGyroCommandOBCResponseData | undefined;
}

export interface testPanelGyroCommandOBCResponseData {
}

export interface testCanGpioCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  port: number;
  pin: number;
  value: boolean;
}

export interface testCanGpioCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCanGpioCommandOBCResponseData | undefined;
}

export interface testCanGpioCommandOBCResponseData {
  gpioErr: number;
}

export interface testFlashRwCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  addr: number;
  len: number;
}

export interface testFlashRwCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testFlashRwCommandOBCResponseData | undefined;
}

export interface testFlashRwCommandOBCResponseData {
  eraseErr: number;
  writeErr: number;
  readErr: number;
  dataMatch: boolean;
}

export interface testFilesystemCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testFilesystemCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testFilesystemCommandOBCResponseData | undefined;
}

export interface testFilesystemCommandOBCResponseData {
  fsErr: number;
}

export interface testCamInitCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testCamInitCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCamInitCommandOBCResponseData | undefined;
}

export interface testCamInitCommandOBCResponseData {
  arducamErr: number;
}

export interface testCamCaptureCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
}

export interface testCamCaptureCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCamCaptureCommandOBCResponseData | undefined;
}

export interface testCamCaptureCommandOBCResponseData {
  arducamErr: number;
  imageSize: number;
  imageData: Uint8Array;
}

export interface testCamWrSregCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  addr: number;
  data: number;
}

export interface testCamWrSregCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCamWrSregCommandOBCResponseData | undefined;
}

export interface testCamWrSregCommandOBCResponseData {
  ov5642Err: number;
}

export interface testCamRdSregCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  addr: number;
}

export interface testCamRdSregCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCamRdSregCommandOBCResponseData | undefined;
}

export interface testCamRdSregCommandOBCResponseData {
  ov5642Err: number;
  data: number;
}

export interface testCamWrSregsCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  regData: Uint8Array;
}

export interface testCamWrSregsCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testCamWrSregsCommandOBCResponseData | undefined;
}

export interface testCamWrSregsCommandOBCResponseData {
  ov5642Err: number;
}

export interface testDsplibCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  x: number;
  y: number;
}

export interface testDsplibCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testDsplibCommandOBCResponseData | undefined;
}

export interface testDsplibCommandOBCResponseData {
  xAbs: number;
  yAbs: number;
}

export interface testQuestCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  sunObs: number[];
  sunRef: number[];
  magObs: number[];
  magRef: number[];
  weights: number[];
}

export interface testQuestCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testQuestCommandOBCResponseData | undefined;
}

export interface testQuestCommandOBCResponseData {
  quat: number[];
  adcsErr: number;
  avgDuration: number;
}

export interface testRtcGetTimeCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  durationS: number;
}

export interface testRtcGetTimeCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testRtcGetTimeCommandOBCResponseData | undefined;
}

export interface testRtcGetTimeCommandOBCResponseData {
  second: number;
  minute: number;
  hour: number;
  day: number;
  month: number;
  year: number;
}

export interface testRtcCapAndGetTimestampCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  durationS: number;
}

export interface testRtcCapAndGetTimestampCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testRtcCapAndGetTimestampCommandOBCResponseData | undefined;
}

export interface testRtcCapAndGetTimestampCommandOBCResponseData {
  initSecond: number;
  initMinute: number;
  initHour: number;
  initDay: number;
  initMonth: number;
  initYear: number;
  tsSecond: number;
  tsMinute: number;
  tsHour: number;
  tsDay: number;
  tsMonth: number;
  tsYear: number;
}

export interface debugReadRtcRegCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  reg: number;
}

export interface debugReadRtcRegCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: debugReadRtcRegCommandOBCResponseData | undefined;
}

export interface debugReadRtcRegCommandOBCResponseData {
  val: number;
}

export interface debugWriteRtcRegCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  reg: number;
  val: number;
}

export interface debugWriteRtcRegCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: debugWriteRtcRegCommandOBCResponseData | undefined;
}

export interface debugWriteRtcRegCommandOBCResponseData {
  valAfterWrite: number;
}

export interface testRtcAlarmNSecCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  durationS: number;
}

export interface testRtcAlarmNSecCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testRtcAlarmNSecCommandOBCResponseData | undefined;
}

export interface testRtcAlarmNSecCommandOBCResponseData {
}

export interface testAdcsSunModelCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  time: number[];
}

export interface testAdcsSunModelCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testAdcsSunModelCommandOBCResponseData | undefined;
}

export interface testAdcsSunModelCommandOBCResponseData {
  jul: number;
  pos: number[];
}

export interface testAdcVoltageCommandArgs {
  timeout: number;
  dateTime: Date | undefined;
  channel: number;
}

export interface testAdcVoltageCommandOBCResponse {
  cmdInstId: number;
  cmdId: number;
  dateTime: Date | undefined;
  flags: number;
  dataHeader: OBCResponseDataHeader | undefined;
  data: testAdcVoltageCommandOBCResponseData | undefined;
}

export interface testAdcVoltageCommandOBCResponseData {
  channelVoltage: number;
  adcErrStatus: number;
}

export interface uint32Matrix {
  rows: number;
  cols: number;
  data: number[];
}

export interface uint64Matrix {
  rows: number;
  cols: number;
  data: number[];
}

export interface sint32Matrix {
  rows: number;
  cols: number;
  data: number[];
}

export interface sint64Matrix {
  rows: number;
  cols: number;
  data: number[];
}

export interface doubleMatrix {
  rows: number;
  cols: number;
  data: number[];
}

export interface OBCResponseDataHeader {
  code: Code;
  execDatetime: Date | undefined;
}

function createBaseresetCommandArgs(): resetCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const resetCommandArgs = {
  encode(message: resetCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): resetCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseresetCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): resetCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: resetCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBaseresetCommandOBCResponse(): resetCommandOBCResponse {
  return {};
}

export const resetCommandOBCResponse = {
  encode(_: resetCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): resetCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseresetCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): resetCommandOBCResponse {
    return {};
  },

  toJSON(_: resetCommandOBCResponse): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasepingCommandArgs(): pingCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const pingCommandArgs = {
  encode(message: pingCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): pingCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasepingCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): pingCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: pingCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasepingCommandOBCResponse(): pingCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const pingCommandOBCResponse = {
  encode(message: pingCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      pingCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): pingCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasepingCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = pingCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): pingCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? pingCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: pingCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = pingCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasepingCommandOBCResponseData(): pingCommandOBCResponseData {
  return {};
}

export const pingCommandOBCResponseData = {
  encode(_: pingCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): pingCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasepingCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): pingCommandOBCResponseData {
    return {};
  },

  toJSON(_: pingCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasegetTimeCommandArgs(): getTimeCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const getTimeCommandArgs = {
  encode(message: getTimeCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): getTimeCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegetTimeCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): getTimeCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: getTimeCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegetTimeCommandOBCResponse(): getTimeCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const getTimeCommandOBCResponse = {
  encode(message: getTimeCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      getTimeCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): getTimeCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegetTimeCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = getTimeCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): getTimeCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? getTimeCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: getTimeCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = getTimeCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegetTimeCommandOBCResponseData(): getTimeCommandOBCResponseData {
  return { timestamp: undefined };
}

export const getTimeCommandOBCResponseData = {
  encode(message: getTimeCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timestamp !== undefined) {
      Timestamp.encode(toTimestamp(message.timestamp), writer.uint32(10).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): getTimeCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegetTimeCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 10) {
            break;
          }

          message.timestamp = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): getTimeCommandOBCResponseData {
    return { timestamp: isSet(object.timestamp) ? fromJsonTimestamp(object.timestamp) : undefined };
  },

  toJSON(message: getTimeCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.timestamp !== undefined) {
      obj.timestamp = message.timestamp.toISOString();
    }
    return obj;
  },
};

function createBasesetTimeCommandArgs(): setTimeCommandArgs {
  return { timeout: 0, dateTime: undefined, timestamp: undefined };
}

export const setTimeCommandArgs = {
  encode(message: setTimeCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.timestamp !== undefined) {
      Timestamp.encode(toTimestamp(message.timestamp), writer.uint32(26).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): setTimeCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasesetTimeCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.timestamp = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): setTimeCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      timestamp: isSet(object.timestamp) ? fromJsonTimestamp(object.timestamp) : undefined,
    };
  },

  toJSON(message: setTimeCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.timestamp !== undefined) {
      obj.timestamp = message.timestamp.toISOString();
    }
    return obj;
  },
};

function createBasesetTimeCommandOBCResponse(): setTimeCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const setTimeCommandOBCResponse = {
  encode(message: setTimeCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      setTimeCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): setTimeCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasesetTimeCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = setTimeCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): setTimeCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? setTimeCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: setTimeCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = setTimeCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasesetTimeCommandOBCResponseData(): setTimeCommandOBCResponseData {
  return {};
}

export const setTimeCommandOBCResponseData = {
  encode(_: setTimeCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): setTimeCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasesetTimeCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): setTimeCommandOBCResponseData {
    return {};
  },

  toJSON(_: setTimeCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasegetMinStackSpaceCommandArgs(): getMinStackSpaceCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const getMinStackSpaceCommandArgs = {
  encode(message: getMinStackSpaceCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): getMinStackSpaceCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegetMinStackSpaceCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): getMinStackSpaceCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: getMinStackSpaceCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegetMinStackSpaceCommandOBCResponse(): getMinStackSpaceCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const getMinStackSpaceCommandOBCResponse = {
  encode(message: getMinStackSpaceCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      getMinStackSpaceCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): getMinStackSpaceCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegetMinStackSpaceCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = getMinStackSpaceCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): getMinStackSpaceCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? getMinStackSpaceCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: getMinStackSpaceCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = getMinStackSpaceCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegetMinStackSpaceCommandOBCResponseData(): getMinStackSpaceCommandOBCResponseData {
  return { minStackSpace: new Uint8Array(0) };
}

export const getMinStackSpaceCommandOBCResponseData = {
  encode(message: getMinStackSpaceCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.minStackSpace.length !== 0) {
      writer.uint32(10).bytes(message.minStackSpace);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): getMinStackSpaceCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegetMinStackSpaceCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 10) {
            break;
          }

          message.minStackSpace = reader.bytes();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): getMinStackSpaceCommandOBCResponseData {
    return { minStackSpace: isSet(object.minStackSpace) ? bytesFromBase64(object.minStackSpace) : new Uint8Array(0) };
  },

  toJSON(message: getMinStackSpaceCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.minStackSpace.length !== 0) {
      obj.minStackSpace = base64FromBytes(message.minStackSpace);
    }
    return obj;
  },
};

function createBasecaptureRtosTraceCommandArgs(): captureRtosTraceCommandArgs {
  return { timeout: 0, dateTime: undefined, length: 0 };
}

export const captureRtosTraceCommandArgs = {
  encode(message: captureRtosTraceCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.length !== 0) {
      writer.uint32(24).uint32(message.length);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): captureRtosTraceCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasecaptureRtosTraceCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.length = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): captureRtosTraceCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      length: isSet(object.length) ? globalThis.Number(object.length) : 0,
    };
  },

  toJSON(message: captureRtosTraceCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.length !== 0) {
      obj.length = Math.round(message.length);
    }
    return obj;
  },
};

function createBasecaptureRtosTraceCommandOBCResponse(): captureRtosTraceCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const captureRtosTraceCommandOBCResponse = {
  encode(message: captureRtosTraceCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      captureRtosTraceCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): captureRtosTraceCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasecaptureRtosTraceCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = captureRtosTraceCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): captureRtosTraceCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? captureRtosTraceCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: captureRtosTraceCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = captureRtosTraceCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasecaptureRtosTraceCommandOBCResponseData(): captureRtosTraceCommandOBCResponseData {
  return { trace: new Uint8Array(0) };
}

export const captureRtosTraceCommandOBCResponseData = {
  encode(message: captureRtosTraceCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.trace.length !== 0) {
      writer.uint32(10).bytes(message.trace);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): captureRtosTraceCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasecaptureRtosTraceCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 10) {
            break;
          }

          message.trace = reader.bytes();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): captureRtosTraceCommandOBCResponseData {
    return { trace: isSet(object.trace) ? bytesFromBase64(object.trace) : new Uint8Array(0) };
  },

  toJSON(message: captureRtosTraceCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.trace.length !== 0) {
      obj.trace = base64FromBytes(message.trace);
    }
    return obj;
  },
};

function createBasertosStateCommandArgs(): rtosStateCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const rtosStateCommandArgs = {
  encode(message: rtosStateCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): rtosStateCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasertosStateCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): rtosStateCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: rtosStateCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasertosStateCommandOBCResponse(): rtosStateCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const rtosStateCommandOBCResponse = {
  encode(message: rtosStateCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      rtosStateCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): rtosStateCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasertosStateCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = rtosStateCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): rtosStateCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? rtosStateCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: rtosStateCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = rtosStateCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasertosStateCommandOBCResponseData(): rtosStateCommandOBCResponseData {
  return {};
}

export const rtosStateCommandOBCResponseData = {
  encode(_: rtosStateCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): rtosStateCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasertosStateCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): rtosStateCommandOBCResponseData {
    return {};
  },

  toJSON(_: rtosStateCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBaselowPowerCommandArgs(): lowPowerCommandArgs {
  return { timeout: 0, dateTime: undefined, enable: false };
}

export const lowPowerCommandArgs = {
  encode(message: lowPowerCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.enable === true) {
      writer.uint32(24).bool(message.enable);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): lowPowerCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaselowPowerCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.enable = reader.bool();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): lowPowerCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      enable: isSet(object.enable) ? globalThis.Boolean(object.enable) : false,
    };
  },

  toJSON(message: lowPowerCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.enable === true) {
      obj.enable = message.enable;
    }
    return obj;
  },
};

function createBaselowPowerCommandOBCResponse(): lowPowerCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const lowPowerCommandOBCResponse = {
  encode(message: lowPowerCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      lowPowerCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): lowPowerCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaselowPowerCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = lowPowerCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): lowPowerCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? lowPowerCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: lowPowerCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = lowPowerCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBaselowPowerCommandOBCResponseData(): lowPowerCommandOBCResponseData {
  return {};
}

export const lowPowerCommandOBCResponseData = {
  encode(_: lowPowerCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): lowPowerCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaselowPowerCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): lowPowerCommandOBCResponseData {
    return {};
  },

  toJSON(_: lowPowerCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasesysTimeCommandArgs(): sysTimeCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const sysTimeCommandArgs = {
  encode(message: sysTimeCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): sysTimeCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasesysTimeCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): sysTimeCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: sysTimeCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasesysTimeCommandOBCResponse(): sysTimeCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const sysTimeCommandOBCResponse = {
  encode(message: sysTimeCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      sysTimeCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): sysTimeCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasesysTimeCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = sysTimeCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): sysTimeCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? sysTimeCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: sysTimeCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = sysTimeCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasesysTimeCommandOBCResponseData(): sysTimeCommandOBCResponseData {
  return { sysTimeUs: 0 };
}

export const sysTimeCommandOBCResponseData = {
  encode(message: sysTimeCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.sysTimeUs !== 0) {
      writer.uint32(8).uint32(message.sysTimeUs);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): sysTimeCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasesysTimeCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.sysTimeUs = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): sysTimeCommandOBCResponseData {
    return { sysTimeUs: isSet(object.sysTimeUs) ? globalThis.Number(object.sysTimeUs) : 0 };
  },

  toJSON(message: sysTimeCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.sysTimeUs !== 0) {
      obj.sysTimeUs = Math.round(message.sysTimeUs);
    }
    return obj;
  },
};

function createBasecpuUsageCommandArgs(): cpuUsageCommandArgs {
  return { timeout: 0, dateTime: undefined, task: 0 };
}

export const cpuUsageCommandArgs = {
  encode(message: cpuUsageCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.task !== 0) {
      writer.uint32(24).uint32(message.task);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): cpuUsageCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasecpuUsageCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.task = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): cpuUsageCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      task: isSet(object.task) ? globalThis.Number(object.task) : 0,
    };
  },

  toJSON(message: cpuUsageCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.task !== 0) {
      obj.task = Math.round(message.task);
    }
    return obj;
  },
};

function createBasecpuUsageCommandOBCResponse(): cpuUsageCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const cpuUsageCommandOBCResponse = {
  encode(message: cpuUsageCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      cpuUsageCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): cpuUsageCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasecpuUsageCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = cpuUsageCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): cpuUsageCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? cpuUsageCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: cpuUsageCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = cpuUsageCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasecpuUsageCommandOBCResponseData(): cpuUsageCommandOBCResponseData {
  return { usagePct: 0 };
}

export const cpuUsageCommandOBCResponseData = {
  encode(message: cpuUsageCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.usagePct !== 0) {
      writer.uint32(8).uint32(message.usagePct);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): cpuUsageCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasecpuUsageCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.usagePct = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): cpuUsageCommandOBCResponseData {
    return { usagePct: isSet(object.usagePct) ? globalThis.Number(object.usagePct) : 0 };
  },

  toJSON(message: cpuUsageCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.usagePct !== 0) {
      obj.usagePct = Math.round(message.usagePct);
    }
    return obj;
  },
};

function createBasetaskRuntimeCommandArgs(): taskRuntimeCommandArgs {
  return { timeout: 0, dateTime: undefined, task: 0 };
}

export const taskRuntimeCommandArgs = {
  encode(message: taskRuntimeCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.task !== 0) {
      writer.uint32(24).uint32(message.task);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): taskRuntimeCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetaskRuntimeCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.task = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): taskRuntimeCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      task: isSet(object.task) ? globalThis.Number(object.task) : 0,
    };
  },

  toJSON(message: taskRuntimeCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.task !== 0) {
      obj.task = Math.round(message.task);
    }
    return obj;
  },
};

function createBasetaskRuntimeCommandOBCResponse(): taskRuntimeCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const taskRuntimeCommandOBCResponse = {
  encode(message: taskRuntimeCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      taskRuntimeCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): taskRuntimeCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetaskRuntimeCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = taskRuntimeCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): taskRuntimeCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? taskRuntimeCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: taskRuntimeCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = taskRuntimeCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetaskRuntimeCommandOBCResponseData(): taskRuntimeCommandOBCResponseData {
  return { runtime: 0 };
}

export const taskRuntimeCommandOBCResponseData = {
  encode(message: taskRuntimeCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.runtime !== 0) {
      writer.uint32(8).uint32(message.runtime);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): taskRuntimeCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetaskRuntimeCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.runtime = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): taskRuntimeCommandOBCResponseData {
    return { runtime: isSet(object.runtime) ? globalThis.Number(object.runtime) : 0 };
  },

  toJSON(message: taskRuntimeCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.runtime !== 0) {
      obj.runtime = Math.round(message.runtime);
    }
    return obj;
  },
};

function createBasegpioExpResetCommandArgs(): gpioExpResetCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const gpioExpResetCommandArgs = {
  encode(message: gpioExpResetCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpioExpResetCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpioExpResetCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpioExpResetCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: gpioExpResetCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegpioExpResetCommandOBCResponse(): gpioExpResetCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpioExpResetCommandOBCResponse = {
  encode(message: gpioExpResetCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpioExpResetCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpioExpResetCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpioExpResetCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpioExpResetCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpioExpResetCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpioExpResetCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpioExpResetCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpioExpResetCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpioExpResetCommandOBCResponseData(): gpioExpResetCommandOBCResponseData {
  return {};
}

export const gpioExpResetCommandOBCResponseData = {
  encode(_: gpioExpResetCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpioExpResetCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpioExpResetCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): gpioExpResetCommandOBCResponseData {
    return {};
  },

  toJSON(_: gpioExpResetCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasei2cResetCommandArgs(): i2cResetCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const i2cResetCommandArgs = {
  encode(message: i2cResetCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): i2cResetCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasei2cResetCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): i2cResetCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: i2cResetCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasei2cResetCommandOBCResponse(): i2cResetCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const i2cResetCommandOBCResponse = {
  encode(message: i2cResetCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      i2cResetCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): i2cResetCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasei2cResetCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = i2cResetCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): i2cResetCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? i2cResetCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: i2cResetCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = i2cResetCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasei2cResetCommandOBCResponseData(): i2cResetCommandOBCResponseData {
  return {};
}

export const i2cResetCommandOBCResponseData = {
  encode(_: i2cResetCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): i2cResetCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasei2cResetCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): i2cResetCommandOBCResponseData {
    return {};
  },

  toJSON(_: i2cResetCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBaseheartbeatCommandArgs(): heartbeatCommandArgs {
  return { timeout: 0, dateTime: undefined, enable: false };
}

export const heartbeatCommandArgs = {
  encode(message: heartbeatCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.enable === true) {
      writer.uint32(24).bool(message.enable);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): heartbeatCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseheartbeatCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.enable = reader.bool();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): heartbeatCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      enable: isSet(object.enable) ? globalThis.Boolean(object.enable) : false,
    };
  },

  toJSON(message: heartbeatCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.enable === true) {
      obj.enable = message.enable;
    }
    return obj;
  },
};

function createBaseheartbeatCommandOBCResponse(): heartbeatCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const heartbeatCommandOBCResponse = {
  encode(message: heartbeatCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      heartbeatCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): heartbeatCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseheartbeatCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = heartbeatCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): heartbeatCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? heartbeatCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: heartbeatCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = heartbeatCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBaseheartbeatCommandOBCResponseData(): heartbeatCommandOBCResponseData {
  return {};
}

export const heartbeatCommandOBCResponseData = {
  encode(_: heartbeatCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): heartbeatCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseheartbeatCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): heartbeatCommandOBCResponseData {
    return {};
  },

  toJSON(_: heartbeatCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasegpsRestartCommandArgs(): gpsRestartCommandArgs {
  return { timeout: 0, dateTime: undefined, startMode: 0 };
}

export const gpsRestartCommandArgs = {
  encode(message: gpsRestartCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.startMode !== 0) {
      writer.uint32(24).uint32(message.startMode);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsRestartCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsRestartCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.startMode = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsRestartCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      startMode: isSet(object.startMode) ? globalThis.Number(object.startMode) : 0,
    };
  },

  toJSON(message: gpsRestartCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.startMode !== 0) {
      obj.startMode = Math.round(message.startMode);
    }
    return obj;
  },
};

function createBasegpsRestartCommandOBCResponse(): gpsRestartCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsRestartCommandOBCResponse = {
  encode(message: gpsRestartCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsRestartCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsRestartCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsRestartCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsRestartCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsRestartCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsRestartCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsRestartCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsRestartCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsRestartCommandOBCResponseData(): gpsRestartCommandOBCResponseData {
  return {};
}

export const gpsRestartCommandOBCResponseData = {
  encode(_: gpsRestartCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsRestartCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsRestartCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): gpsRestartCommandOBCResponseData {
    return {};
  },

  toJSON(_: gpsRestartCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasefwInfoCommandArgs(): fwInfoCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const fwInfoCommandArgs = {
  encode(message: fwInfoCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): fwInfoCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasefwInfoCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): fwInfoCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: fwInfoCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasefwInfoCommandOBCResponse(): fwInfoCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const fwInfoCommandOBCResponse = {
  encode(message: fwInfoCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      fwInfoCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): fwInfoCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasefwInfoCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = fwInfoCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): fwInfoCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? fwInfoCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: fwInfoCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = fwInfoCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasefwInfoCommandOBCResponseData(): fwInfoCommandOBCResponseData {
  return { fwVersion: [], fwHash: 0, flashAddress: 0, platform: 0, target: 0, flags: 0, size: 0, crc32: 0 };
}

export const fwInfoCommandOBCResponseData = {
  encode(message: fwInfoCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    writer.uint32(10).fork();
    for (const v of message.fwVersion) {
      writer.uint32(v);
    }
    writer.ldelim();
    if (message.fwHash !== 0) {
      writer.uint32(16).uint32(message.fwHash);
    }
    if (message.flashAddress !== 0) {
      writer.uint32(24).uint32(message.flashAddress);
    }
    if (message.platform !== 0) {
      writer.uint32(32).uint32(message.platform);
    }
    if (message.target !== 0) {
      writer.uint32(40).uint32(message.target);
    }
    if (message.flags !== 0) {
      writer.uint32(48).uint32(message.flags);
    }
    if (message.size !== 0) {
      writer.uint32(56).uint32(message.size);
    }
    if (message.crc32 !== 0) {
      writer.uint32(64).uint32(message.crc32);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): fwInfoCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasefwInfoCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag === 8) {
            message.fwVersion.push(reader.uint32());

            continue;
          }

          if (tag === 10) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.fwVersion.push(reader.uint32());
            }

            continue;
          }

          break;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.fwHash = reader.uint32();
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.flashAddress = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.platform = reader.uint32();
          continue;
        case 5:
          if (tag !== 40) {
            break;
          }

          message.target = reader.uint32();
          continue;
        case 6:
          if (tag !== 48) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 7:
          if (tag !== 56) {
            break;
          }

          message.size = reader.uint32();
          continue;
        case 8:
          if (tag !== 64) {
            break;
          }

          message.crc32 = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): fwInfoCommandOBCResponseData {
    return {
      fwVersion: globalThis.Array.isArray(object?.fwVersion)
        ? object.fwVersion.map((e: any) => globalThis.Number(e))
        : [],
      fwHash: isSet(object.fwHash) ? globalThis.Number(object.fwHash) : 0,
      flashAddress: isSet(object.flashAddress) ? globalThis.Number(object.flashAddress) : 0,
      platform: isSet(object.platform) ? globalThis.Number(object.platform) : 0,
      target: isSet(object.target) ? globalThis.Number(object.target) : 0,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      size: isSet(object.size) ? globalThis.Number(object.size) : 0,
      crc32: isSet(object.crc32) ? globalThis.Number(object.crc32) : 0,
    };
  },

  toJSON(message: fwInfoCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.fwVersion?.length) {
      obj.fwVersion = message.fwVersion.map((e) => Math.round(e));
    }
    if (message.fwHash !== 0) {
      obj.fwHash = Math.round(message.fwHash);
    }
    if (message.flashAddress !== 0) {
      obj.flashAddress = Math.round(message.flashAddress);
    }
    if (message.platform !== 0) {
      obj.platform = Math.round(message.platform);
    }
    if (message.target !== 0) {
      obj.target = Math.round(message.target);
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.size !== 0) {
      obj.size = Math.round(message.size);
    }
    if (message.crc32 !== 0) {
      obj.crc32 = Math.round(message.crc32);
    }
    return obj;
  },
};

function createBasewmmInitCommandArgs(): wmmInitCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const wmmInitCommandArgs = {
  encode(message: wmmInitCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): wmmInitCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasewmmInitCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): wmmInitCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: wmmInitCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasewmmInitCommandOBCResponse(): wmmInitCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const wmmInitCommandOBCResponse = {
  encode(message: wmmInitCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      wmmInitCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): wmmInitCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasewmmInitCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = wmmInitCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): wmmInitCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? wmmInitCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: wmmInitCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = wmmInitCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasewmmInitCommandOBCResponseData(): wmmInitCommandOBCResponseData {
  return {};
}

export const wmmInitCommandOBCResponseData = {
  encode(_: wmmInitCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): wmmInitCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasewmmInitCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): wmmInitCommandOBCResponseData {
    return {};
  },

  toJSON(_: wmmInitCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasewmmIsInitCommandArgs(): wmmIsInitCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const wmmIsInitCommandArgs = {
  encode(message: wmmIsInitCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): wmmIsInitCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasewmmIsInitCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): wmmIsInitCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: wmmIsInitCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasewmmIsInitCommandOBCResponse(): wmmIsInitCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const wmmIsInitCommandOBCResponse = {
  encode(message: wmmIsInitCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      wmmIsInitCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): wmmIsInitCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasewmmIsInitCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = wmmIsInitCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): wmmIsInitCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? wmmIsInitCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: wmmIsInitCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = wmmIsInitCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasewmmIsInitCommandOBCResponseData(): wmmIsInitCommandOBCResponseData {
  return { isInit: false };
}

export const wmmIsInitCommandOBCResponseData = {
  encode(message: wmmIsInitCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.isInit === true) {
      writer.uint32(8).bool(message.isInit);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): wmmIsInitCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasewmmIsInitCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.isInit = reader.bool();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): wmmIsInitCommandOBCResponseData {
    return { isInit: isSet(object.isInit) ? globalThis.Boolean(object.isInit) : false };
  },

  toJSON(message: wmmIsInitCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.isInit === true) {
      obj.isInit = message.isInit;
    }
    return obj;
  },
};

function createBasewmmGetMagRefCommandArgs(): wmmGetMagRefCommandArgs {
  return { timeout: 0, dateTime: undefined, lon: 0, lat: 0, alt: 0, year: 0, month: 0, day: 0 };
}

export const wmmGetMagRefCommandArgs = {
  encode(message: wmmGetMagRefCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.lon !== 0) {
      writer.uint32(25).double(message.lon);
    }
    if (message.lat !== 0) {
      writer.uint32(33).double(message.lat);
    }
    if (message.alt !== 0) {
      writer.uint32(41).double(message.alt);
    }
    if (message.year !== 0) {
      writer.uint32(48).uint32(message.year);
    }
    if (message.month !== 0) {
      writer.uint32(56).uint32(message.month);
    }
    if (message.day !== 0) {
      writer.uint32(64).uint32(message.day);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): wmmGetMagRefCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasewmmGetMagRefCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 25) {
            break;
          }

          message.lon = reader.double();
          continue;
        case 4:
          if (tag !== 33) {
            break;
          }

          message.lat = reader.double();
          continue;
        case 5:
          if (tag !== 41) {
            break;
          }

          message.alt = reader.double();
          continue;
        case 6:
          if (tag !== 48) {
            break;
          }

          message.year = reader.uint32();
          continue;
        case 7:
          if (tag !== 56) {
            break;
          }

          message.month = reader.uint32();
          continue;
        case 8:
          if (tag !== 64) {
            break;
          }

          message.day = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): wmmGetMagRefCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      lon: isSet(object.lon) ? globalThis.Number(object.lon) : 0,
      lat: isSet(object.lat) ? globalThis.Number(object.lat) : 0,
      alt: isSet(object.alt) ? globalThis.Number(object.alt) : 0,
      year: isSet(object.year) ? globalThis.Number(object.year) : 0,
      month: isSet(object.month) ? globalThis.Number(object.month) : 0,
      day: isSet(object.day) ? globalThis.Number(object.day) : 0,
    };
  },

  toJSON(message: wmmGetMagRefCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.lon !== 0) {
      obj.lon = message.lon;
    }
    if (message.lat !== 0) {
      obj.lat = message.lat;
    }
    if (message.alt !== 0) {
      obj.alt = message.alt;
    }
    if (message.year !== 0) {
      obj.year = Math.round(message.year);
    }
    if (message.month !== 0) {
      obj.month = Math.round(message.month);
    }
    if (message.day !== 0) {
      obj.day = Math.round(message.day);
    }
    return obj;
  },
};

function createBasewmmGetMagRefCommandOBCResponse(): wmmGetMagRefCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const wmmGetMagRefCommandOBCResponse = {
  encode(message: wmmGetMagRefCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      wmmGetMagRefCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): wmmGetMagRefCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasewmmGetMagRefCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = wmmGetMagRefCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): wmmGetMagRefCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? wmmGetMagRefCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: wmmGetMagRefCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = wmmGetMagRefCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasewmmGetMagRefCommandOBCResponseData(): wmmGetMagRefCommandOBCResponseData {
  return { magRef: [] };
}

export const wmmGetMagRefCommandOBCResponseData = {
  encode(message: wmmGetMagRefCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    writer.uint32(10).fork();
    for (const v of message.magRef) {
      writer.double(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): wmmGetMagRefCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasewmmGetMagRefCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag === 9) {
            message.magRef.push(reader.double());

            continue;
          }

          if (tag === 10) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.magRef.push(reader.double());
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): wmmGetMagRefCommandOBCResponseData {
    return {
      magRef: globalThis.Array.isArray(object?.magRef) ? object.magRef.map((e: any) => globalThis.Number(e)) : [],
    };
  },

  toJSON(message: wmmGetMagRefCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.magRef?.length) {
      obj.magRef = message.magRef;
    }
    return obj;
  },
};

function createBasegpsQuerySwVerCommandArgs(): gpsQuerySwVerCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const gpsQuerySwVerCommandArgs = {
  encode(message: gpsQuerySwVerCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQuerySwVerCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQuerySwVerCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQuerySwVerCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: gpsQuerySwVerCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegpsQuerySwVerCommandOBCResponse(): gpsQuerySwVerCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsQuerySwVerCommandOBCResponse = {
  encode(message: gpsQuerySwVerCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsQuerySwVerCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQuerySwVerCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQuerySwVerCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsQuerySwVerCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQuerySwVerCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsQuerySwVerCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsQuerySwVerCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsQuerySwVerCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsQuerySwVerCommandOBCResponseData(): gpsQuerySwVerCommandOBCResponseData {
  return { gpsSoftwareVersion: [] };
}

export const gpsQuerySwVerCommandOBCResponseData = {
  encode(message: gpsQuerySwVerCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    writer.uint32(10).fork();
    for (const v of message.gpsSoftwareVersion) {
      writer.uint32(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQuerySwVerCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQuerySwVerCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag === 8) {
            message.gpsSoftwareVersion.push(reader.uint32());

            continue;
          }

          if (tag === 10) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.gpsSoftwareVersion.push(reader.uint32());
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQuerySwVerCommandOBCResponseData {
    return {
      gpsSoftwareVersion: globalThis.Array.isArray(object?.gpsSoftwareVersion)
        ? object.gpsSoftwareVersion.map((e: any) => globalThis.Number(e))
        : [],
    };
  },

  toJSON(message: gpsQuerySwVerCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.gpsSoftwareVersion?.length) {
      obj.gpsSoftwareVersion = message.gpsSoftwareVersion.map((e) => Math.round(e));
    }
    return obj;
  },
};

function createBasegpsSetFactorySettingsCommandArgs(): gpsSetFactorySettingsCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const gpsSetFactorySettingsCommandArgs = {
  encode(message: gpsSetFactorySettingsCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsSetFactorySettingsCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsSetFactorySettingsCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsSetFactorySettingsCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: gpsSetFactorySettingsCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegpsSetFactorySettingsCommandOBCResponse(): gpsSetFactorySettingsCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsSetFactorySettingsCommandOBCResponse = {
  encode(message: gpsSetFactorySettingsCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsSetFactorySettingsCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsSetFactorySettingsCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsSetFactorySettingsCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsSetFactorySettingsCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsSetFactorySettingsCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsSetFactorySettingsCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsSetFactorySettingsCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsSetFactorySettingsCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsSetFactorySettingsCommandOBCResponseData(): gpsSetFactorySettingsCommandOBCResponseData {
  return {};
}

export const gpsSetFactorySettingsCommandOBCResponseData = {
  encode(_: gpsSetFactorySettingsCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsSetFactorySettingsCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsSetFactorySettingsCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): gpsSetFactorySettingsCommandOBCResponseData {
    return {};
  },

  toJSON(_: gpsSetFactorySettingsCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasegpsConfigurePowerModeCommandArgs(): gpsConfigurePowerModeCommandArgs {
  return { timeout: 0, dateTime: undefined, writeSettings: 0, enablePowerSave: false };
}

export const gpsConfigurePowerModeCommandArgs = {
  encode(message: gpsConfigurePowerModeCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.writeSettings !== 0) {
      writer.uint32(24).uint32(message.writeSettings);
    }
    if (message.enablePowerSave === true) {
      writer.uint32(32).bool(message.enablePowerSave);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigurePowerModeCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigurePowerModeCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.writeSettings = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.enablePowerSave = reader.bool();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigurePowerModeCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      writeSettings: isSet(object.writeSettings) ? globalThis.Number(object.writeSettings) : 0,
      enablePowerSave: isSet(object.enablePowerSave) ? globalThis.Boolean(object.enablePowerSave) : false,
    };
  },

  toJSON(message: gpsConfigurePowerModeCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.writeSettings !== 0) {
      obj.writeSettings = Math.round(message.writeSettings);
    }
    if (message.enablePowerSave === true) {
      obj.enablePowerSave = message.enablePowerSave;
    }
    return obj;
  },
};

function createBasegpsConfigurePowerModeCommandOBCResponse(): gpsConfigurePowerModeCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsConfigurePowerModeCommandOBCResponse = {
  encode(message: gpsConfigurePowerModeCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsConfigurePowerModeCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigurePowerModeCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigurePowerModeCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsConfigurePowerModeCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigurePowerModeCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsConfigurePowerModeCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsConfigurePowerModeCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsConfigurePowerModeCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsConfigurePowerModeCommandOBCResponseData(): gpsConfigurePowerModeCommandOBCResponseData {
  return {};
}

export const gpsConfigurePowerModeCommandOBCResponseData = {
  encode(_: gpsConfigurePowerModeCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigurePowerModeCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigurePowerModeCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): gpsConfigurePowerModeCommandOBCResponseData {
    return {};
  },

  toJSON(_: gpsConfigurePowerModeCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasegpsQueryPowerModeCommandArgs(): gpsQueryPowerModeCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const gpsQueryPowerModeCommandArgs = {
  encode(message: gpsQueryPowerModeCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQueryPowerModeCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQueryPowerModeCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQueryPowerModeCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: gpsQueryPowerModeCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegpsQueryPowerModeCommandOBCResponse(): gpsQueryPowerModeCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsQueryPowerModeCommandOBCResponse = {
  encode(message: gpsQueryPowerModeCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsQueryPowerModeCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQueryPowerModeCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQueryPowerModeCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsQueryPowerModeCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQueryPowerModeCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsQueryPowerModeCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsQueryPowerModeCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsQueryPowerModeCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsQueryPowerModeCommandOBCResponseData(): gpsQueryPowerModeCommandOBCResponseData {
  return { gpsInPowerSaveMode: false };
}

export const gpsQueryPowerModeCommandOBCResponseData = {
  encode(message: gpsQueryPowerModeCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.gpsInPowerSaveMode === true) {
      writer.uint32(8).bool(message.gpsInPowerSaveMode);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQueryPowerModeCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQueryPowerModeCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.gpsInPowerSaveMode = reader.bool();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQueryPowerModeCommandOBCResponseData {
    return {
      gpsInPowerSaveMode: isSet(object.gpsInPowerSaveMode) ? globalThis.Boolean(object.gpsInPowerSaveMode) : false,
    };
  },

  toJSON(message: gpsQueryPowerModeCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.gpsInPowerSaveMode === true) {
      obj.gpsInPowerSaveMode = message.gpsInPowerSaveMode;
    }
    return obj;
  },
};

function createBasegpsQuerySwCrcCommandArgs(): gpsQuerySwCrcCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const gpsQuerySwCrcCommandArgs = {
  encode(message: gpsQuerySwCrcCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQuerySwCrcCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQuerySwCrcCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQuerySwCrcCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: gpsQuerySwCrcCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegpsQuerySwCrcCommandOBCResponse(): gpsQuerySwCrcCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsQuerySwCrcCommandOBCResponse = {
  encode(message: gpsQuerySwCrcCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsQuerySwCrcCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQuerySwCrcCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQuerySwCrcCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsQuerySwCrcCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQuerySwCrcCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsQuerySwCrcCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsQuerySwCrcCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsQuerySwCrcCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsQuerySwCrcCommandOBCResponseData(): gpsQuerySwCrcCommandOBCResponseData {
  return { softwareCrc: 0 };
}

export const gpsQuerySwCrcCommandOBCResponseData = {
  encode(message: gpsQuerySwCrcCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.softwareCrc !== 0) {
      writer.uint32(8).uint32(message.softwareCrc);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQuerySwCrcCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQuerySwCrcCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.softwareCrc = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQuerySwCrcCommandOBCResponseData {
    return { softwareCrc: isSet(object.softwareCrc) ? globalThis.Number(object.softwareCrc) : 0 };
  },

  toJSON(message: gpsQuerySwCrcCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.softwareCrc !== 0) {
      obj.softwareCrc = Math.round(message.softwareCrc);
    }
    return obj;
  },
};

function createBasegpsConfigurePositionUpateRateCommandArgs(): gpsConfigurePositionUpateRateCommandArgs {
  return { timeout: 0, dateTime: undefined, writeSettings: 0, positionUpdateRate: 0 };
}

export const gpsConfigurePositionUpateRateCommandArgs = {
  encode(message: gpsConfigurePositionUpateRateCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.writeSettings !== 0) {
      writer.uint32(24).uint32(message.writeSettings);
    }
    if (message.positionUpdateRate !== 0) {
      writer.uint32(32).uint32(message.positionUpdateRate);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigurePositionUpateRateCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigurePositionUpateRateCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.writeSettings = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.positionUpdateRate = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigurePositionUpateRateCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      writeSettings: isSet(object.writeSettings) ? globalThis.Number(object.writeSettings) : 0,
      positionUpdateRate: isSet(object.positionUpdateRate) ? globalThis.Number(object.positionUpdateRate) : 0,
    };
  },

  toJSON(message: gpsConfigurePositionUpateRateCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.writeSettings !== 0) {
      obj.writeSettings = Math.round(message.writeSettings);
    }
    if (message.positionUpdateRate !== 0) {
      obj.positionUpdateRate = Math.round(message.positionUpdateRate);
    }
    return obj;
  },
};

function createBasegpsConfigurePositionUpateRateCommandOBCResponse(): gpsConfigurePositionUpateRateCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsConfigurePositionUpateRateCommandOBCResponse = {
  encode(
    message: gpsConfigurePositionUpateRateCommandOBCResponse,
    writer: _m0.Writer = _m0.Writer.create(),
  ): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsConfigurePositionUpateRateCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigurePositionUpateRateCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigurePositionUpateRateCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsConfigurePositionUpateRateCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigurePositionUpateRateCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsConfigurePositionUpateRateCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsConfigurePositionUpateRateCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsConfigurePositionUpateRateCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsConfigurePositionUpateRateCommandOBCResponseData(): gpsConfigurePositionUpateRateCommandOBCResponseData {
  return {};
}

export const gpsConfigurePositionUpateRateCommandOBCResponseData = {
  encode(_: gpsConfigurePositionUpateRateCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigurePositionUpateRateCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigurePositionUpateRateCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): gpsConfigurePositionUpateRateCommandOBCResponseData {
    return {};
  },

  toJSON(_: gpsConfigurePositionUpateRateCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasegpsQueryPositionUpdateRateCommandArgs(): gpsQueryPositionUpdateRateCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const gpsQueryPositionUpdateRateCommandArgs = {
  encode(message: gpsQueryPositionUpdateRateCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQueryPositionUpdateRateCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQueryPositionUpdateRateCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQueryPositionUpdateRateCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: gpsQueryPositionUpdateRateCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegpsQueryPositionUpdateRateCommandOBCResponse(): gpsQueryPositionUpdateRateCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsQueryPositionUpdateRateCommandOBCResponse = {
  encode(message: gpsQueryPositionUpdateRateCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsQueryPositionUpdateRateCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQueryPositionUpdateRateCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQueryPositionUpdateRateCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsQueryPositionUpdateRateCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQueryPositionUpdateRateCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsQueryPositionUpdateRateCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsQueryPositionUpdateRateCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsQueryPositionUpdateRateCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsQueryPositionUpdateRateCommandOBCResponseData(): gpsQueryPositionUpdateRateCommandOBCResponseData {
  return { updateRate: 0 };
}

export const gpsQueryPositionUpdateRateCommandOBCResponseData = {
  encode(
    message: gpsQueryPositionUpdateRateCommandOBCResponseData,
    writer: _m0.Writer = _m0.Writer.create(),
  ): _m0.Writer {
    if (message.updateRate !== 0) {
      writer.uint32(8).uint32(message.updateRate);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQueryPositionUpdateRateCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQueryPositionUpdateRateCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.updateRate = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQueryPositionUpdateRateCommandOBCResponseData {
    return { updateRate: isSet(object.updateRate) ? globalThis.Number(object.updateRate) : 0 };
  },

  toJSON(message: gpsQueryPositionUpdateRateCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.updateRate !== 0) {
      obj.updateRate = Math.round(message.updateRate);
    }
    return obj;
  },
};

function createBasegpsConfigureNmeaMessgesCommandArgs(): gpsConfigureNmeaMessgesCommandArgs {
  return {
    timeout: 0,
    dateTime: undefined,
    writeSettings: 0,
    ggaInterval: 0,
    gsaInterval: 0,
    gsvInterval: 0,
    gllInterval: 0,
    rmcInterval: 0,
    vtgInterval: 0,
    zdaInterval: 0,
  };
}

export const gpsConfigureNmeaMessgesCommandArgs = {
  encode(message: gpsConfigureNmeaMessgesCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.writeSettings !== 0) {
      writer.uint32(24).uint32(message.writeSettings);
    }
    if (message.ggaInterval !== 0) {
      writer.uint32(32).uint32(message.ggaInterval);
    }
    if (message.gsaInterval !== 0) {
      writer.uint32(40).uint32(message.gsaInterval);
    }
    if (message.gsvInterval !== 0) {
      writer.uint32(48).uint32(message.gsvInterval);
    }
    if (message.gllInterval !== 0) {
      writer.uint32(56).uint32(message.gllInterval);
    }
    if (message.rmcInterval !== 0) {
      writer.uint32(64).uint32(message.rmcInterval);
    }
    if (message.vtgInterval !== 0) {
      writer.uint32(72).uint32(message.vtgInterval);
    }
    if (message.zdaInterval !== 0) {
      writer.uint32(80).uint32(message.zdaInterval);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigureNmeaMessgesCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigureNmeaMessgesCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.writeSettings = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.ggaInterval = reader.uint32();
          continue;
        case 5:
          if (tag !== 40) {
            break;
          }

          message.gsaInterval = reader.uint32();
          continue;
        case 6:
          if (tag !== 48) {
            break;
          }

          message.gsvInterval = reader.uint32();
          continue;
        case 7:
          if (tag !== 56) {
            break;
          }

          message.gllInterval = reader.uint32();
          continue;
        case 8:
          if (tag !== 64) {
            break;
          }

          message.rmcInterval = reader.uint32();
          continue;
        case 9:
          if (tag !== 72) {
            break;
          }

          message.vtgInterval = reader.uint32();
          continue;
        case 10:
          if (tag !== 80) {
            break;
          }

          message.zdaInterval = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigureNmeaMessgesCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      writeSettings: isSet(object.writeSettings) ? globalThis.Number(object.writeSettings) : 0,
      ggaInterval: isSet(object.ggaInterval) ? globalThis.Number(object.ggaInterval) : 0,
      gsaInterval: isSet(object.gsaInterval) ? globalThis.Number(object.gsaInterval) : 0,
      gsvInterval: isSet(object.gsvInterval) ? globalThis.Number(object.gsvInterval) : 0,
      gllInterval: isSet(object.gllInterval) ? globalThis.Number(object.gllInterval) : 0,
      rmcInterval: isSet(object.rmcInterval) ? globalThis.Number(object.rmcInterval) : 0,
      vtgInterval: isSet(object.vtgInterval) ? globalThis.Number(object.vtgInterval) : 0,
      zdaInterval: isSet(object.zdaInterval) ? globalThis.Number(object.zdaInterval) : 0,
    };
  },

  toJSON(message: gpsConfigureNmeaMessgesCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.writeSettings !== 0) {
      obj.writeSettings = Math.round(message.writeSettings);
    }
    if (message.ggaInterval !== 0) {
      obj.ggaInterval = Math.round(message.ggaInterval);
    }
    if (message.gsaInterval !== 0) {
      obj.gsaInterval = Math.round(message.gsaInterval);
    }
    if (message.gsvInterval !== 0) {
      obj.gsvInterval = Math.round(message.gsvInterval);
    }
    if (message.gllInterval !== 0) {
      obj.gllInterval = Math.round(message.gllInterval);
    }
    if (message.rmcInterval !== 0) {
      obj.rmcInterval = Math.round(message.rmcInterval);
    }
    if (message.vtgInterval !== 0) {
      obj.vtgInterval = Math.round(message.vtgInterval);
    }
    if (message.zdaInterval !== 0) {
      obj.zdaInterval = Math.round(message.zdaInterval);
    }
    return obj;
  },
};

function createBasegpsConfigureNmeaMessgesCommandOBCResponse(): gpsConfigureNmeaMessgesCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsConfigureNmeaMessgesCommandOBCResponse = {
  encode(message: gpsConfigureNmeaMessgesCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsConfigureNmeaMessgesCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigureNmeaMessgesCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigureNmeaMessgesCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsConfigureNmeaMessgesCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigureNmeaMessgesCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsConfigureNmeaMessgesCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsConfigureNmeaMessgesCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsConfigureNmeaMessgesCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsConfigureNmeaMessgesCommandOBCResponseData(): gpsConfigureNmeaMessgesCommandOBCResponseData {
  return {};
}

export const gpsConfigureNmeaMessgesCommandOBCResponseData = {
  encode(_: gpsConfigureNmeaMessgesCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigureNmeaMessgesCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigureNmeaMessgesCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): gpsConfigureNmeaMessgesCommandOBCResponseData {
    return {};
  },

  toJSON(_: gpsConfigureNmeaMessgesCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasegpsQueryNmeaMessgesCommandArgs(): gpsQueryNmeaMessgesCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const gpsQueryNmeaMessgesCommandArgs = {
  encode(message: gpsQueryNmeaMessgesCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQueryNmeaMessgesCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQueryNmeaMessgesCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQueryNmeaMessgesCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: gpsQueryNmeaMessgesCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegpsQueryNmeaMessgesCommandOBCResponse(): gpsQueryNmeaMessgesCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsQueryNmeaMessgesCommandOBCResponse = {
  encode(message: gpsQueryNmeaMessgesCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsQueryNmeaMessgesCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQueryNmeaMessgesCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQueryNmeaMessgesCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsQueryNmeaMessgesCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQueryNmeaMessgesCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsQueryNmeaMessgesCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsQueryNmeaMessgesCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsQueryNmeaMessgesCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsQueryNmeaMessgesCommandOBCResponseData(): gpsQueryNmeaMessgesCommandOBCResponseData {
  return {
    ggaInterval: 0,
    gsaInterval: 0,
    gsvInterval: 0,
    gllInterval: 0,
    rmcInterval: 0,
    vtgInterval: 0,
    zdaInterval: 0,
  };
}

export const gpsQueryNmeaMessgesCommandOBCResponseData = {
  encode(message: gpsQueryNmeaMessgesCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.ggaInterval !== 0) {
      writer.uint32(8).uint32(message.ggaInterval);
    }
    if (message.gsaInterval !== 0) {
      writer.uint32(16).uint32(message.gsaInterval);
    }
    if (message.gsvInterval !== 0) {
      writer.uint32(24).uint32(message.gsvInterval);
    }
    if (message.gllInterval !== 0) {
      writer.uint32(32).uint32(message.gllInterval);
    }
    if (message.rmcInterval !== 0) {
      writer.uint32(40).uint32(message.rmcInterval);
    }
    if (message.vtgInterval !== 0) {
      writer.uint32(48).uint32(message.vtgInterval);
    }
    if (message.zdaInterval !== 0) {
      writer.uint32(56).uint32(message.zdaInterval);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsQueryNmeaMessgesCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsQueryNmeaMessgesCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.ggaInterval = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.gsaInterval = reader.uint32();
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.gsvInterval = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.gllInterval = reader.uint32();
          continue;
        case 5:
          if (tag !== 40) {
            break;
          }

          message.rmcInterval = reader.uint32();
          continue;
        case 6:
          if (tag !== 48) {
            break;
          }

          message.vtgInterval = reader.uint32();
          continue;
        case 7:
          if (tag !== 56) {
            break;
          }

          message.zdaInterval = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsQueryNmeaMessgesCommandOBCResponseData {
    return {
      ggaInterval: isSet(object.ggaInterval) ? globalThis.Number(object.ggaInterval) : 0,
      gsaInterval: isSet(object.gsaInterval) ? globalThis.Number(object.gsaInterval) : 0,
      gsvInterval: isSet(object.gsvInterval) ? globalThis.Number(object.gsvInterval) : 0,
      gllInterval: isSet(object.gllInterval) ? globalThis.Number(object.gllInterval) : 0,
      rmcInterval: isSet(object.rmcInterval) ? globalThis.Number(object.rmcInterval) : 0,
      vtgInterval: isSet(object.vtgInterval) ? globalThis.Number(object.vtgInterval) : 0,
      zdaInterval: isSet(object.zdaInterval) ? globalThis.Number(object.zdaInterval) : 0,
    };
  },

  toJSON(message: gpsQueryNmeaMessgesCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.ggaInterval !== 0) {
      obj.ggaInterval = Math.round(message.ggaInterval);
    }
    if (message.gsaInterval !== 0) {
      obj.gsaInterval = Math.round(message.gsaInterval);
    }
    if (message.gsvInterval !== 0) {
      obj.gsvInterval = Math.round(message.gsvInterval);
    }
    if (message.gllInterval !== 0) {
      obj.gllInterval = Math.round(message.gllInterval);
    }
    if (message.rmcInterval !== 0) {
      obj.rmcInterval = Math.round(message.rmcInterval);
    }
    if (message.vtgInterval !== 0) {
      obj.vtgInterval = Math.round(message.vtgInterval);
    }
    if (message.zdaInterval !== 0) {
      obj.zdaInterval = Math.round(message.zdaInterval);
    }
    return obj;
  },
};

function createBasegpsConfigureSerialPortCommandArgs(): gpsConfigureSerialPortCommandArgs {
  return { timeout: 0, dateTime: undefined, baudRate: 0 };
}

export const gpsConfigureSerialPortCommandArgs = {
  encode(message: gpsConfigureSerialPortCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.baudRate !== 0) {
      writer.uint32(24).uint32(message.baudRate);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigureSerialPortCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigureSerialPortCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.baudRate = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigureSerialPortCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      baudRate: isSet(object.baudRate) ? globalThis.Number(object.baudRate) : 0,
    };
  },

  toJSON(message: gpsConfigureSerialPortCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.baudRate !== 0) {
      obj.baudRate = Math.round(message.baudRate);
    }
    return obj;
  },
};

function createBasegpsConfigureSerialPortCommandOBCResponse(): gpsConfigureSerialPortCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsConfigureSerialPortCommandOBCResponse = {
  encode(message: gpsConfigureSerialPortCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsConfigureSerialPortCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigureSerialPortCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigureSerialPortCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsConfigureSerialPortCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigureSerialPortCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsConfigureSerialPortCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsConfigureSerialPortCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsConfigureSerialPortCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsConfigureSerialPortCommandOBCResponseData(): gpsConfigureSerialPortCommandOBCResponseData {
  return {};
}

export const gpsConfigureSerialPortCommandOBCResponseData = {
  encode(_: gpsConfigureSerialPortCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigureSerialPortCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigureSerialPortCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): gpsConfigureSerialPortCommandOBCResponseData {
    return {};
  },

  toJSON(_: gpsConfigureSerialPortCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasegpsConfigureNavMsgIntervalCommandArgs(): gpsConfigureNavMsgIntervalCommandArgs {
  return { timeout: 0, dateTime: undefined, writeSettings: 0, navigationMsgInt: 0 };
}

export const gpsConfigureNavMsgIntervalCommandArgs = {
  encode(message: gpsConfigureNavMsgIntervalCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.writeSettings !== 0) {
      writer.uint32(24).uint32(message.writeSettings);
    }
    if (message.navigationMsgInt !== 0) {
      writer.uint32(32).uint32(message.navigationMsgInt);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigureNavMsgIntervalCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigureNavMsgIntervalCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.writeSettings = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.navigationMsgInt = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigureNavMsgIntervalCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      writeSettings: isSet(object.writeSettings) ? globalThis.Number(object.writeSettings) : 0,
      navigationMsgInt: isSet(object.navigationMsgInt) ? globalThis.Number(object.navigationMsgInt) : 0,
    };
  },

  toJSON(message: gpsConfigureNavMsgIntervalCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.writeSettings !== 0) {
      obj.writeSettings = Math.round(message.writeSettings);
    }
    if (message.navigationMsgInt !== 0) {
      obj.navigationMsgInt = Math.round(message.navigationMsgInt);
    }
    return obj;
  },
};

function createBasegpsConfigureNavMsgIntervalCommandOBCResponse(): gpsConfigureNavMsgIntervalCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const gpsConfigureNavMsgIntervalCommandOBCResponse = {
  encode(message: gpsConfigureNavMsgIntervalCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      gpsConfigureNavMsgIntervalCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigureNavMsgIntervalCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigureNavMsgIntervalCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = gpsConfigureNavMsgIntervalCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): gpsConfigureNavMsgIntervalCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? gpsConfigureNavMsgIntervalCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: gpsConfigureNavMsgIntervalCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = gpsConfigureNavMsgIntervalCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegpsConfigureNavMsgIntervalCommandOBCResponseData(): gpsConfigureNavMsgIntervalCommandOBCResponseData {
  return {};
}

export const gpsConfigureNavMsgIntervalCommandOBCResponseData = {
  encode(_: gpsConfigureNavMsgIntervalCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): gpsConfigureNavMsgIntervalCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegpsConfigureNavMsgIntervalCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): gpsConfigureNavMsgIntervalCommandOBCResponseData {
    return {};
  },

  toJSON(_: gpsConfigureNavMsgIntervalCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasegetUptimeCommandArgs(): getUptimeCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const getUptimeCommandArgs = {
  encode(message: getUptimeCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): getUptimeCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegetUptimeCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): getUptimeCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: getUptimeCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasegetUptimeCommandOBCResponse(): getUptimeCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const getUptimeCommandOBCResponse = {
  encode(message: getUptimeCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      getUptimeCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): getUptimeCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegetUptimeCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = getUptimeCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): getUptimeCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? getUptimeCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: getUptimeCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = getUptimeCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasegetUptimeCommandOBCResponseData(): getUptimeCommandOBCResponseData {
  return { seconds: 0 };
}

export const getUptimeCommandOBCResponseData = {
  encode(message: getUptimeCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.seconds !== 0) {
      writer.uint32(8).uint32(message.seconds);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): getUptimeCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasegetUptimeCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.seconds = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): getUptimeCommandOBCResponseData {
    return { seconds: isSet(object.seconds) ? globalThis.Number(object.seconds) : 0 };
  },

  toJSON(message: getUptimeCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.seconds !== 0) {
      obj.seconds = Math.round(message.seconds);
    }
    return obj;
  },
};

function createBasetestEchoCommandArgs(): testEchoCommandArgs {
  return { timeout: 0, dateTime: undefined, number: 0, array: undefined, arrayf: undefined, message: "" };
}

export const testEchoCommandArgs = {
  encode(message: testEchoCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.number !== 0) {
      writer.uint32(24).uint32(message.number);
    }
    if (message.array !== undefined) {
      uint32Matrix.encode(message.array, writer.uint32(34).fork()).ldelim();
    }
    if (message.arrayf !== undefined) {
      doubleMatrix.encode(message.arrayf, writer.uint32(42).fork()).ldelim();
    }
    if (message.message !== "") {
      writer.uint32(50).string(message.message);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEchoCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEchoCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.number = reader.uint32();
          continue;
        case 4:
          if (tag !== 34) {
            break;
          }

          message.array = uint32Matrix.decode(reader, reader.uint32());
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.arrayf = doubleMatrix.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.message = reader.string();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEchoCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      number: isSet(object.number) ? globalThis.Number(object.number) : 0,
      array: isSet(object.array) ? uint32Matrix.fromJSON(object.array) : undefined,
      arrayf: isSet(object.arrayf) ? doubleMatrix.fromJSON(object.arrayf) : undefined,
      message: isSet(object.message) ? globalThis.String(object.message) : "",
    };
  },

  toJSON(message: testEchoCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.number !== 0) {
      obj.number = Math.round(message.number);
    }
    if (message.array !== undefined) {
      obj.array = uint32Matrix.toJSON(message.array);
    }
    if (message.arrayf !== undefined) {
      obj.arrayf = doubleMatrix.toJSON(message.arrayf);
    }
    if (message.message !== "") {
      obj.message = message.message;
    }
    return obj;
  },
};

function createBasetestEchoCommandOBCResponse(): testEchoCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEchoCommandOBCResponse = {
  encode(message: testEchoCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEchoCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEchoCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEchoCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEchoCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEchoCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEchoCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEchoCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEchoCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEchoCommandOBCResponseData(): testEchoCommandOBCResponseData {
  return { number: 0, array: undefined, arrayf: undefined, message: "" };
}

export const testEchoCommandOBCResponseData = {
  encode(message: testEchoCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.number !== 0) {
      writer.uint32(8).uint32(message.number);
    }
    if (message.array !== undefined) {
      uint32Matrix.encode(message.array, writer.uint32(18).fork()).ldelim();
    }
    if (message.arrayf !== undefined) {
      doubleMatrix.encode(message.arrayf, writer.uint32(26).fork()).ldelim();
    }
    if (message.message !== "") {
      writer.uint32(34).string(message.message);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEchoCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEchoCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.number = reader.uint32();
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.array = uint32Matrix.decode(reader, reader.uint32());
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.arrayf = doubleMatrix.decode(reader, reader.uint32());
          continue;
        case 4:
          if (tag !== 34) {
            break;
          }

          message.message = reader.string();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEchoCommandOBCResponseData {
    return {
      number: isSet(object.number) ? globalThis.Number(object.number) : 0,
      array: isSet(object.array) ? uint32Matrix.fromJSON(object.array) : undefined,
      arrayf: isSet(object.arrayf) ? doubleMatrix.fromJSON(object.arrayf) : undefined,
      message: isSet(object.message) ? globalThis.String(object.message) : "",
    };
  },

  toJSON(message: testEchoCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.number !== 0) {
      obj.number = Math.round(message.number);
    }
    if (message.array !== undefined) {
      obj.array = uint32Matrix.toJSON(message.array);
    }
    if (message.arrayf !== undefined) {
      obj.arrayf = doubleMatrix.toJSON(message.arrayf);
    }
    if (message.message !== "") {
      obj.message = message.message;
    }
    return obj;
  },
};

function createBasetestHangCommandArgs(): testHangCommandArgs {
  return { timeout: 0, dateTime: undefined, durationUs: 0 };
}

export const testHangCommandArgs = {
  encode(message: testHangCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.durationUs !== 0) {
      writer.uint32(24).uint32(message.durationUs);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testHangCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestHangCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.durationUs = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testHangCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      durationUs: isSet(object.durationUs) ? globalThis.Number(object.durationUs) : 0,
    };
  },

  toJSON(message: testHangCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.durationUs !== 0) {
      obj.durationUs = Math.round(message.durationUs);
    }
    return obj;
  },
};

function createBasetestHangCommandOBCResponse(): testHangCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testHangCommandOBCResponse = {
  encode(message: testHangCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testHangCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testHangCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestHangCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testHangCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testHangCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testHangCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testHangCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testHangCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestHangCommandOBCResponseData(): testHangCommandOBCResponseData {
  return {};
}

export const testHangCommandOBCResponseData = {
  encode(_: testHangCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testHangCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestHangCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): testHangCommandOBCResponseData {
    return {};
  },

  toJSON(_: testHangCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasetestCommsTxRxCommandArgs(): testCommsTxRxCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testCommsTxRxCommandArgs = {
  encode(message: testCommsTxRxCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsTxRxCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsTxRxCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsTxRxCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testCommsTxRxCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestCommsTxRxCommandOBCResponse(): testCommsTxRxCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCommsTxRxCommandOBCResponse = {
  encode(message: testCommsTxRxCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCommsTxRxCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsTxRxCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsTxRxCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCommsTxRxCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsTxRxCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCommsTxRxCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCommsTxRxCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCommsTxRxCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCommsTxRxCommandOBCResponseData(): testCommsTxRxCommandOBCResponseData {
  return { commsErr: 0, command: 0 };
}

export const testCommsTxRxCommandOBCResponseData = {
  encode(message: testCommsTxRxCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.commsErr !== 0) {
      writer.uint32(8).sint32(message.commsErr);
    }
    if (message.command !== 0) {
      writer.uint32(16).uint32(message.command);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsTxRxCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsTxRxCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.commsErr = reader.sint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.command = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsTxRxCommandOBCResponseData {
    return {
      commsErr: isSet(object.commsErr) ? globalThis.Number(object.commsErr) : 0,
      command: isSet(object.command) ? globalThis.Number(object.command) : 0,
    };
  },

  toJSON(message: testCommsTxRxCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.commsErr !== 0) {
      obj.commsErr = Math.round(message.commsErr);
    }
    if (message.command !== 0) {
      obj.command = Math.round(message.command);
    }
    return obj;
  },
};

function createBasetestCommsStress1CommandArgs(): testCommsStress1CommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testCommsStress1CommandArgs = {
  encode(message: testCommsStress1CommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsStress1CommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsStress1CommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsStress1CommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testCommsStress1CommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestCommsStress1CommandOBCResponse(): testCommsStress1CommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCommsStress1CommandOBCResponse = {
  encode(message: testCommsStress1CommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCommsStress1CommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsStress1CommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsStress1CommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCommsStress1CommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsStress1CommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCommsStress1CommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCommsStress1CommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCommsStress1CommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCommsStress1CommandOBCResponseData(): testCommsStress1CommandOBCResponseData {
  return { pass: 0, fail: 0 };
}

export const testCommsStress1CommandOBCResponseData = {
  encode(message: testCommsStress1CommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.pass !== 0) {
      writer.uint32(8).uint32(message.pass);
    }
    if (message.fail !== 0) {
      writer.uint32(16).uint32(message.fail);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsStress1CommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsStress1CommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.pass = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.fail = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsStress1CommandOBCResponseData {
    return {
      pass: isSet(object.pass) ? globalThis.Number(object.pass) : 0,
      fail: isSet(object.fail) ? globalThis.Number(object.fail) : 0,
    };
  },

  toJSON(message: testCommsStress1CommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.pass !== 0) {
      obj.pass = Math.round(message.pass);
    }
    if (message.fail !== 0) {
      obj.fail = Math.round(message.fail);
    }
    return obj;
  },
};

function createBasetestCommsFlashAppCommandArgs(): testCommsFlashAppCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testCommsFlashAppCommandArgs = {
  encode(message: testCommsFlashAppCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsFlashAppCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsFlashAppCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsFlashAppCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testCommsFlashAppCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestCommsFlashAppCommandOBCResponse(): testCommsFlashAppCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCommsFlashAppCommandOBCResponse = {
  encode(message: testCommsFlashAppCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCommsFlashAppCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsFlashAppCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsFlashAppCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCommsFlashAppCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsFlashAppCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCommsFlashAppCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCommsFlashAppCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCommsFlashAppCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCommsFlashAppCommandOBCResponseData(): testCommsFlashAppCommandOBCResponseData {
  return { commsErr: 0 };
}

export const testCommsFlashAppCommandOBCResponseData = {
  encode(message: testCommsFlashAppCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.commsErr !== 0) {
      writer.uint32(8).sint32(message.commsErr);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsFlashAppCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsFlashAppCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.commsErr = reader.sint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsFlashAppCommandOBCResponseData {
    return { commsErr: isSet(object.commsErr) ? globalThis.Number(object.commsErr) : 0 };
  },

  toJSON(message: testCommsFlashAppCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.commsErr !== 0) {
      obj.commsErr = Math.round(message.commsErr);
    }
    return obj;
  },
};

function createBasetestCommsRebootCommandArgs(): testCommsRebootCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testCommsRebootCommandArgs = {
  encode(message: testCommsRebootCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsRebootCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsRebootCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsRebootCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testCommsRebootCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestCommsRebootCommandOBCResponse(): testCommsRebootCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCommsRebootCommandOBCResponse = {
  encode(message: testCommsRebootCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCommsRebootCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsRebootCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsRebootCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCommsRebootCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsRebootCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCommsRebootCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCommsRebootCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCommsRebootCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCommsRebootCommandOBCResponseData(): testCommsRebootCommandOBCResponseData {
  return {};
}

export const testCommsRebootCommandOBCResponseData = {
  encode(_: testCommsRebootCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsRebootCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsRebootCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): testCommsRebootCommandOBCResponseData {
    return {};
  },

  toJSON(_: testCommsRebootCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasetestCommsGetTelemCommandArgs(): testCommsGetTelemCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testCommsGetTelemCommandArgs = {
  encode(message: testCommsGetTelemCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsGetTelemCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsGetTelemCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsGetTelemCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testCommsGetTelemCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestCommsGetTelemCommandOBCResponse(): testCommsGetTelemCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCommsGetTelemCommandOBCResponse = {
  encode(message: testCommsGetTelemCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCommsGetTelemCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsGetTelemCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsGetTelemCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCommsGetTelemCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsGetTelemCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCommsGetTelemCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCommsGetTelemCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCommsGetTelemCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCommsGetTelemCommandOBCResponseData(): testCommsGetTelemCommandOBCResponseData {
  return {
    uptime: 0,
    uart0RxCount: 0,
    uart1RxCount: 0,
    rxMode: 0,
    txMode: 0,
    adc: [],
    rssi: 0,
    lastLqi: 0,
    lastFrequest: 0,
    packetsSent: 0,
    csCount: 0,
    packetsGood: 0,
    packetsRejectedChecksum: 0,
    packetsRejectedOther: 0,
    custom0: 0,
    custom1: 0,
  };
}

export const testCommsGetTelemCommandOBCResponseData = {
  encode(message: testCommsGetTelemCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.uptime !== 0) {
      writer.uint32(8).uint32(message.uptime);
    }
    if (message.uart0RxCount !== 0) {
      writer.uint32(16).uint32(message.uart0RxCount);
    }
    if (message.uart1RxCount !== 0) {
      writer.uint32(24).uint32(message.uart1RxCount);
    }
    if (message.rxMode !== 0) {
      writer.uint32(32).uint32(message.rxMode);
    }
    if (message.txMode !== 0) {
      writer.uint32(40).uint32(message.txMode);
    }
    writer.uint32(50).fork();
    for (const v of message.adc) {
      writer.sint32(v);
    }
    writer.ldelim();
    if (message.rssi !== 0) {
      writer.uint32(56).sint32(message.rssi);
    }
    if (message.lastLqi !== 0) {
      writer.uint32(64).uint32(message.lastLqi);
    }
    if (message.lastFrequest !== 0) {
      writer.uint32(72).sint32(message.lastFrequest);
    }
    if (message.packetsSent !== 0) {
      writer.uint32(80).uint32(message.packetsSent);
    }
    if (message.csCount !== 0) {
      writer.uint32(88).uint32(message.csCount);
    }
    if (message.packetsGood !== 0) {
      writer.uint32(96).uint32(message.packetsGood);
    }
    if (message.packetsRejectedChecksum !== 0) {
      writer.uint32(104).uint32(message.packetsRejectedChecksum);
    }
    if (message.packetsRejectedOther !== 0) {
      writer.uint32(112).uint32(message.packetsRejectedOther);
    }
    if (message.custom0 !== 0) {
      writer.uint32(120).uint32(message.custom0);
    }
    if (message.custom1 !== 0) {
      writer.uint32(128).uint32(message.custom1);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCommsGetTelemCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCommsGetTelemCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.uptime = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.uart0RxCount = reader.uint32();
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.uart1RxCount = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.rxMode = reader.uint32();
          continue;
        case 5:
          if (tag !== 40) {
            break;
          }

          message.txMode = reader.uint32();
          continue;
        case 6:
          if (tag === 48) {
            message.adc.push(reader.sint32());

            continue;
          }

          if (tag === 50) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.adc.push(reader.sint32());
            }

            continue;
          }

          break;
        case 7:
          if (tag !== 56) {
            break;
          }

          message.rssi = reader.sint32();
          continue;
        case 8:
          if (tag !== 64) {
            break;
          }

          message.lastLqi = reader.uint32();
          continue;
        case 9:
          if (tag !== 72) {
            break;
          }

          message.lastFrequest = reader.sint32();
          continue;
        case 10:
          if (tag !== 80) {
            break;
          }

          message.packetsSent = reader.uint32();
          continue;
        case 11:
          if (tag !== 88) {
            break;
          }

          message.csCount = reader.uint32();
          continue;
        case 12:
          if (tag !== 96) {
            break;
          }

          message.packetsGood = reader.uint32();
          continue;
        case 13:
          if (tag !== 104) {
            break;
          }

          message.packetsRejectedChecksum = reader.uint32();
          continue;
        case 14:
          if (tag !== 112) {
            break;
          }

          message.packetsRejectedOther = reader.uint32();
          continue;
        case 15:
          if (tag !== 120) {
            break;
          }

          message.custom0 = reader.uint32();
          continue;
        case 16:
          if (tag !== 128) {
            break;
          }

          message.custom1 = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCommsGetTelemCommandOBCResponseData {
    return {
      uptime: isSet(object.uptime) ? globalThis.Number(object.uptime) : 0,
      uart0RxCount: isSet(object.uart0RxCount) ? globalThis.Number(object.uart0RxCount) : 0,
      uart1RxCount: isSet(object.uart1RxCount) ? globalThis.Number(object.uart1RxCount) : 0,
      rxMode: isSet(object.rxMode) ? globalThis.Number(object.rxMode) : 0,
      txMode: isSet(object.txMode) ? globalThis.Number(object.txMode) : 0,
      adc: globalThis.Array.isArray(object?.adc) ? object.adc.map((e: any) => globalThis.Number(e)) : [],
      rssi: isSet(object.rssi) ? globalThis.Number(object.rssi) : 0,
      lastLqi: isSet(object.lastLqi) ? globalThis.Number(object.lastLqi) : 0,
      lastFrequest: isSet(object.lastFrequest) ? globalThis.Number(object.lastFrequest) : 0,
      packetsSent: isSet(object.packetsSent) ? globalThis.Number(object.packetsSent) : 0,
      csCount: isSet(object.csCount) ? globalThis.Number(object.csCount) : 0,
      packetsGood: isSet(object.packetsGood) ? globalThis.Number(object.packetsGood) : 0,
      packetsRejectedChecksum: isSet(object.packetsRejectedChecksum)
        ? globalThis.Number(object.packetsRejectedChecksum)
        : 0,
      packetsRejectedOther: isSet(object.packetsRejectedOther) ? globalThis.Number(object.packetsRejectedOther) : 0,
      custom0: isSet(object.custom0) ? globalThis.Number(object.custom0) : 0,
      custom1: isSet(object.custom1) ? globalThis.Number(object.custom1) : 0,
    };
  },

  toJSON(message: testCommsGetTelemCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.uptime !== 0) {
      obj.uptime = Math.round(message.uptime);
    }
    if (message.uart0RxCount !== 0) {
      obj.uart0RxCount = Math.round(message.uart0RxCount);
    }
    if (message.uart1RxCount !== 0) {
      obj.uart1RxCount = Math.round(message.uart1RxCount);
    }
    if (message.rxMode !== 0) {
      obj.rxMode = Math.round(message.rxMode);
    }
    if (message.txMode !== 0) {
      obj.txMode = Math.round(message.txMode);
    }
    if (message.adc?.length) {
      obj.adc = message.adc.map((e) => Math.round(e));
    }
    if (message.rssi !== 0) {
      obj.rssi = Math.round(message.rssi);
    }
    if (message.lastLqi !== 0) {
      obj.lastLqi = Math.round(message.lastLqi);
    }
    if (message.lastFrequest !== 0) {
      obj.lastFrequest = Math.round(message.lastFrequest);
    }
    if (message.packetsSent !== 0) {
      obj.packetsSent = Math.round(message.packetsSent);
    }
    if (message.csCount !== 0) {
      obj.csCount = Math.round(message.csCount);
    }
    if (message.packetsGood !== 0) {
      obj.packetsGood = Math.round(message.packetsGood);
    }
    if (message.packetsRejectedChecksum !== 0) {
      obj.packetsRejectedChecksum = Math.round(message.packetsRejectedChecksum);
    }
    if (message.packetsRejectedOther !== 0) {
      obj.packetsRejectedOther = Math.round(message.packetsRejectedOther);
    }
    if (message.custom0 !== 0) {
      obj.custom0 = Math.round(message.custom0);
    }
    if (message.custom1 !== 0) {
      obj.custom1 = Math.round(message.custom1);
    }
    return obj;
  },
};

function createBasetestEpsReadFloatCommandArgs(): testEpsReadFloatCommandArgs {
  return { timeout: 0, dateTime: undefined, readCmd: 0 };
}

export const testEpsReadFloatCommandArgs = {
  encode(message: testEpsReadFloatCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.readCmd !== 0) {
      writer.uint32(24).uint32(message.readCmd);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadFloatCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadFloatCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.readCmd = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadFloatCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      readCmd: isSet(object.readCmd) ? globalThis.Number(object.readCmd) : 0,
    };
  },

  toJSON(message: testEpsReadFloatCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.readCmd !== 0) {
      obj.readCmd = Math.round(message.readCmd);
    }
    return obj;
  },
};

function createBasetestEpsReadFloatCommandOBCResponse(): testEpsReadFloatCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsReadFloatCommandOBCResponse = {
  encode(message: testEpsReadFloatCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsReadFloatCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadFloatCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadFloatCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsReadFloatCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadFloatCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsReadFloatCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsReadFloatCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsReadFloatCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsReadFloatCommandOBCResponseData(): testEpsReadFloatCommandOBCResponseData {
  return { epsErr: 0, data: 0 };
}

export const testEpsReadFloatCommandOBCResponseData = {
  encode(message: testEpsReadFloatCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.epsErr !== 0) {
      writer.uint32(8).sint32(message.epsErr);
    }
    if (message.data !== 0) {
      writer.uint32(17).double(message.data);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadFloatCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadFloatCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.epsErr = reader.sint32();
          continue;
        case 2:
          if (tag !== 17) {
            break;
          }

          message.data = reader.double();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadFloatCommandOBCResponseData {
    return {
      epsErr: isSet(object.epsErr) ? globalThis.Number(object.epsErr) : 0,
      data: isSet(object.data) ? globalThis.Number(object.data) : 0,
    };
  },

  toJSON(message: testEpsReadFloatCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.epsErr !== 0) {
      obj.epsErr = Math.round(message.epsErr);
    }
    if (message.data !== 0) {
      obj.data = message.data;
    }
    return obj;
  },
};

function createBasetestEpsReadFloatRawCommandArgs(): testEpsReadFloatRawCommandArgs {
  return { timeout: 0, dateTime: undefined, readCmd: 0 };
}

export const testEpsReadFloatRawCommandArgs = {
  encode(message: testEpsReadFloatRawCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.readCmd !== 0) {
      writer.uint32(24).uint32(message.readCmd);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadFloatRawCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadFloatRawCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.readCmd = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadFloatRawCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      readCmd: isSet(object.readCmd) ? globalThis.Number(object.readCmd) : 0,
    };
  },

  toJSON(message: testEpsReadFloatRawCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.readCmd !== 0) {
      obj.readCmd = Math.round(message.readCmd);
    }
    return obj;
  },
};

function createBasetestEpsReadFloatRawCommandOBCResponse(): testEpsReadFloatRawCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsReadFloatRawCommandOBCResponse = {
  encode(message: testEpsReadFloatRawCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsReadFloatRawCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadFloatRawCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadFloatRawCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsReadFloatRawCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadFloatRawCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsReadFloatRawCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsReadFloatRawCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsReadFloatRawCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsReadFloatRawCommandOBCResponseData(): testEpsReadFloatRawCommandOBCResponseData {
  return { epsErr: 0, data: 0 };
}

export const testEpsReadFloatRawCommandOBCResponseData = {
  encode(message: testEpsReadFloatRawCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.epsErr !== 0) {
      writer.uint32(8).sint32(message.epsErr);
    }
    if (message.data !== 0) {
      writer.uint32(16).uint32(message.data);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadFloatRawCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadFloatRawCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.epsErr = reader.sint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.data = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadFloatRawCommandOBCResponseData {
    return {
      epsErr: isSet(object.epsErr) ? globalThis.Number(object.epsErr) : 0,
      data: isSet(object.data) ? globalThis.Number(object.data) : 0,
    };
  },

  toJSON(message: testEpsReadFloatRawCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.epsErr !== 0) {
      obj.epsErr = Math.round(message.epsErr);
    }
    if (message.data !== 0) {
      obj.data = Math.round(message.data);
    }
    return obj;
  },
};

function createBasetestEpsReadIntCommandArgs(): testEpsReadIntCommandArgs {
  return { timeout: 0, dateTime: undefined, readCmd: 0 };
}

export const testEpsReadIntCommandArgs = {
  encode(message: testEpsReadIntCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.readCmd !== 0) {
      writer.uint32(24).uint32(message.readCmd);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadIntCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadIntCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.readCmd = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadIntCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      readCmd: isSet(object.readCmd) ? globalThis.Number(object.readCmd) : 0,
    };
  },

  toJSON(message: testEpsReadIntCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.readCmd !== 0) {
      obj.readCmd = Math.round(message.readCmd);
    }
    return obj;
  },
};

function createBasetestEpsReadIntCommandOBCResponse(): testEpsReadIntCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsReadIntCommandOBCResponse = {
  encode(message: testEpsReadIntCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsReadIntCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadIntCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadIntCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsReadIntCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadIntCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsReadIntCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsReadIntCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsReadIntCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsReadIntCommandOBCResponseData(): testEpsReadIntCommandOBCResponseData {
  return { epsErr: 0, data: 0 };
}

export const testEpsReadIntCommandOBCResponseData = {
  encode(message: testEpsReadIntCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.epsErr !== 0) {
      writer.uint32(8).sint32(message.epsErr);
    }
    if (message.data !== 0) {
      writer.uint32(16).uint32(message.data);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadIntCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadIntCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.epsErr = reader.sint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.data = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadIntCommandOBCResponseData {
    return {
      epsErr: isSet(object.epsErr) ? globalThis.Number(object.epsErr) : 0,
      data: isSet(object.data) ? globalThis.Number(object.data) : 0,
    };
  },

  toJSON(message: testEpsReadIntCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.epsErr !== 0) {
      obj.epsErr = Math.round(message.epsErr);
    }
    if (message.data !== 0) {
      obj.data = Math.round(message.data);
    }
    return obj;
  },
};

function createBasetestEpsWriteCommandArgs(): testEpsWriteCommandArgs {
  return { timeout: 0, dateTime: undefined, writeCmd: 0, state: 0 };
}

export const testEpsWriteCommandArgs = {
  encode(message: testEpsWriteCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.writeCmd !== 0) {
      writer.uint32(24).uint32(message.writeCmd);
    }
    if (message.state !== 0) {
      writer.uint32(32).uint32(message.state);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsWriteCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsWriteCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.writeCmd = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.state = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsWriteCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      writeCmd: isSet(object.writeCmd) ? globalThis.Number(object.writeCmd) : 0,
      state: isSet(object.state) ? globalThis.Number(object.state) : 0,
    };
  },

  toJSON(message: testEpsWriteCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.writeCmd !== 0) {
      obj.writeCmd = Math.round(message.writeCmd);
    }
    if (message.state !== 0) {
      obj.state = Math.round(message.state);
    }
    return obj;
  },
};

function createBasetestEpsWriteCommandOBCResponse(): testEpsWriteCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsWriteCommandOBCResponse = {
  encode(message: testEpsWriteCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsWriteCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsWriteCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsWriteCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsWriteCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsWriteCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsWriteCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsWriteCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsWriteCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsWriteCommandOBCResponseData(): testEpsWriteCommandOBCResponseData {
  return { epsErr: 0 };
}

export const testEpsWriteCommandOBCResponseData = {
  encode(message: testEpsWriteCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.epsErr !== 0) {
      writer.uint32(8).sint32(message.epsErr);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsWriteCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsWriteCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.epsErr = reader.sint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsWriteCommandOBCResponseData {
    return { epsErr: isSet(object.epsErr) ? globalThis.Number(object.epsErr) : 0 };
  },

  toJSON(message: testEpsWriteCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.epsErr !== 0) {
      obj.epsErr = Math.round(message.epsErr);
    }
    return obj;
  },
};

function createBasetestEpsReadSanityCommandArgs(): testEpsReadSanityCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testEpsReadSanityCommandArgs = {
  encode(message: testEpsReadSanityCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadSanityCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadSanityCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadSanityCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testEpsReadSanityCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestEpsReadSanityCommandOBCResponse(): testEpsReadSanityCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsReadSanityCommandOBCResponse = {
  encode(message: testEpsReadSanityCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsReadSanityCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadSanityCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadSanityCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsReadSanityCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadSanityCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsReadSanityCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsReadSanityCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsReadSanityCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsReadSanityCommandOBCResponseData(): testEpsReadSanityCommandOBCResponseData {
  return { successCounter: 0 };
}

export const testEpsReadSanityCommandOBCResponseData = {
  encode(message: testEpsReadSanityCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.successCounter !== 0) {
      writer.uint32(8).uint32(message.successCounter);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadSanityCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadSanityCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.successCounter = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadSanityCommandOBCResponseData {
    return { successCounter: isSet(object.successCounter) ? globalThis.Number(object.successCounter) : 0 };
  },

  toJSON(message: testEpsReadSanityCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.successCounter !== 0) {
      obj.successCounter = Math.round(message.successCounter);
    }
    return obj;
  },
};

function createBasetestEpsWritereadSanityCommandArgs(): testEpsWritereadSanityCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testEpsWritereadSanityCommandArgs = {
  encode(message: testEpsWritereadSanityCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsWritereadSanityCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsWritereadSanityCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsWritereadSanityCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testEpsWritereadSanityCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestEpsWritereadSanityCommandOBCResponse(): testEpsWritereadSanityCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsWritereadSanityCommandOBCResponse = {
  encode(message: testEpsWritereadSanityCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsWritereadSanityCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsWritereadSanityCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsWritereadSanityCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsWritereadSanityCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsWritereadSanityCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsWritereadSanityCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsWritereadSanityCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsWritereadSanityCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsWritereadSanityCommandOBCResponseData(): testEpsWritereadSanityCommandOBCResponseData {
  return { successCounter: 0 };
}

export const testEpsWritereadSanityCommandOBCResponseData = {
  encode(message: testEpsWritereadSanityCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.successCounter !== 0) {
      writer.uint32(8).uint32(message.successCounter);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsWritereadSanityCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsWritereadSanityCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.successCounter = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsWritereadSanityCommandOBCResponseData {
    return { successCounter: isSet(object.successCounter) ? globalThis.Number(object.successCounter) : 0 };
  },

  toJSON(message: testEpsWritereadSanityCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.successCounter !== 0) {
      obj.successCounter = Math.round(message.successCounter);
    }
    return obj;
  },
};

function createBasetestEpsMeasureBusesCommandArgs(): testEpsMeasureBusesCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testEpsMeasureBusesCommandArgs = {
  encode(message: testEpsMeasureBusesCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsMeasureBusesCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsMeasureBusesCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsMeasureBusesCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testEpsMeasureBusesCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestEpsMeasureBusesCommandOBCResponse(): testEpsMeasureBusesCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsMeasureBusesCommandOBCResponse = {
  encode(message: testEpsMeasureBusesCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsMeasureBusesCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsMeasureBusesCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsMeasureBusesCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsMeasureBusesCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsMeasureBusesCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsMeasureBusesCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsMeasureBusesCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsMeasureBusesCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsMeasureBusesCommandOBCResponseData(): testEpsMeasureBusesCommandOBCResponseData {
  return {
    epsErr: 0,
    battV: 0,
    battA: 0,
    bcrV: 0,
    bcrA: 0,
    bus3v3V: 0,
    bus3v3A: 0,
    bus5vV: 0,
    bus5vA: 0,
    lup3v3V: 0,
    lup5vV: 0,
  };
}

export const testEpsMeasureBusesCommandOBCResponseData = {
  encode(message: testEpsMeasureBusesCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.epsErr !== 0) {
      writer.uint32(8).sint32(message.epsErr);
    }
    if (message.battV !== 0) {
      writer.uint32(17).double(message.battV);
    }
    if (message.battA !== 0) {
      writer.uint32(25).double(message.battA);
    }
    if (message.bcrV !== 0) {
      writer.uint32(33).double(message.bcrV);
    }
    if (message.bcrA !== 0) {
      writer.uint32(41).double(message.bcrA);
    }
    if (message.bus3v3V !== 0) {
      writer.uint32(49).double(message.bus3v3V);
    }
    if (message.bus3v3A !== 0) {
      writer.uint32(57).double(message.bus3v3A);
    }
    if (message.bus5vV !== 0) {
      writer.uint32(65).double(message.bus5vV);
    }
    if (message.bus5vA !== 0) {
      writer.uint32(73).double(message.bus5vA);
    }
    if (message.lup3v3V !== 0) {
      writer.uint32(81).double(message.lup3v3V);
    }
    if (message.lup5vV !== 0) {
      writer.uint32(89).double(message.lup5vV);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsMeasureBusesCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsMeasureBusesCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.epsErr = reader.sint32();
          continue;
        case 2:
          if (tag !== 17) {
            break;
          }

          message.battV = reader.double();
          continue;
        case 3:
          if (tag !== 25) {
            break;
          }

          message.battA = reader.double();
          continue;
        case 4:
          if (tag !== 33) {
            break;
          }

          message.bcrV = reader.double();
          continue;
        case 5:
          if (tag !== 41) {
            break;
          }

          message.bcrA = reader.double();
          continue;
        case 6:
          if (tag !== 49) {
            break;
          }

          message.bus3v3V = reader.double();
          continue;
        case 7:
          if (tag !== 57) {
            break;
          }

          message.bus3v3A = reader.double();
          continue;
        case 8:
          if (tag !== 65) {
            break;
          }

          message.bus5vV = reader.double();
          continue;
        case 9:
          if (tag !== 73) {
            break;
          }

          message.bus5vA = reader.double();
          continue;
        case 10:
          if (tag !== 81) {
            break;
          }

          message.lup3v3V = reader.double();
          continue;
        case 11:
          if (tag !== 89) {
            break;
          }

          message.lup5vV = reader.double();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsMeasureBusesCommandOBCResponseData {
    return {
      epsErr: isSet(object.epsErr) ? globalThis.Number(object.epsErr) : 0,
      battV: isSet(object.battV) ? globalThis.Number(object.battV) : 0,
      battA: isSet(object.battA) ? globalThis.Number(object.battA) : 0,
      bcrV: isSet(object.bcrV) ? globalThis.Number(object.bcrV) : 0,
      bcrA: isSet(object.bcrA) ? globalThis.Number(object.bcrA) : 0,
      bus3v3V: isSet(object.bus3v3V) ? globalThis.Number(object.bus3v3V) : 0,
      bus3v3A: isSet(object.bus3v3A) ? globalThis.Number(object.bus3v3A) : 0,
      bus5vV: isSet(object.bus5vV) ? globalThis.Number(object.bus5vV) : 0,
      bus5vA: isSet(object.bus5vA) ? globalThis.Number(object.bus5vA) : 0,
      lup3v3V: isSet(object.lup3v3V) ? globalThis.Number(object.lup3v3V) : 0,
      lup5vV: isSet(object.lup5vV) ? globalThis.Number(object.lup5vV) : 0,
    };
  },

  toJSON(message: testEpsMeasureBusesCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.epsErr !== 0) {
      obj.epsErr = Math.round(message.epsErr);
    }
    if (message.battV !== 0) {
      obj.battV = message.battV;
    }
    if (message.battA !== 0) {
      obj.battA = message.battA;
    }
    if (message.bcrV !== 0) {
      obj.bcrV = message.bcrV;
    }
    if (message.bcrA !== 0) {
      obj.bcrA = message.bcrA;
    }
    if (message.bus3v3V !== 0) {
      obj.bus3v3V = message.bus3v3V;
    }
    if (message.bus3v3A !== 0) {
      obj.bus3v3A = message.bus3v3A;
    }
    if (message.bus5vV !== 0) {
      obj.bus5vV = message.bus5vV;
    }
    if (message.bus5vA !== 0) {
      obj.bus5vA = message.bus5vA;
    }
    if (message.lup3v3V !== 0) {
      obj.lup3v3V = message.lup3v3V;
    }
    if (message.lup5vV !== 0) {
      obj.lup5vV = message.lup5vV;
    }
    return obj;
  },
};

function createBasetestEpsMeasureTempsCommandArgs(): testEpsMeasureTempsCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testEpsMeasureTempsCommandArgs = {
  encode(message: testEpsMeasureTempsCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsMeasureTempsCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsMeasureTempsCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsMeasureTempsCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testEpsMeasureTempsCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestEpsMeasureTempsCommandOBCResponse(): testEpsMeasureTempsCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsMeasureTempsCommandOBCResponse = {
  encode(message: testEpsMeasureTempsCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsMeasureTempsCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsMeasureTempsCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsMeasureTempsCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsMeasureTempsCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsMeasureTempsCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsMeasureTempsCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsMeasureTempsCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsMeasureTempsCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsMeasureTempsCommandOBCResponseData(): testEpsMeasureTempsCommandOBCResponseData {
  return { epsErr: 0, cellTemps: [], mcuTemp: 0 };
}

export const testEpsMeasureTempsCommandOBCResponseData = {
  encode(message: testEpsMeasureTempsCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.epsErr !== 0) {
      writer.uint32(8).sint32(message.epsErr);
    }
    writer.uint32(18).fork();
    for (const v of message.cellTemps) {
      writer.double(v);
    }
    writer.ldelim();
    if (message.mcuTemp !== 0) {
      writer.uint32(25).double(message.mcuTemp);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsMeasureTempsCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsMeasureTempsCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.epsErr = reader.sint32();
          continue;
        case 2:
          if (tag === 17) {
            message.cellTemps.push(reader.double());

            continue;
          }

          if (tag === 18) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.cellTemps.push(reader.double());
            }

            continue;
          }

          break;
        case 3:
          if (tag !== 25) {
            break;
          }

          message.mcuTemp = reader.double();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsMeasureTempsCommandOBCResponseData {
    return {
      epsErr: isSet(object.epsErr) ? globalThis.Number(object.epsErr) : 0,
      cellTemps: globalThis.Array.isArray(object?.cellTemps)
        ? object.cellTemps.map((e: any) => globalThis.Number(e))
        : [],
      mcuTemp: isSet(object.mcuTemp) ? globalThis.Number(object.mcuTemp) : 0,
    };
  },

  toJSON(message: testEpsMeasureTempsCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.epsErr !== 0) {
      obj.epsErr = Math.round(message.epsErr);
    }
    if (message.cellTemps?.length) {
      obj.cellTemps = message.cellTemps;
    }
    if (message.mcuTemp !== 0) {
      obj.mcuTemp = message.mcuTemp;
    }
    return obj;
  },
};

function createBasetestEpsReadCountersCommandArgs(): testEpsReadCountersCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testEpsReadCountersCommandArgs = {
  encode(message: testEpsReadCountersCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadCountersCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadCountersCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadCountersCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testEpsReadCountersCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestEpsReadCountersCommandOBCResponse(): testEpsReadCountersCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsReadCountersCommandOBCResponse = {
  encode(message: testEpsReadCountersCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsReadCountersCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadCountersCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadCountersCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsReadCountersCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadCountersCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsReadCountersCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsReadCountersCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsReadCountersCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsReadCountersCommandOBCResponseData(): testEpsReadCountersCommandOBCResponseData {
  return { epsErr: 0, uptime: 0, powerOnCycles: 0, underVoltage: 0, shortCircuit: 0, overTemp: 0 };
}

export const testEpsReadCountersCommandOBCResponseData = {
  encode(message: testEpsReadCountersCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.epsErr !== 0) {
      writer.uint32(8).sint32(message.epsErr);
    }
    if (message.uptime !== 0) {
      writer.uint32(16).uint32(message.uptime);
    }
    if (message.powerOnCycles !== 0) {
      writer.uint32(24).uint32(message.powerOnCycles);
    }
    if (message.underVoltage !== 0) {
      writer.uint32(32).uint32(message.underVoltage);
    }
    if (message.shortCircuit !== 0) {
      writer.uint32(40).uint32(message.shortCircuit);
    }
    if (message.overTemp !== 0) {
      writer.uint32(48).uint32(message.overTemp);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadCountersCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadCountersCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.epsErr = reader.sint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.uptime = reader.uint32();
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.powerOnCycles = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.underVoltage = reader.uint32();
          continue;
        case 5:
          if (tag !== 40) {
            break;
          }

          message.shortCircuit = reader.uint32();
          continue;
        case 6:
          if (tag !== 48) {
            break;
          }

          message.overTemp = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadCountersCommandOBCResponseData {
    return {
      epsErr: isSet(object.epsErr) ? globalThis.Number(object.epsErr) : 0,
      uptime: isSet(object.uptime) ? globalThis.Number(object.uptime) : 0,
      powerOnCycles: isSet(object.powerOnCycles) ? globalThis.Number(object.powerOnCycles) : 0,
      underVoltage: isSet(object.underVoltage) ? globalThis.Number(object.underVoltage) : 0,
      shortCircuit: isSet(object.shortCircuit) ? globalThis.Number(object.shortCircuit) : 0,
      overTemp: isSet(object.overTemp) ? globalThis.Number(object.overTemp) : 0,
    };
  },

  toJSON(message: testEpsReadCountersCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.epsErr !== 0) {
      obj.epsErr = Math.round(message.epsErr);
    }
    if (message.uptime !== 0) {
      obj.uptime = Math.round(message.uptime);
    }
    if (message.powerOnCycles !== 0) {
      obj.powerOnCycles = Math.round(message.powerOnCycles);
    }
    if (message.underVoltage !== 0) {
      obj.underVoltage = Math.round(message.underVoltage);
    }
    if (message.shortCircuit !== 0) {
      obj.shortCircuit = Math.round(message.shortCircuit);
    }
    if (message.overTemp !== 0) {
      obj.overTemp = Math.round(message.overTemp);
    }
    return obj;
  },
};

function createBasetestEpsReadStatusCommandArgs(): testEpsReadStatusCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testEpsReadStatusCommandArgs = {
  encode(message: testEpsReadStatusCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadStatusCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadStatusCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadStatusCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testEpsReadStatusCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestEpsReadStatusCommandOBCResponse(): testEpsReadStatusCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testEpsReadStatusCommandOBCResponse = {
  encode(message: testEpsReadStatusCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testEpsReadStatusCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadStatusCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadStatusCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testEpsReadStatusCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadStatusCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testEpsReadStatusCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testEpsReadStatusCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testEpsReadStatusCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestEpsReadStatusCommandOBCResponseData(): testEpsReadStatusCommandOBCResponseData {
  return { epsErr: 0, outCond1: 0, outCond2: 0 };
}

export const testEpsReadStatusCommandOBCResponseData = {
  encode(message: testEpsReadStatusCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.epsErr !== 0) {
      writer.uint32(8).sint32(message.epsErr);
    }
    if (message.outCond1 !== 0) {
      writer.uint32(16).uint32(message.outCond1);
    }
    if (message.outCond2 !== 0) {
      writer.uint32(24).uint32(message.outCond2);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testEpsReadStatusCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestEpsReadStatusCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.epsErr = reader.sint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.outCond1 = reader.uint32();
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.outCond2 = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testEpsReadStatusCommandOBCResponseData {
    return {
      epsErr: isSet(object.epsErr) ? globalThis.Number(object.epsErr) : 0,
      outCond1: isSet(object.outCond1) ? globalThis.Number(object.outCond1) : 0,
      outCond2: isSet(object.outCond2) ? globalThis.Number(object.outCond2) : 0,
    };
  },

  toJSON(message: testEpsReadStatusCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.epsErr !== 0) {
      obj.epsErr = Math.round(message.epsErr);
    }
    if (message.outCond1 !== 0) {
      obj.outCond1 = Math.round(message.outCond1);
    }
    if (message.outCond2 !== 0) {
      obj.outCond2 = Math.round(message.outCond2);
    }
    return obj;
  },
};

function createBasetestMagCommandArgs(): testMagCommandArgs {
  return { timeout: 0, dateTime: undefined, magnetorquer: 0 };
}

export const testMagCommandArgs = {
  encode(message: testMagCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.magnetorquer !== 0) {
      writer.uint32(24).uint32(message.magnetorquer);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testMagCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestMagCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.magnetorquer = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testMagCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      magnetorquer: isSet(object.magnetorquer) ? globalThis.Number(object.magnetorquer) : 0,
    };
  },

  toJSON(message: testMagCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.magnetorquer !== 0) {
      obj.magnetorquer = Math.round(message.magnetorquer);
    }
    return obj;
  },
};

function createBasetestMagCommandOBCResponse(): testMagCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testMagCommandOBCResponse = {
  encode(message: testMagCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testMagCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testMagCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestMagCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testMagCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testMagCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testMagCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testMagCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testMagCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestMagCommandOBCResponseData(): testMagCommandOBCResponseData {
  return {};
}

export const testMagCommandOBCResponseData = {
  encode(_: testMagCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testMagCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestMagCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): testMagCommandOBCResponseData {
    return {};
  },

  toJSON(_: testMagCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasetestMagAllCommandArgs(): testMagAllCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testMagAllCommandArgs = {
  encode(message: testMagAllCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testMagAllCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestMagAllCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testMagAllCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testMagAllCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestMagAllCommandOBCResponse(): testMagAllCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testMagAllCommandOBCResponse = {
  encode(message: testMagAllCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testMagAllCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testMagAllCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestMagAllCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testMagAllCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testMagAllCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testMagAllCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testMagAllCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testMagAllCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestMagAllCommandOBCResponseData(): testMagAllCommandOBCResponseData {
  return {};
}

export const testMagAllCommandOBCResponseData = {
  encode(_: testMagAllCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testMagAllCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestMagAllCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): testMagAllCommandOBCResponseData {
    return {};
  },

  toJSON(_: testMagAllCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasetestMagInitCommandArgs(): testMagInitCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testMagInitCommandArgs = {
  encode(message: testMagInitCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testMagInitCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestMagInitCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testMagInitCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testMagInitCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestMagInitCommandOBCResponse(): testMagInitCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testMagInitCommandOBCResponse = {
  encode(message: testMagInitCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testMagInitCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testMagInitCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestMagInitCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testMagInitCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testMagInitCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testMagInitCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testMagInitCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testMagInitCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestMagInitCommandOBCResponseData(): testMagInitCommandOBCResponseData {
  return {};
}

export const testMagInitCommandOBCResponseData = {
  encode(_: testMagInitCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testMagInitCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestMagInitCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): testMagInitCommandOBCResponseData {
    return {};
  },

  toJSON(_: testMagInitCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasetestImuCommandArgs(): testImuCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testImuCommandArgs = {
  encode(message: testImuCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testImuCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestImuCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testImuCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testImuCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestImuCommandOBCResponse(): testImuCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testImuCommandOBCResponse = {
  encode(message: testImuCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testImuCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testImuCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestImuCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testImuCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testImuCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testImuCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testImuCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testImuCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestImuCommandOBCResponseData(): testImuCommandOBCResponseData {
  return {};
}

export const testImuCommandOBCResponseData = {
  encode(_: testImuCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testImuCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestImuCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): testImuCommandOBCResponseData {
    return {};
  },

  toJSON(_: testImuCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasetestPanelGyroCommandArgs(): testPanelGyroCommandArgs {
  return { timeout: 0, dateTime: undefined, gyro: 0 };
}

export const testPanelGyroCommandArgs = {
  encode(message: testPanelGyroCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.gyro !== 0) {
      writer.uint32(24).uint32(message.gyro);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testPanelGyroCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestPanelGyroCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.gyro = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testPanelGyroCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      gyro: isSet(object.gyro) ? globalThis.Number(object.gyro) : 0,
    };
  },

  toJSON(message: testPanelGyroCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.gyro !== 0) {
      obj.gyro = Math.round(message.gyro);
    }
    return obj;
  },
};

function createBasetestPanelGyroCommandOBCResponse(): testPanelGyroCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testPanelGyroCommandOBCResponse = {
  encode(message: testPanelGyroCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testPanelGyroCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testPanelGyroCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestPanelGyroCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testPanelGyroCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testPanelGyroCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testPanelGyroCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testPanelGyroCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testPanelGyroCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestPanelGyroCommandOBCResponseData(): testPanelGyroCommandOBCResponseData {
  return {};
}

export const testPanelGyroCommandOBCResponseData = {
  encode(_: testPanelGyroCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testPanelGyroCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestPanelGyroCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): testPanelGyroCommandOBCResponseData {
    return {};
  },

  toJSON(_: testPanelGyroCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasetestCanGpioCommandArgs(): testCanGpioCommandArgs {
  return { timeout: 0, dateTime: undefined, port: 0, pin: 0, value: false };
}

export const testCanGpioCommandArgs = {
  encode(message: testCanGpioCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.port !== 0) {
      writer.uint32(24).uint32(message.port);
    }
    if (message.pin !== 0) {
      writer.uint32(32).uint32(message.pin);
    }
    if (message.value === true) {
      writer.uint32(40).bool(message.value);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCanGpioCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCanGpioCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.port = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.pin = reader.uint32();
          continue;
        case 5:
          if (tag !== 40) {
            break;
          }

          message.value = reader.bool();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCanGpioCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      port: isSet(object.port) ? globalThis.Number(object.port) : 0,
      pin: isSet(object.pin) ? globalThis.Number(object.pin) : 0,
      value: isSet(object.value) ? globalThis.Boolean(object.value) : false,
    };
  },

  toJSON(message: testCanGpioCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.port !== 0) {
      obj.port = Math.round(message.port);
    }
    if (message.pin !== 0) {
      obj.pin = Math.round(message.pin);
    }
    if (message.value === true) {
      obj.value = message.value;
    }
    return obj;
  },
};

function createBasetestCanGpioCommandOBCResponse(): testCanGpioCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCanGpioCommandOBCResponse = {
  encode(message: testCanGpioCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCanGpioCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCanGpioCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCanGpioCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCanGpioCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCanGpioCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCanGpioCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCanGpioCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCanGpioCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCanGpioCommandOBCResponseData(): testCanGpioCommandOBCResponseData {
  return { gpioErr: 0 };
}

export const testCanGpioCommandOBCResponseData = {
  encode(message: testCanGpioCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.gpioErr !== 0) {
      writer.uint32(8).sint32(message.gpioErr);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCanGpioCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCanGpioCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.gpioErr = reader.sint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCanGpioCommandOBCResponseData {
    return { gpioErr: isSet(object.gpioErr) ? globalThis.Number(object.gpioErr) : 0 };
  },

  toJSON(message: testCanGpioCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.gpioErr !== 0) {
      obj.gpioErr = Math.round(message.gpioErr);
    }
    return obj;
  },
};

function createBasetestFlashRwCommandArgs(): testFlashRwCommandArgs {
  return { timeout: 0, dateTime: undefined, addr: 0, len: 0 };
}

export const testFlashRwCommandArgs = {
  encode(message: testFlashRwCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.addr !== 0) {
      writer.uint32(24).uint32(message.addr);
    }
    if (message.len !== 0) {
      writer.uint32(32).uint32(message.len);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testFlashRwCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestFlashRwCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.addr = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.len = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testFlashRwCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      addr: isSet(object.addr) ? globalThis.Number(object.addr) : 0,
      len: isSet(object.len) ? globalThis.Number(object.len) : 0,
    };
  },

  toJSON(message: testFlashRwCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.addr !== 0) {
      obj.addr = Math.round(message.addr);
    }
    if (message.len !== 0) {
      obj.len = Math.round(message.len);
    }
    return obj;
  },
};

function createBasetestFlashRwCommandOBCResponse(): testFlashRwCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testFlashRwCommandOBCResponse = {
  encode(message: testFlashRwCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testFlashRwCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testFlashRwCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestFlashRwCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testFlashRwCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testFlashRwCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testFlashRwCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testFlashRwCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testFlashRwCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestFlashRwCommandOBCResponseData(): testFlashRwCommandOBCResponseData {
  return { eraseErr: 0, writeErr: 0, readErr: 0, dataMatch: false };
}

export const testFlashRwCommandOBCResponseData = {
  encode(message: testFlashRwCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.eraseErr !== 0) {
      writer.uint32(8).sint32(message.eraseErr);
    }
    if (message.writeErr !== 0) {
      writer.uint32(16).sint32(message.writeErr);
    }
    if (message.readErr !== 0) {
      writer.uint32(24).sint32(message.readErr);
    }
    if (message.dataMatch === true) {
      writer.uint32(32).bool(message.dataMatch);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testFlashRwCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestFlashRwCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.eraseErr = reader.sint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.writeErr = reader.sint32();
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.readErr = reader.sint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.dataMatch = reader.bool();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testFlashRwCommandOBCResponseData {
    return {
      eraseErr: isSet(object.eraseErr) ? globalThis.Number(object.eraseErr) : 0,
      writeErr: isSet(object.writeErr) ? globalThis.Number(object.writeErr) : 0,
      readErr: isSet(object.readErr) ? globalThis.Number(object.readErr) : 0,
      dataMatch: isSet(object.dataMatch) ? globalThis.Boolean(object.dataMatch) : false,
    };
  },

  toJSON(message: testFlashRwCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.eraseErr !== 0) {
      obj.eraseErr = Math.round(message.eraseErr);
    }
    if (message.writeErr !== 0) {
      obj.writeErr = Math.round(message.writeErr);
    }
    if (message.readErr !== 0) {
      obj.readErr = Math.round(message.readErr);
    }
    if (message.dataMatch === true) {
      obj.dataMatch = message.dataMatch;
    }
    return obj;
  },
};

function createBasetestFilesystemCommandArgs(): testFilesystemCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testFilesystemCommandArgs = {
  encode(message: testFilesystemCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testFilesystemCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestFilesystemCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testFilesystemCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testFilesystemCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestFilesystemCommandOBCResponse(): testFilesystemCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testFilesystemCommandOBCResponse = {
  encode(message: testFilesystemCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testFilesystemCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testFilesystemCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestFilesystemCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testFilesystemCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testFilesystemCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testFilesystemCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testFilesystemCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testFilesystemCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestFilesystemCommandOBCResponseData(): testFilesystemCommandOBCResponseData {
  return { fsErr: 0 };
}

export const testFilesystemCommandOBCResponseData = {
  encode(message: testFilesystemCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.fsErr !== 0) {
      writer.uint32(8).sint32(message.fsErr);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testFilesystemCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestFilesystemCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.fsErr = reader.sint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testFilesystemCommandOBCResponseData {
    return { fsErr: isSet(object.fsErr) ? globalThis.Number(object.fsErr) : 0 };
  },

  toJSON(message: testFilesystemCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.fsErr !== 0) {
      obj.fsErr = Math.round(message.fsErr);
    }
    return obj;
  },
};

function createBasetestCamInitCommandArgs(): testCamInitCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testCamInitCommandArgs = {
  encode(message: testCamInitCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamInitCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamInitCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamInitCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testCamInitCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestCamInitCommandOBCResponse(): testCamInitCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCamInitCommandOBCResponse = {
  encode(message: testCamInitCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCamInitCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamInitCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamInitCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCamInitCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamInitCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCamInitCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCamInitCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCamInitCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCamInitCommandOBCResponseData(): testCamInitCommandOBCResponseData {
  return { arducamErr: 0 };
}

export const testCamInitCommandOBCResponseData = {
  encode(message: testCamInitCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.arducamErr !== 0) {
      writer.uint32(8).sint32(message.arducamErr);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamInitCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamInitCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.arducamErr = reader.sint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamInitCommandOBCResponseData {
    return { arducamErr: isSet(object.arducamErr) ? globalThis.Number(object.arducamErr) : 0 };
  },

  toJSON(message: testCamInitCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.arducamErr !== 0) {
      obj.arducamErr = Math.round(message.arducamErr);
    }
    return obj;
  },
};

function createBasetestCamCaptureCommandArgs(): testCamCaptureCommandArgs {
  return { timeout: 0, dateTime: undefined };
}

export const testCamCaptureCommandArgs = {
  encode(message: testCamCaptureCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamCaptureCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamCaptureCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamCaptureCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
    };
  },

  toJSON(message: testCamCaptureCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    return obj;
  },
};

function createBasetestCamCaptureCommandOBCResponse(): testCamCaptureCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCamCaptureCommandOBCResponse = {
  encode(message: testCamCaptureCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCamCaptureCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamCaptureCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamCaptureCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCamCaptureCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamCaptureCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCamCaptureCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCamCaptureCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCamCaptureCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCamCaptureCommandOBCResponseData(): testCamCaptureCommandOBCResponseData {
  return { arducamErr: 0, imageSize: 0, imageData: new Uint8Array(0) };
}

export const testCamCaptureCommandOBCResponseData = {
  encode(message: testCamCaptureCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.arducamErr !== 0) {
      writer.uint32(8).sint32(message.arducamErr);
    }
    if (message.imageSize !== 0) {
      writer.uint32(16).uint32(message.imageSize);
    }
    if (message.imageData.length !== 0) {
      writer.uint32(26).bytes(message.imageData);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamCaptureCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamCaptureCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.arducamErr = reader.sint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.imageSize = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.imageData = reader.bytes();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamCaptureCommandOBCResponseData {
    return {
      arducamErr: isSet(object.arducamErr) ? globalThis.Number(object.arducamErr) : 0,
      imageSize: isSet(object.imageSize) ? globalThis.Number(object.imageSize) : 0,
      imageData: isSet(object.imageData) ? bytesFromBase64(object.imageData) : new Uint8Array(0),
    };
  },

  toJSON(message: testCamCaptureCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.arducamErr !== 0) {
      obj.arducamErr = Math.round(message.arducamErr);
    }
    if (message.imageSize !== 0) {
      obj.imageSize = Math.round(message.imageSize);
    }
    if (message.imageData.length !== 0) {
      obj.imageData = base64FromBytes(message.imageData);
    }
    return obj;
  },
};

function createBasetestCamWrSregCommandArgs(): testCamWrSregCommandArgs {
  return { timeout: 0, dateTime: undefined, addr: 0, data: 0 };
}

export const testCamWrSregCommandArgs = {
  encode(message: testCamWrSregCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.addr !== 0) {
      writer.uint32(24).uint32(message.addr);
    }
    if (message.data !== 0) {
      writer.uint32(32).uint32(message.data);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamWrSregCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamWrSregCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.addr = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.data = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamWrSregCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      addr: isSet(object.addr) ? globalThis.Number(object.addr) : 0,
      data: isSet(object.data) ? globalThis.Number(object.data) : 0,
    };
  },

  toJSON(message: testCamWrSregCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.addr !== 0) {
      obj.addr = Math.round(message.addr);
    }
    if (message.data !== 0) {
      obj.data = Math.round(message.data);
    }
    return obj;
  },
};

function createBasetestCamWrSregCommandOBCResponse(): testCamWrSregCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCamWrSregCommandOBCResponse = {
  encode(message: testCamWrSregCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCamWrSregCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamWrSregCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamWrSregCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCamWrSregCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamWrSregCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCamWrSregCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCamWrSregCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCamWrSregCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCamWrSregCommandOBCResponseData(): testCamWrSregCommandOBCResponseData {
  return { ov5642Err: 0 };
}

export const testCamWrSregCommandOBCResponseData = {
  encode(message: testCamWrSregCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.ov5642Err !== 0) {
      writer.uint32(8).sint32(message.ov5642Err);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamWrSregCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamWrSregCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.ov5642Err = reader.sint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamWrSregCommandOBCResponseData {
    return { ov5642Err: isSet(object.ov5642Err) ? globalThis.Number(object.ov5642Err) : 0 };
  },

  toJSON(message: testCamWrSregCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.ov5642Err !== 0) {
      obj.ov5642Err = Math.round(message.ov5642Err);
    }
    return obj;
  },
};

function createBasetestCamRdSregCommandArgs(): testCamRdSregCommandArgs {
  return { timeout: 0, dateTime: undefined, addr: 0 };
}

export const testCamRdSregCommandArgs = {
  encode(message: testCamRdSregCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.addr !== 0) {
      writer.uint32(24).uint32(message.addr);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamRdSregCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamRdSregCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.addr = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamRdSregCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      addr: isSet(object.addr) ? globalThis.Number(object.addr) : 0,
    };
  },

  toJSON(message: testCamRdSregCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.addr !== 0) {
      obj.addr = Math.round(message.addr);
    }
    return obj;
  },
};

function createBasetestCamRdSregCommandOBCResponse(): testCamRdSregCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCamRdSregCommandOBCResponse = {
  encode(message: testCamRdSregCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCamRdSregCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamRdSregCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamRdSregCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCamRdSregCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamRdSregCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCamRdSregCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCamRdSregCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCamRdSregCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCamRdSregCommandOBCResponseData(): testCamRdSregCommandOBCResponseData {
  return { ov5642Err: 0, data: 0 };
}

export const testCamRdSregCommandOBCResponseData = {
  encode(message: testCamRdSregCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.ov5642Err !== 0) {
      writer.uint32(8).sint32(message.ov5642Err);
    }
    if (message.data !== 0) {
      writer.uint32(16).uint32(message.data);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamRdSregCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamRdSregCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.ov5642Err = reader.sint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.data = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamRdSregCommandOBCResponseData {
    return {
      ov5642Err: isSet(object.ov5642Err) ? globalThis.Number(object.ov5642Err) : 0,
      data: isSet(object.data) ? globalThis.Number(object.data) : 0,
    };
  },

  toJSON(message: testCamRdSregCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.ov5642Err !== 0) {
      obj.ov5642Err = Math.round(message.ov5642Err);
    }
    if (message.data !== 0) {
      obj.data = Math.round(message.data);
    }
    return obj;
  },
};

function createBasetestCamWrSregsCommandArgs(): testCamWrSregsCommandArgs {
  return { timeout: 0, dateTime: undefined, regData: new Uint8Array(0) };
}

export const testCamWrSregsCommandArgs = {
  encode(message: testCamWrSregsCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.regData.length !== 0) {
      writer.uint32(26).bytes(message.regData);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamWrSregsCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamWrSregsCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.regData = reader.bytes();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamWrSregsCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      regData: isSet(object.regData) ? bytesFromBase64(object.regData) : new Uint8Array(0),
    };
  },

  toJSON(message: testCamWrSregsCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.regData.length !== 0) {
      obj.regData = base64FromBytes(message.regData);
    }
    return obj;
  },
};

function createBasetestCamWrSregsCommandOBCResponse(): testCamWrSregsCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testCamWrSregsCommandOBCResponse = {
  encode(message: testCamWrSregsCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testCamWrSregsCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamWrSregsCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamWrSregsCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testCamWrSregsCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamWrSregsCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testCamWrSregsCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testCamWrSregsCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testCamWrSregsCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestCamWrSregsCommandOBCResponseData(): testCamWrSregsCommandOBCResponseData {
  return { ov5642Err: 0 };
}

export const testCamWrSregsCommandOBCResponseData = {
  encode(message: testCamWrSregsCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.ov5642Err !== 0) {
      writer.uint32(8).sint32(message.ov5642Err);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testCamWrSregsCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestCamWrSregsCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.ov5642Err = reader.sint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testCamWrSregsCommandOBCResponseData {
    return { ov5642Err: isSet(object.ov5642Err) ? globalThis.Number(object.ov5642Err) : 0 };
  },

  toJSON(message: testCamWrSregsCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.ov5642Err !== 0) {
      obj.ov5642Err = Math.round(message.ov5642Err);
    }
    return obj;
  },
};

function createBasetestDsplibCommandArgs(): testDsplibCommandArgs {
  return { timeout: 0, dateTime: undefined, x: 0, y: 0 };
}

export const testDsplibCommandArgs = {
  encode(message: testDsplibCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.x !== 0) {
      writer.uint32(25).double(message.x);
    }
    if (message.y !== 0) {
      writer.uint32(33).double(message.y);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testDsplibCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestDsplibCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 25) {
            break;
          }

          message.x = reader.double();
          continue;
        case 4:
          if (tag !== 33) {
            break;
          }

          message.y = reader.double();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testDsplibCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      x: isSet(object.x) ? globalThis.Number(object.x) : 0,
      y: isSet(object.y) ? globalThis.Number(object.y) : 0,
    };
  },

  toJSON(message: testDsplibCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.x !== 0) {
      obj.x = message.x;
    }
    if (message.y !== 0) {
      obj.y = message.y;
    }
    return obj;
  },
};

function createBasetestDsplibCommandOBCResponse(): testDsplibCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testDsplibCommandOBCResponse = {
  encode(message: testDsplibCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testDsplibCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testDsplibCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestDsplibCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testDsplibCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testDsplibCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testDsplibCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testDsplibCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testDsplibCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestDsplibCommandOBCResponseData(): testDsplibCommandOBCResponseData {
  return { xAbs: 0, yAbs: 0 };
}

export const testDsplibCommandOBCResponseData = {
  encode(message: testDsplibCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.xAbs !== 0) {
      writer.uint32(9).double(message.xAbs);
    }
    if (message.yAbs !== 0) {
      writer.uint32(17).double(message.yAbs);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testDsplibCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestDsplibCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 9) {
            break;
          }

          message.xAbs = reader.double();
          continue;
        case 2:
          if (tag !== 17) {
            break;
          }

          message.yAbs = reader.double();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testDsplibCommandOBCResponseData {
    return {
      xAbs: isSet(object.xAbs) ? globalThis.Number(object.xAbs) : 0,
      yAbs: isSet(object.yAbs) ? globalThis.Number(object.yAbs) : 0,
    };
  },

  toJSON(message: testDsplibCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.xAbs !== 0) {
      obj.xAbs = message.xAbs;
    }
    if (message.yAbs !== 0) {
      obj.yAbs = message.yAbs;
    }
    return obj;
  },
};

function createBasetestQuestCommandArgs(): testQuestCommandArgs {
  return { timeout: 0, dateTime: undefined, sunObs: [], sunRef: [], magObs: [], magRef: [], weights: [] };
}

export const testQuestCommandArgs = {
  encode(message: testQuestCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    writer.uint32(26).fork();
    for (const v of message.sunObs) {
      writer.double(v);
    }
    writer.ldelim();
    writer.uint32(34).fork();
    for (const v of message.sunRef) {
      writer.double(v);
    }
    writer.ldelim();
    writer.uint32(42).fork();
    for (const v of message.magObs) {
      writer.double(v);
    }
    writer.ldelim();
    writer.uint32(50).fork();
    for (const v of message.magRef) {
      writer.double(v);
    }
    writer.ldelim();
    writer.uint32(58).fork();
    for (const v of message.weights) {
      writer.double(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testQuestCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestQuestCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag === 25) {
            message.sunObs.push(reader.double());

            continue;
          }

          if (tag === 26) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.sunObs.push(reader.double());
            }

            continue;
          }

          break;
        case 4:
          if (tag === 33) {
            message.sunRef.push(reader.double());

            continue;
          }

          if (tag === 34) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.sunRef.push(reader.double());
            }

            continue;
          }

          break;
        case 5:
          if (tag === 41) {
            message.magObs.push(reader.double());

            continue;
          }

          if (tag === 42) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.magObs.push(reader.double());
            }

            continue;
          }

          break;
        case 6:
          if (tag === 49) {
            message.magRef.push(reader.double());

            continue;
          }

          if (tag === 50) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.magRef.push(reader.double());
            }

            continue;
          }

          break;
        case 7:
          if (tag === 57) {
            message.weights.push(reader.double());

            continue;
          }

          if (tag === 58) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.weights.push(reader.double());
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testQuestCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      sunObs: globalThis.Array.isArray(object?.sunObs) ? object.sunObs.map((e: any) => globalThis.Number(e)) : [],
      sunRef: globalThis.Array.isArray(object?.sunRef) ? object.sunRef.map((e: any) => globalThis.Number(e)) : [],
      magObs: globalThis.Array.isArray(object?.magObs) ? object.magObs.map((e: any) => globalThis.Number(e)) : [],
      magRef: globalThis.Array.isArray(object?.magRef) ? object.magRef.map((e: any) => globalThis.Number(e)) : [],
      weights: globalThis.Array.isArray(object?.weights) ? object.weights.map((e: any) => globalThis.Number(e)) : [],
    };
  },

  toJSON(message: testQuestCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.sunObs?.length) {
      obj.sunObs = message.sunObs;
    }
    if (message.sunRef?.length) {
      obj.sunRef = message.sunRef;
    }
    if (message.magObs?.length) {
      obj.magObs = message.magObs;
    }
    if (message.magRef?.length) {
      obj.magRef = message.magRef;
    }
    if (message.weights?.length) {
      obj.weights = message.weights;
    }
    return obj;
  },
};

function createBasetestQuestCommandOBCResponse(): testQuestCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testQuestCommandOBCResponse = {
  encode(message: testQuestCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testQuestCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testQuestCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestQuestCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testQuestCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testQuestCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testQuestCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testQuestCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testQuestCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestQuestCommandOBCResponseData(): testQuestCommandOBCResponseData {
  return { quat: [], adcsErr: 0, avgDuration: 0 };
}

export const testQuestCommandOBCResponseData = {
  encode(message: testQuestCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    writer.uint32(10).fork();
    for (const v of message.quat) {
      writer.double(v);
    }
    writer.ldelim();
    if (message.adcsErr !== 0) {
      writer.uint32(16).sint32(message.adcsErr);
    }
    if (message.avgDuration !== 0) {
      writer.uint32(25).double(message.avgDuration);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testQuestCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestQuestCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag === 9) {
            message.quat.push(reader.double());

            continue;
          }

          if (tag === 10) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.quat.push(reader.double());
            }

            continue;
          }

          break;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.adcsErr = reader.sint32();
          continue;
        case 3:
          if (tag !== 25) {
            break;
          }

          message.avgDuration = reader.double();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testQuestCommandOBCResponseData {
    return {
      quat: globalThis.Array.isArray(object?.quat) ? object.quat.map((e: any) => globalThis.Number(e)) : [],
      adcsErr: isSet(object.adcsErr) ? globalThis.Number(object.adcsErr) : 0,
      avgDuration: isSet(object.avgDuration) ? globalThis.Number(object.avgDuration) : 0,
    };
  },

  toJSON(message: testQuestCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.quat?.length) {
      obj.quat = message.quat;
    }
    if (message.adcsErr !== 0) {
      obj.adcsErr = Math.round(message.adcsErr);
    }
    if (message.avgDuration !== 0) {
      obj.avgDuration = message.avgDuration;
    }
    return obj;
  },
};

function createBasetestRtcGetTimeCommandArgs(): testRtcGetTimeCommandArgs {
  return { timeout: 0, dateTime: undefined, durationS: 0 };
}

export const testRtcGetTimeCommandArgs = {
  encode(message: testRtcGetTimeCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.durationS !== 0) {
      writer.uint32(24).uint32(message.durationS);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testRtcGetTimeCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestRtcGetTimeCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.durationS = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testRtcGetTimeCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      durationS: isSet(object.durationS) ? globalThis.Number(object.durationS) : 0,
    };
  },

  toJSON(message: testRtcGetTimeCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.durationS !== 0) {
      obj.durationS = Math.round(message.durationS);
    }
    return obj;
  },
};

function createBasetestRtcGetTimeCommandOBCResponse(): testRtcGetTimeCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testRtcGetTimeCommandOBCResponse = {
  encode(message: testRtcGetTimeCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testRtcGetTimeCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testRtcGetTimeCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestRtcGetTimeCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testRtcGetTimeCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testRtcGetTimeCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testRtcGetTimeCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testRtcGetTimeCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testRtcGetTimeCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestRtcGetTimeCommandOBCResponseData(): testRtcGetTimeCommandOBCResponseData {
  return { second: 0, minute: 0, hour: 0, day: 0, month: 0, year: 0 };
}

export const testRtcGetTimeCommandOBCResponseData = {
  encode(message: testRtcGetTimeCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.second !== 0) {
      writer.uint32(8).uint32(message.second);
    }
    if (message.minute !== 0) {
      writer.uint32(16).uint32(message.minute);
    }
    if (message.hour !== 0) {
      writer.uint32(24).uint32(message.hour);
    }
    if (message.day !== 0) {
      writer.uint32(32).uint32(message.day);
    }
    if (message.month !== 0) {
      writer.uint32(40).uint32(message.month);
    }
    if (message.year !== 0) {
      writer.uint32(48).uint32(message.year);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testRtcGetTimeCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestRtcGetTimeCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.second = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.minute = reader.uint32();
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.hour = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.day = reader.uint32();
          continue;
        case 5:
          if (tag !== 40) {
            break;
          }

          message.month = reader.uint32();
          continue;
        case 6:
          if (tag !== 48) {
            break;
          }

          message.year = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testRtcGetTimeCommandOBCResponseData {
    return {
      second: isSet(object.second) ? globalThis.Number(object.second) : 0,
      minute: isSet(object.minute) ? globalThis.Number(object.minute) : 0,
      hour: isSet(object.hour) ? globalThis.Number(object.hour) : 0,
      day: isSet(object.day) ? globalThis.Number(object.day) : 0,
      month: isSet(object.month) ? globalThis.Number(object.month) : 0,
      year: isSet(object.year) ? globalThis.Number(object.year) : 0,
    };
  },

  toJSON(message: testRtcGetTimeCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.second !== 0) {
      obj.second = Math.round(message.second);
    }
    if (message.minute !== 0) {
      obj.minute = Math.round(message.minute);
    }
    if (message.hour !== 0) {
      obj.hour = Math.round(message.hour);
    }
    if (message.day !== 0) {
      obj.day = Math.round(message.day);
    }
    if (message.month !== 0) {
      obj.month = Math.round(message.month);
    }
    if (message.year !== 0) {
      obj.year = Math.round(message.year);
    }
    return obj;
  },
};

function createBasetestRtcCapAndGetTimestampCommandArgs(): testRtcCapAndGetTimestampCommandArgs {
  return { timeout: 0, dateTime: undefined, durationS: 0 };
}

export const testRtcCapAndGetTimestampCommandArgs = {
  encode(message: testRtcCapAndGetTimestampCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.durationS !== 0) {
      writer.uint32(24).uint32(message.durationS);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testRtcCapAndGetTimestampCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestRtcCapAndGetTimestampCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.durationS = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testRtcCapAndGetTimestampCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      durationS: isSet(object.durationS) ? globalThis.Number(object.durationS) : 0,
    };
  },

  toJSON(message: testRtcCapAndGetTimestampCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.durationS !== 0) {
      obj.durationS = Math.round(message.durationS);
    }
    return obj;
  },
};

function createBasetestRtcCapAndGetTimestampCommandOBCResponse(): testRtcCapAndGetTimestampCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testRtcCapAndGetTimestampCommandOBCResponse = {
  encode(message: testRtcCapAndGetTimestampCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testRtcCapAndGetTimestampCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testRtcCapAndGetTimestampCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestRtcCapAndGetTimestampCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testRtcCapAndGetTimestampCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testRtcCapAndGetTimestampCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testRtcCapAndGetTimestampCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testRtcCapAndGetTimestampCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testRtcCapAndGetTimestampCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestRtcCapAndGetTimestampCommandOBCResponseData(): testRtcCapAndGetTimestampCommandOBCResponseData {
  return {
    initSecond: 0,
    initMinute: 0,
    initHour: 0,
    initDay: 0,
    initMonth: 0,
    initYear: 0,
    tsSecond: 0,
    tsMinute: 0,
    tsHour: 0,
    tsDay: 0,
    tsMonth: 0,
    tsYear: 0,
  };
}

export const testRtcCapAndGetTimestampCommandOBCResponseData = {
  encode(
    message: testRtcCapAndGetTimestampCommandOBCResponseData,
    writer: _m0.Writer = _m0.Writer.create(),
  ): _m0.Writer {
    if (message.initSecond !== 0) {
      writer.uint32(8).uint32(message.initSecond);
    }
    if (message.initMinute !== 0) {
      writer.uint32(16).uint32(message.initMinute);
    }
    if (message.initHour !== 0) {
      writer.uint32(24).uint32(message.initHour);
    }
    if (message.initDay !== 0) {
      writer.uint32(32).uint32(message.initDay);
    }
    if (message.initMonth !== 0) {
      writer.uint32(40).uint32(message.initMonth);
    }
    if (message.initYear !== 0) {
      writer.uint32(48).uint32(message.initYear);
    }
    if (message.tsSecond !== 0) {
      writer.uint32(56).uint32(message.tsSecond);
    }
    if (message.tsMinute !== 0) {
      writer.uint32(64).uint32(message.tsMinute);
    }
    if (message.tsHour !== 0) {
      writer.uint32(72).uint32(message.tsHour);
    }
    if (message.tsDay !== 0) {
      writer.uint32(80).uint32(message.tsDay);
    }
    if (message.tsMonth !== 0) {
      writer.uint32(88).uint32(message.tsMonth);
    }
    if (message.tsYear !== 0) {
      writer.uint32(96).uint32(message.tsYear);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testRtcCapAndGetTimestampCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestRtcCapAndGetTimestampCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.initSecond = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.initMinute = reader.uint32();
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.initHour = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.initDay = reader.uint32();
          continue;
        case 5:
          if (tag !== 40) {
            break;
          }

          message.initMonth = reader.uint32();
          continue;
        case 6:
          if (tag !== 48) {
            break;
          }

          message.initYear = reader.uint32();
          continue;
        case 7:
          if (tag !== 56) {
            break;
          }

          message.tsSecond = reader.uint32();
          continue;
        case 8:
          if (tag !== 64) {
            break;
          }

          message.tsMinute = reader.uint32();
          continue;
        case 9:
          if (tag !== 72) {
            break;
          }

          message.tsHour = reader.uint32();
          continue;
        case 10:
          if (tag !== 80) {
            break;
          }

          message.tsDay = reader.uint32();
          continue;
        case 11:
          if (tag !== 88) {
            break;
          }

          message.tsMonth = reader.uint32();
          continue;
        case 12:
          if (tag !== 96) {
            break;
          }

          message.tsYear = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testRtcCapAndGetTimestampCommandOBCResponseData {
    return {
      initSecond: isSet(object.initSecond) ? globalThis.Number(object.initSecond) : 0,
      initMinute: isSet(object.initMinute) ? globalThis.Number(object.initMinute) : 0,
      initHour: isSet(object.initHour) ? globalThis.Number(object.initHour) : 0,
      initDay: isSet(object.initDay) ? globalThis.Number(object.initDay) : 0,
      initMonth: isSet(object.initMonth) ? globalThis.Number(object.initMonth) : 0,
      initYear: isSet(object.initYear) ? globalThis.Number(object.initYear) : 0,
      tsSecond: isSet(object.tsSecond) ? globalThis.Number(object.tsSecond) : 0,
      tsMinute: isSet(object.tsMinute) ? globalThis.Number(object.tsMinute) : 0,
      tsHour: isSet(object.tsHour) ? globalThis.Number(object.tsHour) : 0,
      tsDay: isSet(object.tsDay) ? globalThis.Number(object.tsDay) : 0,
      tsMonth: isSet(object.tsMonth) ? globalThis.Number(object.tsMonth) : 0,
      tsYear: isSet(object.tsYear) ? globalThis.Number(object.tsYear) : 0,
    };
  },

  toJSON(message: testRtcCapAndGetTimestampCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.initSecond !== 0) {
      obj.initSecond = Math.round(message.initSecond);
    }
    if (message.initMinute !== 0) {
      obj.initMinute = Math.round(message.initMinute);
    }
    if (message.initHour !== 0) {
      obj.initHour = Math.round(message.initHour);
    }
    if (message.initDay !== 0) {
      obj.initDay = Math.round(message.initDay);
    }
    if (message.initMonth !== 0) {
      obj.initMonth = Math.round(message.initMonth);
    }
    if (message.initYear !== 0) {
      obj.initYear = Math.round(message.initYear);
    }
    if (message.tsSecond !== 0) {
      obj.tsSecond = Math.round(message.tsSecond);
    }
    if (message.tsMinute !== 0) {
      obj.tsMinute = Math.round(message.tsMinute);
    }
    if (message.tsHour !== 0) {
      obj.tsHour = Math.round(message.tsHour);
    }
    if (message.tsDay !== 0) {
      obj.tsDay = Math.round(message.tsDay);
    }
    if (message.tsMonth !== 0) {
      obj.tsMonth = Math.round(message.tsMonth);
    }
    if (message.tsYear !== 0) {
      obj.tsYear = Math.round(message.tsYear);
    }
    return obj;
  },
};

function createBasedebugReadRtcRegCommandArgs(): debugReadRtcRegCommandArgs {
  return { timeout: 0, dateTime: undefined, reg: 0 };
}

export const debugReadRtcRegCommandArgs = {
  encode(message: debugReadRtcRegCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.reg !== 0) {
      writer.uint32(24).uint32(message.reg);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): debugReadRtcRegCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasedebugReadRtcRegCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.reg = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): debugReadRtcRegCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      reg: isSet(object.reg) ? globalThis.Number(object.reg) : 0,
    };
  },

  toJSON(message: debugReadRtcRegCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.reg !== 0) {
      obj.reg = Math.round(message.reg);
    }
    return obj;
  },
};

function createBasedebugReadRtcRegCommandOBCResponse(): debugReadRtcRegCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const debugReadRtcRegCommandOBCResponse = {
  encode(message: debugReadRtcRegCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      debugReadRtcRegCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): debugReadRtcRegCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasedebugReadRtcRegCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = debugReadRtcRegCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): debugReadRtcRegCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? debugReadRtcRegCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: debugReadRtcRegCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = debugReadRtcRegCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasedebugReadRtcRegCommandOBCResponseData(): debugReadRtcRegCommandOBCResponseData {
  return { val: 0 };
}

export const debugReadRtcRegCommandOBCResponseData = {
  encode(message: debugReadRtcRegCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.val !== 0) {
      writer.uint32(8).uint32(message.val);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): debugReadRtcRegCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasedebugReadRtcRegCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.val = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): debugReadRtcRegCommandOBCResponseData {
    return { val: isSet(object.val) ? globalThis.Number(object.val) : 0 };
  },

  toJSON(message: debugReadRtcRegCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.val !== 0) {
      obj.val = Math.round(message.val);
    }
    return obj;
  },
};

function createBasedebugWriteRtcRegCommandArgs(): debugWriteRtcRegCommandArgs {
  return { timeout: 0, dateTime: undefined, reg: 0, val: 0 };
}

export const debugWriteRtcRegCommandArgs = {
  encode(message: debugWriteRtcRegCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.reg !== 0) {
      writer.uint32(24).uint32(message.reg);
    }
    if (message.val !== 0) {
      writer.uint32(32).uint32(message.val);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): debugWriteRtcRegCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasedebugWriteRtcRegCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.reg = reader.uint32();
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.val = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): debugWriteRtcRegCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      reg: isSet(object.reg) ? globalThis.Number(object.reg) : 0,
      val: isSet(object.val) ? globalThis.Number(object.val) : 0,
    };
  },

  toJSON(message: debugWriteRtcRegCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.reg !== 0) {
      obj.reg = Math.round(message.reg);
    }
    if (message.val !== 0) {
      obj.val = Math.round(message.val);
    }
    return obj;
  },
};

function createBasedebugWriteRtcRegCommandOBCResponse(): debugWriteRtcRegCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const debugWriteRtcRegCommandOBCResponse = {
  encode(message: debugWriteRtcRegCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      debugWriteRtcRegCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): debugWriteRtcRegCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasedebugWriteRtcRegCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = debugWriteRtcRegCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): debugWriteRtcRegCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? debugWriteRtcRegCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: debugWriteRtcRegCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = debugWriteRtcRegCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasedebugWriteRtcRegCommandOBCResponseData(): debugWriteRtcRegCommandOBCResponseData {
  return { valAfterWrite: 0 };
}

export const debugWriteRtcRegCommandOBCResponseData = {
  encode(message: debugWriteRtcRegCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.valAfterWrite !== 0) {
      writer.uint32(8).uint32(message.valAfterWrite);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): debugWriteRtcRegCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasedebugWriteRtcRegCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.valAfterWrite = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): debugWriteRtcRegCommandOBCResponseData {
    return { valAfterWrite: isSet(object.valAfterWrite) ? globalThis.Number(object.valAfterWrite) : 0 };
  },

  toJSON(message: debugWriteRtcRegCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.valAfterWrite !== 0) {
      obj.valAfterWrite = Math.round(message.valAfterWrite);
    }
    return obj;
  },
};

function createBasetestRtcAlarmNSecCommandArgs(): testRtcAlarmNSecCommandArgs {
  return { timeout: 0, dateTime: undefined, durationS: 0 };
}

export const testRtcAlarmNSecCommandArgs = {
  encode(message: testRtcAlarmNSecCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.durationS !== 0) {
      writer.uint32(24).uint32(message.durationS);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testRtcAlarmNSecCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestRtcAlarmNSecCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.durationS = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testRtcAlarmNSecCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      durationS: isSet(object.durationS) ? globalThis.Number(object.durationS) : 0,
    };
  },

  toJSON(message: testRtcAlarmNSecCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.durationS !== 0) {
      obj.durationS = Math.round(message.durationS);
    }
    return obj;
  },
};

function createBasetestRtcAlarmNSecCommandOBCResponse(): testRtcAlarmNSecCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testRtcAlarmNSecCommandOBCResponse = {
  encode(message: testRtcAlarmNSecCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testRtcAlarmNSecCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testRtcAlarmNSecCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestRtcAlarmNSecCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testRtcAlarmNSecCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testRtcAlarmNSecCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testRtcAlarmNSecCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testRtcAlarmNSecCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testRtcAlarmNSecCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestRtcAlarmNSecCommandOBCResponseData(): testRtcAlarmNSecCommandOBCResponseData {
  return {};
}

export const testRtcAlarmNSecCommandOBCResponseData = {
  encode(_: testRtcAlarmNSecCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testRtcAlarmNSecCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestRtcAlarmNSecCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(_: any): testRtcAlarmNSecCommandOBCResponseData {
    return {};
  },

  toJSON(_: testRtcAlarmNSecCommandOBCResponseData): unknown {
    const obj: any = {};
    return obj;
  },
};

function createBasetestAdcsSunModelCommandArgs(): testAdcsSunModelCommandArgs {
  return { timeout: 0, dateTime: undefined, time: [] };
}

export const testAdcsSunModelCommandArgs = {
  encode(message: testAdcsSunModelCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    writer.uint32(26).fork();
    for (const v of message.time) {
      writer.sint32(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testAdcsSunModelCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestAdcsSunModelCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag === 24) {
            message.time.push(reader.sint32());

            continue;
          }

          if (tag === 26) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.time.push(reader.sint32());
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testAdcsSunModelCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      time: globalThis.Array.isArray(object?.time) ? object.time.map((e: any) => globalThis.Number(e)) : [],
    };
  },

  toJSON(message: testAdcsSunModelCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.time?.length) {
      obj.time = message.time.map((e) => Math.round(e));
    }
    return obj;
  },
};

function createBasetestAdcsSunModelCommandOBCResponse(): testAdcsSunModelCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testAdcsSunModelCommandOBCResponse = {
  encode(message: testAdcsSunModelCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testAdcsSunModelCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testAdcsSunModelCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestAdcsSunModelCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testAdcsSunModelCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testAdcsSunModelCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testAdcsSunModelCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testAdcsSunModelCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testAdcsSunModelCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestAdcsSunModelCommandOBCResponseData(): testAdcsSunModelCommandOBCResponseData {
  return { jul: 0, pos: [] };
}

export const testAdcsSunModelCommandOBCResponseData = {
  encode(message: testAdcsSunModelCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.jul !== 0) {
      writer.uint32(9).double(message.jul);
    }
    writer.uint32(18).fork();
    for (const v of message.pos) {
      writer.double(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testAdcsSunModelCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestAdcsSunModelCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 9) {
            break;
          }

          message.jul = reader.double();
          continue;
        case 2:
          if (tag === 17) {
            message.pos.push(reader.double());

            continue;
          }

          if (tag === 18) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.pos.push(reader.double());
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testAdcsSunModelCommandOBCResponseData {
    return {
      jul: isSet(object.jul) ? globalThis.Number(object.jul) : 0,
      pos: globalThis.Array.isArray(object?.pos) ? object.pos.map((e: any) => globalThis.Number(e)) : [],
    };
  },

  toJSON(message: testAdcsSunModelCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.jul !== 0) {
      obj.jul = message.jul;
    }
    if (message.pos?.length) {
      obj.pos = message.pos;
    }
    return obj;
  },
};

function createBasetestAdcVoltageCommandArgs(): testAdcVoltageCommandArgs {
  return { timeout: 0, dateTime: undefined, channel: 0 };
}

export const testAdcVoltageCommandArgs = {
  encode(message: testAdcVoltageCommandArgs, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.timeout !== 0) {
      writer.uint32(8).uint64(message.timeout);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(18).fork()).ldelim();
    }
    if (message.channel !== 0) {
      writer.uint32(24).uint32(message.channel);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testAdcVoltageCommandArgs {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestAdcVoltageCommandArgs();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.timeout = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 3:
          if (tag !== 24) {
            break;
          }

          message.channel = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testAdcVoltageCommandArgs {
    return {
      timeout: isSet(object.timeout) ? globalThis.Number(object.timeout) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      channel: isSet(object.channel) ? globalThis.Number(object.channel) : 0,
    };
  },

  toJSON(message: testAdcVoltageCommandArgs): unknown {
    const obj: any = {};
    if (message.timeout !== 0) {
      obj.timeout = Math.round(message.timeout);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.channel !== 0) {
      obj.channel = Math.round(message.channel);
    }
    return obj;
  },
};

function createBasetestAdcVoltageCommandOBCResponse(): testAdcVoltageCommandOBCResponse {
  return { cmdInstId: 0, cmdId: 0, dateTime: undefined, flags: 0, dataHeader: undefined, data: undefined };
}

export const testAdcVoltageCommandOBCResponse = {
  encode(message: testAdcVoltageCommandOBCResponse, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.cmdInstId !== 0) {
      writer.uint32(8).uint64(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      writer.uint32(16).uint32(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      Timestamp.encode(toTimestamp(message.dateTime), writer.uint32(26).fork()).ldelim();
    }
    if (message.flags !== 0) {
      writer.uint32(32).uint32(message.flags);
    }
    if (message.dataHeader !== undefined) {
      OBCResponseDataHeader.encode(message.dataHeader, writer.uint32(42).fork()).ldelim();
    }
    if (message.data !== undefined) {
      testAdcVoltageCommandOBCResponseData.encode(message.data, writer.uint32(50).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testAdcVoltageCommandOBCResponse {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestAdcVoltageCommandOBCResponse();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.cmdInstId = longToNumber(reader.uint64() as Long);
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cmdId = reader.uint32();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.dateTime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
        case 4:
          if (tag !== 32) {
            break;
          }

          message.flags = reader.uint32();
          continue;
        case 5:
          if (tag !== 42) {
            break;
          }

          message.dataHeader = OBCResponseDataHeader.decode(reader, reader.uint32());
          continue;
        case 6:
          if (tag !== 50) {
            break;
          }

          message.data = testAdcVoltageCommandOBCResponseData.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testAdcVoltageCommandOBCResponse {
    return {
      cmdInstId: isSet(object.cmdInstId) ? globalThis.Number(object.cmdInstId) : 0,
      cmdId: isSet(object.cmdId) ? globalThis.Number(object.cmdId) : 0,
      dateTime: isSet(object.dateTime) ? fromJsonTimestamp(object.dateTime) : undefined,
      flags: isSet(object.flags) ? globalThis.Number(object.flags) : 0,
      dataHeader: isSet(object.dataHeader) ? OBCResponseDataHeader.fromJSON(object.dataHeader) : undefined,
      data: isSet(object.data) ? testAdcVoltageCommandOBCResponseData.fromJSON(object.data) : undefined,
    };
  },

  toJSON(message: testAdcVoltageCommandOBCResponse): unknown {
    const obj: any = {};
    if (message.cmdInstId !== 0) {
      obj.cmdInstId = Math.round(message.cmdInstId);
    }
    if (message.cmdId !== 0) {
      obj.cmdId = Math.round(message.cmdId);
    }
    if (message.dateTime !== undefined) {
      obj.dateTime = message.dateTime.toISOString();
    }
    if (message.flags !== 0) {
      obj.flags = Math.round(message.flags);
    }
    if (message.dataHeader !== undefined) {
      obj.dataHeader = OBCResponseDataHeader.toJSON(message.dataHeader);
    }
    if (message.data !== undefined) {
      obj.data = testAdcVoltageCommandOBCResponseData.toJSON(message.data);
    }
    return obj;
  },
};

function createBasetestAdcVoltageCommandOBCResponseData(): testAdcVoltageCommandOBCResponseData {
  return { channelVoltage: 0, adcErrStatus: 0 };
}

export const testAdcVoltageCommandOBCResponseData = {
  encode(message: testAdcVoltageCommandOBCResponseData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.channelVoltage !== 0) {
      writer.uint32(8).uint32(message.channelVoltage);
    }
    if (message.adcErrStatus !== 0) {
      writer.uint32(16).uint32(message.adcErrStatus);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): testAdcVoltageCommandOBCResponseData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasetestAdcVoltageCommandOBCResponseData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.channelVoltage = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.adcErrStatus = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): testAdcVoltageCommandOBCResponseData {
    return {
      channelVoltage: isSet(object.channelVoltage) ? globalThis.Number(object.channelVoltage) : 0,
      adcErrStatus: isSet(object.adcErrStatus) ? globalThis.Number(object.adcErrStatus) : 0,
    };
  },

  toJSON(message: testAdcVoltageCommandOBCResponseData): unknown {
    const obj: any = {};
    if (message.channelVoltage !== 0) {
      obj.channelVoltage = Math.round(message.channelVoltage);
    }
    if (message.adcErrStatus !== 0) {
      obj.adcErrStatus = Math.round(message.adcErrStatus);
    }
    return obj;
  },
};

function createBaseuint32Matrix(): uint32Matrix {
  return { rows: 0, cols: 0, data: [] };
}

export const uint32Matrix = {
  encode(message: uint32Matrix, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.rows !== 0) {
      writer.uint32(8).uint32(message.rows);
    }
    if (message.cols !== 0) {
      writer.uint32(16).uint32(message.cols);
    }
    writer.uint32(26).fork();
    for (const v of message.data) {
      writer.uint32(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): uint32Matrix {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseuint32Matrix();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.rows = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cols = reader.uint32();
          continue;
        case 3:
          if (tag === 24) {
            message.data.push(reader.uint32());

            continue;
          }

          if (tag === 26) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.data.push(reader.uint32());
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): uint32Matrix {
    return {
      rows: isSet(object.rows) ? globalThis.Number(object.rows) : 0,
      cols: isSet(object.cols) ? globalThis.Number(object.cols) : 0,
      data: globalThis.Array.isArray(object?.data) ? object.data.map((e: any) => globalThis.Number(e)) : [],
    };
  },

  toJSON(message: uint32Matrix): unknown {
    const obj: any = {};
    if (message.rows !== 0) {
      obj.rows = Math.round(message.rows);
    }
    if (message.cols !== 0) {
      obj.cols = Math.round(message.cols);
    }
    if (message.data?.length) {
      obj.data = message.data.map((e) => Math.round(e));
    }
    return obj;
  },
};

function createBaseuint64Matrix(): uint64Matrix {
  return { rows: 0, cols: 0, data: [] };
}

export const uint64Matrix = {
  encode(message: uint64Matrix, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.rows !== 0) {
      writer.uint32(8).uint32(message.rows);
    }
    if (message.cols !== 0) {
      writer.uint32(16).uint32(message.cols);
    }
    writer.uint32(26).fork();
    for (const v of message.data) {
      writer.uint64(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): uint64Matrix {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseuint64Matrix();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.rows = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cols = reader.uint32();
          continue;
        case 3:
          if (tag === 24) {
            message.data.push(longToNumber(reader.uint64() as Long));

            continue;
          }

          if (tag === 26) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.data.push(longToNumber(reader.uint64() as Long));
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): uint64Matrix {
    return {
      rows: isSet(object.rows) ? globalThis.Number(object.rows) : 0,
      cols: isSet(object.cols) ? globalThis.Number(object.cols) : 0,
      data: globalThis.Array.isArray(object?.data) ? object.data.map((e: any) => globalThis.Number(e)) : [],
    };
  },

  toJSON(message: uint64Matrix): unknown {
    const obj: any = {};
    if (message.rows !== 0) {
      obj.rows = Math.round(message.rows);
    }
    if (message.cols !== 0) {
      obj.cols = Math.round(message.cols);
    }
    if (message.data?.length) {
      obj.data = message.data.map((e) => Math.round(e));
    }
    return obj;
  },
};

function createBasesint32Matrix(): sint32Matrix {
  return { rows: 0, cols: 0, data: [] };
}

export const sint32Matrix = {
  encode(message: sint32Matrix, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.rows !== 0) {
      writer.uint32(8).uint32(message.rows);
    }
    if (message.cols !== 0) {
      writer.uint32(16).uint32(message.cols);
    }
    writer.uint32(26).fork();
    for (const v of message.data) {
      writer.sint32(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): sint32Matrix {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasesint32Matrix();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.rows = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cols = reader.uint32();
          continue;
        case 3:
          if (tag === 24) {
            message.data.push(reader.sint32());

            continue;
          }

          if (tag === 26) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.data.push(reader.sint32());
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): sint32Matrix {
    return {
      rows: isSet(object.rows) ? globalThis.Number(object.rows) : 0,
      cols: isSet(object.cols) ? globalThis.Number(object.cols) : 0,
      data: globalThis.Array.isArray(object?.data) ? object.data.map((e: any) => globalThis.Number(e)) : [],
    };
  },

  toJSON(message: sint32Matrix): unknown {
    const obj: any = {};
    if (message.rows !== 0) {
      obj.rows = Math.round(message.rows);
    }
    if (message.cols !== 0) {
      obj.cols = Math.round(message.cols);
    }
    if (message.data?.length) {
      obj.data = message.data.map((e) => Math.round(e));
    }
    return obj;
  },
};

function createBasesint64Matrix(): sint64Matrix {
  return { rows: 0, cols: 0, data: [] };
}

export const sint64Matrix = {
  encode(message: sint64Matrix, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.rows !== 0) {
      writer.uint32(8).uint32(message.rows);
    }
    if (message.cols !== 0) {
      writer.uint32(16).uint32(message.cols);
    }
    writer.uint32(26).fork();
    for (const v of message.data) {
      writer.sint64(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): sint64Matrix {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasesint64Matrix();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.rows = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cols = reader.uint32();
          continue;
        case 3:
          if (tag === 24) {
            message.data.push(longToNumber(reader.sint64() as Long));

            continue;
          }

          if (tag === 26) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.data.push(longToNumber(reader.sint64() as Long));
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): sint64Matrix {
    return {
      rows: isSet(object.rows) ? globalThis.Number(object.rows) : 0,
      cols: isSet(object.cols) ? globalThis.Number(object.cols) : 0,
      data: globalThis.Array.isArray(object?.data) ? object.data.map((e: any) => globalThis.Number(e)) : [],
    };
  },

  toJSON(message: sint64Matrix): unknown {
    const obj: any = {};
    if (message.rows !== 0) {
      obj.rows = Math.round(message.rows);
    }
    if (message.cols !== 0) {
      obj.cols = Math.round(message.cols);
    }
    if (message.data?.length) {
      obj.data = message.data.map((e) => Math.round(e));
    }
    return obj;
  },
};

function createBasedoubleMatrix(): doubleMatrix {
  return { rows: 0, cols: 0, data: [] };
}

export const doubleMatrix = {
  encode(message: doubleMatrix, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.rows !== 0) {
      writer.uint32(8).uint32(message.rows);
    }
    if (message.cols !== 0) {
      writer.uint32(16).uint32(message.cols);
    }
    writer.uint32(26).fork();
    for (const v of message.data) {
      writer.double(v);
    }
    writer.ldelim();
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): doubleMatrix {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBasedoubleMatrix();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.rows = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.cols = reader.uint32();
          continue;
        case 3:
          if (tag === 25) {
            message.data.push(reader.double());

            continue;
          }

          if (tag === 26) {
            const end2 = reader.uint32() + reader.pos;
            while (reader.pos < end2) {
              message.data.push(reader.double());
            }

            continue;
          }

          break;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): doubleMatrix {
    return {
      rows: isSet(object.rows) ? globalThis.Number(object.rows) : 0,
      cols: isSet(object.cols) ? globalThis.Number(object.cols) : 0,
      data: globalThis.Array.isArray(object?.data) ? object.data.map((e: any) => globalThis.Number(e)) : [],
    };
  },

  toJSON(message: doubleMatrix): unknown {
    const obj: any = {};
    if (message.rows !== 0) {
      obj.rows = Math.round(message.rows);
    }
    if (message.cols !== 0) {
      obj.cols = Math.round(message.cols);
    }
    if (message.data?.length) {
      obj.data = message.data;
    }
    return obj;
  },
};

function createBaseOBCResponseDataHeader(): OBCResponseDataHeader {
  return { code: 0, execDatetime: undefined };
}

export const OBCResponseDataHeader = {
  encode(message: OBCResponseDataHeader, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.code !== 0) {
      writer.uint32(8).int32(message.code);
    }
    if (message.execDatetime !== undefined) {
      Timestamp.encode(toTimestamp(message.execDatetime), writer.uint32(18).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): OBCResponseDataHeader {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseOBCResponseDataHeader();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.code = reader.int32() as any;
          continue;
        case 2:
          if (tag !== 18) {
            break;
          }

          message.execDatetime = fromTimestamp(Timestamp.decode(reader, reader.uint32()));
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): OBCResponseDataHeader {
    return {
      code: isSet(object.code) ? codeFromJSON(object.code) : 0,
      execDatetime: isSet(object.execDatetime) ? fromJsonTimestamp(object.execDatetime) : undefined,
    };
  },

  toJSON(message: OBCResponseDataHeader): unknown {
    const obj: any = {};
    if (message.code !== 0) {
      obj.code = codeToJSON(message.code);
    }
    if (message.execDatetime !== undefined) {
      obj.execDatetime = message.execDatetime.toISOString();
    }
    return obj;
  },
};

export type obcDefinition = typeof obcDefinition;
export const obcDefinition = {
  name: "obc",
  fullName: "obc",
  methods: {
    reset: {
      name: "reset",
      requestType: resetCommandArgs,
      requestStream: false,
      responseType: resetCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    ping: {
      name: "ping",
      requestType: pingCommandArgs,
      requestStream: false,
      responseType: pingCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    get_time: {
      name: "get_time",
      requestType: getTimeCommandArgs,
      requestStream: false,
      responseType: getTimeCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    set_time: {
      name: "set_time",
      requestType: setTimeCommandArgs,
      requestStream: false,
      responseType: setTimeCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    get_min_stack_space: {
      name: "get_min_stack_space",
      requestType: getMinStackSpaceCommandArgs,
      requestStream: false,
      responseType: getMinStackSpaceCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    capture_rtos_trace: {
      name: "capture_rtos_trace",
      requestType: captureRtosTraceCommandArgs,
      requestStream: false,
      responseType: captureRtosTraceCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    rtos_state: {
      name: "rtos_state",
      requestType: rtosStateCommandArgs,
      requestStream: false,
      responseType: rtosStateCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    low_power: {
      name: "low_power",
      requestType: lowPowerCommandArgs,
      requestStream: false,
      responseType: lowPowerCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    sys_time: {
      name: "sys_time",
      requestType: sysTimeCommandArgs,
      requestStream: false,
      responseType: sysTimeCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    cpu_usage: {
      name: "cpu_usage",
      requestType: cpuUsageCommandArgs,
      requestStream: false,
      responseType: cpuUsageCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    task_runtime: {
      name: "task_runtime",
      requestType: taskRuntimeCommandArgs,
      requestStream: false,
      responseType: taskRuntimeCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gpio_exp_reset: {
      name: "gpio_exp_reset",
      requestType: gpioExpResetCommandArgs,
      requestStream: false,
      responseType: gpioExpResetCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    i2c_reset: {
      name: "i2c_reset",
      requestType: i2cResetCommandArgs,
      requestStream: false,
      responseType: i2cResetCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    heartbeat: {
      name: "heartbeat",
      requestType: heartbeatCommandArgs,
      requestStream: false,
      responseType: heartbeatCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_restart: {
      name: "gps_restart",
      requestType: gpsRestartCommandArgs,
      requestStream: false,
      responseType: gpsRestartCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    fw_info: {
      name: "fw_info",
      requestType: fwInfoCommandArgs,
      requestStream: false,
      responseType: fwInfoCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    wmm_init: {
      name: "wmm_init",
      requestType: wmmInitCommandArgs,
      requestStream: false,
      responseType: wmmInitCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    wmm_is_init: {
      name: "wmm_is_init",
      requestType: wmmIsInitCommandArgs,
      requestStream: false,
      responseType: wmmIsInitCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    wmm_get_mag_ref: {
      name: "wmm_get_mag_ref",
      requestType: wmmGetMagRefCommandArgs,
      requestStream: false,
      responseType: wmmGetMagRefCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_query_sw_ver: {
      name: "gps_query_sw_ver",
      requestType: gpsQuerySwVerCommandArgs,
      requestStream: false,
      responseType: gpsQuerySwVerCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_set_factory_settings: {
      name: "gps_set_factory_settings",
      requestType: gpsSetFactorySettingsCommandArgs,
      requestStream: false,
      responseType: gpsSetFactorySettingsCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_configure_power_mode: {
      name: "gps_configure_power_mode",
      requestType: gpsConfigurePowerModeCommandArgs,
      requestStream: false,
      responseType: gpsConfigurePowerModeCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_query_power_mode: {
      name: "gps_query_power_mode",
      requestType: gpsQueryPowerModeCommandArgs,
      requestStream: false,
      responseType: gpsQueryPowerModeCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_query_sw_crc: {
      name: "gps_query_sw_crc",
      requestType: gpsQuerySwCrcCommandArgs,
      requestStream: false,
      responseType: gpsQuerySwCrcCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_configure_position_upate_rate: {
      name: "gps_configure_position_upate_rate",
      requestType: gpsConfigurePositionUpateRateCommandArgs,
      requestStream: false,
      responseType: gpsConfigurePositionUpateRateCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_query_position_update_rate: {
      name: "gps_query_position_update_rate",
      requestType: gpsQueryPositionUpdateRateCommandArgs,
      requestStream: false,
      responseType: gpsQueryPositionUpdateRateCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_configure_nmea_messges: {
      name: "gps_configure_nmea_messges",
      requestType: gpsConfigureNmeaMessgesCommandArgs,
      requestStream: false,
      responseType: gpsConfigureNmeaMessgesCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_query_nmea_messges: {
      name: "gps_query_nmea_messges",
      requestType: gpsQueryNmeaMessgesCommandArgs,
      requestStream: false,
      responseType: gpsQueryNmeaMessgesCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_configure_serial_port: {
      name: "gps_configure_serial_port",
      requestType: gpsConfigureSerialPortCommandArgs,
      requestStream: false,
      responseType: gpsConfigureSerialPortCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    gps_configure_nav_msg_interval: {
      name: "gps_configure_nav_msg_interval",
      requestType: gpsConfigureNavMsgIntervalCommandArgs,
      requestStream: false,
      responseType: gpsConfigureNavMsgIntervalCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    get_uptime: {
      name: "get_uptime",
      requestType: getUptimeCommandArgs,
      requestStream: false,
      responseType: getUptimeCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_echo: {
      name: "test_echo",
      requestType: testEchoCommandArgs,
      requestStream: false,
      responseType: testEchoCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_hang: {
      name: "test_hang",
      requestType: testHangCommandArgs,
      requestStream: false,
      responseType: testHangCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_comms_tx_rx: {
      name: "test_comms_tx_rx",
      requestType: testCommsTxRxCommandArgs,
      requestStream: false,
      responseType: testCommsTxRxCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_comms_stress1: {
      name: "test_comms_stress1",
      requestType: testCommsStress1CommandArgs,
      requestStream: false,
      responseType: testCommsStress1CommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_comms_flash_app: {
      name: "test_comms_flash_app",
      requestType: testCommsFlashAppCommandArgs,
      requestStream: false,
      responseType: testCommsFlashAppCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_comms_reboot: {
      name: "test_comms_reboot",
      requestType: testCommsRebootCommandArgs,
      requestStream: false,
      responseType: testCommsRebootCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_comms_get_telem: {
      name: "test_comms_get_telem",
      requestType: testCommsGetTelemCommandArgs,
      requestStream: false,
      responseType: testCommsGetTelemCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_read_float: {
      name: "test_eps_read_float",
      requestType: testEpsReadFloatCommandArgs,
      requestStream: false,
      responseType: testEpsReadFloatCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_read_float_raw: {
      name: "test_eps_read_float_raw",
      requestType: testEpsReadFloatRawCommandArgs,
      requestStream: false,
      responseType: testEpsReadFloatRawCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_read_int: {
      name: "test_eps_read_int",
      requestType: testEpsReadIntCommandArgs,
      requestStream: false,
      responseType: testEpsReadIntCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_write: {
      name: "test_eps_write",
      requestType: testEpsWriteCommandArgs,
      requestStream: false,
      responseType: testEpsWriteCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_read_sanity: {
      name: "test_eps_read_sanity",
      requestType: testEpsReadSanityCommandArgs,
      requestStream: false,
      responseType: testEpsReadSanityCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_writeread_sanity: {
      name: "test_eps_writeread_sanity",
      requestType: testEpsWritereadSanityCommandArgs,
      requestStream: false,
      responseType: testEpsWritereadSanityCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_measure_buses: {
      name: "test_eps_measure_buses",
      requestType: testEpsMeasureBusesCommandArgs,
      requestStream: false,
      responseType: testEpsMeasureBusesCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_measure_temps: {
      name: "test_eps_measure_temps",
      requestType: testEpsMeasureTempsCommandArgs,
      requestStream: false,
      responseType: testEpsMeasureTempsCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_read_counters: {
      name: "test_eps_read_counters",
      requestType: testEpsReadCountersCommandArgs,
      requestStream: false,
      responseType: testEpsReadCountersCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_eps_read_status: {
      name: "test_eps_read_status",
      requestType: testEpsReadStatusCommandArgs,
      requestStream: false,
      responseType: testEpsReadStatusCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_mag: {
      name: "test_mag",
      requestType: testMagCommandArgs,
      requestStream: false,
      responseType: testMagCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_mag_all: {
      name: "test_mag_all",
      requestType: testMagAllCommandArgs,
      requestStream: false,
      responseType: testMagAllCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_mag_init: {
      name: "test_mag_init",
      requestType: testMagInitCommandArgs,
      requestStream: false,
      responseType: testMagInitCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_imu: {
      name: "test_imu",
      requestType: testImuCommandArgs,
      requestStream: false,
      responseType: testImuCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_panel_gyro: {
      name: "test_panel_gyro",
      requestType: testPanelGyroCommandArgs,
      requestStream: false,
      responseType: testPanelGyroCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_can_gpio: {
      name: "test_can_gpio",
      requestType: testCanGpioCommandArgs,
      requestStream: false,
      responseType: testCanGpioCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_flash_rw: {
      name: "test_flash_rw",
      requestType: testFlashRwCommandArgs,
      requestStream: false,
      responseType: testFlashRwCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_filesystem: {
      name: "test_filesystem",
      requestType: testFilesystemCommandArgs,
      requestStream: false,
      responseType: testFilesystemCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_cam_init: {
      name: "test_cam_init",
      requestType: testCamInitCommandArgs,
      requestStream: false,
      responseType: testCamInitCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_cam_capture: {
      name: "test_cam_capture",
      requestType: testCamCaptureCommandArgs,
      requestStream: false,
      responseType: testCamCaptureCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_cam_wr_sreg: {
      name: "test_cam_wr_sreg",
      requestType: testCamWrSregCommandArgs,
      requestStream: false,
      responseType: testCamWrSregCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_cam_rd_sreg: {
      name: "test_cam_rd_sreg",
      requestType: testCamRdSregCommandArgs,
      requestStream: false,
      responseType: testCamRdSregCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_cam_wr_sregs: {
      name: "test_cam_wr_sregs",
      requestType: testCamWrSregsCommandArgs,
      requestStream: false,
      responseType: testCamWrSregsCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_dsplib: {
      name: "test_dsplib",
      requestType: testDsplibCommandArgs,
      requestStream: false,
      responseType: testDsplibCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_quest: {
      name: "test_quest",
      requestType: testQuestCommandArgs,
      requestStream: false,
      responseType: testQuestCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_rtc_get_time: {
      name: "test_rtc_get_time",
      requestType: testRtcGetTimeCommandArgs,
      requestStream: false,
      responseType: testRtcGetTimeCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_rtc_cap_and_get_timestamp: {
      name: "test_rtc_cap_and_get_timestamp",
      requestType: testRtcCapAndGetTimestampCommandArgs,
      requestStream: false,
      responseType: testRtcCapAndGetTimestampCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    debug_read_rtc_reg: {
      name: "debug_read_rtc_reg",
      requestType: debugReadRtcRegCommandArgs,
      requestStream: false,
      responseType: debugReadRtcRegCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    debug_write_rtc_reg: {
      name: "debug_write_rtc_reg",
      requestType: debugWriteRtcRegCommandArgs,
      requestStream: false,
      responseType: debugWriteRtcRegCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_rtc_alarm_n_sec: {
      name: "test_rtc_alarm_n_sec",
      requestType: testRtcAlarmNSecCommandArgs,
      requestStream: false,
      responseType: testRtcAlarmNSecCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_adcs_sun_model: {
      name: "test_adcs_sun_model",
      requestType: testAdcsSunModelCommandArgs,
      requestStream: false,
      responseType: testAdcsSunModelCommandOBCResponse,
      responseStream: false,
      options: {},
    },
    test_adc_voltage: {
      name: "test_adc_voltage",
      requestType: testAdcVoltageCommandArgs,
      requestStream: false,
      responseType: testAdcVoltageCommandOBCResponse,
      responseStream: false,
      options: {},
    },
  },
} as const;

export interface obcServiceImplementation<CallContextExt = {}> {
  reset(request: resetCommandArgs, context: CallContext & CallContextExt): Promise<resetCommandOBCResponse>;
  ping(request: pingCommandArgs, context: CallContext & CallContextExt): Promise<pingCommandOBCResponse>;
  get_time(request: getTimeCommandArgs, context: CallContext & CallContextExt): Promise<getTimeCommandOBCResponse>;
  set_time(request: setTimeCommandArgs, context: CallContext & CallContextExt): Promise<setTimeCommandOBCResponse>;
  get_min_stack_space(
    request: getMinStackSpaceCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<getMinStackSpaceCommandOBCResponse>;
  capture_rtos_trace(
    request: captureRtosTraceCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<captureRtosTraceCommandOBCResponse>;
  rtos_state(
    request: rtosStateCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<rtosStateCommandOBCResponse>;
  low_power(request: lowPowerCommandArgs, context: CallContext & CallContextExt): Promise<lowPowerCommandOBCResponse>;
  sys_time(request: sysTimeCommandArgs, context: CallContext & CallContextExt): Promise<sysTimeCommandOBCResponse>;
  cpu_usage(request: cpuUsageCommandArgs, context: CallContext & CallContextExt): Promise<cpuUsageCommandOBCResponse>;
  task_runtime(
    request: taskRuntimeCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<taskRuntimeCommandOBCResponse>;
  gpio_exp_reset(
    request: gpioExpResetCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpioExpResetCommandOBCResponse>;
  i2c_reset(request: i2cResetCommandArgs, context: CallContext & CallContextExt): Promise<i2cResetCommandOBCResponse>;
  heartbeat(request: heartbeatCommandArgs, context: CallContext & CallContextExt): Promise<heartbeatCommandOBCResponse>;
  gps_restart(
    request: gpsRestartCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsRestartCommandOBCResponse>;
  fw_info(request: fwInfoCommandArgs, context: CallContext & CallContextExt): Promise<fwInfoCommandOBCResponse>;
  wmm_init(request: wmmInitCommandArgs, context: CallContext & CallContextExt): Promise<wmmInitCommandOBCResponse>;
  wmm_is_init(
    request: wmmIsInitCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<wmmIsInitCommandOBCResponse>;
  wmm_get_mag_ref(
    request: wmmGetMagRefCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<wmmGetMagRefCommandOBCResponse>;
  gps_query_sw_ver(
    request: gpsQuerySwVerCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsQuerySwVerCommandOBCResponse>;
  gps_set_factory_settings(
    request: gpsSetFactorySettingsCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsSetFactorySettingsCommandOBCResponse>;
  gps_configure_power_mode(
    request: gpsConfigurePowerModeCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsConfigurePowerModeCommandOBCResponse>;
  gps_query_power_mode(
    request: gpsQueryPowerModeCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsQueryPowerModeCommandOBCResponse>;
  gps_query_sw_crc(
    request: gpsQuerySwCrcCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsQuerySwCrcCommandOBCResponse>;
  gps_configure_position_upate_rate(
    request: gpsConfigurePositionUpateRateCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsConfigurePositionUpateRateCommandOBCResponse>;
  gps_query_position_update_rate(
    request: gpsQueryPositionUpdateRateCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsQueryPositionUpdateRateCommandOBCResponse>;
  gps_configure_nmea_messges(
    request: gpsConfigureNmeaMessgesCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsConfigureNmeaMessgesCommandOBCResponse>;
  gps_query_nmea_messges(
    request: gpsQueryNmeaMessgesCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsQueryNmeaMessgesCommandOBCResponse>;
  gps_configure_serial_port(
    request: gpsConfigureSerialPortCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsConfigureSerialPortCommandOBCResponse>;
  gps_configure_nav_msg_interval(
    request: gpsConfigureNavMsgIntervalCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<gpsConfigureNavMsgIntervalCommandOBCResponse>;
  get_uptime(
    request: getUptimeCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<getUptimeCommandOBCResponse>;
  test_echo(request: testEchoCommandArgs, context: CallContext & CallContextExt): Promise<testEchoCommandOBCResponse>;
  test_hang(request: testHangCommandArgs, context: CallContext & CallContextExt): Promise<testHangCommandOBCResponse>;
  test_comms_tx_rx(
    request: testCommsTxRxCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCommsTxRxCommandOBCResponse>;
  test_comms_stress1(
    request: testCommsStress1CommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCommsStress1CommandOBCResponse>;
  test_comms_flash_app(
    request: testCommsFlashAppCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCommsFlashAppCommandOBCResponse>;
  test_comms_reboot(
    request: testCommsRebootCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCommsRebootCommandOBCResponse>;
  test_comms_get_telem(
    request: testCommsGetTelemCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCommsGetTelemCommandOBCResponse>;
  test_eps_read_float(
    request: testEpsReadFloatCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsReadFloatCommandOBCResponse>;
  test_eps_read_float_raw(
    request: testEpsReadFloatRawCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsReadFloatRawCommandOBCResponse>;
  test_eps_read_int(
    request: testEpsReadIntCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsReadIntCommandOBCResponse>;
  test_eps_write(
    request: testEpsWriteCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsWriteCommandOBCResponse>;
  test_eps_read_sanity(
    request: testEpsReadSanityCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsReadSanityCommandOBCResponse>;
  test_eps_writeread_sanity(
    request: testEpsWritereadSanityCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsWritereadSanityCommandOBCResponse>;
  test_eps_measure_buses(
    request: testEpsMeasureBusesCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsMeasureBusesCommandOBCResponse>;
  test_eps_measure_temps(
    request: testEpsMeasureTempsCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsMeasureTempsCommandOBCResponse>;
  test_eps_read_counters(
    request: testEpsReadCountersCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsReadCountersCommandOBCResponse>;
  test_eps_read_status(
    request: testEpsReadStatusCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testEpsReadStatusCommandOBCResponse>;
  test_mag(request: testMagCommandArgs, context: CallContext & CallContextExt): Promise<testMagCommandOBCResponse>;
  test_mag_all(
    request: testMagAllCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testMagAllCommandOBCResponse>;
  test_mag_init(
    request: testMagInitCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testMagInitCommandOBCResponse>;
  test_imu(request: testImuCommandArgs, context: CallContext & CallContextExt): Promise<testImuCommandOBCResponse>;
  test_panel_gyro(
    request: testPanelGyroCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testPanelGyroCommandOBCResponse>;
  test_can_gpio(
    request: testCanGpioCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCanGpioCommandOBCResponse>;
  test_flash_rw(
    request: testFlashRwCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testFlashRwCommandOBCResponse>;
  test_filesystem(
    request: testFilesystemCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testFilesystemCommandOBCResponse>;
  test_cam_init(
    request: testCamInitCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCamInitCommandOBCResponse>;
  test_cam_capture(
    request: testCamCaptureCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCamCaptureCommandOBCResponse>;
  test_cam_wr_sreg(
    request: testCamWrSregCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCamWrSregCommandOBCResponse>;
  test_cam_rd_sreg(
    request: testCamRdSregCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCamRdSregCommandOBCResponse>;
  test_cam_wr_sregs(
    request: testCamWrSregsCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testCamWrSregsCommandOBCResponse>;
  test_dsplib(
    request: testDsplibCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testDsplibCommandOBCResponse>;
  test_quest(
    request: testQuestCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testQuestCommandOBCResponse>;
  test_rtc_get_time(
    request: testRtcGetTimeCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testRtcGetTimeCommandOBCResponse>;
  test_rtc_cap_and_get_timestamp(
    request: testRtcCapAndGetTimestampCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testRtcCapAndGetTimestampCommandOBCResponse>;
  debug_read_rtc_reg(
    request: debugReadRtcRegCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<debugReadRtcRegCommandOBCResponse>;
  debug_write_rtc_reg(
    request: debugWriteRtcRegCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<debugWriteRtcRegCommandOBCResponse>;
  test_rtc_alarm_n_sec(
    request: testRtcAlarmNSecCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testRtcAlarmNSecCommandOBCResponse>;
  test_adcs_sun_model(
    request: testAdcsSunModelCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testAdcsSunModelCommandOBCResponse>;
  test_adc_voltage(
    request: testAdcVoltageCommandArgs,
    context: CallContext & CallContextExt,
  ): Promise<testAdcVoltageCommandOBCResponse>;
}

export interface obcClient<CallOptionsExt = {}> {
  reset(request: resetCommandArgs, options?: CallOptions & CallOptionsExt): Promise<resetCommandOBCResponse>;
  ping(request: pingCommandArgs, options?: CallOptions & CallOptionsExt): Promise<pingCommandOBCResponse>;
  get_time(request: getTimeCommandArgs, options?: CallOptions & CallOptionsExt): Promise<getTimeCommandOBCResponse>;
  set_time(request: setTimeCommandArgs, options?: CallOptions & CallOptionsExt): Promise<setTimeCommandOBCResponse>;
  get_min_stack_space(
    request: getMinStackSpaceCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<getMinStackSpaceCommandOBCResponse>;
  capture_rtos_trace(
    request: captureRtosTraceCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<captureRtosTraceCommandOBCResponse>;
  rtos_state(
    request: rtosStateCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<rtosStateCommandOBCResponse>;
  low_power(request: lowPowerCommandArgs, options?: CallOptions & CallOptionsExt): Promise<lowPowerCommandOBCResponse>;
  sys_time(request: sysTimeCommandArgs, options?: CallOptions & CallOptionsExt): Promise<sysTimeCommandOBCResponse>;
  cpu_usage(request: cpuUsageCommandArgs, options?: CallOptions & CallOptionsExt): Promise<cpuUsageCommandOBCResponse>;
  task_runtime(
    request: taskRuntimeCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<taskRuntimeCommandOBCResponse>;
  gpio_exp_reset(
    request: gpioExpResetCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpioExpResetCommandOBCResponse>;
  i2c_reset(request: i2cResetCommandArgs, options?: CallOptions & CallOptionsExt): Promise<i2cResetCommandOBCResponse>;
  heartbeat(
    request: heartbeatCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<heartbeatCommandOBCResponse>;
  gps_restart(
    request: gpsRestartCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsRestartCommandOBCResponse>;
  fw_info(request: fwInfoCommandArgs, options?: CallOptions & CallOptionsExt): Promise<fwInfoCommandOBCResponse>;
  wmm_init(request: wmmInitCommandArgs, options?: CallOptions & CallOptionsExt): Promise<wmmInitCommandOBCResponse>;
  wmm_is_init(
    request: wmmIsInitCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<wmmIsInitCommandOBCResponse>;
  wmm_get_mag_ref(
    request: wmmGetMagRefCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<wmmGetMagRefCommandOBCResponse>;
  gps_query_sw_ver(
    request: gpsQuerySwVerCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsQuerySwVerCommandOBCResponse>;
  gps_set_factory_settings(
    request: gpsSetFactorySettingsCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsSetFactorySettingsCommandOBCResponse>;
  gps_configure_power_mode(
    request: gpsConfigurePowerModeCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsConfigurePowerModeCommandOBCResponse>;
  gps_query_power_mode(
    request: gpsQueryPowerModeCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsQueryPowerModeCommandOBCResponse>;
  gps_query_sw_crc(
    request: gpsQuerySwCrcCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsQuerySwCrcCommandOBCResponse>;
  gps_configure_position_upate_rate(
    request: gpsConfigurePositionUpateRateCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsConfigurePositionUpateRateCommandOBCResponse>;
  gps_query_position_update_rate(
    request: gpsQueryPositionUpdateRateCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsQueryPositionUpdateRateCommandOBCResponse>;
  gps_configure_nmea_messges(
    request: gpsConfigureNmeaMessgesCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsConfigureNmeaMessgesCommandOBCResponse>;
  gps_query_nmea_messges(
    request: gpsQueryNmeaMessgesCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsQueryNmeaMessgesCommandOBCResponse>;
  gps_configure_serial_port(
    request: gpsConfigureSerialPortCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsConfigureSerialPortCommandOBCResponse>;
  gps_configure_nav_msg_interval(
    request: gpsConfigureNavMsgIntervalCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<gpsConfigureNavMsgIntervalCommandOBCResponse>;
  get_uptime(
    request: getUptimeCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<getUptimeCommandOBCResponse>;
  test_echo(request: testEchoCommandArgs, options?: CallOptions & CallOptionsExt): Promise<testEchoCommandOBCResponse>;
  test_hang(request: testHangCommandArgs, options?: CallOptions & CallOptionsExt): Promise<testHangCommandOBCResponse>;
  test_comms_tx_rx(
    request: testCommsTxRxCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCommsTxRxCommandOBCResponse>;
  test_comms_stress1(
    request: testCommsStress1CommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCommsStress1CommandOBCResponse>;
  test_comms_flash_app(
    request: testCommsFlashAppCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCommsFlashAppCommandOBCResponse>;
  test_comms_reboot(
    request: testCommsRebootCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCommsRebootCommandOBCResponse>;
  test_comms_get_telem(
    request: testCommsGetTelemCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCommsGetTelemCommandOBCResponse>;
  test_eps_read_float(
    request: testEpsReadFloatCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsReadFloatCommandOBCResponse>;
  test_eps_read_float_raw(
    request: testEpsReadFloatRawCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsReadFloatRawCommandOBCResponse>;
  test_eps_read_int(
    request: testEpsReadIntCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsReadIntCommandOBCResponse>;
  test_eps_write(
    request: testEpsWriteCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsWriteCommandOBCResponse>;
  test_eps_read_sanity(
    request: testEpsReadSanityCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsReadSanityCommandOBCResponse>;
  test_eps_writeread_sanity(
    request: testEpsWritereadSanityCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsWritereadSanityCommandOBCResponse>;
  test_eps_measure_buses(
    request: testEpsMeasureBusesCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsMeasureBusesCommandOBCResponse>;
  test_eps_measure_temps(
    request: testEpsMeasureTempsCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsMeasureTempsCommandOBCResponse>;
  test_eps_read_counters(
    request: testEpsReadCountersCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsReadCountersCommandOBCResponse>;
  test_eps_read_status(
    request: testEpsReadStatusCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testEpsReadStatusCommandOBCResponse>;
  test_mag(request: testMagCommandArgs, options?: CallOptions & CallOptionsExt): Promise<testMagCommandOBCResponse>;
  test_mag_all(
    request: testMagAllCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testMagAllCommandOBCResponse>;
  test_mag_init(
    request: testMagInitCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testMagInitCommandOBCResponse>;
  test_imu(request: testImuCommandArgs, options?: CallOptions & CallOptionsExt): Promise<testImuCommandOBCResponse>;
  test_panel_gyro(
    request: testPanelGyroCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testPanelGyroCommandOBCResponse>;
  test_can_gpio(
    request: testCanGpioCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCanGpioCommandOBCResponse>;
  test_flash_rw(
    request: testFlashRwCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testFlashRwCommandOBCResponse>;
  test_filesystem(
    request: testFilesystemCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testFilesystemCommandOBCResponse>;
  test_cam_init(
    request: testCamInitCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCamInitCommandOBCResponse>;
  test_cam_capture(
    request: testCamCaptureCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCamCaptureCommandOBCResponse>;
  test_cam_wr_sreg(
    request: testCamWrSregCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCamWrSregCommandOBCResponse>;
  test_cam_rd_sreg(
    request: testCamRdSregCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCamRdSregCommandOBCResponse>;
  test_cam_wr_sregs(
    request: testCamWrSregsCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testCamWrSregsCommandOBCResponse>;
  test_dsplib(
    request: testDsplibCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testDsplibCommandOBCResponse>;
  test_quest(
    request: testQuestCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testQuestCommandOBCResponse>;
  test_rtc_get_time(
    request: testRtcGetTimeCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testRtcGetTimeCommandOBCResponse>;
  test_rtc_cap_and_get_timestamp(
    request: testRtcCapAndGetTimestampCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testRtcCapAndGetTimestampCommandOBCResponse>;
  debug_read_rtc_reg(
    request: debugReadRtcRegCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<debugReadRtcRegCommandOBCResponse>;
  debug_write_rtc_reg(
    request: debugWriteRtcRegCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<debugWriteRtcRegCommandOBCResponse>;
  test_rtc_alarm_n_sec(
    request: testRtcAlarmNSecCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testRtcAlarmNSecCommandOBCResponse>;
  test_adcs_sun_model(
    request: testAdcsSunModelCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testAdcsSunModelCommandOBCResponse>;
  test_adc_voltage(
    request: testAdcVoltageCommandArgs,
    options?: CallOptions & CallOptionsExt,
  ): Promise<testAdcVoltageCommandOBCResponse>;
}

function bytesFromBase64(b64: string): Uint8Array {
  if (globalThis.Buffer) {
    return Uint8Array.from(globalThis.Buffer.from(b64, "base64"));
  } else {
    const bin = globalThis.atob(b64);
    const arr = new Uint8Array(bin.length);
    for (let i = 0; i < bin.length; ++i) {
      arr[i] = bin.charCodeAt(i);
    }
    return arr;
  }
}

function base64FromBytes(arr: Uint8Array): string {
  if (globalThis.Buffer) {
    return globalThis.Buffer.from(arr).toString("base64");
  } else {
    const bin: string[] = [];
    arr.forEach((byte) => {
      bin.push(globalThis.String.fromCharCode(byte));
    });
    return globalThis.btoa(bin.join(""));
  }
}

function toTimestamp(date: Date): Timestamp {
  const seconds = date.getTime() / 1_000;
  const nanos = (date.getTime() % 1_000) * 1_000_000;
  return { seconds, nanos };
}

function fromTimestamp(t: Timestamp): Date {
  let millis = (t.seconds || 0) * 1_000;
  millis += (t.nanos || 0) / 1_000_000;
  return new globalThis.Date(millis);
}

function fromJsonTimestamp(o: any): Date {
  if (o instanceof globalThis.Date) {
    return o;
  } else if (typeof o === "string") {
    return new globalThis.Date(o);
  } else {
    return fromTimestamp(Timestamp.fromJSON(o));
  }
}

function longToNumber(long: Long): number {
  if (long.gt(globalThis.Number.MAX_SAFE_INTEGER)) {
    throw new globalThis.Error("Value is larger than Number.MAX_SAFE_INTEGER");
  }
  return long.toNumber();
}

if (_m0.util.Long !== Long) {
  _m0.util.Long = Long as any;
  _m0.configure();
}

function isSet(value: any): boolean {
  return value !== null && value !== undefined;
}
