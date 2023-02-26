/**
 * @file cam_ov5642_defs.h
 * @brief Definitions for OV5642 image sensor
 */

#ifndef CAM_OV5642_DEFS_H_
#define CAM_OV5642_DEFS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

 // Standard Library
 #include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/***************************** REGISTER ADDRESSES *****************************/

////////////////////////// System and I/O Pad Control //////////////////////////
#define OV5642_REG_SYSTEM_RESET_00           0x3000
#define OV5642_REG_SYSTEM_RESET_01           0x3001
#define OV5642_REG_SYSTEM_RESET_02           0x3002
#define OV5642_REG_SYSTEM_RESET_03           0x3003
#define OV5642_REG_CLOCK_ENABLE_00           0x3004
#define OV5642_REG_CLOCK_ENABLE_01           0x3005
#define OV5642_REG_CLOCK_ENABLE_02           0x3006
#define OV5642_REG_CLOCK_ENABLE_03           0x3007
#define OV5642_REG_SC_SYS_0                  0x3008
#define OV5642_REG_MIPI_PCLK_DIVIDER_CONTROL 0x3009
#define OV5642_REG_CHIP_ID_HIGH_BYTE         0x300A
#define OV5642_REG_CHIP_ID_LOW_BYTE          0x300B
#define OV5642_REG_PLL_CTRL                  0x300C
#define OV5642_REG_ALFD_CTRL                 0x300D
#define OV5642_REG_MIPI_CONTROL_00           0x300E
#define OV5642_REG_PLL_CONTROL_00            0x300F
#define OV5642_REG_PLL_CONTROL_01            0x3010
#define OV5642_REG_PLL_CONTROL_02            0x3011
#define OV5642_REG_PLL_CONTROL_03            0x3012
// System Control Registers                  0x3013 - 0x3015
#define OV5642_REG_PAD_OUTPUT_ENABLE_00      0x3016
#define OV5642_REG_PAD_OUTPUT_ENABLE_01      0x3017
#define OV5642_REG_PAD_OUTPUT_ENABLE_02      0x3018
#define OV5642_REG_PAD_OUTPUT_VALUE_00       0x3019
#define OV5642_REG_PAD_OUTPUT_VALUE_01       0x301A
#define OV5642_REG_PAD_OUTPUT_VALUE_02       0x301B
#define OV5642_REG_PAD_OUTPUT_SELECT_00      0x301C
#define OV5642_REG_PAD_OUTPUT_SELECT_01      0x301D
#define OV5642_REG_PAD_OUTPUT_SELECT_02      0x301E
// System Control Registers                  0x301F - 0x302B
#define OV5642_REG_PAD_CONTROL_00            0x302C
// System Control Registers                  0x302D - 0x302F
#define OV5642_REG_SC_A_PWC_PK_O             0x3030
// System Control Registers                  0x3031 - 0x303F
#define OV5642_REG_PAD_INPUT_VALUE_00        0x3040
#define OV5642_REG_PAD_INPUT_VALUE_01        0x3041
#define OV5642_REG_PAD_INPUT_VALUE_02        0x3042

///////////////////////////////// SCCB Control /////////////////////////////////
#define OV5642_REG_SCCB_ID                   0x3100
// SCCB Control Registers                    0x3101 - 0x3102
#define OV5642_REG_PCLK_CLOCK_SELECT         0x3103
#define OV5642_REG_SCCB_PAD_CLOCK_DIVIDER    0x3104

///////////////////////////////// Group Write //////////////////////////////////
#define OV5642_REG_GROUP_ADDR_0              0x3200
#define OV5642_REG_GROUP_ADDR_1              0x3201
#define OV5642_REG_GROUP_ADDR_2              0x3202
#define OV5642_REG_GROUP_ADDR_3              0x3203
// Group Write Control Registers             0x3204 - 0x3211
#define OV5642_REG_GROUP_ACCESS              0x3212
// Group Write Control Registers             0x3213 - 0x3214

///////////////////////////////// AWB Control //////////////////////////////////
#define OV5642_REG_AWB_R_GAIN_HIGH           0x3400
#define OV5642_REG_AWB_R_GAIN_LOW            0x3401
#define OV5642_REG_AWB_G_GAIN_HIGH           0x3402
#define OV5642_REG_AWB_G_GAIN_LOW            0x3403
#define OV5642_REG_AWB_B_GAIN_HIGH           0x3404
#define OV5642_REG_AWB_B_GAIN_LOW            0x3405
#define OV5642_REG_AWB_MANUAL                0x3406

//////////////////////// AEC / AGC (Power Keep Domain) /////////////////////////
#define OV5642_REG_AEC_PK_LONG_EXPO_0        0x3500
#define OV5642_REG_AEC_PK_LONG_EXPO_1        0x3501
#define OV5642_REG_AEC_PK_LONG_EXPO_2        0x3502
#define OV5642_REG_AEC_PK_MANUAL             0x3503
// Not Used                                  0x3504 - 0x3507
#define OV5642_REG_AEC_PK_LONG_GAIN_0        0x3508
#define OV5642_REG_AEC_PK_LONG_GAIN_1        0x3509
#define OV5642_REG_AEC_PK_AGC_ADJ_0          0x350A
#define OV5642_REG_AEC_PK_AGC_ADJ_1          0x350B
#define OV5642_REG_AEC_PK_VTS_0              0x350C
#define OV5642_REG_AEC_PK_VTS_1              0x350D

//////////////////////////////// Sensor Control ////////////////////////////////
// Analog Control Registers                  0x3600 - 0x3620
#define OV5642_REG_ARRAY_CONTROL_01          0x3621
// Analog Control Registers                  0x3622 - 0x3634
// Analog Control Registers                  0x3700 - 0x370C
#define OV5642_REG_ANALOG_CONTROL_D          0x370D
// Analog Control Registers                  0x370E - 0x3711

//////////////////////////////// Timing Control ////////////////////////////////
#define OV5642_REG_TIMING_HS_0               0x3800
#define OV5642_REG_TIMING_HS_1               0x3801
#define OV5642_REG_TIMING_VS_0               0x3802
#define OV5642_REG_TIMING_VS_1               0x3803
#define OV5642_REG_TIMING_HW_0               0x3804
#define OV5642_REG_TIMING_HW_1               0x3805
#define OV5642_REG_TIMING_VH_0               0x3806
#define OV5642_REG_TIMING_VH_1               0x3807
#define OV5642_REG_TIMING_DVPHO_0            0x3808
#define OV5642_REG_TIMING_DVPHO_1            0x3809
#define OV5642_REG_TIMING_DVPVO_0            0x380A
#define OV5642_REG_TIMING_DVPVO_1            0x380B
#define OV5642_REG_TIMING_HTS_0              0x380C
#define OV5642_REG_TIMING_HTS_1              0x380D
#define OV5642_REG_TIMING_VTS_0              0x380E
#define OV5642_REG_TIMING_VTS_1              0x380F
#define OV5642_REG_TIMING_HVOFFS             0x3810
#define OV5642_REG_TIMING_THNVH              0x3811
#define OV5642_REG_TIMING_THNHW_0            0x3812
#define OV5642_REG_TIMING_THNHW_1            0x3813
#define OV5642_REG_TIMING_CONTROL_18         0x3818

//////////////////////// AEC / AGC (Power Down Domain) /////////////////////////
#define OV5642_REG_AEC_CTRL_00               0x3A00
#define OV5642_REG_AEC_CTRL_01               0x3A01
#define OV5642_REG_AEC_B60_MAX_EXPO_0        0x3A02
#define OV5642_REG_AEC_B60_MAX_EXPO_1        0x3A03
#define OV5642_REG_AEC_B60_MAX_EXPO_2        0x3A04
#define OV5642_REG_AEC_CTRL_05               0x3A05
#define OV5642_REG_AEC_CTRL_06               0x3A06
#define OV5642_REG_AEC_CTRL_07               0x3A07
#define OV5642_REG_AEC_B50_STEP_0            0x3A08
#define OV5642_REG_AEC_B50_STEP_1            0x3A09
#define OV5642_REG_AEC_B60_STEP_0            0x3A0A
#define OV5642_REG_AEC_B60_STEP_1            0x3A0B
// AEC CTRL Register                         0x3A0C
#define OV5642_REG_AEC_B60_MAX               0x3A0D
#define OV5642_REG_AEC_B50_MAX               0x3A0E
#define OV5642_REG_AEC_STABLE_HIGH_LIM_ENT   0x3A0F
#define OV5642_REG_AEC_STABLE_LOW_LIM_ENT    0x3A10
#define OV5642_REG_AEC_FAST_ZONE_HIGH_LIM    0x3A11
#define OV5642_REG_AEC_CTRL_12               0x3A12
#define OV5642_REG_AEC_CTRL_13               0x3A13
#define OV5642_REG_AEC_B50_MAX_EXPO_0        0x3A14
#define OV5642_REG_AEC_B50_MAX_EXPO_1        0x3A15
#define OV5642_REG_AEC_B50_MAX_EXPO_2        0x3A16
#define OV5642_REG_AEC_CTRL_17               0x3A17
#define OV5642_REG_AEC_GAIN_CEILING_0        0x3A18
#define OV5642_REG_AEC_GAIN_CEILING_1        0x3A19
// AEC CTRL Register                         0x3A1A
#define OV5642_REG_AEC_STABLE_HIGH_LIM_EXIT  0x3A1B
#define OV5642_REG_LED_ADD_ROW_0             0x3A1C
#define OV5642_REG_LED_ADD_ROW_1             0x3A1D
#define OV5642_REG_AEC_STABLE_LOW_LIM_EXIT   0x3A1E
#define OV5642_REG_AEC_FAST_ZONE_LOW_LIM     0x3A1F
#define OV5642_REG_AEC_CTRL_20               0x3A20

/////////////////////////////// Light Frequency ////////////////////////////////
#define OV5642_REG_ALFD_CTRL_2               0x3C00
#define OV5642_REG_ALFD_CTRL_1               0x3C01
// ALFD_CTRL                                 0x3C02 - 0x3C0B
#define OV5642_REG_ALFD_CTRL_C               0x3C0C
// ALFD_CTRL                                 0x3C0D - 0x3C1F

///////////////////////////////////// BLC //////////////////////////////////////
#define OV5642_REG_BLC_CTRL_00               0x4000
// BLC Reserved                              0x4001 - 0x4002
#define OV5642_REG_BLC_CTRL_03_0             0x4003
// BLC Reserved                              0x4004 - 0x4005
#define OV5642_REG_LONG_EXP_BL_TARGET_0      0x4006
#define OV5642_REG_LONG_EXP_BL_TARGET_1      0x4007
// Not Used                                  0x4008 - 0x4009
// BLC Reserved                              0x400A - 0x400B
#define OV5642_REG_BLC_CTRL_03_1             0x400C
// BLC Reserved                              0x400D - 0x401C
#define OV5642_REG_BLC_FRAME_CTRL            0x401D
#define OV5642_REG_BLC_FRAME                 0x401E

///////////////////////////////// Compression //////////////////////////////////
#define OV5642_REG_COMPRESSION_CTRL_00       0x4400
#define OV5642_REG_COMPRESSION_CTRL_01       0x4401
#define OV5642_REG_COMPRESSION_CTRL_02       0x4402
#define OV5642_REG_COMPRESSION_CTRL_03       0x4403
#define OV5642_REG_COMPRESSION_CTRL_04       0x4404
#define OV5642_REG_COMPRESSION_CTRL_05       0x4405
#define OV5642_REG_COMPRESSION_CTRL_06       0x4406
#define OV5642_REG_COMPRESSION_CTRL_07       0x4407
#define OV5642_REG_COMPRESSION_ISI_CTRL      0x4408
#define OV5642_REG_COMPRESSION_CTRL_09       0x4409
#define OV5642_REG_COMPRESSION_CTRL_0A       0x440A
// Compression Control                       0x440B - 0x440C
// Not Used                                  0x440D - 0x440F
#define OV5642_REG_COMPRESSION_QT_DATA       0x4410
#define OV5642_REG_COMPRESSION_QT_ADDR       0x4411
// Compression ISI Data                      0x4412 - 0x4413
#define OV5642_REG_COMPRESSION_LENGTH_0      0x4414
#define OV5642_REG_COMPRESSION_LENGTH_1      0x4415
#define OV5642_REG_COMPRESSION_LENGTH_2      0x4416
#define OV5642_REG_JFIFO_OVERFLOW            0x4417

//////////////////////////////////// VFIFO /////////////////////////////////////
#define OV5642_REG_VFIFO_CTRL_00             0x4600
#define OV5642_REG_VFIFO_HSIZE_0             0x4602
#define OV5642_REG_VFIFO_HSIZE_1             0x4603
#define OV5642_REG_VFIFO_VSIZE_0             0x4604
#define OV5642_REG_VFIFO_VSIZE_1             0x4605
#define OV5642_REG_VFIFO_CTRL_06             0x4606
#define OV5642_REG_VFIFO_CTRL_0C             0x460C

///////////////////////////////////// DVP //////////////////////////////////////
// DVP Control                               0x4700 - 0x4710
#define OV5642_REG_PAD_LEFT_CTRL             0x4711
#define OV5642_REG_PAD_RIGHT_CTRL            0x4712
#define OV5642_REG_JPG_MODE_SELECT           0x4713
// DVP Control                               0x4714
#define OV5642_REG_656_DUMMY_LINE            0x4715
// DVP Control                               0x4716 - 0x471A
#define OV5642_REG_HSYNC_CTRL_00             0x471B
#define OV5642_REG_DVP_CONTROL_1C            0x471C
#define OV5642_REG_DVP_CONTROL_1D            0x471D
// DVP Control                               0x471E - 0x472F
#define OV5642_REG_CCIR656_CTRL_00           0x4730
// DVP Control                               0x4731
#define OV5642_REG_CCIR656_FS                0x4732
#define OV5642_REG_CCIR656_FE                0x4733
#define OV5642_REG_CCIR656_LS                0x4734
#define OV5642_REG_CCIR656_LE                0x4735
#define OV5642_REG_POLARITY_CTRL_00          0x4740
#define OV5642_REG_TEST_PATTERN              0x4741
// DVP Control                               0x4742 - 0x4744
#define OV5642_REG_DATA_ORDER                0x4745

/////////////////////////////// ISP Top Control ////////////////////////////////
#define OV5642_REG_ISP_CONTROL_00            0x5000
#define OV5642_REG_ISP_CONTROL_01            0x5001
#define OV5642_REG_ISP_CONTROL_02            0x5002
#define OV5642_REG_ISP_CONTROL_03            0x5003
// ISP Reserved                              0x5004
#define OV5642_REG_ISP_CONTROL_05            0x5005
// ISP Reserved                              0x5006 - 0x501B
#define OV5642_REG_HORIZON_SUBSAMPLE_MODE    0x501C
// ISP Reserved                              0x501D
#define OV5642_REG_RGB_DITHER_CONTROL        0x501E
#define OV5642_REG_FORMAT_MUX_CONTROL        0x501F
// ISP Reserved                              0x5020 - 0x5024
#define OV5642_REG_ISP_CONTROL_37            0x5025
// ISP Reserved                              0x5026
#define OV5642_REG_DRAW_WIN_CTRL_00          0x5027
#define OV5642_REG_DRAW_WIN_LEFT_POS_CTRL_0  0x5028
#define OV5642_REG_DRAW_WIN_LEFT_POS_CTRL_1  0x5029
#define OV5642_REG_DRAW_WIN_RIGHT_POS_CTRL_0 0x502A
#define OV5642_REG_DRAW_WIN_RIGHT_POS_CTRL_1 0x502B
#define OV5642_REG_DRAW_WIN_TOP_POS_CTRL_0   0x502C
#define OV5642_REG_DRAW_WIN_TOP_POS_CTRL_1   0x502D
#define OV5642_REG_DRAW_WIN_BOT_POS_CTRL_0   0x502E
#define OV5642_REG_DRAW_WIN_BOT_POS_CTRL_1   0x502F
#define OV5642_REG_DRAW_WIN_HBW_CTRL_0       0x5030
#define OV5642_REG_DRAW_WIN_HBW_CTRL_1       0x5031
#define OV5642_REG_DRAW_WIN_VBW_CTRL_0       0x5032
#define OV5642_REG_DRAW_WIN_VBW_CTRL_1       0x5033
#define OV5642_REG_DRAW_WIN_Y_CTRL           0x5034
#define OV5642_REG_DRAW_WIN_U_CTRL           0x5035
#define OV5642_REG_DRAW_WIN_V_CTRL           0x5036
// ISP Reserved                              0x5037 - 0x503C
#define OV5642_REG_PRE_ISP_TEST_SET_1        0x503D
#define OV5642_REG_PRE_ISP_TEST_SET_2        0x503E
#define OV5642_REG_PRE_ISP_TEST_SET_3        0x503F
#define OV5642_REG_EVEN_CTRL_00              0x5080

///////////////////////////////////// AWB //////////////////////////////////////
#define OV5642_REG_AWB_CONTROL_00            0x5180
#define OV5642_REG_AWB_CONTROL_01            0x5181
#define OV5642_REG_AWB_CONTROL_02            0x5182
#define OV5642_REG_AWB_CONTROL_03            0x5183
#define OV5642_REG_AWB_CONTROL_04            0x5184
#define OV5642_REG_AWB_CONTROL_05            0x5185
#define OV5642_REG_AWB_CONTROL_06            0x5186
#define OV5642_REG_AWB_CONTROL_07            0x5187
#define OV5642_REG_AWB_CONTROL_08            0x5188
#define OV5642_REG_AWB_CONTROL_09            0x5189
#define OV5642_REG_AWB_CONTROL_10            0x518A
#define OV5642_REG_AWB_CONTROL_11            0x518B
#define OV5642_REG_AWB_CONTROL_12            0x518C
#define OV5642_REG_AWB_CONTROL_13            0x518D
#define OV5642_REG_AWB_CONTROL_14            0x518E
#define OV5642_REG_AWB_CONTROL_15            0x518F
#define OV5642_REG_AWB_CONTROL_16            0x5190
#define OV5642_REG_AWB_CONTROL_17            0x5191
#define OV5642_REG_AWB_CONTROL_18            0x5192
#define OV5642_REG_AWB_CONTROL_19            0x5193
#define OV5642_REG_AWB_CONTROL_20            0x5194
#define OV5642_REG_AWB_CONTROL_21            0x5195
#define OV5642_REG_AWB_CONTROL_22            0x5196
#define OV5642_REG_AWB_CONTROL_23            0x5197
#define OV5642_REG_AWB_R_GAIN_MANUAL_0       0x5198
#define OV5642_REG_AWB_R_GAIN_MANUAL_1       0x5199
#define OV5642_REG_AWB_G_GAIN_MANUAL_0       0x519A
#define OV5642_REG_AWB_G_GAIN_MANUAL_1       0x519B
#define OV5642_REG_AWB_B_GAIN_MANUAL_0       0x519C
#define OV5642_REG_AWB_B_GAIN_MANUAL_1       0x519D
#define OV5642_REG_AWB_CONTROL_30            0x519E
#define OV5642_REG_AWB_CURRENT_R_GAIN_0      0x519F
#define OV5642_REG_AWB_CURRENT_R_GAIN_1      0x51A0
#define OV5642_REG_AWB_CURRENT_G_GAIN_0      0x51A1
#define OV5642_REG_AWB_CURRENT_G_GAIN_1      0x51A2
#define OV5642_REG_AWB_CURRENT_B_GAIN_0      0x51A3
#define OV5642_REG_AWB_CURRENT_B_GAIN_1      0x51A4
#define OV5642_REG_AWB_AVERAGE_R             0x51A5
#define OV5642_REG_AWB_AVERAGE_G             0x51A6
#define OV5642_REG_AWB_AVERAGE_B             0x51A7

////////////////////////////////// UV Adjust ///////////////////////////////////
#define OV5642_REG_UV_ADJUST_CONTROL_0       0x5500
#define OV5642_REG_UV_ADJUST_CONTROL_3       0x5501
#define OV5642_REG_UV_ADJ_TH1_0              0x5502
#define OV5642_REG_UV_ADJ_TH1_1              0x5503
#define OV5642_REG_UV_ADJ_TH2_0              0x5504
#define OV5642_REG_UV_ADJ_TH2_1              0x5505
#define OV5642_REG_UV_ADJUST_IN_AUTO         0x5506

/////////////////////////////// Scale / Average ////////////////////////////////
#define OV5642_REG_SCALE_CONTROL_00          0x5600
#define OV5642_REG_AVG_X_START_0             0x5680
#define OV5642_REG_AVG_X_START_1             0x5681
#define OV5642_REG_AVG_X_END_0               0x5682
#define OV5642_REG_AVG_X_END_1               0x5683
#define OV5642_REG_AVG_Y_START_0             0x5684
#define OV5642_REG_AVG_Y_START_1             0x5685
#define OV5642_REG_AVG_Y_END_0               0x5686
#define OV5642_REG_AVG_Y_END_1               0x5687
#define OV5642_REG_AVG_R8                    0x5688
#define OV5642_REG_AVG_R9                    0x5689
#define OV5642_REG_AVG_RA                    0x568A
#define OV5642_REG_AVG_RB                    0x568B
#define OV5642_REG_AVG_RC                    0x568C
#define OV5642_REG_AVG_RD                    0x568D
#define OV5642_REG_AVG_RE                    0x568E
#define OV5642_REG_AVG_RF                    0x568F
#define OV5642_REG_AVG_R10                   0x5690
#define OV5642_REG_AVG_R11                   0x5691

///////////////////////////////////// LENC /////////////////////////////////////
#define OV5642_REG_LENC_PARA_G00             0x5800
#define OV5642_REG_LENC_PARA_G01             0x5801
#define OV5642_REG_LENC_PARA_G02             0x5802
#define OV5642_REG_LENC_PARA_G03             0x5803
#define OV5642_REG_LENC_PARA_G04             0x5804
#define OV5642_REG_LENC_PARA_G05             0x5805
#define OV5642_REG_LENC_PARA_G06             0x5806
#define OV5642_REG_LENC_PARA_G07             0x5807
#define OV5642_REG_LENC_PARA_G08             0x5808
#define OV5642_REG_LENC_PARA_G09             0x5809
#define OV5642_REG_LENC_PARA_G10             0x580A
#define OV5642_REG_LENC_PARA_G11             0x580B
#define OV5642_REG_LENC_PARA_G12             0x580C
#define OV5642_REG_LENC_PARA_G13             0x580D
#define OV5642_REG_LENC_PARA_G14             0x580E
#define OV5642_REG_LENC_PARA_G15             0x580F
#define OV5642_REG_LENC_PARA_G16             0x5810
#define OV5642_REG_LENC_PARA_G17             0x5811
#define OV5642_REG_LENC_PARA_G18             0x5812
#define OV5642_REG_LENC_PARA_G19             0x5813
#define OV5642_REG_LENC_PARA_G20             0x5814
#define OV5642_REG_LENC_PARA_G21             0x5815
#define OV5642_REG_LENC_PARA_G22             0x5816
#define OV5642_REG_LENC_PARA_G23             0x5817
#define OV5642_REG_LENC_PARA_G24             0x5818
#define OV5642_REG_LENC_PARA_G25             0x5819
#define OV5642_REG_LENC_PARA_G26             0x581A
#define OV5642_REG_LENC_PARA_G27             0x581B
#define OV5642_REG_LENC_PARA_G28             0x581C
#define OV5642_REG_LENC_PARA_G29             0x581D
#define OV5642_REG_LENC_PARA_G30             0x581E
#define OV5642_REG_LENC_PARA_G31             0x581F
#define OV5642_REG_LENC_PARA_G32             0x5820
#define OV5642_REG_LENC_PARA_G33             0x5821
#define OV5642_REG_LENC_PARA_G34             0x5822
#define OV5642_REG_LENC_PARA_G35             0x5823
#define OV5642_REG_LENC_PARA_G36             0x5824
#define OV5642_REG_LENC_PARA_G37             0x5825
#define OV5642_REG_LENC_PARA_G38             0x5826
#define OV5642_REG_LENC_PARA_G39             0x5827
#define OV5642_REG_LENC_PARA_G40             0x5828
#define OV5642_REG_LENC_PARA_G41             0x5829
#define OV5642_REG_LENC_PARA_G42             0x582A
#define OV5642_REG_LENC_PARA_G43             0x582B
#define OV5642_REG_LENC_PARA_G44             0x582C
#define OV5642_REG_LENC_PARA_G45             0x582D
#define OV5642_REG_LENC_PARA_G46             0x582E
#define OV5642_REG_LENC_PARA_G47             0x582F
#define OV5642_REG_LENC_PARA_G48             0x5830
#define OV5642_REG_LENC_PARA_G49             0x5831
#define OV5642_REG_LENC_PARA_G50             0x5832
#define OV5642_REG_LENC_PARA_G51             0x5833
#define OV5642_REG_LENC_PARA_G52             0x5834
#define OV5642_REG_LENC_PARA_G53             0x5835
#define OV5642_REG_LENC_PARA_G54             0x5836
#define OV5642_REG_LENC_PARA_G55             0x5837
#define OV5642_REG_LENC_PARA_G56             0x5838
#define OV5642_REG_LENC_PARA_G57             0x5839
#define OV5642_REG_LENC_PARA_G58             0x583A
#define OV5642_REG_LENC_PARA_G59             0x583B
#define OV5642_REG_LENC_PARA_G60             0x583C
#define OV5642_REG_LENC_PARA_G61             0x583D
#define OV5642_REG_LENC_PARA_G62             0x583E
#define OV5642_REG_LENC_PARA_G63             0x583F

#define OV5642_REG_LENC_PARA_BR00            0x5840
#define OV5642_REG_LENC_PARA_BR01            0x5841
#define OV5642_REG_LENC_PARA_BR02            0x5842
#define OV5642_REG_LENC_PARA_BR03            0x5843
#define OV5642_REG_LENC_PARA_BR04            0x5844
#define OV5642_REG_LENC_PARA_BR05            0x5845
#define OV5642_REG_LENC_PARA_BR06            0x5846
#define OV5642_REG_LENC_PARA_BR07            0x5847
#define OV5642_REG_LENC_PARA_BR08            0x5848
#define OV5642_REG_LENC_PARA_BR09            0x5849
#define OV5642_REG_LENC_PARA_BR10            0x584A
#define OV5642_REG_LENC_PARA_BR11            0x584B
#define OV5642_REG_LENC_PARA_BR12            0x584C
#define OV5642_REG_LENC_PARA_BR13            0x584D
#define OV5642_REG_LENC_PARA_BR14            0x584E
#define OV5642_REG_LENC_PARA_BR15            0x584F
#define OV5642_REG_LENC_PARA_BR16            0x5850
#define OV5642_REG_LENC_PARA_BR17            0x5851
#define OV5642_REG_LENC_PARA_BR18            0x5852
#define OV5642_REG_LENC_PARA_BR19            0x5853
#define OV5642_REG_LENC_PARA_BR20            0x5854
#define OV5642_REG_LENC_PARA_BR21            0x5855
#define OV5642_REG_LENC_PARA_BR22            0x5856
#define OV5642_REG_LENC_PARA_BR23            0x5857
#define OV5642_REG_LENC_PARA_BR24            0x5858
#define OV5642_REG_LENC_PARA_BR25            0x5859
#define OV5642_REG_LENC_PARA_BR26            0x585A
#define OV5642_REG_LENC_PARA_BR27            0x585B
#define OV5642_REG_LENC_PARA_BR28            0x585C
#define OV5642_REG_LENC_PARA_BR29            0x585D
#define OV5642_REG_LENC_PARA_BR30            0x585E
#define OV5642_REG_LENC_PARA_BR31            0x585F
#define OV5642_REG_LENC_PARA_BR32            0x5860
#define OV5642_REG_LENC_PARA_BR33            0x5861
#define OV5642_REG_LENC_PARA_BR34            0x5862
#define OV5642_REG_LENC_PARA_BR35            0x5863
#define OV5642_REG_LENC_PARA_BR36            0x5864
#define OV5642_REG_LENC_PARA_BR37            0x5865
#define OV5642_REG_LENC_PARA_BR38            0x5866
#define OV5642_REG_LENC_PARA_BR39            0x5867
#define OV5642_REG_LENC_PARA_BR40            0x5868
#define OV5642_REG_LENC_PARA_BR41            0x5869
#define OV5642_REG_LENC_PARA_BR42            0x586A
#define OV5642_REG_LENC_PARA_BR43            0x586B
#define OV5642_REG_LENC_PARA_BR44            0x586C
#define OV5642_REG_LENC_PARA_BR45            0x586D
#define OV5642_REG_LENC_PARA_BR46            0x586E
#define OV5642_REG_LENC_PARA_BR47            0x586F
#define OV5642_REG_LENC_PARA_BR48            0x5870
#define OV5642_REG_LENC_PARA_BR49            0x5871
#define OV5642_REG_LENC_PARA_BR50            0x5872
#define OV5642_REG_LENC_PARA_BR51            0x5873
#define OV5642_REG_LENC_PARA_BR52            0x5874
#define OV5642_REG_LENC_PARA_BR53            0x5875
#define OV5642_REG_LENC_PARA_BR54            0x5876
#define OV5642_REG_LENC_PARA_BR55            0x5877
#define OV5642_REG_LENC_PARA_BR56            0x5878
#define OV5642_REG_LENC_PARA_BR57            0x5879
#define OV5642_REG_LENC_PARA_BR58            0x587A
#define OV5642_REG_LENC_PARA_BR59            0x587B
#define OV5642_REG_LENC_PARA_BR60            0x587C
#define OV5642_REG_LENC_PARA_BR61            0x587D
#define OV5642_REG_LENC_PARA_BR62            0x587E
#define OV5642_REG_LENC_PARA_BR63            0x587F
#define OV5642_REG_LENC_PARA_BR64            0x5880
#define OV5642_REG_LENC_PARA_BR65            0x5881
#define OV5642_REG_LENC_PARA_BR66            0x5882
#define OV5642_REG_LENC_PARA_BR67            0x5883
#define OV5642_REG_LENC_PARA_BR68            0x5884
#define OV5642_REG_LENC_PARA_BR69            0x5885
#define OV5642_REG_LENC_PARA_BR70            0x5886
#define OV5642_REG_LENC_PARA_BR71            0x5887

#define OV5642_REG_LENC_BRV_SCALE_HIGH       0x5888
#define OV5642_REG_LENC_BRV_SCALE_LOW        0x5889
#define OV5642_REG_LENC_BRH_SCALE_HIGH       0x588A
#define OV5642_REG_LENC_BRH_SCALE_LOW        0x588B
#define OV5642_REG_LENC_GV_SCALE_HIGH        0x588C
#define OV5642_REG_LENC_GV_SCALE_LOW         0x588D
#define OV5642_REG_LENC_GH_SCALE_HIGH        0x588E
#define OV5642_REG_LENC_GH_SCALE_LOW         0x588F

#define OV5642_REG_LENC_X_OFFSET_0           0x5890
#define OV5642_REG_LENC_X_OFFSET_1           0x5891
#define OV5642_REG_LENC_Y_OFFSET_0           0x5892
#define OV5642_REG_LENC_Y_OFFSET_1           0x5893

#define OV5642_REG_LENC_NGAIN                0x5894

/****************************** REGISTER VALUES *******************************/

#define OV5642_CHIPID                          0x5642

#define OV5642_REG_SC_SYS_0_SOFT_RESET         ((uint8_t)(1U << 7))
#define OV5642_REG_SC_SYS_0_SOFT_PWDN          ((uint8_t)(1U << 6))

#define OV5642_REG_DATA_QVGA_PREVIEW_LEN       584U
#define OV5642_REG_DATA_JPEG_CAPTURE_QSXGA_LEN 72U
#define OV5642_REG_DATA_320X240_LEN            27U
#define OV5642_REG_DATA_2592X1944_LEN          26U

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    uint16_t addr;
    uint8_t data;
} ov5642_reg_data_t;

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

extern const ov5642_reg_data_t ov5642_reg_data_qvga_preview[OV5642_REG_DATA_QVGA_PREVIEW_LEN];
extern const ov5642_reg_data_t ov5642_reg_data_jpeg_capture_qsxga[OV5642_REG_DATA_JPEG_CAPTURE_QSXGA_LEN];
extern const ov5642_reg_data_t ov5642_reg_data_320x240[OV5642_REG_DATA_320X240_LEN];
extern const ov5642_reg_data_t ov5642_reg_data_2592x1944[OV5642_REG_DATA_2592X1944_LEN];

#endif // CAM_OV5642_DEFS_H_
