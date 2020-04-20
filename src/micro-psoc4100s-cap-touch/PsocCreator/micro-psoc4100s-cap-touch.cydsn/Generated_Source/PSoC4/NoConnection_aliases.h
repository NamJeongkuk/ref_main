/*******************************************************************************
* File Name: NoConnection.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_NoConnection_ALIASES_H) /* Pins NoConnection_ALIASES_H */
#define CY_PINS_NoConnection_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define NoConnection_0			(NoConnection__0__PC)
#define NoConnection_0_PS		(NoConnection__0__PS)
#define NoConnection_0_PC		(NoConnection__0__PC)
#define NoConnection_0_DR		(NoConnection__0__DR)
#define NoConnection_0_SHIFT	(NoConnection__0__SHIFT)
#define NoConnection_0_INTR	((uint16)((uint16)0x0003u << (NoConnection__0__SHIFT*2u)))

#define NoConnection_1			(NoConnection__1__PC)
#define NoConnection_1_PS		(NoConnection__1__PS)
#define NoConnection_1_PC		(NoConnection__1__PC)
#define NoConnection_1_DR		(NoConnection__1__DR)
#define NoConnection_1_SHIFT	(NoConnection__1__SHIFT)
#define NoConnection_1_INTR	((uint16)((uint16)0x0003u << (NoConnection__1__SHIFT*2u)))

#define NoConnection_2			(NoConnection__2__PC)
#define NoConnection_2_PS		(NoConnection__2__PS)
#define NoConnection_2_PC		(NoConnection__2__PC)
#define NoConnection_2_DR		(NoConnection__2__DR)
#define NoConnection_2_SHIFT	(NoConnection__2__SHIFT)
#define NoConnection_2_INTR	((uint16)((uint16)0x0003u << (NoConnection__2__SHIFT*2u)))

#define NoConnection_3			(NoConnection__3__PC)
#define NoConnection_3_PS		(NoConnection__3__PS)
#define NoConnection_3_PC		(NoConnection__3__PC)
#define NoConnection_3_DR		(NoConnection__3__DR)
#define NoConnection_3_SHIFT	(NoConnection__3__SHIFT)
#define NoConnection_3_INTR	((uint16)((uint16)0x0003u << (NoConnection__3__SHIFT*2u)))

#define NoConnection_4			(NoConnection__4__PC)
#define NoConnection_4_PS		(NoConnection__4__PS)
#define NoConnection_4_PC		(NoConnection__4__PC)
#define NoConnection_4_DR		(NoConnection__4__DR)
#define NoConnection_4_SHIFT	(NoConnection__4__SHIFT)
#define NoConnection_4_INTR	((uint16)((uint16)0x0003u << (NoConnection__4__SHIFT*2u)))

#define NoConnection_INTR_ALL	 ((uint16)(NoConnection_0_INTR| NoConnection_1_INTR| NoConnection_2_INTR| NoConnection_3_INTR| NoConnection_4_INTR))


#endif /* End Pins NoConnection_ALIASES_H */


/* [] END OF FILE */
