/*******************************************************************************
* File Name: Gea3_tx.h  
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

#if !defined(CY_PINS_Gea3_tx_ALIASES_H) /* Pins Gea3_tx_ALIASES_H */
#define CY_PINS_Gea3_tx_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Gea3_tx_0			(Gea3_tx__0__PC)
#define Gea3_tx_0_PS		(Gea3_tx__0__PS)
#define Gea3_tx_0_PC		(Gea3_tx__0__PC)
#define Gea3_tx_0_DR		(Gea3_tx__0__DR)
#define Gea3_tx_0_SHIFT	(Gea3_tx__0__SHIFT)
#define Gea3_tx_0_INTR	((uint16)((uint16)0x0003u << (Gea3_tx__0__SHIFT*2u)))

#define Gea3_tx_INTR_ALL	 ((uint16)(Gea3_tx_0_INTR))


#endif /* End Pins Gea3_tx_ALIASES_H */


/* [] END OF FILE */
