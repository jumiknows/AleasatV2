/**
 * @file crt.h
 * @brief C Runtime header
 */

#ifndef CRT_H_
#define CRT_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// Functions defined in the Runtime Support Library (RTS)
void __TI_auto_init(void);

// Application main function
int32_t main(void);

#endif // CRT_H_
