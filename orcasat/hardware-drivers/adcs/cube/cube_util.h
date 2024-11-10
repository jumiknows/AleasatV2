/**
 * @file cube_util.h
 * @brief CubeADCS driver support library
 *        Adapted from CubeSpace Software Support repository:
 *        https://bitbucket.org/cubespace/cubespace-software-support/
 * @date Oct 28, 2020
 * @version 0.1.2
 */

#ifndef CUBE_UTIL_H
#define CUBE_UTIL_H

#include <stdint.h>
#include <stdbool.h>

/// Maximum size of CubeComputer telemetry frames & telecommand parameter sets (bytes)
#define CUBE_COMPUTER_MAX_PAYLOAD_SIZE 464

/**
 * @brief CubeTlm Telemetry enumeration.
 * List of TLM frames can be found in Section 6 of CubeADCS Firmware Reference Manual:
 * https://drive.google.com/file/d/1bo5rNEvvLPYMZYvYsosqxEjmwzKBp-o5/view?usp=sharing
 */
typedef enum {
    CubeTlm_AcpLoopState             = 220, // Returns information about the ACP loop
    CubeTlm_ActuatorCmd              = 192, // Actuator commands
    CubeTlm_AdcsConfig               = 206, // Current configuration
    CubeTlm_AdcsExecTimes            = 196, // Returns information about execution times of ACP functions
    CubeTlm_AdcsMeasure              = 191, // Calibrated sensor measurements
    CubeTlm_AdcsMiscCurrents         = 198, // CubeStar and Torquer current and temperature measurements
    CubeTlm_AdcsPower                = 197, // Control power to selected components
    CubeTlm_AdcsPowerMeasure         = 195, // Power and temperature measurements
    CubeTlm_AdcsState                = 190, // Current ADCS state
    CubeTlm_AdcsSystemConfig         = 225, // Current hard-coded system configuration
    CubeTlm_AdcsTemperatures1        = 174, // Magnetometer + MCU temperature measurements
    CubeTlm_AdcsTemperatures2        = 175, // Rate sensor temperatures
    CubeTlm_Asgp4KepTle              = 228, // ASGP4 TLEs generated
    CubeTlm_ASgp4Params              = 227, // Settings for GPS augmented SGP4
    CubeTlm_BlockChecksum            = 246, // File upload Block CRC16 Checksum
    CubeTlm_BootIndexStatus          = 130, // Current selected boot index and status of last boot
    CubeTlm_Cache                    = 131, // Cache enabled state
    CubeTlm_CmdAttitude              = 199, // Commanded attitude angles
    CubeTlm_CmdTrackingTarget        = 200, // Target reference for tracking control mode
    CubeTlm_CoarseSunVec             = 152, // Measured coarse sun vector
    CubeTlm_CommsStatus              = 144, // Communication status - includes command and telemetry counters and error flags
    CubeTlm_ConversionProgress       = 133, // Conversion progress
    CubeTlm_CssConfig                = 139, // Photodiode pointing directions and scale factors
    CubeTlm_CubeACPState             = 135, // Contains flags regarding the state of the ACP
    CubeTlm_CubeControlCurrents      = 172, // CubeControl current measurements
    CubeTlm_CubeSenseConfig          = 203, // CubeSense configuration parameters
    CubeTlm_CubeSenseCurrents        = 171, // CubeSense current measurements
    CubeTlm_CurrentAdcsState         = 132, // Current state of the Attitude Control Processor - frame 1
    CubeTlm_CurrentAdcsState2        = 224, // Current state of the Attitude Control Processor - frame 2
    CubeTlm_DetumbleParams           = 208, // Set controller gains and reference values for Detumbling control mode
    CubeTlm_DownloadBlockReady       = 242, // Status about download block preparation
    CubeTlm_EdacErrors               = 143, // EDAC Error Counters
    CubeTlm_EstimAttitude            = 146, // Estimated attitude angles
    CubeTlm_EstimData                = 193, // Estimation meta-data
    CubeTlm_EstimGyroBias            = 161, // Estimated rate sensor bias
    CubeTlm_EstimParams              = 223, // Estimation noise covariance and sensor mask
    CubeTlm_EstimQuaternion          = 218, // Estimated quaternion set
    CubeTlm_EstimRates               = 147, // Estimated angular rates relative to orbit reference frame
    CubeTlm_ExtendedIdentification   = 129, // Boot And Running Program Status
    CubeTlm_FileDownload             = 241, // File Download buffer 20-byte packet
    CubeTlm_FileInfo                 = 243, // File Information
    CubeTlm_FineEstimRates           = 201, // High resolution estimated angular rates relative to orbit reference frame
    CubeTlm_FineSunVec               = 153, // Measured fine sun vector
    CubeTlm_GyroConfig               = 138, // Set rate gyro configuration parameters
    CubeTlm_HoleMap1                 = 247, // File Upload Hole Map 1
    CubeTlm_HoleMap2                 = 248, // File Upload Hole Map 2
    CubeTlm_HoleMap3                 = 249, // File Upload Hole Map 3
    CubeTlm_HoleMap4                 = 250, // File Upload Hole Map 4
    CubeTlm_HoleMap5                 = 251, // File Upload Hole Map 5
    CubeTlm_HoleMap6                 = 252, // File Upload Hole Map 6
    CubeTlm_HoleMap7                 = 253, // File Upload Hole Map 7
    CubeTlm_HoleMap8                 = 254, // File Upload Hole Map 8
    CubeTlm_Identification           = 128, // Identification information for this node
    CubeTlm_ImageSaveStatus          = 233, // Status of Image Capture and Save Operation
    CubeTlm_InitializeUploadComplete = 244, // Initialize Upload Complete
    CubeTlm_InnovationVec            = 162, // Estimation innovation vector
    CubeTlm_LastLogEvent             = 141, // Last Logged Event (relative to pointer - adjusted via Advance and Reset TCs (3 & 4)
    CubeTlm_LatchupErrors            = 142, // SRAM Latchup counters
    CubeTlm_LogSdConfig1             = 235, // Log selection and period for LOG1
    CubeTlm_LogSdConfig2             = 236, // Log selection and period for LOG2
    CubeTlm_LogUartConfig            = 237, // Log selection and period for UART (unsolicited TLM)
    CubeTlm_MagConfig                = 204, // Magnetometer configuration parameters
    CubeTlm_MagField                 = 151, // Measured magnetic field vector
    CubeTlm_MagRaw2                  = 215, // Secondary Magnetometer raw measurements
    CubeTlm_MagtorqConfig            = 136, // Set magnetorquer configuration parameters
    CubeTlm_MagTorquerCmd            = 157, // Magnetorquer commands
    CubeTlm_ModelMagVec              = 159, // IGRF modelled magnetic field vector (orbit frame referenced)
    CubeTlm_ModelSunVec              = 160, // Modelled sun vector (orbit frame referenced)
    CubeTlm_MoI                      = 222, // Satellite moment of inertia matrix
    CubeTlm_NadirVec                 = 154, // Measured nadir vector
    CubeTlm_OrbitParameters          = 207, // SGP4 Orbit Parameters
    CubeTlm_PositionECEF             = 219, // Satellite position in ECEF coordinates
    CubeTlm_PositionECI              = 148, // Satellite position in ECI frame
    CubeTlm_PositionLLH              = 150, // Satellite position in WGS-84 coordinate frame
    CubeTlm_QuaternionCovar          = 164, // Quaternion covariance
    CubeTlm_QuaternionErrVec         = 163, // Quaternion error vector
    CubeTlm_RateCovar                = 165, // Angular rate covariance
    CubeTlm_RateSensor               = 155, // Rate sensor measurements
    CubeTlm_RawCam1Sens              = 167, // Cam1 sensor capture and detection result
    CubeTlm_RawCam2Sens              = 166, // Cam2 sensor capture and detection result
    CubeTlm_RawCss1                  = 168, // Raw CSS measurements 1 to 6
    CubeTlm_RawCss2                  = 169, // Raw CSS measurements 7 to 10
    CubeTlm_RawGPS                   = 210, // Raw GPS measurements
    CubeTlm_RawGpsStatus             = 176, // Raw GPS status
    CubeTlm_RawGpsTime               = 177, // Raw GPS time
    CubeTlm_RawGpsX                  = 178, // Raw GPS X position and velocity (ECI referenced)
    CubeTlm_RawGpsY                  = 179, // Raw GPS Y position and velocity (ECI referenced)
    CubeTlm_RawGpsZ                  = 180, // Raw GPS Z position and velocity (ECI referenced)
    CubeTlm_RawMagnetometer          = 170, // Raw magnetometer measurements
    CubeTlm_RawRate                  = 216, // Raw rate sensor measurements
    CubeTlm_RawSensor                = 194, // Raw sensor measurements
    CubeTlm_RawStarTracker           = 211, // Raw Star Tracker Measurement
    CubeTlm_RedMagConfig             = 205, // Redundant magnetometer configuration parameters
    CubeTlm_RWheelParams             = 217, // Set controller gains and reference value for reaction wheel control mode
    CubeTlm_SpeedCmd                 = 158, // Wheel speed commands
    CubeTlm_SramScrubSettings        = 134, // SRAM scrubbing size
    CubeTlm_Star1Body                = 181, // Star 1 Body Vector
    CubeTlm_Star1Orbit               = 184, // Star 1 Orbit Vector
    CubeTlm_Star1Raw                 = 212, // Catalogue index and detected coordinates for star 1
    CubeTlm_Star2Body                = 182, // Star 2 Body Vector
    CubeTlm_Star2Orbit               = 185, // Star 2 Orbit Vector
    CubeTlm_Star2Raw                 = 213, // Catalogue index and detected coordinates for star 2
    CubeTlm_Star3Body                = 183, // Star 3 Body Vector
    CubeTlm_Star3Orbit               = 186, // Star 3 Orbit Vector
    CubeTlm_Star3Raw                 = 214, // Catalogue index and detected coordinates for star 3
    CubeTlm_StarConfig               = 202, // Set configurations of CubeStar
    CubeTlm_StarEstimQ               = 230, // Attitude quaternion estimated by CubeStar
    CubeTlm_StarEstimRate            = 229, // Angular rates estimated by CubeStar
    CubeTlm_StarMagnitude            = 187, // Instrument magnitude of identified stars
    CubeTlm_StarPerformance1         = 188, // Performance parameters of star measurement
    CubeTlm_StarPerformance2         = 231, // Performance parameters of star measurement
    CubeTlm_StarTiming               = 189, // Timing information of star measurement
    CubeTlm_TelecommandAcknowledge   = 240, // Telemetry frame with acknowledge status of the previously sent command
    CubeTlm_TrackingParams           = 221, // Set controller gains for tracking control mode
    CubeTlm_UnixTime                 = 140, // Current Unix Time
    CubeTlm_UnixTimeSave             = 145, // Configuration settings for unixtime flash memory persistence
    CubeTlm_UploadBlockComplete      = 245, // Finalize Upload Block Complete
    CubeTlm_UserConEstParams         = 226, // Settings for user-coded estimation and control modes
    CubeTlm_VelocityECI              = 149, // Satellite velocity in ECI frame
    CubeTlm_WheelConfig              = 137, // Set  wheel configuration parameters
    CubeTlm_WheelCurrents            = 173, // XYZ Wheel current measurement
    CubeTlm_WheelSpeed               = 156, // Wheel speed measurement
    CubeTlm_YWheelParams             = 209, // Set controller gains and reference value for Y-wheel control mode
} cube_tlm_t;

/// CubeACP Telecommand enumeration
typedef enum {
    // Common telecommands
    CubeTcm_Reset             = 1,   // Perform reset
    CubeTcm_ResetLogPtr       = 4,   // Reset log pointer
    CubeTcm_AdvLogPtr         = 5,   // Advance log pointer
    CubeTcm_ResetBootReg      = 6,   // Reset boot registers
    CubeTcm_EraseFile         = 108, // Erase file
    CubeTcm_LogFileDownBlk    = 112, // Load file download block
    CubeTcm_AdvFileListPtr    = 113, // Advance file list read pointer
    CubeTcm_InitFileUpload    = 114, // Initiate file upload
    CubeTcm_FileUploadPkt     = 115, // File upload packet
    CubeTcm_FinalizeUploadBlk = 116, // Finalize upload block
    CubeTcm_ResetUploadBlk    = 117, // Reset upload block
    CubeTcm_ResetFileListPtr  = 118, // Reset file list read pointer
    CubeTcm_InitDownBurst     = 119, // Initiate download burst

    // Common configuration setters
    CubeTcm_SetCache             = 3,   // Cache enabled state
    CubeTcm_SetSramScrubSettings = 8,   // SRAM scrubbing size
    CubeTcm_SetUnixTime          = 2,   // Current Unix Time
    CubeTcm_SetUnixTimeSave      = 9,   // Configuration settings for unixtime flash memory persistence
    CubeTcm_SetHoleMap1          = 120, // File Upload Hole Map 1
    CubeTcm_SetHoleMap2          = 121, // File Upload Hole Map 2
    CubeTcm_SetHoleMap3          = 122, // File Upload Hole Map 3
    CubeTcm_SetHoleMap4          = 123, // File Upload Hole Map 4
    CubeTcm_SetHoleMap5          = 124, // File Upload Hole Map 5
    CubeTcm_SetHoleMap6          = 125, // File Upload Hole Map 6
    CubeTcm_SetHoleMap7          = 126, // File Upload Hole Map 7
    CubeTcm_SetHoleMap8          = 127, // File Upload Hole Map 8

    // CubeACP commands
    CubeTcm_DeployMag         = 7,  // Deploy magenetometer boom
    CubeTcm_RunMode           = 10, // Set ADCS run mode
    CubeTcm_ClrErr            = 12, // Clear errors
    CubeTcm_SetControlMode    = 13, // Set attitude control mode
    CubeTcm_SetEstimationMode = 14, // Set attitude estimation mode
    CubeTcm_SetMagOutput      = 16, // Set magnetorquer output
    CubeTcm_SetWheelSpd       = 17, // Set wheel speed
    CubeTcm_TriggerLoop       = 18, // Trigger ADCS loop
    CubeTcm_TriggerLoopSim    = 19, // Trigger ADCS loop with simulated sensor data
    CubeTcm_AGSP4RunMode      = 31, // ASGP4 run mode
    CubeTcm_AGSP4Trigger      = 32, // ASGP4 trigger
    CubeTcm_SetSPG4Incl       = 46, // Set SGP4 orbit inclination
    CubeTcm_SetSPG4Ecc        = 47, // Set SGP4 orbit eccentricity
    CubeTcm_SetSPG4InclRAAN   = 48, // Set SGP4 orbit RAAN
    CubeTcm_SetSPG4InclArgPer = 49, // Set SGP4 orbit argument of perigee
    CubeTcm_SetSPG4BStarDrag  = 50, // Set SGP4 orbit B-star drag term
    CubeTcm_SetSPG4MeanMot    = 51, // Set SGP4 orbit mean motion
    CubeTcm_SetSPG4MeanAnom   = 52, // Set SGP4 orbit mean anomaly
    CubeTcm_SetSPG4OrbitEpoch = 53, // Set SGP4 orbit epoch
    CubeTcm_SetMagOperation   = 56, // Set mode of magnetometer operation
    CubeTcm_ConvertToJpg      = 57, // Convert to JPG file
    CubeTcm_SaveConfig        = 63, // Save configuration
    CubeTcm_SaveOrbitParam    = 64, // Save orbit parameters
    CubeTcm_SaveImage         = 80, // Save image

    // CubeACP configuration setters
    CubeTcm_SetMagtorqConfig     = 21,  // Set magnetorquer configuration parameters
    CubeTcm_SetWheelConfig       = 22,  // Set  wheel configuration parameters
    CubeTcm_SetGyroConfig        = 23,  // Set rate gyro configuration parameters
    CubeTcm_SetCssConfig         = 24,  // Photodiode pointing directions and scale factors
    CubeTcm_SetAdcsPower         = 11,  // Control power to selected components
    CubeTcm_SetCmdAttitude       = 15,  // Commanded attitude angles
    CubeTcm_SetCmdTrackingTarget = 55,  // Target reference for tracking control mode
    CubeTcm_SetStarConfig        = 37,  // Set configurations of CubeStar
    CubeTcm_SetCubeSenseConfig   = 25,  // CubeSense configuration parameters
    CubeTcm_SetMagConfig         = 26,  // Magnetometer configuration parameters
    CubeTcm_SetRedMagConfig      = 36,  // Redundant magnetometer configuration parameters
    CubeTcm_SetAdcsConfig        = 20,  // Current ADCS configuration
    CubeTcm_SetOrbitParameters   = 45,  // SGP4 Orbit Parameters
    CubeTcm_SetDetumbleParams    = 38,  // Set controller gains and reference values for Detumbling control mode
    CubeTcm_SetYWheelParams      = 39,  // Set controller gains and reference value for Y-wheel control mode
    CubeTcm_SetRWheelParams      = 40,  // Set controller gains and reference value for reaction wheel control mode
    CubeTcm_SetTrackingParams    = 54,  // Set controller gains for tracking control mode
    CubeTcm_SetMoI               = 41,  // Satellite moment of inertia matrix
    CubeTcm_SetEstimParams       = 27,  // Estimation noise covariance and sensor mask
    CubeTcm_SetAdcsSystemConfig  = 30,  // Current hard-coded system configuration
    CubeTcm_SetUserConEstParams  = 29,  // Settings for user-coded estimation and control modes
    CubeTcm_SetASgp4Params       = 28,  // Settings for GPS augmented SGP4
    CubeTcm_SetLogSdConfig1      = 104, // Log selection and period for LOG1
    CubeTcm_SetLogSdConfig2      = 105, // Log selection and period for LOG2
    CubeTcm_SetLogUartConfig     = 106, // Log selection and period for UART (unsolicited TLM)

} cube_tcm_t;

/// Telecommand Acknowledge error definitions
typedef enum {
    TcErrorReason_NoError         = 0, // No error
    TcErrorReason_InvalidTc       = 1, // Invalid telecommand ID
    TcErrorReason_IncorrectLen    = 2, // Incorrect TC parameter length
    TcErrorReason_IncorrectParams = 3, // Incorrect TC parameter value
    TcErrorReason_CrcError        = 4, // CRC check failed
} cube_tcm_err_t;

/// Telecommand Acknowledge message structure. Telemetry frame with acknowledge status of the previously sent command
typedef struct {
    uint8_t last_tcm_id;    // ID of last received TC
    bool tcm_processed;     // Flag to indicate if the last TC has been processed.
    cube_tcm_err_t tcm_err; // Status of last processed telecommand
    uint8_t tcm_err_idx;    // Index of incorrect TC parameter
} cube_tcm_ack_t;

/// CubeComputer communication status - includes command and telemetry counters and error flags
typedef struct {
    uint16_t tcm_count;       // No. of telecommands received
    uint16_t tlm_count;       // No. of telemetry requests received
    bool uart_buff_overrun;   // TC buffer was overrun while receiving a telecommand
    bool uart_protocol_err;   // UART protocol error occurred
    bool uart_msg_incomplete; // UART start-of-message identifier was received without a preceding end-of-message
    bool i2c_tlm_err;         // Number of data clocked out was more than telemetry package
    bool i2c_buff_err;        // Telecommand sent exceeds buffer size
    bool can_buff_err;        // Telecommand sent exceeds buffer size
} cube_comms_status_t;

/// CubeADCS library error codes
typedef enum {
    CUBE_LIB_OK         = 0,
    CUBE_NULL_PTR       = 3,     // Preserve error code values from original CubeADCS driver support library
    CUBE_INVALID_MSG_ID = 0xffff // Invalid TCM/TLM message ID
} cube_lib_result_t;

cube_lib_result_t cube_parse_tcm_ack(const uint8_t* buff, cube_tcm_ack_t* obj);
cube_lib_result_t cube_parse_comms_status(const uint8_t* buff, cube_comms_status_t* obj);

cube_lib_result_t cube_serialize_tcm_ack(const cube_tcm_ack_t* obj, uint8_t* buff);
cube_lib_result_t cube_serialize_comms_status(const cube_comms_status_t* obj, uint8_t* buff);

uint16_t lookup_tlm_len(uint8_t tlm_id);
uint16_t lookup_tcm_len(uint8_t tcm_id);

#endif
