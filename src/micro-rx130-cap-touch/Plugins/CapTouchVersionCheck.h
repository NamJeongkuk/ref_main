/*!
 * @file
 * @brief Ensures v1.10 is used for QE CTSU and QE Touch.
 *
 * See https://github.com/geappliances/applcommon.getting-started/blob/master/rx/README.md#installing-v110 for instructions
 * for installing v1.10.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHVERSIONCHECK_H
#define CAPTOUCHVERSIONCHECK_H

#include "r_ctsu_qe_if.h"
#include "r_touch_qe_if.h"

#if(QECTSU_RX_VERSION_MAJOR != 1 || QECTSU_RX_VERSION_MINOR != 10)
#error "QE CTSU v1.10 must be used"
#endif

#if(QETOUCH_RX_VERSION_MAJOR != 1 || QETOUCH_RX_VERSION_MINOR != 10)
#error "QE Touch v1.10 must be used"
#endif

#endif
