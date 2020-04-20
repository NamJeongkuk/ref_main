/***************************************************************************//**
* \file CapTouch_Filter.h
* \version 6.0
*
* \brief 
*   This file contains the definitions for all firmware filters
*   implementation.
*
* \see CapTouch v6.0 Datasheet
*
*//*****************************************************************************
* Copyright (2016-2018), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#if !defined(CY_SENSE_CapTouch_FILTER_H)
#define CY_SENSE_CapTouch_FILTER_H

#include "cytypes.h"
#include "CyLib.h"
#include "cyfitter.h"

#include "CapTouch_Structure.h"
#include "CapTouch_Configuration.h"

/***************************************
* Function Prototypes
***************************************/

/*******************************************************************************
* LOW LEVEL API
*******************************************************************************/
/**
* \cond (SECTION_C_LOW_LEVEL || SECTION_I_LOW_LEVEL)
* \addtogroup group_c_low_level
* \{
*/

cystatus CapTouch_UpdateAllBaselines(void);
cystatus CapTouch_UpdateWidgetBaseline(uint32 widgetId);
cystatus CapTouch_UpdateSensorBaseline(uint32 widgetId, uint32 sensorId);

void CapTouch_InitializeAllBaselines(void);
void CapTouch_InitializeWidgetBaseline(uint32 widgetId);
void CapTouch_InitializeSensorBaseline(uint32 widgetId, uint32 sensorId);

#if ((CapTouch_ENABLE == CapTouch_RC_FILTER_EN) || \
     (0u != (CapTouch_CSD_AUTOTUNE & CapTouch_CSD_SS_TH_EN)))
    void CapTouch_InitializeAllFilters(void);
    void CapTouch_InitializeWidgetFilter(uint32 widgetId);
#endif /* ((CapTouch_ENABLE == CapTouch_RC_FILTER_EN) || \
           (0u != (CapTouch_CSD_AUTOTUNE & CapTouch_CSD_SS_TH_EN))) */

/** \}
* \endcond */


/*******************************************************************************
* Function Prototypes - internal functions
*******************************************************************************/
/**
* \cond SECTION_C_INTERNAL
* \addtogroup group_c_internal
* \{
*/

void CapTouch_FtInitialize(void);

#if (0u != (CapTouch_CSD_AUTOTUNE & CapTouch_CSD_SS_TH_EN))
    void CapTouch_RunNoiseEnvelope(uint32 widgetId, uint32 sensorId);
    void CapTouch_InitializeNoiseEnvelope(uint32 widgetId, uint32 sensorId);
#endif /* (0u != (CapTouch_CSD_AUTOTUNE & CapTouch_CSD_SS_TH_EN)) */

#if (CapTouch_REGULAR_RC_IIR_FILTER_EN || CapTouch_PROX_RC_IIR_FILTER_EN)
    void CapTouch_InitializeIIR(uint32 widgetId, uint32 sensorId);
    void CapTouch_RunIIR(uint32 widgetId, uint32 sensorId);
#endif /* (CapTouch_REGULAR_RC_IIR_FILTER_EN || CapTouch_PROX_RC_IIR_FILTER_EN) */

#if (CapTouch_REGULAR_RC_MEDIAN_FILTER_EN || CapTouch_PROX_RC_MEDIAN_FILTER_EN)
    void CapTouch_InitializeMedian(uint32 widgetId, uint32 sensorId);
    void CapTouch_RunMedian(uint32 widgetId, uint32 sensorId);
#endif /* (CapTouch_REGULAR_RC_MEDIAN_FILTER_EN || CapTouch_PROX_RC_MEDIAN_FILTER_EN) */

#if (CapTouch_REGULAR_RC_AVERAGE_FILTER_EN || CapTouch_PROX_RC_AVERAGE_FILTER_EN)
    void CapTouch_InitializeAverage(uint32 widgetId, uint32 sensorId);
    void CapTouch_RunAverage(uint32 widgetId, uint32 sensorId);
#endif /* (CapTouch_REGULAR_RC_AVERAGE_FILTER_EN || CapTouch_PROX_RC_AVERAGE_FILTER_EN) */

void CapTouch_FtInitializeBaseline(CapTouch_RAM_SNS_STRUCT *ptrSensor, uint32 wdType);
uint32 CapTouch_FtUpdateBaseline(
                            CapTouch_RAM_WD_BASE_STRUCT *ptrWidgetRam,
                            CapTouch_RAM_SNS_STRUCT *ptrSensor,
                            uint32 wdType);

#if (CapTouch_POS_MEDIAN_FILTER_EN || CapTouch_REGULAR_RC_MEDIAN_FILTER_EN || CapTouch_PROX_RC_MEDIAN_FILTER_EN)
uint32 CapTouch_FtMedian(uint32 x1, uint32 x2, uint32 x3);
#endif /*CapTouch_POS_MEDIAN_FILTER_EN || CapTouch_REGULAR_RC_MEDIAN_FILTER_EN || CapTouch_PROX_RC_MEDIAN_FILTER_EN*/

uint32 CapTouch_FtIIR1stOrder(uint32 input, uint32 prevOutput, uint32 n, uint32 shift);

#if (CapTouch_POS_JITTER_FILTER_EN)
    uint32 CapTouch_FtJitter(uint32 input, uint32 prevOutput);
#endif /* CapTouch_POS_JITTER_FILTER_EN */

void CapTouch_FtInitializeBaselineChannel(CapTouch_RAM_SNS_STRUCT *ptrSensor, uint32 wdType, uint32 channel);

#if (CapTouch_ENABLE == CapTouch_RC_FILTER_EN)
    void CapTouch_FtRunEnabledFilters(uint32 widgetId, uint32 sensorId);
    void CapTouch_FtRunEnabledFiltersInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                                      CapTouch_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
#endif /* (CapTouch_ENABLE == CapTouch_RC_FILTER_EN) */


#if (CapTouch_REGULAR_RC_IIR_FILTER_EN || CapTouch_PROX_RC_IIR_FILTER_EN)
    void CapTouch_InitializeIIRInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                                  CapTouch_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
    void CapTouch_RunIIRInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                           CapTouch_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
#endif /* (CapTouch_REGULAR_RC_IIR_FILTER_EN || CapTouch_PROX_RC_IIR_FILTER_EN) */

#if (CapTouch_REGULAR_RC_MEDIAN_FILTER_EN || CapTouch_PROX_RC_MEDIAN_FILTER_EN)
    void CapTouch_InitializeMedianInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                                     CapTouch_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
    void CapTouch_RunMedianInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                              CapTouch_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
#endif /* (CapTouch_REGULAR_RC_MEDIAN_FILTER_EN || CapTouch_PROX_RC_MEDIAN_FILTER_EN) */

#if (CapTouch_REGULAR_RC_AVERAGE_FILTER_EN || CapTouch_PROX_RC_AVERAGE_FILTER_EN)
    void CapTouch_InitializeAverageInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                                      CapTouch_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
    void CapTouch_RunAverageInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                               CapTouch_RAM_SNS_STRUCT *ptrSensorObj, uint32 wdType);
#endif /* (CapTouch_REGULAR_RC_AVERAGE_FILTER_EN || CapTouch_PROX_RC_AVERAGE_FILTER_EN) */

#if (CapTouch_ENABLE == CapTouch_ALP_FILTER_EN)
    void CapTouch_InitializeALP(uint32 widgetId, uint32 sensorId);
    void CapTouch_RunALP(uint32 widgetId, uint32 sensorId);
    void CapTouch_InitializeALPInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                                CapTouch_RAM_SNS_STRUCT *ptrSensorObj,
                                                uint32 wdType);
    void CapTouch_ConfigRunALPInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                        CapTouch_RAM_WD_BASE_STRUCT *ptrRamWdgt,
                                        CapTouch_RAM_SNS_STRUCT *ptrSensorObj,
                                        uint32 wdType);
    void CapTouch_RunALPInternal(CapTouch_PTR_FILTER_VARIANT ptrFilterHistObj,
                                        ALP_FLTR_CONFIG_STRUCT *ptrAlpFilterConfig,
                                        CapTouch_RAM_SNS_STRUCT *ptrSensorObj,
                                        uint32 wdType);
#endif /* (CapTouch_ENABLE == CapTouch_ALP_FILTER_EN) */

/** \}
* \endcond */

/***************************************
* Initial Parameter Constants
***************************************/
#define NOISE_ENVELOPE_SHIFT                        (0x02u)
#define NOISE_ENVELOPE_RUN_DIFF_SHIFT               (0x03u)
#define NOISE_ENVELOPE_SIGN_REG                     (0x0Fu)
#define NOISE_ENVELOPE_SIGN_REG_SHIFT               (0x01u)
#define NOISE_ENVELOPE_RESET_COUNTER                (0x0Au)
#define NOISE_ENVELOPE_4_TIMES                      (0x02u)

#endif /* End CY_SENSE_CapTouch_FILTER_H */


/* [] END OF FILE */
