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
/*********************************************************************************************************************
 * File Name    : r_qetouch_diagnostics.c
 * Description  : This file contains code for the qe_touch diagnostics API
 ***********************************************************************************************************************
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
/***********************************************************************************************************************
 Includes
 ***********************************************************************************************************************/

#include "r_smc_entry.h"
#include "r_ctsu_qe_if.h"
#include "r_touch_qe_if.h"
#include "r_qetouch_diagnostics_if.h"

/* Macro Definitions */
#if (QE_CFG_DIAG_PERFORM_CHECKSUM == 1)
#define QE_PRV_CHECKSUM_RANGE_NUM (sizeof(gs_qe_checksum_ranges)/sizeof(st_qe_checksum_range_t))
#endif /* Conditional compilation end */

#if (QE_CFG_DIAG_PERFORM_CRC == 1)
#define QE_PRV_CRC_RANGE_NUM (sizeof(gs_qe_crc_ranges)/sizeof(st_qe_crc_range_t))
#endif /* Conditional compilation end */

/* Local Typedef Definitions */
#if (QE_CFG_DIAG_PERFORM_CRC == 1)
typedef struct QE_CRC_RANGE
{
	uint32_t start;
	uint32_t end;
}st_qe_crc_range_t;
#endif

#if (QE_CFG_DIAG_PERFORM_CHECKSUM == 1)
typedef struct QE_CHECKSUM_RANGE
{
	uint32_t start;
	uint32_t end;
}st_qe_checksum_range_t;
#endif

#if (QE_CFG_DIAG_PERFORM_CRC == 1)
/* Arrays for holding an address range for CRC calculations */
/* Code and CONST sections that are verified with CRC. DO NOT MODIFY ORDER OF ARRAY */
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
static const st_qe_crc_range_t gs_qe_crc_ranges[] =
{
	/* these are for the ROM CRC Evaluation */
	{
		/* Casting is intentional here */
		(uint32_t)__sectop("P_QE_TOUCH_DRIVER"), (uint32_t)__secend("P_QE_TOUCH_DRIVER")-1
	},

	{
		/* Casting is intentional here */
		(uint32_t)__sectop("C_QE_TOUCH_SAFETY_CONST"), (uint32_t)__secend("C_QE_TOUCH_SAFETY_CONST")-1
	},

	{
		/* Casting is intentional here */
		(uint32_t)__sectop("C_QE_TOUCH_SAFETY_CONST_1"), (uint32_t)__secend("C_QE_TOUCH_SAFETY_CONST_1")-1
	},

	{
		/* Casting is intentional here */
		(uint32_t)__sectop("C_QE_TOUCH_SAFETY_CONST_2"), (uint32_t)__secend("C_QE_TOUCH_SAFETY_CONST_2")-1
	}
};
#else
R_BSP_SECTION_OPERATORS_INIT(P_QE_TOUCH_DRIVER);
R_BSP_SECTION_OPERATORS_INIT(C_QE_TOUCH_SAFETY_CONSTANT);
R_BSP_SECTION_OPERATORS_INIT(C_QE_TOUCH_SAFETY_CONSTANT_1);
R_BSP_SECTION_OPERATORS_INIT(C_QE_TOUCH_SAFETY_CONSTANT_2);
static const st_qe_crc_range_t gs_qe_crc_ranges[] =
{
	/* these are for the ROM CRC Evaluation */
	{
		/* Casting is intentional here */
		(uint32_t)R_BSP_SECTOP(P_QE_TOUCH_DRIVER), (uint32_t)R_BSP_SECEND(P_QE_TOUCH_DRIVER)-1
	},

	{
		/* Casting is intentional here */
		(uint32_t)R_BSP_SECTOP(C_QE_TOUCH_SAFETY_CONSTANT), (uint32_t)R_BSP_SECEND(C_QE_TOUCH_SAFETY_CONSTANT)-1
	},

	{
		/* Casting is intentional here */
		(uint32_t)R_BSP_SECTOP(C_QE_TOUCH_SAFETY_CONSTANT_1), (uint32_t)R_BSP_SECEND(C_QE_TOUCH_SAFETY_CONSTANT_1)-1
	},

	{
		/* Casting is intentional here */
		(uint32_t)R_BSP_SECTOP(C_QE_TOUCH_SAFETY_CONSTANT_2), (uint32_t)R_BSP_SECEND(C_QE_TOUCH_SAFETY_CONSTANT_2)-1
	}
};
#endif /* BSP inclusion end */
#endif /* Conditional compilation end */

#if (QE_CFG_DIAG_PERFORM_CHECKSUM == 1)
/* Arrays for holding an address range for RAM Checksum calculations */
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
static const st_qe_checksum_range_t gs_qe_checksum_ranges[] =
{
	/* these are for the checksum Evaluation */
	{
		/* Casting is intentional here */
		(uint32_t)__sectop("R_QE_TOUCH_SAFETY_RAMINIT"), (uint32_t)__secend("R_QE_TOUCH_SAFETY_RAMINIT")-1
	},

	{
		/* Casting is intentional here */
		(uint32_t)__sectop("R_QE_TOUCH_SAFETY_RAMINIT_2"), (uint32_t)__secend("R_QE_TOUCH_SAFETY_RAMINIT_2")-1
	},

	{
		/* Casting is intentional here */
		(uint32_t)__sectop("B_QE_TOUCH_SAFETY_RAM_2"), (uint32_t)__secend("B_QE_TOUCH_SAFETY_RAM_2")-1
	}
};
#else
R_BSP_SECTION_OPERATORS_INIT(D_QE_TOUCH_SAFETY_RAMINIT);
R_BSP_SECTION_OPERATORS_INIT(D_QE_TOUCH_SAFETY_RAMINIT_2);
R_BSP_SECTION_OPERATORS_INIT(B_QE_TOUCH_SAFETY_RAM_2);
static const st_qe_checksum_range_t gs_qe_checksum_ranges[] =
{
	/* these are for the checksum Evaluation */
	{
		/* Casting is intentional here */
		#if defined(__CCRX__)
		(uint32_t)R_BSP_SECTOP(R_QE_TOUCH_SAFETY_RAMINIT), (uint32_t)R_BSP_SECEND(R_QE_TOUCH_SAFETY_RAMINIT)-1
		#else
		(uint32_t)R_BSP_SECTOP(D_QE_TOUCH_SAFETY_RAMINIT), (uint32_t)R_BSP_SECEND(D_QE_TOUCH_SAFETY_RAMINIT)-1
		#endif
	},

	{
		/* Casting is intentional here */
		#if defined(__CCRX__)
		(uint32_t)R_BSP_SECTOP(R_QE_TOUCH_SAFETY_RAMINIT_2), (uint32_t)R_BSP_SECEND(R_QE_TOUCH_SAFETY_RAMINIT_2)-1
		#else
		(uint32_t)R_BSP_SECTOP(D_QE_TOUCH_SAFETY_RAMINIT_2), (uint32_t)R_BSP_SECEND(D_QE_TOUCH_SAFETY_RAMINIT_2)-1
		#endif
	},

	{
		/* Casting is intentional here */
		(uint32_t)R_BSP_SECTOP(B_QE_TOUCH_SAFETY_RAM_2), (uint32_t)R_BSP_SECEND(B_QE_TOUCH_SAFETY_RAM_2)-1
	}
};
#endif /* BSP inclusion end */
#endif /* Conditional compilation end */

/* Private (static) global variables */
#if (QE_CFG_DIAG_PERFORM_CHECKSUM == 1)
static bool	gs_first_checksum = true;
static uint16_t gs_stored_checksum = 0;
#endif

#if (QE_CFG_DIAG_PERFORM_CRC == 1)
static bool gs_first_crc = true;
static uint16_t	gs_stored_crc = 0x1234;
#endif

/* Private functions */
static e_diagerr_t diag_self_cap_multi_all(void);
static e_diagerr_t diag_self_cap_multi_sngl(uint8_t index);
static e_diagerr_t diag_mutual_cap_full_scan_all(void);
static e_diagerr_t diag_mutual_cap_full_scan_sngl(uint8_t index);
static e_diagerr_t check_ctsu_registers(void);

#if (QE_CFG_DIAG_PERFORM_CHECKSUM == 1)
static e_diagerr_t driver_ram_checksum(void);
#endif

#if (QE_CFG_DIAG_PERFORM_CRC == 1)
static e_diagerr_t check_driver_crc(void);
static void qe_crc_add_range(const uint8_t* pui8_Data, uint32_t ui32_Length);
#endif

/* Globals needed for module error return data */
uint16_t g_diag_return_array[2] = { 0u, 0u };

/******************************************************************************
 * Function Name : R_CTSU_DiagAll
 *
 * Description   : Application function that performs diagnostic scanning
 * 				 against Renesas FMEA matrix. Retrieves the current method
 * 				 operational mode and method. This is needed to understand how
 * 				 to configure the subsequent test and buffers. Then, this
 * 				 function calls scans for all enabled channels in the
 * 				 configuration (method) that is passed and returns the status
 * 				 of this evaluation. Finally CTSU error/status registers are
 * 				 evaluated and status is returned to calling function.
 *
 *				 Internal mode retrieval is performed using CTSU control call and
 *				 provides this data from ctsu_mode_t
 *                  CTSU_MODE_SELF_SINGLE_SCAN   - Self single scan (currently not supported)
 *                  CTSU_MODE_SELF_MULTI_SCAN   - Self multi-scan
 *                  CTSU_MODE_MUTUAL_FULL_SCAN    - Mutual scan
 *                  DIAG_NO_MODE_DEFINED     - invalid parameter returned
 *
 * Arguments :   method - configuration that is to be scanned. This is found
 *               in the QE tuning output files
 *
 * Return Value(s) : e_diagerr_t which will reflect the following error codes:
 *
 *                 DIAG_ALL_CHECK_SUCCESS - All checks successful
 *                 DIAG_CTSU_UNCONFIGURED - CTSU is not configured
 *                 DIAG_CONFIG_SUCCESS - no issues found with operation
 *                 DIAG_SENSOR_CHECK_SUCCESS - sensors evaluated correctly
 *                 DIAG_CTSU_REG_CHECK_SUCCESS - CTSU registers evaluated correctly
 *                 DIAG_CTSU_NO_CONFIG_SENSORS - no sensors enabled in config
 *                 DIAG_MUTUAL_FIRST_MEASUREMENT_FAIL
 *                 DIAG_MUTUAL_SECOND_MEASUREMENT_FAIL
 *                 DIAG_SELF_MEASUREMENT_FAIL
 *                 DIAG_TS_CAP_FAIL
 *                 DIAG_OVERFLOW_FLAG_TRIP_FAIL
 *                 DIAG_CTSU_READ_FAIL - cannot get data from scan
 *                 DIAG_NO_MODE_DEFINED - invalid CTSU mode
 *                 DIAG_STATUS_REG_READ_FAIL - cannot get CTSU reg data
 *                 DIAG_CTSU_SCAN_IN_PROGRESS
 *
 * Usage Note: This MUST be called AFTER the update data routine is called to
 * 			   ensure data concurrency.
 ******************************************************************************/
e_diagerr_t R_CTSU_DiagAll (uint32_t method)
{
    e_diagerr_t diag_err;
    ctsu_mode_t mode;
    qe_err_t qe_err;

    /* get current CTSU operating mode for this method */
    qe_err = R_CTSU_Control(CTSU_CMD_GET_METHOD_MODE, method, &mode);

    /* Is a scan taking place? */
    if ( QE_ERR_BUSY == qe_err )
    {
    	return DIAG_CTSU_SCAN_IN_PROGRESS;
    }

    /* based on mode, execute the diagnostic  required */
    switch (mode)
    {
        case CTSU_MODE_SELF_MULTI_SCAN:
        {
        	diag_err = diag_self_cap_multi_all();
            break;
        }
        case CTSU_MODE_MUTUAL_FULL_SCAN:
        {
        	diag_err = diag_mutual_cap_full_scan_all();
            break;
        }
        case CTSU_MODE_SELF_SINGLE_SCAN:
        {
            /* this mode is not defined at this time */
            return DIAG_CTSU_MODE_NOT_SUPPORTED;
            break;
        }
        default:
        {
            /* if here, then error has occurred */
        	return DIAG_NO_MODE_DEFINED;
        }
    }

    if (DIAG_SENSOR_CHECK_SUCCESS != diag_err)
    {
        return diag_err;
    }

    /* Check the CTSU registers themselves */
    diag_err = check_ctsu_registers();
    if (DIAG_CTSU_REG_CHECK_SUCCESS != diag_err)
    {
        return diag_err;
    }

    /* Perform checksum on critical driver RAM */
#if (QE_CFG_DIAG_PERFORM_CHECKSUM == 1)
    diag_err = driver_ram_checksum();
    if (DIAG_CHECKSUM_ROUTINE_SUCCESS != diag_err)
    {
        return DIAG_CHECKSUM_ROUTINE_FAIL;
    }
#endif

    /* Perform CRC on critical driver code and CONST values */
#if (QE_CFG_DIAG_PERFORM_CRC == 1)
    diag_err = check_driver_crc();
    if (DIAG_CRC_ROUTINE_SUCCESS != diag_err)
    {
        return diag_err;
    }
#endif

    /* if all checks passed to this point, return success */
    return DIAG_ALL_CHECK_SUCCESS;
}
/******************************************************************************
 End of function R_CTSU_DiagAll
 ******************************************************************************/

/******************************************************************************
 * Function Name : R_CTSU_DiagSingle
 *
 * Description   : Application function that performs diagnostic scanning against
 *               Renesas FMEA matrix. This function scans ONLY ONE channel in
 *               the configuration (method) that is passed and returns the
 *               status of this evaluation. Operational mode is determined
 *               of the method that is passed. This is needed to understand how
 * 				 to configure the subsequent test and buffers. Then, this
 * 				 function calls scans for only that enabled channel in the
 * 				 configuration (method) that is passed and returns the status
 * 				 of this evaluation. Finally CTSU error/status registers are
 * 				 evaluated and status is returned to calling function.
 *
 * 				 Internal mode retrieval is performed using CTSU control call and
 *				 provides this data from ctsu_mode_t
 *                  CTSU_MODE_SELF_SINGLE_SCAN   - Self single scan (currently not supported)
 *                  CTSU_MODE_SELF_MULTI_SCAN   - Self multi-scan
 *                  CTSU_MODE_MUTUAL_FULL_SCAN    - Mutual scan
 *                  DIAG_NO_MODE_DEFINED     - invalid parameter returned
 *
 * Arguments     : button_index - index of the single button to be checked
 * 			  	 which is available in the QE tuned output files
 *
 *               method - configuration that is to be scanned. This is found
 *               in the QE tuning output files
 *
 * Return Value(s) : e_diagerr_t which will reflect the following error codes:
 *
 *                 DIAG_ALL_CHECK_SUCCESS - All checks successful
 *                 DIAG_CTSU_UNCONFIGURED - CTSU is not configured
 *                 DIAG_SENSOR_CHECK_SUCCESS - sensors evaluated correctly
 *                 DIAG_CTSU_REG_CHECK_SUCCESS - CTSU registers evaluated correctly
 *                 DIAG_CTSU_NO_CONFIG_SENSORS - no sensors enabled in config
 *                 DIAG_MUTUAL_FIRST_MEASUREMENT_FAIL
 *                 DIAG_MUTUAL_SECOND_MEASUREMENT_FAIL
 *                 DIAG_SELF_MEASUREMENT_FAIL
 *                 DIAG_TS_CAP_FAIL
 *                 DIAG_OVERFLOW_FLAG_TRIP_FAIL
 *                 DIAG_CTSU_READ_FAIL - cannot get data from scan
 *                 DIAG_NO_MODE_DEFINED - invalid CTSU mode
 *                 DIAG_STATUS_REG_READ_FAIL - cannot get CTSU reg data
 *                 DIAG_CTSU_SCAN_IN_PROGRESS
 *
 * Usage Note: This MUST be called AFTER the update data routine is called to
 * 			   ensure data concurrency.
 ******************************************************************************/
e_diagerr_t R_CTSU_DiagSingle (uint32_t method, uint8_t button_index)
{
    e_diagerr_t diag_err;
    ctsu_mode_t mode;
    qe_err_t qe_err;

    /* get current CTSU operating mode for this method */
    qe_err = R_CTSU_Control(CTSU_CMD_GET_METHOD_MODE, method, &mode);

    /* Is a scan taking place? */
    if ( QE_ERR_BUSY == qe_err )
    {
    	return DIAG_CTSU_SCAN_IN_PROGRESS;
    }

    /* based on mode, execute the diagnostic  required */
    switch (mode)
    {
        case CTSU_MODE_SELF_MULTI_SCAN:
        {
        	diag_err = diag_self_cap_multi_sngl(button_index);
            break;
        }
        case CTSU_MODE_MUTUAL_FULL_SCAN:
        {
        	diag_err = diag_mutual_cap_full_scan_sngl(button_index);
            break;
        }
        case CTSU_MODE_SELF_SINGLE_SCAN:
        {
            /* currently this mode is not implemented, so return failure */
            return DIAG_CTSU_MODE_NOT_SUPPORTED;
            break;
        }
        default:
        {
            return DIAG_NO_MODE_DEFINED;
        }
    }

    if (DIAG_SENSOR_CHECK_SUCCESS != diag_err)
    {
        return diag_err;
    }

    /* Check the CTSU registers themselves */
    diag_err = check_ctsu_registers();
    if (DIAG_CTSU_REG_CHECK_SUCCESS != diag_err)
    {
        return diag_err;
    }

    /* Perform checksum on critical driver RAM */
#if (QE_CFG_DIAG_PERFORM_CHECKSUM == 1)
    diag_err = driver_ram_checksum();
    if (DIAG_CHECKSUM_ROUTINE_SUCCESS != diag_err)
    {
        return DIAG_CHECKSUM_ROUTINE_FAIL;
    }
#endif

    /* Perform CRC on critical driver code and CONST values */
#if (QE_CFG_DIAG_PERFORM_CRC == 1)
    diag_err = check_driver_crc();
    if (DIAG_CRC_ROUTINE_SUCCESS != diag_err)
    {
        return diag_err;
    }
#endif

    return DIAG_ALL_CHECK_SUCCESS;
}
/******************************************************************************
 End of function R_CTSU_DiagSingle
 ******************************************************************************/


/******************************************************************************
 * Function Name: diag_self_cap_multi_all
 *
 * Description : Performs scan on CTSU values returned from driver and validates
 *               them based on define constants from .h file. Only the sensor
 *               counter is used for validating against the FMEA matrix.
 *
 *               Instances local buffer for data storage as follows:
 *               In self mode, there is a MAX of 36 channels, so array is
 *               defined by QETOUCH_IF_DIAGSELF_BUFFSIZE
 *
 * Arguments :   method - configuration that is to be scanned. This is found
 *               in the QE tuning output files
 *
 * Return Value(s) : e_diagerr_t which will reflect the following error codes:
 *
 *                 DIAG_SENSOR_CHECK_SUCCESS - sensors evaluated correctly
 *                 DIAG_CTSU_NO_CONFIG_SENSORS - no sensors enabled in config
 *                 DIAG_SELF_MEASUREMENT_FAIL
 *                 DIAG_CTSU_READ_FAIL - cannot get data from scan
 *
 * Usage Note: internal API function not intended to be called from application
  ******************************************************************************/
static e_diagerr_t diag_self_cap_multi_all (void)
{
    qe_err_t err;
    uint16_t buf[QETOUCH_IF_DIAGSELF_BUFFSIZE] =
    { 0 };
    uint16_t cnt;

    /** get data from the last scan for all elements. if anything other than
     * success is returned from this call, then fail the diag check */
    err = R_CTSU_ReadData(buf, &cnt);
    if (QE_SUCCESS != err)
    {
        return DIAG_CTSU_READ_FAIL;
    }

    /* put a fail-safe check here if there are no enabled channels, then return error */
    if (0 == cnt)
    {
        return DIAG_CTSU_NO_CONFIG_SENSORS;
    }

    /* ensure error return array is reset each time thru */
    g_diag_return_array[0] = 0;
    g_diag_return_array[1] = 0;

    /* setup loop variables */
    uint8_t i;
    uint8_t j = 0;
    for (i = 0; i < cnt; i++)
    {
        /* first check Sensor Counter */
        /* covers HW Self Capacitance line items: 1.1.0, 1.1.1, 1.1.2, 1.1.3, 1.1.4 */
        if (buf[j] >= SELFCAP_CFG_UPPERLIMIT)
        {
            /* if error, load array elements */
            g_diag_return_array[0] = buf[j];
            g_diag_return_array[1] = i;
            return DIAG_SELF_MEASUREMENT_FAIL;
        }

        /* covers HW Self Capacitance line items: 1.1.5, 1.1.6 */
        if (buf[j] <= SELFCAP_CFG_LOWERLIMIT)
        {
            /* if error, load array elements */
            g_diag_return_array[0] = buf[j];
            g_diag_return_array[1] = i;
            return DIAG_SELF_MEASUREMENT_FAIL;
        }

        /* increment to next position for Reference Sensor count */
        j++;
    }

    return DIAG_SENSOR_CHECK_SUCCESS;
}
/******************************************************************************
 End of function diag_self_cap_multi_all
 ******************************************************************************/

/******************************************************************************
 * Function Name: diag_self_cap_multi_sngl
 *
 * Description : Performs scan on a single sensor (button) against the FMEA
 *               matrix. If errors are encountered, then return those up so
 *               they can be passed to application for action. This is for
 *               SELF mode only.
 *
 * Arguments :  index - value of the sensor to evaluate that is passed from
 *              the application. This is found in the QE tuning output parameter
 *              files.
 *
 * Return Value(s) : e_diagerr_t which will reflect the following error codes:
 *
 *                 DIAG_SENSOR_CHECK_SUCCESS - sensors evaluated correctly
 *                 DIAG_CTSU_REG_CHECK_SUCCESS - CTSU registers evaluated correctly
 *                 DIAG_CTSU_NO_CONFIG_SENSORS - no sensors enabled in config
 *                 DIAG_SELF_MEASUREMENT_FAIL
 *                 DIAG_CTSU_READ_FAIL - cannot get data from scan
 *
 * Usage Note: internal API function not intended to be called from application
  ******************************************************************************/
static e_diagerr_t diag_self_cap_multi_sngl (uint8_t index)
{
    qe_err_t err;
    uint16_t sensor_val;

    /* retrieve sensor data from previous scan */
    err = R_CTSU_ReadElement(index, &sensor_val, NULL);

    /* put a fail-safe check here CTSU cannot read, then return error */
    if (QE_SUCCESS != err)
    {
        return DIAG_CTSU_READ_FAIL;
    }

    /* ensure error return array is reset each time thru */
    g_diag_return_array[0] = 0;
    g_diag_return_array[1] = 0;

    /* first check Sensor Counter */
    /* covers HW Self Capacitance line items: 1.1.0, 1.1.1, 1.1.2, 1.1.3, 1.1.4 */
    if (sensor_val >= SELFCAP_CFG_UPPERLIMIT)
    {
        /* cast here intentionally */
        g_diag_return_array[0] = sensor_val;
        g_diag_return_array[1] = index;
        return DIAG_SELF_MEASUREMENT_FAIL;
    }

    /* covers HW Self Capacitance line items: 1.1.5, 1.1.6 */
    if (sensor_val <= SELFCAP_CFG_LOWERLIMIT)
    {
        /* cast here intentionally */
        g_diag_return_array[0] = sensor_val;
        g_diag_return_array[1] = index;
        return DIAG_SELF_MEASUREMENT_FAIL;
    }

    return DIAG_SENSOR_CHECK_SUCCESS;
}
/******************************************************************************
 End of function diag_self_cap_multi_sngl
 ******************************************************************************/

/******************************************************************************
 * Function Name: diag_mutual_cap_full_scan_all
 *
 * Description : Performs scan on all sensors (button) against the FMEA
 *               matrix. If errors are encountered, then return those up so
 *               they can be passed to application for action.
 *
 *               Instances local buffer for data storage as follows:
 *               For mutual, the max local data structure size is: 36 ch x 2
 *               bits or 72 16-bit words for primary and secondary sensor
 *               counter values
 *
 * Arguments :  method - value of the sensor to evaluate that is passed from
 *              the application. This is found in the QE tuning output parameter
 *              files.
 *
 * Return Value(s) : e_diagerr_t which will reflect the following error codes:
 *
 *                 DIAG_SENSOR_CHECK_SUCCESS - sensors evaluated correctly
 *                 DIAG_CTSU_NO_CONFIG_SENSORS - no sensors enabled in config
 *                 DIAG_MUTUAL_FIRST_MEASUREMENT_FAIL
 *                 DIAG_MUTUAL_SECOND_MEASUREMENT_FAIL
 *                 DIAG_CTSU_READ_FAIL - cannot get data from scan
 *
 * Usage Note: internal API function not intended to be called from application
  ******************************************************************************/
static e_diagerr_t diag_mutual_cap_full_scan_all (void)
{
    qe_err_t err;
    uint16_t buf[QETOUCH_IF_DIAGMUTUAL_BUFFSIZE] =
    { 0 };
    uint16_t cnt;

    /** get data from the last scan for all elements. if anything other than
     * success is returned from this call, then fail the diag check */
    err = R_CTSU_ReadData(buf, &cnt);
    if (QE_SUCCESS != err)
    {
        return DIAG_CTSU_READ_FAIL;
    }

    /* put a fail-safe check here if there are no enabled channels, then return error */
    if (0 == cnt)
    {
        return DIAG_CTSU_NO_CONFIG_SENSORS;
    }

    /* ensure error return array is reset each time thru */
    g_diag_return_array[0] = 0;
    g_diag_return_array[1] = 0;

    /* setup loop variables */
    uint8_t k = 0;
    uint16_t l = 0;
    for (l = 0; l < cnt;)
    {
        /* First Sensor Counter Measurement */
        /* FMEA 2.1.1, 2.1.2, 2.1.3, 2.1.4, 2.1.5, 2.1.6, 2.1.7, 2.1.8
         *      2.2.1, 2.2.2
         *      2.3.1, 2.3.2, 2.3.3, 2.3.4
         *      2.4.1, 2.4.2, 2.4.3, 2.4.4, 2.4.5, 2.4.6, 2.4.7, 2.4.8  */
        if ((buf[l] >= MUTUAL_CFG_FIRSTUPPERLIMIT) || (buf[l] <= MUTUAL_CFG_FIRSTLOWERLIMIT))
        {
            /* cast here intentionally */
            g_diag_return_array[0] = (uint16_t) buf[l];
            g_diag_return_array[1] = k;
            return DIAG_MUTUAL_FIRST_MEASUREMENT_FAIL;
        }

        /* Second Sensor Counter Measurement */
        /* FMEA 2.1.1, 2.1.2, 2.1.3, 2.1.4, 2.1.5, 2.1.6, 2.1.7, 2.1.8
         *      2.2.1, 2.2.2
         *      2.3.1, 2.3.2, 2.3.3, 2.3.4
         *      2.4.1, 2.4.2, 2.4.3, 2.4.4, 2.4.5, 2.4.6, 2.4.7, 2.4.8  */
        l++;
        if ((buf[l] >= MUTUAL_CFG_SECONDUPPERLIMIT) || (buf[l] <= MUTUAL_CFG_SECONDLOWERLIMIT))
        {
            /* cast here intentionally */
            g_diag_return_array[0] = (uint16_t) buf[l];
            g_diag_return_array[1] = k;
            return DIAG_MUTUAL_SECOND_MEASUREMENT_FAIL;
        }
        k++;
		l++;
    }

    return DIAG_SENSOR_CHECK_SUCCESS;
}
/******************************************************************************
 End of function diag_mutual_cap_full_scan_all
 ******************************************************************************/

/******************************************************************************
 * Function Name: diag_mutual_cap_full_scan_sngl
 *
 * Description : Performs scan on a single sensor (button) against the FMEA
 *               matrix. If errors are encountered, then return those up so
 *               they can be passed to application for action. This is for
 *               MUTUAL mode only.
 *
 * Arguments :  index - value of the sensor to evaluate that is passed from
 *              the application. This is found in the QE tuning output parameter
 *              files.
 *
 * Return Value(s) : e_diagerr_t which will reflect the following error codes:
 *
 *                 DIAG_SENSOR_CHECK_SUCCESS - sensors evaluated correctly
 *                 DIAG_CTSU_REG_CHECK_SUCCESS - CTSU registers evaluated correctly
 *                 DIAG_MUTUAL_FIRST_MEASUREMENT_FAIL
 *                 DIAG_MUTUAL_SECOND_MEASUREMENT_FAIL
 *                 DIAG_CTSU_READ_FAIL - cannot get data from scan
 *
 * Usage Note: internal API function not intended to be called from application
  ******************************************************************************/
static e_diagerr_t diag_mutual_cap_full_scan_sngl (uint8_t index)
{
    qe_err_t err;
    uint16_t primary;
    uint16_t secondary;

    /* retrieve sensor data from previous scan */
    err = R_CTSU_ReadElement(index, &primary, &secondary);

    /* put a fail-safe check here CTSU cannot read, then return error */
    if (QE_SUCCESS != err)
    {
        return DIAG_CTSU_READ_FAIL;
    }

    /* ensure error return array is reset each time thru */
    g_diag_return_array[0] = 0;
    g_diag_return_array[1] = 0;

    /* First Sensor Counter Measurement */
    /* FMEA 2.1.1, 2.1.2, 2.1.3, 2.1.4, 2.1.5, 2.1.6, 2.1.7, 2.1.8
     *      2.2.1, 2.2.2
     *      2.3.1, 2.3.2, 2.3.3, 2.3.4
     *      2.4.1, 2.4.2, 2.4.3, 2.4.4, 2.4.5, 2.4.6, 2.4.7, 2.4.8  */
    if ((primary >= MUTUAL_CFG_FIRSTUPPERLIMIT) || (primary <= MUTUAL_CFG_FIRSTLOWERLIMIT))
    {
        /* cast here intentionally */
        g_diag_return_array[0] = primary;
        g_diag_return_array[1] = index;
        return DIAG_MUTUAL_FIRST_MEASUREMENT_FAIL;
    }

    /* Second Sensor Counter Measurement */
    /* FMEA 2.1.1, 2.1.2, 2.1.3, 2.1.4, 2.1.5, 2.1.6, 2.1.7, 2.1.8
     *      2.2.1, 2.2.2
     *      2.3.1, 2.3.2, 2.3.3, 2.3.4
     *      2.4.1, 2.4.2, 2.4.3, 2.4.4, 2.4.5, 2.4.6, 2.4.7, 2.4.8  */
    if ((secondary >= MUTUAL_CFG_SECONDUPPERLIMIT) || (secondary <= MUTUAL_CFG_SECONDLOWERLIMIT))
    {
        /* cast here intentionally */
        g_diag_return_array[0] = secondary;
        g_diag_return_array[1] = index;
        return DIAG_MUTUAL_SECOND_MEASUREMENT_FAIL;
    }

    return DIAG_SENSOR_CHECK_SUCCESS;
}
/******************************************************************************
 End of function diag_mutual_cap_full_scan_sngl
 ******************************************************************************/

/******************************************************************************
 * Function Name: check_ctsu_registers
 *
 * Description :     Now, to verify if the CTSU is working properly, need to
 *                   verify if the overflow flags happen to be set or cleared
 *                   even if the individual channel testing passed in this case.
 *
 * Arguments :       None
 *
 * Return Value(s) : e_diagerr_t which will reflect the following error codes:
 *
 *                 DIAG_CTSU_REG_CHECK_SUCCESS - CTSU registers evaluated correctly
 *                 DIAG_TS_CAP_FAIL
 *                 DIAG_OVERFLOW_FLAG_TRIP_FAIL
 *                 DIAG_CTSU_READ_FAIL - cannot get data from scan
 *                 DIAG_STATUS_REG_READ_FAIL - cannot get CTSU reg data
 *
 * Usage Note: internal API function not intended to be called from application
 ******************************************************************************/
static e_diagerr_t check_ctsu_registers (void)
{
    qe_err_t err;
    scan_info_t scan_info;
    uint8_t method;

    /* First we need to retrieve the last run method, then the status registers saved from that method */
    R_TOUCH_Control(TOUCH_CMD_GET_LAST_SCAN_METHOD, &method);
    err = R_CTSU_Control(CTSU_CMD_GET_SCAN_INFO, method, &scan_info);
    if (QE_SUCCESS != err)
    {
        return DIAG_STATUS_REG_READ_FAIL;
    }

    /* now check to see if the overflow flag for sensor counter */
    /* has been set but not caught by the raw data check */
    /* FMEA SELF Line Items:    1.1.1, 1.1.2, 1.1.3, 1.1.4, 2.2.1, 2.2.2
     * FEMA MUTUAL Line Items:  2.2.1, 2.2.2
     *                          2.4.1, 2.4.6,  */
    if (( (scan_info.ctsust & 0x60) == 0x20) )
    {
        return DIAG_OVERFLOW_FLAG_TRIP_FAIL;
    }

    /* now check to see if the overflow flag for reference counter */
    /* has been set but not caught by the raw data check */
    /* FMEA SELF Line Items:    1.1.1, 1.1.2, 1.1.3, 1.1.4, 2.2.1, 2.2.2
     * FEMA MUTUAL Line Items:  2.2.1, 2.2.2
     *                          2.4.1, 2.4.6,  */
    if (( (scan_info.ctsust & 0x60) == 0x40) )
    {
        return DIAG_OVERFLOW_FLAG_TRIP_FAIL;
    }

    /* now check to see if the TSCAP error register has been set. if so, then this is an error that */
    /* was not caught by the API or thru the check of the raw counts */
    /* FMEA SELF Line Items: 1.1.5, 1.1.6, 2.2.1, 2.2.2
     * FEMA MUTUAL Line Items:  2.2.1, 2.2.2
     *                          2.4.3, 2.4.4, 2.4.5, 2.4.6  */
    if ((scan_info.ctsuerrs & 0x8000) == 0x8000)
    {
        return DIAG_TS_CAP_FAIL;
    }

    /* Evaluate Module Stop bit has been set inadvertently. Bit consistent for ALL 100 and 200 series MCU families. */
    /* Cast here is intentional */
    if ( (1u) == MSTP(CTSU) )
    {
    	return DIAG_CTSU_MODULE_STOP_FAIL;
    }

    /* casting below is intentional due to SFR access. Check if interrupt is disabled */
    if ( ((0u) == IEN(CTSU,CTSUWR)) || ((0u) == IEN(CTSU,CTSURD)) || ((0u) == IEN(CTSU,CTSUFN)) )
    {
    	return DIAG_CTSU_IEN_ZERO_FAIL;
    }

    /* casting below is intentional due to SFR access. Check if priority is 0 */
    if ( ((0u) == IPR(CTSU,CTSUWR)) || ((0u) == IPR(CTSU,CTSURD)) || ((0u) == IPR(CTSU,CTSUFN)) )
    {
    	return DIAG_CTSU_IPR_ZERO_FAIL;
    }

    return DIAG_CTSU_REG_CHECK_SUCCESS;
}
/******************************************************************************
 End of function check_ctsu_registers
 ******************************************************************************/

#if (QE_CFG_DIAG_PERFORM_CHECKSUM == 1)
/******************************************************************************
 * Function Name: driver_ram_checksum
 *
 * Description :    Routine performs additive checksum with inverse on identified
 * 					critical RAM data used by the CTSU and touch layers. Data
 * 					checksum is created on first pass, then stored. Subsequent
 * 					calls to this routine perform the same calculation, then
 * 					result is compared to stored and results are passed back
 * 					to calling function.
 *
 * 					Checksum stored in: static uint16_t stored_checksum
 *
 *					Arguments :       None
 *
 * Return Value(s) : e_diagerr_t which will reflect the following error codes:
 *
 *                 	DIAG_CHECKSUM_ROUTINE_SUCCESS
 *                 	DIAG_CHECKSUM_ROUTINE_FAIL
 *
 * Usage Note: internal API function not intended to be called from application
 ******************************************************************************/
static e_diagerr_t driver_ram_checksum (void)
{
	uint8_t index;
	uint8_t number_bytes;
	uint8_t * p_address;
	uint16_t checksum;

	/* initialize local calculation result to 0 */
	checksum = 0;

	/* iterative loop used to calculate checksum value */
	for(index = 0; index < QE_PRV_CHECKSUM_RANGE_NUM; index++)
	{
		/* reset address pointer to first address of section for evaluation */
		p_address = (uint8_t*)gs_qe_checksum_ranges[index].start;

		/* set number of bytes in section for summation */
		number_bytes = 1 + ((uint32_t)gs_qe_checksum_ranges[index].end - (uint32_t)gs_qe_checksum_ranges[index].start);

	    while ( (number_bytes--) > 0)
	    {
	    	checksum += (*(p_address++));
	    }
	}

	/* finally invert checksum to capture single bit errors */
	checksum =~ checksum;

	/* on startup, compiler cannot create a checksum by default, so first call will initialize checksum after Open()
	 * completes. Fist call, routine always exits with success
	 */
	if ( true == gs_first_checksum )
	{
		/* reset initial checksum flag */
		gs_first_checksum = false;

		/* store this first calculated checksum */
		gs_stored_checksum = checksum;
		return DIAG_CHECKSUM_ROUTINE_SUCCESS;
	}

	if ( gs_stored_checksum != checksum )
	{
		return DIAG_CHECKSUM_ROUTINE_FAIL;
	}

	return DIAG_CHECKSUM_ROUTINE_SUCCESS;
}
/******************************************************************************
 End of function driver_ram_checksum
 ******************************************************************************/
#endif /* Conditional compilation end */

#if (QE_CFG_DIAG_PERFORM_CRC == 1)
/******************************************************************************
 * Function Name: check_driver_crc
 *
 * Description :    Routine performs CRC calculation using MCU-based CRC block, then
 * 					compares this value against the compiler generated CRC value
 * 					inserted at build time. This validates all CTSU and Touch
 * 					Application code. CRC routine that is utilized is taken from the
 * 					VDE IEC 60730 safety library routines that are referenced by
 * 					R01AN2061ED0100 (RX100 series) and R01AN3364EG0100 (RX200
 * 					series).
 *
 * 					For proper operation and compilation, the CRC modules from
 * 					the above Renesas application notes MUST be a part of the end
 * 					included application.
 *
 *					Arguments :       None
 *
 * Return Value(s) : e_diagerr_t which will reflect the following error codes:
 *
 *                 	DIAG_CRC_ROUTINE_FAIL
 *                 	DIAG_CRC_ROUTINE_SUCCESS
 *
 * Usage Note: internal API function not intended to be called from application
 ******************************************************************************/
static e_diagerr_t check_driver_crc (void)
{
	uint8_t index;
	uint16_t crc_value;

	/* enable the CRC hardware block */
	/*Disable register protection (PRC1)*/
	SYSTEM.PRCR.WORD = 0xA502;

	/*Enable CRC module*/
	SYSTEM.MSTPCRB.BIT.MSTPB23 = 0;

	/*Re-Enable register protection*/
	SYSTEM.PRCR.WORD = 0xA500;

	/*Configure CRC Control Register:
	1.Set Polynomial to CRC-CCITT XModem (h'1021)
		(p(x) = x ** 16 + x ** 12 + x ** 5 + 1)
	2.Set MSB First. (This makes result agree with online CRC calculators.)
	3.Clear CRCDOR register*/
	CRC.CRCCR.BYTE = 0x87;

	/* Initialize the CRC hardware block to 0xFFFF. Clear any previous result.*/
	CRC.CRCCR.BIT.DORCLR = 1;

	/*Initial value */
	CRC.CRCDOR = 0xFFFF;

	/*Specify address ranges for CRC calculation*/
	for(index = 0; index < QE_PRV_CRC_RANGE_NUM; index++)
	{
		/* Casting is intentional here */
		qe_crc_add_range((uint8_t*)gs_qe_crc_ranges[index].start, 1 + ((uint32_t)gs_qe_crc_ranges[index].end - (uint32_t)gs_qe_crc_ranges[index].start));
	}

	/*Return the CRC result. This is as read from CRC Data Output register and bit inverted*/
	crc_value = (uint16_t)~CRC.CRCDOR;


	/* Verify this value is the same as the reference CRC value stored in ROM. First time routine executes, a CRC is generated
	 * and stored. This makes the first pass always successful. Any subsequent failures against the CRC calculation and stored
	 * result are failures and flagged as such
	 */
	if ( true == gs_first_crc )
	{
		gs_first_crc = false;
		gs_stored_crc = crc_value;
	}

	if( gs_stored_crc == crc_value )
	{
		/*Test Passed*/
		return DIAG_CRC_ROUTINE_SUCCESS;
	}
	else
	{
		/*Test Failed*/
		return DIAG_CRC_ROUTINE_FAIL;
	}
}
/******************************************************************************
 End of function check_driver_crc
 ******************************************************************************/

/***********************************************************************************
Function Name : qe_crc_add_range
Description   : Use this function rather than CRC_Calculate if wanting
				to calculate the CRC on data made up of more than one address range.
				Call CRC_Start first then CRC_AddRange for each address range
				required and then call CRC_Result to get the CRC value.

Parameters    : pui8_Data: Start address of dta
			    ui32_Length : Number of bytes of data.
Return value  :
***********************************************************************************/
static void qe_crc_add_range(const uint8_t* pui8_Data, uint32_t ui32_Length)
{
	/*Write the data a byte at a time to the CRC Data Input register.*/
	while(0 != ui32_Length)
	{
		/* Casting performed here intentionally */
		CRC.CRCDIR = *pui8_Data;

		/*Onto the next byte*/
		pui8_Data++;
		ui32_Length--;
	}
}
/******************************************************************************
 End of function qe_crc_add_range
 ******************************************************************************/
#endif /* Conditional compilation end */

/***********************************************************************************************************************
 * Function Name: R_CTSU_DiagGetVersion
 * Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
 *                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25
 *                would be returned as 0x00040019.
 * Arguments    : none
 * Return Value : Version of this module.
 ***********************************************************************************************************************/
uint32_t R_CTSU_DiagGetVersion(void) {

	/* Casting is intentional here. These version macros are defined in r_qetouch_diagnostics_if.h. */
	return ((((uint32_t) QETOUCH_DIAGNOSTICS_VERSION_MAJOR) << 16) | (uint32_t) QETOUCH_DIAGNOSTICS_VERSION_MINOR);
}
/******************************************************************************
 End of function R_CTSU_DiagGetVersion
 ******************************************************************************/


