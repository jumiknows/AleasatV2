/**
 * @file cam_ov5642.h
 * @brief Driver for OV5642 image sensor
 */

#ifndef CAM_OV5642_H_
#define CAM_OV5642_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Error codes that can be returned by OV5642 driver functions
 */
typedef enum {
    OV5642_SUCCESS          = 0,
    OV5642_ERR_INVALID_ARGS = -1,
    OV5642_ERR_I2C          = -2,
    OV5642_ERR_CHIPID       = -3,
} ov5642_err_t;

/**
 * @brief Image data output formats
 */
typedef enum {
    OV5642_FMT_RAW,
    OV5642_FMT_JPEG,
} ov5642_format_t;

/**
 * @brief Resolution options for output images in the JPEG format
 */
typedef enum {
    OV5642_JPEG_SIZE_320X240,
    OV5642_JPEG_SIZE_640X480,
    OV5642_JPEG_SIZE_1024X768,
    OV5642_JPEG_SIZE_1280X960,
    OV5642_JPEG_SIZE_1600X1200,
    OV5642_JPEG_SIZE_2048X1536,
    OV5642_JPEG_SIZE_2592X1944,
} ov5642_jpeg_size_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

ov5642_err_t ov5642_init(void);
ov5642_err_t ov5642_soft_reset(void);

ov5642_err_t ov5642_set_format(ov5642_format_t format);
ov5642_err_t ov5642_set_jpeg_size(ov5642_jpeg_size_t jpeg_size);

#endif // CAM_OV5642_H_
