/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

void Reset_Handler(void);
extern char __StackTop;

typedef void (*Vector_t)(void);

const Vector_t __debugVectors[] __attribute__((section(".debug_vector_table"))) = {
   (Vector_t)&__StackTop,
   Reset_Handler
};
