/*!
 * @file
 * @brief The definition of UL60730 system defines to force artificial errors and to provoke detection
 *  of defects by self tests for PSoC4
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TESTCONFIGURATION_H
#define TESTCONFIGURATION_H

/*!
 *  Status flags to return function results
 */
#define ERROR_STATUS   (1)
#define OK_STATUS      (0)
#define PASS_STILL_TESTING_STATUS  (2)
#define PASS_COMPLETE_STATUS       (3)

/*!
 * Set this macro to 1u to force artificial error in CPU check
 */
#define ERROR_IN_CPU_REGISTERS      (0)

/*!
 * Set this macro to 1u to force artificial error in PC check
 */
#define ERROR_IN_PROGRAM_COUNTER    (0)

/*!
 * Set this macro to 1u to force artificial error in RAM checks
 */
#define ERROR_IN_STACK              (0)
#define ERROR_IN_UDB_CONF_REG       (0)
#define ERROR_IN_STARTUP_CONF_REG   (0)

/*!
 * Set this macro to 1u to force artificial error in RAM checks
 */
#define ERROR_IN_SRAM_MARCH         ERROR_IN_RAM
#define ERROR_IN_RAM                (0)

/*!
 * Set this macro to 1u to force artificial error in RAM checks
 */
#define ERROR_IN_STACK_MARCH        ERROR_IN_STACKM
#define ERROR_IN_STACKM             (0)

/*!
 * Set this macro to 1u to force artificial error in flash check
 */
#define ERROR_IN_FLASH              (0)

#endif
