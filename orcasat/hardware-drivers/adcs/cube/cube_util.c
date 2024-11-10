/**
 * @file cube_util.c
 * @brief CubeADCS driver support library
 *        Adapted from CubeSpace Software Support repository:
 *        https://bitbucket.org/cubespace/cubespace-software-support/
 * @date Oct 28, 2020
 * @version 0.1.2
 */

#include "cube_util.h"

#include <stddef.h>

static inline bool bits_set(uint8_t bit_field, uint8_t bit_mask);

/**
 * @brief Look up the payload size of the specified TLM frame.
 * List of TLM frames can be found in Section 6 of CubeADCS Firmware Reference Manual:
 * https://drive.google.com/file/d/1bo5rNEvvLPYMZYvYsosqxEjmwzKBp-o5/view?usp=sharing
 *
 * @param tlm_id[in]: TLM frame ID
 *
 * @return TLM frame size if successful, CUBE_INVALID_MSG_ID otherwise
 */
uint16_t lookup_tlm_len(uint8_t tlm_id) {
    uint16_t len;

    switch (tlm_id) {
        case CubeTlm_AcpLoopState:
            len = 3;
            break;
        case CubeTlm_ActuatorCmd:
            len = 12;
            break;
        case CubeTlm_AdcsConfig:
            len = 464;
            break;
        case CubeTlm_AdcsExecTimes:
            len = 8;
            break;
        case CubeTlm_AdcsMeasure:
            len = 72;
            break;
        case CubeTlm_AdcsMiscCurrents:
            len = 6;
            break;
        case CubeTlm_AdcsPower:
            len = 3;
            break;
        case CubeTlm_AdcsPowerMeasure:
            len = 36;
            break;
        case CubeTlm_AdcsState:
            len = 54;
            break;
        case CubeTlm_AdcsSystemConfig:
            len = 172;
            break;
        case CubeTlm_AdcsTemperatures1:
            len = 6;
            break;
        case CubeTlm_AdcsTemperatures2:
            len = 6;
            break;
        case CubeTlm_Asgp4KepTle:
            len = 33;
            break;
        case CubeTlm_ASgp4Params:
            len = 30;
            break;
        case CubeTlm_BlockChecksum:
            len = 2;
            break;
        case CubeTlm_BootIndexStatus:
            len = 2;
            break;
        case CubeTlm_Cache:
            len = 1;
            break;
        case CubeTlm_CmdAttitude:
            len = 6;
            break;
        case CubeTlm_CmdTrackingTarget:
            len = 12;
            break;
        case CubeTlm_CoarseSunVec:
            len = 6;
            break;
        case CubeTlm_CommsStatus:
            len = 6;
            break;
        case CubeTlm_ConversionProgress:
            len = 3;
            break;
        case CubeTlm_CssConfig:
            len = 21;
            break;
        case CubeTlm_CubeACPState:
            len = 1;
            break;
        case CubeTlm_CubeControlCurrents:
            len = 6;
            break;
        case CubeTlm_CubeSenseConfig:
            len = 74;
            break;
        case CubeTlm_CubeSenseCurrents:
            len = 6;
            break;
        case CubeTlm_CurrentAdcsState:
            len = 6;
            break;
        case CubeTlm_CurrentAdcsState2:
            len = 6;
            break;
        case CubeTlm_DetumbleParams:
            len = 14;
            break;
        case CubeTlm_DownloadBlockReady:
            len = 5;
            break;
        case CubeTlm_EdacErrors:
            len = 6;
            break;
        case CubeTlm_EstimAttitude:
            len = 6;
            break;
        case CubeTlm_EstimData:
            len = 42;
            break;
        case CubeTlm_EstimGyroBias:
            len = 6;
            break;
        case CubeTlm_EstimParams:
            len = 31;
            break;
        case CubeTlm_EstimQuaternion:
            len = 6;
            break;
        case CubeTlm_EstimRates:
            len = 6;
            break;
        case CubeTlm_ExtendedIdentification:
            len = 6;
            break;
        case CubeTlm_FileDownload:
            len = 22;
            break;
        case CubeTlm_FileInfo:
            len = 12;
            break;
        case CubeTlm_FineEstimRates:
            len = 6;
            break;
        case CubeTlm_FineSunVec:
            len = 6;
            break;
        case CubeTlm_GyroConfig:
            len = 10;
            break;
        case CubeTlm_HoleMap1:
            len = 16;
            break;
        case CubeTlm_HoleMap2:
            len = 16;
            break;
        case CubeTlm_HoleMap3:
            len = 16;
            break;
        case CubeTlm_HoleMap4:
            len = 16;
            break;
        case CubeTlm_HoleMap5:
            len = 16;
            break;
        case CubeTlm_HoleMap6:
            len = 16;
            break;
        case CubeTlm_HoleMap7:
            len = 16;
            break;
        case CubeTlm_HoleMap8:
            len = 16;
            break;
        case CubeTlm_Identification:
            len = 8;
            break;
        case CubeTlm_ImageSaveStatus:
            len = 2;
            break;
        case CubeTlm_InitializeUploadComplete:
            len = 1;
            break;
        case CubeTlm_InnovationVec:
            len = 6;
            break;
        case CubeTlm_LastLogEvent:
            len = 6;
            break;
        case CubeTlm_LatchupErrors:
            len = 6;
            break;
        case CubeTlm_LogSdConfig1:
            len = 13;
            break;
        case CubeTlm_LogSdConfig2:
            len = 13;
            break;
        case CubeTlm_LogUartConfig:
            len = 12;
            break;
        case CubeTlm_MagConfig:
            len = 30;
            break;
        case CubeTlm_MagField:
            len = 6;
            break;
        case CubeTlm_MagRaw2:
            len = 6;
            break;
        case CubeTlm_MagtorqConfig:
            len = 3;
            break;
        case CubeTlm_MagTorquerCmd:
            len = 6;
            break;
        case CubeTlm_ModelMagVec:
            len = 6;
            break;
        case CubeTlm_ModelSunVec:
            len = 6;
            break;
        case CubeTlm_MoI:
            len = 24;
            break;
        case CubeTlm_NadirVec:
            len = 6;
            break;
        case CubeTlm_OrbitParameters:
            len = 64;
            break;
        case CubeTlm_PositionECEF:
            len = 6;
            break;
        case CubeTlm_PositionECI:
            len = 6;
            break;
        case CubeTlm_PositionLLH:
            len = 6;
            break;
        case CubeTlm_QuaternionCovar:
            len = 6;
            break;
        case CubeTlm_QuaternionErrVec:
            len = 6;
            break;
        case CubeTlm_RateCovar:
            len = 6;
            break;
        case CubeTlm_RateSensor:
            len = 6;
            break;
        case CubeTlm_RawCam1Sens:
            len = 6;
            break;
        case CubeTlm_RawCam2Sens:
            len = 6;
            break;
        case CubeTlm_RawCss1:
            len = 6;
            break;
        case CubeTlm_RawCss2:
            len = 6;
            break;
        case CubeTlm_RawGPS:
            len = 36;
            break;
        case CubeTlm_RawGpsStatus:
            len = 6;
            break;
        case CubeTlm_RawGpsTime:
            len = 6;
            break;
        case CubeTlm_RawGpsX:
            len = 6;
            break;
        case CubeTlm_RawGpsY:
            len = 6;
            break;
        case CubeTlm_RawGpsZ:
            len = 6;
            break;
        case CubeTlm_RawMagnetometer:
            len = 6;
            break;
        case CubeTlm_RawRate:
            len = 6;
            break;
        case CubeTlm_RawSensor:
            len = 34;
            break;
        case CubeTlm_RawStarTracker:
            len = 54;
            break;
        case CubeTlm_RedMagConfig:
            len = 30;
            break;
        case CubeTlm_RWheelParams:
            len = 13;
            break;
        case CubeTlm_SpeedCmd:
            len = 6;
            break;
        case CubeTlm_SramScrubSettings:
            len = 2;
            break;
        case CubeTlm_Star1Body:
            len = 6;
            break;
        case CubeTlm_Star1Orbit:
            len = 6;
            break;
        case CubeTlm_Star1Raw:
            len = 6;
            break;
        case CubeTlm_Star2Body:
            len = 6;
            break;
        case CubeTlm_Star2Orbit:
            len = 6;
            break;
        case CubeTlm_Star2Raw:
            len = 6;
            break;
        case CubeTlm_Star3Body:
            len = 6;
            break;
        case CubeTlm_Star3Orbit:
            len = 6;
            break;
        case CubeTlm_Star3Raw:
            len = 6;
            break;
        case CubeTlm_StarConfig:
            len = 51;
            break;
        case CubeTlm_StarEstimQ:
            len = 6;
            break;
        case CubeTlm_StarEstimRate:
            len = 6;
            break;
        case CubeTlm_StarMagnitude:
            len = 6;
            break;
        case CubeTlm_StarPerformance1:
            len = 6;
            break;
        case CubeTlm_StarPerformance2:
            len = 6;
            break;
        case CubeTlm_StarTiming:
            len = 6;
            break;
        case CubeTlm_TelecommandAcknowledge:
            len = 4;
            break;
        case CubeTlm_TrackingParams:
            len = 13;
            break;
        case CubeTlm_UnixTime:
            len = 6;
            break;
        case CubeTlm_UnixTimeSave:
            len = 2;
            break;
        case CubeTlm_UploadBlockComplete:
            len = 1;
            break;
        case CubeTlm_UserConEstParams:
            len = 96;
            break;
        case CubeTlm_VelocityECI:
            len = 6;
            break;
        case CubeTlm_WheelConfig:
            len = 4;
            break;
        case CubeTlm_WheelCurrents:
            len = 6;
            break;
        case CubeTlm_WheelSpeed:
            len = 6;
            break;
        case CubeTlm_YWheelParams:
            len = 20;
            break;
        default:
            len = CUBE_INVALID_MSG_ID;
            break;
    }

    return len;
}

/**
 * @brief Look up the payload size of the specified TCM frame
 * List of TCM frames can be found in Section 6 of CubeADCS Firmware Reference Manual:
 * https://drive.google.com/file/d/1bo5rNEvvLPYMZYvYsosqxEjmwzKBp-o5/view?usp=sharing
 *
 * @param tcm_id[in]: TCM frame ID
 *
 * @return TCM frame size if successful, CUBE_INVALID_MSG_ID otherwise
 */
uint16_t lookup_tcm_len(uint8_t tcm_id) {
    uint16_t len;

    switch (tcm_id) {
        // Common telecommands
        case CubeTcm_Reset:
            len = 1;
            break;
        case CubeTcm_ResetLogPtr:
            len = 0;
            break;
        case CubeTcm_AdvLogPtr:
            len = 0;
            break;
        case CubeTcm_ResetBootReg:
            len = 0;
            break;
        case CubeTcm_EraseFile:
            len = 3;
            break;
        case CubeTcm_LogFileDownBlk:
            len = 8;
            break;
        case CubeTcm_AdvFileListPtr:
            len = 0;
            break;
        case CubeTcm_InitFileUpload:
            len = 2;
            break;
        case CubeTcm_FileUploadPkt:
            len = 22;
            break;
        case CubeTcm_FinalizeUploadBlk:
            len = 7;
            break;
        case CubeTcm_ResetUploadBlk:
            len = 0;
            break;
        case CubeTcm_ResetFileListPtr:
            len = 0;
            break;
        case CubeTcm_InitDownBurst:
            len = 2;
            break;

        // Common configuration setters
        case CubeTcm_SetCache:
            len = 1;
            break;
        case CubeTcm_SetSramScrubSettings:
            len = 2;
            break;
        case CubeTcm_SetUnixTime:
            len = 6;
            break;
        case CubeTcm_SetUnixTimeSave:
            len = 2;
            break;
        case CubeTcm_SetHoleMap1:
            len = 16;
            break;
        case CubeTcm_SetHoleMap2:
            len = 16;
            break;
        case CubeTcm_SetHoleMap3:
            len = 16;
            break;
        case CubeTcm_SetHoleMap4:
            len = 16;
            break;
        case CubeTcm_SetHoleMap5:
            len = 16;
            break;
        case CubeTcm_SetHoleMap6:
            len = 16;
            break;
        case CubeTcm_SetHoleMap7:
            len = 16;
            break;
        case CubeTcm_SetHoleMap8:
            len = 16;
            break;

        // CubeACP commands
        case CubeTcm_DeployMag:
            len = 1;
            break;
        case CubeTcm_RunMode:
            len = 1;
            break;
        case CubeTcm_ClrErr:
            len = 1;
            break;
        case CubeTcm_SetControlMode:
            len = 4;
            break;
        case CubeTcm_SetEstimationMode:
            len = 1;
            break;
        case CubeTcm_SetMagOutput:
            len = 6;
            break;
        case CubeTcm_SetWheelSpd:
            len = 6;
            break;
        case CubeTcm_TriggerLoop:
            len = 0;
            break;
        case CubeTcm_TriggerLoopSim:
            len = 127;
            break;
        case CubeTcm_AGSP4RunMode:
            len = 1;
            break;
        case CubeTcm_AGSP4Trigger:
            len = 0;
            break;
        case CubeTcm_SetSPG4Incl:
            len = 8;
            break;
        case CubeTcm_SetSPG4Ecc:
            len = 8;
            break;
        case CubeTcm_SetSPG4InclRAAN:
            len = 8;
            break;
        case CubeTcm_SetSPG4InclArgPer:
            len = 8;
            break;
        case CubeTcm_SetSPG4BStarDrag:
            len = 8;
            break;
        case CubeTcm_SetSPG4MeanMot:
            len = 8;
            break;
        case CubeTcm_SetSPG4MeanAnom:
            len = 8;
            break;
        case CubeTcm_SetSPG4OrbitEpoch:
            len = 8;
            break;
        case CubeTcm_SetMagOperation:
            len = 1;
            break;
        case CubeTcm_ConvertToJpg:
            len = 3;
            break;
        case CubeTcm_SaveConfig:
            len = 0;
            break;
        case CubeTcm_SaveOrbitParam:
            len = 0;
            break;
        case CubeTcm_SaveImage:
            len = 2;
            break;

        // CubeACP configuration setters
        case CubeTcm_SetMagtorqConfig:
            len = 3;
            break;
        case CubeTcm_SetWheelConfig:
            len = 4;
            break;
        case CubeTcm_SetGyroConfig:
            len = 10;
            break;
        case CubeTcm_SetCssConfig:
            len = 21;
            break;
        case CubeTcm_SetAdcsPower:
            len = 3;
            break;
        case CubeTcm_SetCmdAttitude:
            len = 6;
            break;
        case CubeTcm_SetCmdTrackingTarget:
            len = 12;
            break;
        case CubeTcm_SetStarConfig:
            len = 51;
            break;
        case CubeTcm_SetCubeSenseConfig:
            len = 74;
            break;
        case CubeTcm_SetMagConfig:
            len = 30;
            break;
        case CubeTcm_SetRedMagConfig:
            len = 30;
            break;
        case CubeTcm_SetAdcsConfig:
            len = 464;
            break;
        case CubeTcm_SetOrbitParameters:
            len = 64;
            break;
        case CubeTcm_SetDetumbleParams:
            len = 14;
            break;
        case CubeTcm_SetYWheelParams:
            len = 20;
            break;
        case CubeTcm_SetRWheelParams:
            len = 13;
            break;
        case CubeTcm_SetTrackingParams:
            len = 13;
            break;
        case CubeTcm_SetMoI:
            len = 24;
            break;
        case CubeTcm_SetEstimParams:
            len = 31;
            break;
        case CubeTcm_SetAdcsSystemConfig:
            len = 172;
            break;
        case CubeTcm_SetUserConEstParams:
            len = 96;
            break;
        case CubeTcm_SetASgp4Params:
            len = 30;
            break;
        case CubeTcm_SetLogSdConfig1:
            len = 13;
            break;
        case CubeTcm_SetLogSdConfig2:
            len = 13;
            break;
        case CubeTcm_SetLogUartConfig:
            len = 12;
            break;
        default:
            len = CUBE_INVALID_MSG_ID;
            break;
    }

    return len;
}

/**
 * @brief Check if the specified bits within a field are set
 *
 * @param bit_field[in]: Bit field to assess
 * @param bit_mask[in]: Bit mask that will be checked (0xff - check all bits)
 *
 * @return TRUE if all bits within the mask are set, FALSE otherwise
 */
static inline bool bits_set(uint8_t bit_field, uint8_t bit_mask) {
    return (bit_field & bit_mask) == bit_mask;
}

/**
 * @brief Deserialize the TCM acknowledgement frame
 *
 * @param buff[in]: Buffer containing serialized frame
 * @param cube_tcm_ack[out]: Parsed frame data
 *
 * @return CUBE_LIB_OK if data successfully parsed, error code otherwise
 */
cube_lib_result_t cube_parse_tcm_ack(const uint8_t* buff, cube_tcm_ack_t* obj) {
    if ((obj == NULL) || (buff == NULL)) {
        return CUBE_NULL_PTR;
    }

    obj->last_tcm_id = buff[0];

    obj->tcm_processed = bits_set(buff[1], 0x01);

    obj->tcm_err = (cube_tcm_err_t)buff[2];

    obj->tcm_err_idx = buff[3];

    return CUBE_LIB_OK;
}

/**
 * @brief Deserialize the CubeComputer comms status frame
 *
 * @param buff[in]: Buffer containing serialized frame
 * @param cube_comms_status_t[out]: Parsed frame data
 *
 * @return CUBE_LIB_OK if data successfully parsed, error code otherwise
 */
cube_lib_result_t cube_parse_comms_status(const uint8_t* buff, cube_comms_status_t* obj) {
    if ((obj == NULL) || (buff == NULL)) {
        return CUBE_NULL_PTR;
    }

    obj->tcm_count = *((const uint16_t*)(buff + 0));

    obj->tlm_count = *((const uint16_t*)(buff + 2));

    obj->uart_buff_overrun = bits_set(buff[4], 0x01);

    obj->uart_protocol_err = bits_set(buff[4], 0x02);

    obj->uart_msg_incomplete = bits_set(buff[4], 0x04);

    obj->i2c_tlm_err = bits_set(buff[4], 0x08);

    obj->i2c_buff_err = bits_set(buff[4], 0x10);

    obj->can_buff_err = bits_set(buff[4], 0x20);

    return CUBE_LIB_OK;
}

/**
 * @brief Serialize the TCM acknowledgement frame
 *
 * @param cube_comms_status_t[in]: Telemetry frame data
 * @param buff[out]: Buffer containing serialized frame
 *
 * @return CUBE_LIB_OK if frame successfully serialized, error code otherwise
 */
cube_lib_result_t cube_serialize_tcm_ack(const cube_tcm_ack_t* obj, uint8_t* buff) {
    if ((obj == NULL) || (buff == NULL)) {
        return CUBE_NULL_PTR;
    }

    buff[0] = obj->last_tcm_id;

    buff[1] = obj->tcm_processed == true ? 1 : 0;

    buff[2] = obj->tcm_err;

    buff[3] = obj->tcm_err_idx;

    return CUBE_LIB_OK;
}

/**
 * @brief Serialize the CubeComputer comms status frame
 *
 * @param cube_comms_status_t[in]: Telemetry frame data
 * @param buff[out]: Buffer containing serialized frame
 *
 * @return CUBE_LIB_OK if frame successfully serialized, error code otherwise
 */
cube_lib_result_t cube_serialize_comms_status(const cube_comms_status_t* obj, uint8_t* buff) {
    if ((obj == NULL) || (buff == NULL)) {
        return CUBE_NULL_PTR;
    }

    uint16_t* short_ptr = (uint16_t*)buff;

    short_ptr[0] = obj->tcm_count;
    short_ptr[1] = obj->tlm_count;

    buff[4] = 0;
    buff[4] |= obj->uart_buff_overrun == true ? 0x01 : 0;
    buff[4] |= obj->uart_protocol_err == true ? 0x02 : 0;
    buff[4] |= obj->uart_msg_incomplete == true ? 0x04 : 0;
    buff[4] |= obj->i2c_tlm_err == true ? 0x08 : 0;
    buff[4] |= obj->i2c_buff_err == true ? 0x10 : 0;
    buff[4] |= obj->can_buff_err == true ? 0x20 : 0;

    return CUBE_LIB_OK;
}
