/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_qetouch_diagnostics_if.h
 * Description  : This file contains the API typedefs and should be included by the application which uses this API.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *           01.31.2019 0.1     Initial development complete. Full functional testing not performed
 *           02.08.2019 0.2     Peer code review held and changes implemented to comment fields of all function blocks
 *                              Also, in diagnostics.c file., change in line 104 to not support single
 *                              scan mode for self at this time
 *           02.15.2019 0.3		changed mode retrieval mechanism for method to use control call for both all and single
 *           					modified Single API call to add in needed method passed parameter
 *           					removed e_diaginfo_t for mode retrieval as it is no longer needed
 *           03.08.2019 0.4		modified CTSU register check to use API call for last run method and stored register
 *           					values. This keeps register values concurrent with the scan data after every scan is
 *           					complete
 *           03.21.2019 0.5		Modified loop iterator in mutual to prevent checking over-bounds array of retrieved
 *           					channels. Modified API function call for single channel scans to accept uint8 as the
 *           					channel argument output by the QE tool. Modified the return value for successful CTSU
 *           					register scan to pass back correct value for API
 *           03.29.2019 0.6		Implemented CRC check and checksum calculation functions. Reviewed diagnostic return
 *           					codes in e_diag_t for better description and removed unused codes.
 *           04.11.2019 0.7		Implemented #define structures for inclusion of the 3 levels of evaluation needed for
 *           					safety compliance. Validation by touch data/register comparison is always enabled by
 *           					default if other checks are not included. Full 'Class B' certification requires all
 *           					3 levels of checks be enabled.
 *           04.22.2019 0.8		Integrated CRC functions into diagnostics source file to remove issue of user needing
 *           					to include the 60730 safety library to the code. Intent is to reduce any dependencies
 *           					on this module to outside sources. Also cleaned up conditional compilation #defines to
 *           					make code more readable. Removed method passed parameter from mutual_full_scan_all and
 *           					self_multi_all as it is not needed by these functions.
 *           04.22.2019 0.81	Added IPL and Module Stop checks as required by the FMEA analysis documents
 *           04.24.2019 0.82	Minor code modifications in register check section to add additional features. Modified
 *           					some comments and minimal code structure.
 *           04.25.2019 0.90	Cleanup to remove any dead code and comments. Module and support files are now fully GSCE
 *           					compliant. Integration with conditionals for BSP usage and use with new ported driver are
 *           					implemented.
 *           04.30.2019 0.95	Edits to BSP includes and removed inline expansion at end of file.
 *           04.30.2019 1.00	Minor edits to comments. Version prior to final test.
 *           05.07.2019 1.10	First release with QE 1.10 of driver.
 *
 ***********************************************************************************************************************/

#include "r_qetouch_diagnostics_cfg.h"

#ifndef QETOUCH_IF_DIAGNOSTICS
#define QETOUCH_IF_DIAGNOSTICS

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/
/* Driver Version Number. */
#define QETOUCH_DIAGNOSTICS_VERSION_MAJOR    (01)
#define QETOUCH_DIAGNOSTICS_VERSION_MINOR    (10)

/* Define max number of channels for test buffers. This is double for mutual for primary/secondary */
#define QETOUCH_IF_DIAGSELF_BUFFSIZE         (48)
#define QETOUCH_IF_DIAGMUTUAL_BUFFSIZE       (QETOUCH_IF_DIAGSELF_BUFFSIZE * 2)

/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/
typedef enum e_diag_err {
	DIAG_ALL_CHECK_SUCCESS = 				(100),
	DIAG_SENSOR_CHECK_SUCCESS = 			(101),
	DIAG_CTSU_NO_CONFIG_SENSORS = 			(102),
	DIAG_CTSU_SCAN_IN_PROGRESS = 			(103),
	DIAG_MUTUAL_FIRST_MEASUREMENT_FAIL = 	(104),
	DIAG_MUTUAL_SECOND_MEASUREMENT_FAIL = 	(105),
	DIAG_SELF_MEASUREMENT_FAIL = 			(106),
	DIAG_TS_CAP_FAIL = 						(107),
	DIAG_OVERFLOW_FLAG_TRIP_FAIL = 			(108),
	DIAG_CTSU_READ_FAIL = 					(109),
	DIAG_NO_MODE_DEFINED = 					(110),
	DIAG_CTSU_MODE_NOT_SUPPORTED = 			(111),
	DIAG_STATUS_REG_READ_FAIL = 			(112),
	DIAG_CTSU_REG_CHECK_SUCCESS = 			(113),
	DIAG_CTSU_MODULE_STOP_FAIL = 			(114),
	DIAG_CTSU_IEN_ZERO_FAIL = 				(115),
	DIAG_CTSU_IPR_ZERO_FAIL = 				(116),
	DIAG_CRC_ROUTINE_SUCCESS = 				(117),
	DIAG_CRC_ROUTINE_FAIL = 				(118),
	DIAG_CHECKSUM_ROUTINE_SUCCESS = 		(119),
	DIAG_CHECKSUM_ROUTINE_FAIL = 			(120)
} e_diagerr_t;

/***********************************************************************************************************************
 Exported global variables
 ***********************************************************************************************************************/
extern uint16_t g_diag_return_array[2];

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/
/* Application layer function definition */
e_diagerr_t R_CTSU_DiagAll(uint32_t method);
e_diagerr_t R_CTSU_DiagSingle(uint32_t method, uint8_t button_index);
uint32_t R_CTSU_DiagGetVersion(void);

#endif /* Conditional compilation protection define */
/* End QETOUCH_IF_DIAGNOSTICS_FILE */
