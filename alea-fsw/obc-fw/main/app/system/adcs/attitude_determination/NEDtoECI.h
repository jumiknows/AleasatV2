/**
 * @file NEDtoECI.h
 * @brief this file contains all the functions required to convert a vector from the NED body frame to the Earth-Centered Intertial (ECI) frame
 */

#ifndef NEDtoECI_H_
#define NEDtoECI_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// DSPLIB types
#include "type_defs.h"

#include "adcs_types.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Public defines that may be used by other files

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

// Public typedefs that may be used by other files

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

// Public global variables that may be used by other files
// (always declare extern)

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/**
  @brief         converts the given NED vector to an ECI vector
  @param         input the input vector in NED
  @param         position the position of the origin of the NED frame
  @param         lat Latitude of origin in degrees and fractional degrees, negative south
  @param         lon Longitude of origin in degrees and fractional degrees, negative west
  @param         time seconds since the ECI frame was aligned with the ECEF frame
  @param         dest output vector to store ECI coordinates
  @return        None
 */
void NEDtoECI(const float32_t *input, const float32_t *position, float32_t lat, float32_t lon, float32_t time, float32_t *dest);

/**
  @brief         given geodetic coordinates, outputs cartesian coordinates in the ECEF frame
  @param         lat Latitude in degrees and fractional degrees, negative south
  @param         lon Longitude in degrees and fractional degrees, negative west
  @param         altitude WGS84 ellipsoidal altitude in meters
  @param         dest output vector to store ECEF coordinates
  @return        None
 */
void get_ECEF(float32_t lat, float32_t lon, float32_t altitude, float32_t *dest);

/******************************************************************************/
/*                       I N L I N E  F U N C T I O N S                       */
/******************************************************************************/

// Inline function declarations and implementions

#endif // NEDtoECI.h