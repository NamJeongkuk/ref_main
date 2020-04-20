/*******************************************************************************
* File Name: Gea3_SCBCLK.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_Gea3_SCBCLK_H)
#define CY_CLOCK_Gea3_SCBCLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void Gea3_SCBCLK_StartEx(uint32 alignClkDiv);
#define Gea3_SCBCLK_Start() \
    Gea3_SCBCLK_StartEx(Gea3_SCBCLK__PA_DIV_ID)

#else

void Gea3_SCBCLK_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void Gea3_SCBCLK_Stop(void);

void Gea3_SCBCLK_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 Gea3_SCBCLK_GetDividerRegister(void);
uint8  Gea3_SCBCLK_GetFractionalDividerRegister(void);

#define Gea3_SCBCLK_Enable()                         Gea3_SCBCLK_Start()
#define Gea3_SCBCLK_Disable()                        Gea3_SCBCLK_Stop()
#define Gea3_SCBCLK_SetDividerRegister(clkDivider, reset)  \
    Gea3_SCBCLK_SetFractionalDividerRegister((clkDivider), 0u)
#define Gea3_SCBCLK_SetDivider(clkDivider)           Gea3_SCBCLK_SetDividerRegister((clkDivider), 1u)
#define Gea3_SCBCLK_SetDividerValue(clkDivider)      Gea3_SCBCLK_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define Gea3_SCBCLK_DIV_ID     Gea3_SCBCLK__DIV_ID

#define Gea3_SCBCLK_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define Gea3_SCBCLK_CTRL_REG   (*(reg32 *)Gea3_SCBCLK__CTRL_REGISTER)
#define Gea3_SCBCLK_DIV_REG    (*(reg32 *)Gea3_SCBCLK__DIV_REGISTER)

#define Gea3_SCBCLK_CMD_DIV_SHIFT          (0u)
#define Gea3_SCBCLK_CMD_PA_DIV_SHIFT       (8u)
#define Gea3_SCBCLK_CMD_DISABLE_SHIFT      (30u)
#define Gea3_SCBCLK_CMD_ENABLE_SHIFT       (31u)

#define Gea3_SCBCLK_CMD_DISABLE_MASK       ((uint32)((uint32)1u << Gea3_SCBCLK_CMD_DISABLE_SHIFT))
#define Gea3_SCBCLK_CMD_ENABLE_MASK        ((uint32)((uint32)1u << Gea3_SCBCLK_CMD_ENABLE_SHIFT))

#define Gea3_SCBCLK_DIV_FRAC_MASK  (0x000000F8u)
#define Gea3_SCBCLK_DIV_FRAC_SHIFT (3u)
#define Gea3_SCBCLK_DIV_INT_MASK   (0xFFFFFF00u)
#define Gea3_SCBCLK_DIV_INT_SHIFT  (8u)

#else 

#define Gea3_SCBCLK_DIV_REG        (*(reg32 *)Gea3_SCBCLK__REGISTER)
#define Gea3_SCBCLK_ENABLE_REG     Gea3_SCBCLK_DIV_REG
#define Gea3_SCBCLK_DIV_FRAC_MASK  Gea3_SCBCLK__FRAC_MASK
#define Gea3_SCBCLK_DIV_FRAC_SHIFT (16u)
#define Gea3_SCBCLK_DIV_INT_MASK   Gea3_SCBCLK__DIVIDER_MASK
#define Gea3_SCBCLK_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_Gea3_SCBCLK_H) */

/* [] END OF FILE */
