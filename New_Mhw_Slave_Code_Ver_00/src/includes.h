/*
 * includes.h
 *
 *  Created on: Feb 28, 2022
 *      Author: Dnyaneshwar
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_

#include "r_smc_entry.h"
#include <stdio.h>              // sprintf()
#include <string.h>             // strlen()
#include "platform.h"

#include "r_sci_rx_if.h"
#include "r_rscan_rx_if.h"      // The RSCAN FIT module API interface file
#include "r_rscan_rx_config.h"
#include "r_cg_wdt.h"
#include "r_cg_cmt.h"
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_sci_rx_if.h"
#include "r_cg_macrodriver.h"
#include "r_cg_icu.h"
#include "Config_CMT0.h"
#include "Config_CMT1.h"
#include "Config_CMT2.h"
#include "Config_CMT3.h"
#include "Config_ICU.h"
#include "math.h"


//#include "can_variables.h"
#include "can_functions.h"

#define GLOBAL_VAR
#include "global_variables.h"
#include "global_functions.h"

#define MANUAL_VAR
#include "manual_functions.h"
#include "manual_variables.h"

#define CALIBRATION_VAR
#include "calibration_variables.h"
#include "calibration_functions.h"

#define RUN_VAR
#include "runcycle_variables.h"
#include "runcycle_functions.h"

#define DELAY_VAR
#include "delay_variables.h"
#include "delay_functions.h"

#define STEPPER_VAR
#include "stepper_variables.h"
#include "stepper_functions.h"

#define ADC_VAR
#include "adc_cs5532_variables.h"
#include "adc_cs5532_functions.h"

#include "vibrator_functions.h"

#endif /* INCLUDES_H_ */
