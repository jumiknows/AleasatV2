/*
 * obc_misra.h
 *
 *  Created on: Jul 13, 2019
 *      Author: Richard Arthurs
 *
 *      Provides a set of #defines for easily and consistently enabling and disabling MISRA
 * checking.
 *
 *      USAGE:
 *      		- In a file or region where you want to disable MISRA checking:
 * "OBC_MISRA_CHECK_OFF"
 *      		- Farther down or at the end of the file: "OBC_MISRA_CHECK_ON"
 *
 *      	WARNING:
 *      		- Make sure to call "OBC_MISRA_CHECK_ON" after OBC_MISRA_CHECK_OFF!
 */

#ifndef OBC_MISRA_H_
#define OBC_MISRA_H_

#define OBC_MISRA_CHECK_OFF _Pragma("CHECK_MISRA(\"none\")")
#define OBC_MISRA_CHECK_ON  _Pragma("CHECK_MISRA(\"all,-1.1,-2.2,-5.6,-5.7,-6.1,-8.1,-8.5,-10.1,-10.3,-10.6,-11.3,-11.4,-12.2,-12.7,-12.13,-13.5,-14.6,-14.7,-16.1,-16.7,-17.4,-18.4,-19.4,-19.7,-20.1,-16.3\")")

#endif /* OBC_MISRA_H_ */
