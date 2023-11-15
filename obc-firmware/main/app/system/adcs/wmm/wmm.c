/**
 * @file wmm.c
 * @brief Implementation of functions declared in wmm.h
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

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

//WMM
#include "wmm.h"

//STD LIBS
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//DSPLIB
#include "dsp_math.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

//Note: 2020 epoch wmm coefficients are valid until 2024. New coefficients are released in 2025.
#define WMM_EPOCH 2020.0f
#define PI_CONST 3.14159265359f
#define RADIANS_TO_DEGREES 0.017453292f
#define DEGREES_TO_RADIANS (PI_CONST / 180.0f)
#define A_CONST 6378.137f
#define A2_CONST (A_CONST * A_CONST)
#define B_CONST 6356.7523142f
#define B2_CONST (B_CONST * B_CONST)
#define RE_CONST 6371.2f
#define A4_CONST (A2_CONST * A2_CONST)
#define B4_CONST (B2_CONST * B2_CONST)
#define C2_CONST (A2_CONST - B2_CONST)
#define C4_CONST (A4_CONST - B4_CONST)
#define WMM_WGS84_MAX_ALT_M 850000
#define WMM_WGS84_MIN_ALT_M -1000
#define COEFFICIENTS_COUNT		90U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

/**
 * Function to decompress the encoded byte array for wmm coefficients
 *
 * @param year encoded bytes array
 * @return float32_t coefficient value from bytes
 */
static float32_t convert_varint_to_float32_t(char **bytes);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static wmm_cof_record_t wmm_cof_entries[COEFFICIENTS_COUNT];
static wmm_state_t wmm_state;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

float32_t wmm_get_date(uint8_t year, uint8_t month, uint8_t date)
{
	float32_t days_in_year = 365.0f;

	if ((year % 4U) == 0U)
	{
		days_in_year = 366.0f;
	}

	return (float32_t)year + 2000.0f + ((float32_t)(month - 1U) / 12.0f) + ((float32_t)(date - 1U) / (days_in_year));
}

void wmm_init(void)
{
	uint8_t j;
	uint8_t m;
	uint8_t n;
	uint8_t D2;
	float32_t gnm;
	float32_t hnm;
	float32_t dgnm;
	float32_t dhnm;
	float32_t flnmj;
	uint8_t i;
	char *bytes = (char *)&wmm_cof_entries_encoded[0];

	// unpack coefficients
	for (i = 0U; i < COEFFICIENTS_COUNT; i++)
	{
		wmm_cof_entries[i].gnm = convert_varint_to_float32_t(&bytes);
		wmm_cof_entries[i].hnm = convert_varint_to_float32_t(&bytes);
		wmm_cof_entries[i].dgnm = convert_varint_to_float32_t(&bytes);
		wmm_cof_entries[i].dhnm = convert_varint_to_float32_t(&bytes);
	}

	wmm_state.c[0][0] = 0.0f;
	wmm_state.cd[0][0] = 0.0f;

	j = 0U;
	for (n = 1U; n <= 12U; n++)
	{
		for (m = 0U; m <= n; m++)
		{
			gnm = wmm_cof_entries[j].gnm;
			hnm = wmm_cof_entries[j].hnm;
			dgnm = wmm_cof_entries[j].dgnm;
			dhnm = wmm_cof_entries[j].dhnm;
			j++;

			if (m <= n)
			{
				wmm_state.c[m][n] = gnm;
				wmm_state.cd[m][n] = dgnm;
				if (m != 0U)
				{
					wmm_state.c[n][m - 1U] = hnm;
					wmm_state.cd[n][m - 1U] = dhnm;
				}
			}
		}
	}

	// CONVERT SCHMIDT NORMALIZED GAUSS COEFFICIENTS TO UNNORMALIZED
	*wmm_state.snorm = 1.0f;
	for (n = 1U; n <= 12U; n++)
	{
		*(wmm_state.snorm + n) = *(wmm_state.snorm + n - 1U) * (float32_t)(2U * n - 1U) / (float32_t)n;
		j = 2U;
		m = 0U;
		for (D2 = n - m + 1U; D2 > 0U; D2--)
		{
			wmm_state.k[m][n] = (float32_t)(((n - 1U) * (n - 1U)) - (m * m)) / (float32_t)((2U * n - 1U) * (2U * n - 3U));
			if (m > 0U)
			{
				flnmj = (float32_t)((n - m + 1U) * j) / (float32_t)(n + m);
				float32_t sqrt_flnmj;
				arm_sqrt_f32(flnmj, &sqrt_flnmj);
				*(wmm_state.snorm + n + m * 13U) = *(wmm_state.snorm + n + (m - 1U) * 13U) * sqrt_flnmj;
				j = 1U;
				wmm_state.c[n][m - 1U] = *(wmm_state.snorm + n + m * 13U) * wmm_state.c[n][m - 1U];
				wmm_state.cd[n][m - 1U] = *(wmm_state.snorm + n + m * 13U) * wmm_state.cd[n][m - 1U];
			}
			wmm_state.c[m][n] = *(wmm_state.snorm + n + m * 13U) * wmm_state.c[m][n];
			wmm_state.cd[m][n] = *(wmm_state.snorm + n + m *13U) * wmm_state.cd[m][n];
			m += 1U;
	    }
		wmm_state.fn[n] = (float32_t)(n + 1U);
		wmm_state.fm[n] = (float32_t)n;
	}
	wmm_state.k[1][1] = 0.0f;
	wmm_state.wmm_init_flag = true;
}

bool wmm_is_init(void)
{
	return wmm_state.wmm_init_flag;
}

adcs_err_t wmm_get_mag_ref(float32_t glat, float32_t glon, float32_t alt, float32_t time_years, float32_t *mag_ref)
{
	if(!wmm_state.wmm_init_flag)
	{
		return ADCS_ERROR_WMM_NO_INIT;
	}
	static float32_t tc[13][13];
	static float32_t sp[13];
	static float32_t cp[13];
	static float32_t dp[13][13];
	static float32_t pp[13];
	float32_t dt = time_years - WMM_EPOCH;
	float32_t rlon = glon * DEGREES_TO_RADIANS;
	float32_t rlat = glat * DEGREES_TO_RADIANS;
	float32_t srlon = arm_sin_f32(rlon);
	float32_t srlat = arm_sin_f32(rlat);
	float32_t crlon = arm_cos_f32(rlon);
	float32_t crlat = arm_cos_f32(rlat);
	float32_t srlat2 = srlat * srlat;
	float32_t crlat2 = crlat * crlat;
	sp[0] = 0.0f;
	sp[1] = srlon;
	cp[0] = 1.0f;
	cp[1] = crlon;
	dp[0][0] = 0.0f;
	pp[0] = 1.0f;

	// CONVERT FROM GEODETIC COORDS TO SPHERICAL COORDS
	float32_t q; 
	arm_sqrt_f32(A2_CONST - C2_CONST * srlat2, &q);
	float32_t q1 = alt * q;
	float32_t q2 = ((q1 + A2_CONST) / (q1 + B2_CONST)) * ((q1 + A2_CONST) / (q1 + B2_CONST));
	float32_t ct;
	arm_sqrt_f32(q2 * crlat2 + srlat2, &ct); //temp step
	ct = srlat / ct; 
	float32_t st;
	arm_sqrt_f32(1.0f - (ct * ct), &st);
	float32_t r2 = (alt * alt) + 2.0f * q1 + (A4_CONST - C4_CONST * srlat2) / (q * q);
	float32_t r;
	arm_sqrt_f32(r2, &r);
	float32_t d;
	arm_sqrt_f32(A2_CONST * crlat2 + B2_CONST * srlat2, &d);
	float32_t ca = (alt + d) / r;
	float32_t sa = C2_CONST * crlat * srlat / (r * d);
	for (uint8_t m = 2U; m <= 12U; m++)
	{
		sp[m] = sp[1] * cp[m - 1U] + cp[1] * sp[m - 1U];
		cp[m] = cp[1] * cp[m - 1U] - sp[1] * sp[m - 1U];
	}
	float32_t aor = RE_CONST / r;
	float32_t ar = aor * aor;
	float32_t br = 0.0f;
	float32_t bt = 0.0f;
	float32_t bp = 0.0f;
	float32_t bpp = 0.0f;

	for (uint16_t n = 1U; n <= 12U; n++)
    {
		ar = ar * aor;
		uint8_t m = 0U;
		for (uint8_t D4 = n + 1U; D4 > 0U; D4--)
		{
			// COMPUTE UNNORMALIZED ASSOCIATED LEGENDRE POLYNOMIALS AND DERIVATIVES VIA RECURSION RELATIONS
			if (n == m)
			{
				*(wmm_state.snorm + n + m * 13U) = st * *(wmm_state.snorm + n - 1U + (m - 1U) * 13U);
				dp[m][n] = st * dp[m - 1U][n - 1U] + ct * *(wmm_state.snorm + n - 1U + (m - 1U) * 13U);
				goto S50;
			}
			if (n == 1U && m == 0U)
			{
				*(wmm_state.snorm + n + m * 13U) = ct * *(wmm_state.snorm + n - 1U + m * 13U);
				dp[m][n] = ct * dp[m][n - 1U] - st * *(wmm_state.snorm + n - 1U + m * 13U);
				goto S50;
			}
			if (n > 1U && n != m)
			{
				if (m > n - 2U)
				{
					*(wmm_state.snorm + n - 2U + m * 13U) = 0.0f;
				}
				if (m > n - 2U)
				{
					dp[m][n - 2U] = 0.0f;
				}
				*(wmm_state.snorm + n + m * 13U) = ct * *(wmm_state.snorm + n - 1U + m * 13U) - wmm_state.k[m][n] * *(wmm_state.snorm + n - 2U + m * 13U);
				dp[m][n] = ct * dp[m][n - 1U] - st * *(wmm_state.snorm + n - 1U + m * 13U) - wmm_state.k[m][n] * dp[m][n - 2U];
			}
        S50:

			// TIME ADJUST THE GAUSS COEFFICIENTS
			tc[m][n] = wmm_state.c[m][n] + dt * wmm_state.cd[m][n];
			if (m != 0U)
			{
				tc[n][m - 1U] = wmm_state.c[n][m - 1U] + dt * wmm_state.cd[n][m - 1U];
			}

			// ACCUMULATE TERMS OF THE SPHERICAL HARMONIC EXPANSIONS
			float32_t par = ar * *(wmm_state.snorm + n + m * 13U);
			float32_t temp1;
			float32_t temp2;

			if (m == 0)
			{
				temp1 = tc[m][n] * cp[m];
				temp2 = tc[m][n] * sp[m];
			}
			else
			{
				temp1 = tc[m][n] * cp[m] + tc[n][m - 1U] * sp[m];
				temp2 = tc[m][n] * sp[m] - tc[n][m - 1U] * cp[m];
			}

			bt = bt - ar * temp1 * dp[m][n];
			bp += (wmm_state.fm[m] * temp2 * par);
			br += (wmm_state.fn[n] * temp1 * par);

			// SPECIAL CASE: NORTH/SOUTH GEOGRAPHIC POLES
			if (st == 0.0f && m == 1U)
			{
				if (n == 1U)
				{
					pp[n] = pp[n - 1U];
				}
				else
				{
					pp[n] = ct * pp[n - 1U] - wmm_state.k[m][n] * pp[n - 2U];
				}
				bpp += (wmm_state.fm[m] * temp2 * ar * pp[n]);
			}
			m += 1U;
        }
    }
	if (st == 0.0f)
	{
		bp = bpp;
	}
	else
	{
		bp /= st;
	}

	// ROTATE MAGNETIC VECTOR COMPONENTS FROM SPHERICAL TO GEODETIC COORDINATES

	mag_ref[0] = -bt * ca - br * sa ; //x
	mag_ref[1] = bp					; //y
	mag_ref[2] = -bt * sa - br * ca ; //z
	return ADCS_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static float32_t convert_varint_to_float32_t(char **bytes)
{
	float32_t result;
	int32_t result_int;
	bool negative = false;
	bool first_byte = true;
	uint8_t shift;

	do
	{
		if (first_byte)
		{
			if (**bytes & 0x40)
			{
				negative = true;
			}

			result_int = **bytes & 0x3f;
			shift = 6U;
			first_byte = false;
		}
		else
		{
			result_int += ((uint32_t)(**bytes & 0x7f) << shift);
			shift += 7U;
		}

		if ((**bytes & 0x80) == 0U)
		{
			(*bytes)++;
			break;
		}

		(*bytes)++;

	} while (true);


	result = ((float32_t)result_int) / 10.0f;
	if (negative)
	{
		result = -result;
	}

	return result;
}
