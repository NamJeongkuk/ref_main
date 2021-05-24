/*!
 * @file
 * @brief Creates a vector table so the application will start up while debugging.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

typedef void (*Vector_t)(void);

void Reset_Handler(void);
extern char __stack;

const Vector_t __debugVectors[] __attribute__((used, section(".debug_vectors"))) = {
   (Vector_t)&__stack,
   Reset_Handler,
};
