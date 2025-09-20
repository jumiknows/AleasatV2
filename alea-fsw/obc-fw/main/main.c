/**
 * @file main.c
 * @brief Implementation of C main function that calls through to obc_main
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_main.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

int32_t main(void); // To satisfy MISRA-C:2004 8.1/R

int32_t main(void) {
    obc_main();
    return 0;
}
