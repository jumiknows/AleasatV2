# README

Some modifications to the CMSIS-DSP library were required to avoid compiler warnings:

## type_defs.h

Replaced the following lines:
```c
  /**
   * @brief 8-bit integer data type.
   */
 typedef signed int int32_t;
 typedef unsigned int uint32_t;

   /**
   * @brief 8-bit integer data type.
   */
 typedef signed char int8_t;
 typedef unsigned char uint8_t;

    /**
   * @brief 8-bit integer data type.
   */
 typedef signed short int16_t;
 typedef unsigned short uint16_t;

  
 #if defined (CCS )
 typedef long long __int64;
 #define __packed	 
 #endif
 
 #if defined (__GNUC__)
 typedef long long __int64;
 #define __packed	__attribute__ ((packed))	 
 #endif
 
 typedef __int64 int64_t ;

 typedef unsigned long long uint64_t;
```
with:
```c
#include <stdint.h>
```

## arm_math.h

Replaced lines 361 and 365:
```c
361: #define __SIMD32(addr)  (*(__packed int32_t **) & (addr))  

365: #define _SIMD32_OFFSET(addr)  (*(__packed int32_t *)  (addr))  
```
with:
```c
361: #define __SIMD32(addr)  (*(int32_t **) & (addr))  

365: #define _SIMD32_OFFSET(addr)  (*(int32_t *)  (addr))  
```
(removed the `__packed` qualifier which is invalid there under the TI compiler)
