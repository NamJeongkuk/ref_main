/***************************************************************************/ /**
* \file CapTouch_Tuner.c
* \version 6.0
*
* \brief
*   This file provides the source code for the Tuner module API of the Component.
*
* \see CapTouch v6.0 Datasheet
*
*/
/*****************************************************************************
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

#include "cytypes.h"
#include "CyLib.h"
#include "CapTouch_Structure.h"
#include "CapTouch_Tuner.h"
#include "CapTouch_Control.h"
#include "CapTouch_Configuration.h"

/* Tuner states definition */
#define TU_FSM_RUNNING (0u)
#define TU_FSM_SUSPENDED (1u)
#define TU_FSM_SNR_TEST (2u)
#define TU_FSM_ONE_SCAN (3u)

/* Tuner state variable */
// static uint32 tunerFsm = TU_FSM_RUNNING;

/*******************************************************************************
* Function Name: CapTouch_TuInitialize
****************************************************************************/
/**
*
* \brief
*   This function initializes the communication interface with the Tuner GUI.
*
* \details
*   This API sets the tuner state machine to the default state. It is called by
*   the CapTouch_Start() API.
*
*******************************************************************************/
void CapTouch_TuInitialize(void)
{
}

/*******************************************************************************
* Function Name: CapTouch_RunTuner
****************************************************************************/
/**
*
* \brief
*  Establishes synchronized communication with the Tuner application.
*
* \details
*  This function is used to establish synchronized communication between the
*  CapTouch Component and Tuner application (or other host controllers).
*  This function is called periodically in the application program loop
*  to serve the Tuner application (or host controller) requests and commands.
*  In most cases, the best place to call this function is after processing and
*  before next scanning.
*
*  If this function is absent in the application program, then communication is
*  asynchronous and the following disadvantages are applicable:
*    - The raw counts displayed in the tuner may be filtered and/or unfiltered.
*      As a result, noise and SNR measurements will not be accurate.
*    - The Tuner tool may read the sensor data such as raw counts from a scan
*      multiple times, as a result, noise and SNR measurement will not be
*      accurate.
*    - The Tuner tool and host controller should not change the Component parameters
*      via the tuner interface. Changing the Component parameters via the tuner
*      interface in the async mode will result in Component abnormal behavior.
*
*  Note that calling this function is not mandatory for the application, but
*  required only to synchronize the communication with the host controller or
*  tuner application.
*
* \return
*  In some cases, the application program may need to know if the Component was
*  re-initialized. The return indicates if a restart command was executed or not:
*    - CapTouch_STATUS_RESTART_DONE - Based on a received command, the
*      Component was restarted.
*    -  CapTouch_STATUS_RESTART_NONE - No restart was executed by this
*      function.
*
*******************************************************************************/
uint32 CapTouch_RunTuner(void)
{
   return 0;
}

/* [] END OF FILE */
