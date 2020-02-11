/*!
 * @file
 * @brief Creates a vector table so the application will start up while debugging.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

typedef void (*pFunc)(void);

void Reset_Handler(void);
extern char __StackTop;

const pFunc __debugVectors[] __attribute__((section(".debug_vector_table"))) = {
   (pFunc)&__StackTop,
   Reset_Handler
};
