/*!
 * @file
 * @brief Creates a vector table so the application will start up while debugging.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */
#include "bsp_compiler_support.h"

typedef void (*Vector_t)(void);

void Reset_Handler(void);
extern char __stack;

BSP_DONT_REMOVE const Vector_t __debugVectors[] BSP_PLACE_IN_SECTION(".debug_vectors") = {
   (Vector_t)&__stack,
   Reset_Handler,
};
