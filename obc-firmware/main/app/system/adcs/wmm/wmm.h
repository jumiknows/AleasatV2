/**
 * @file wmm.h
 * @brief Implementation of world magnetic model in C to obtain the reference
 * ambient magnetic field vector components [bx, by, bz].
 * This software is ported from https://github.com/miniwinwm/WMM_Tiny 
 * which was orginally developed by NOAA / NCEI https://www.ngdc.noaa.gov/geomag/WMM/.
 * Modifications made for port:
 * Removed stm32 related files leaving only the core files: wmm.h, wmm.c, WMM_COF.c
 * Added function wmm_get_mag_ref to return magnetic vector components.
 * New function also takes altitude as an input
 */

/* ----------------------------------------------------------------------    
MIT License

Copyright (c) 2018 John Blaiklock

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-------------------------------------------------------------------- */ 

#ifndef WMM_H
#define WMM_H

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

//DSPLIB TYPES
#include "type_defs.h"

//STDINT
#include <stdint.h>

#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define WMM_COF_ENTRIES_ENCODED_SIZE 465
#define WMM_STATE_SIZE 13

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct
{
	float32_t gnm;
	float32_t hnm;
	float32_t dgnm;
	float32_t dhnm;
} wmm_cof_record_t;

typedef struct
{
	float32_t c[WMM_STATE_SIZE][WMM_STATE_SIZE];
	float32_t cd[WMM_STATE_SIZE][WMM_STATE_SIZE];
	float32_t k[WMM_STATE_SIZE][WMM_STATE_SIZE];
	float32_t snorm[WMM_STATE_SIZE*WMM_STATE_SIZE];
	float32_t fn[WMM_STATE_SIZE];
	float32_t fm[WMM_STATE_SIZE];
	bool wmm_init_flag;
} wmm_state_t;

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

extern const uint8_t wmm_cof_entries_encoded[WMM_COF_ENTRIES_ENCODED_SIZE];

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/**
 * Initialize the WMM. Needs calling only once.
 * Loads wmm gauss coefficients (f32) onto RAM (initially stored as compressed bytes in WMM_COF.c)
 */
void wmm_init(void);

/**
 * Returns true if wmm_init has been called since startup
 */
bool wmm_is_init(void);

/**
 * Get the date in WMM format
 *
 * @param year Year in 2 digit format of 21st centuary, i.e. 20 represents 2020
 * @param month Month, 1 to 12
 * @param date Date of month, 1 to 31
 * @return Date in WMM format
 * @note No checking of illegal dates is done
 */
float32_t wmm_get_date(uint8_t year, uint8_t month, uint8_t date);

/**
 * Get the magnetic vector components (in  units of nT: nano Tesla / 10-9 Tesla) at a point on the earth's surface.
 *
 * @param glat Latitude in degrees and fractional degrees, negative south
 * @param glon Longitude in degrees and fractional degrees, negative west
 * @param alt Altitude in kilometers from the ellipsoid (wgs84) surface (geodetic height)
 * @param time_years The date as returned from wmm_get_date
 * @param mag_ref Pointer to float32_t array of size 3 to store output: calculated magnetic vector components x y and z. 
 * 				  X is positive northward, Y is positive eastward, Z is positive downward.
 * @return indicate if the calculation completed successfully, returns false if wmm_init was not called first.
 * @note The magentic model is not a good approxmiation for altitudes exceeding 1km below the surface or 850km
 * 		 above.
 */
bool wmm_get_mag_ref(float32_t glat, float32_t glon, float32_t alt, float32_t time_years, float32_t *mag_ref);

#endif
