/*
 * calibration_code.c
 *
 *  Created on: Feb 28, 2022
 *      Author: Dnyaneshwar
 */
#include "platform.h"
#include "r_cg_userdefine.h"
#include "Config_CMT0.h"
#include "Config_CMT1.h"
#include "Config_CMT2.h"
#include "Config_CMT3.h"
#include "Config_ICU.h"

#include "global_functions.h"
#include "global_variables.h"

#include "calibration_variables.h"
#include "calibration_functions.h"

#include "manual_variables.h"

#include "stepper_variables.h"

#include "runcycle_variables.h"

#include "adc_cs5532_variables.h"
#include "adc_cs5532_functions.h"

void Calibration_Operation(void)
{
	if((Dump_Flag==TRUE)&&(Flag_CalOpr_Stage1 == 0))	// Check if Dump Button is pressed
	{
		//if((HIN_WB==0 && ENB_WB == 0)||(wb_at_homing_flag == 0))
		if((HIN_WB==0 && ENB_WB == 0))
		{
			Flag_WB_Trig_Edge  = 0;
			Flag_WB_Trig       = 1;				// Used to Trigger WeighBucket of the System
			CLK_WB             = 1;
			Flag_CalOpr_Stage1 = 1;			// Inter-Clock of Stage 1 Complition Flag
			Dump_Flag          = FALSE;
		}
		else
		{
			//if(wb_at_homing_flag == 1 && homing_flag_wb == 0 && Flag_WB_Trig == 0)
			if(homing_flag_wb == 0 && Flag_WB_Trig == 0)
			{
				homing_flag_wb     = 1;
				Flag_CalOpr_Stage1 = 1;
				Dump_Flag          = FALSE;
			}
		}

	}

	if(((Dump_Flag==FALSE))&&(Flag_CalOpr_Stage1 == 1))		// Check is Dump Button is released
	{
		Flag_CalOpr_Stage1 = 0;			// Reset the Inter-Lock of Stage 1 Complition Flag
	}

	if((Tare_Flag==TRUE)&&(Cycle_TMR == 0))		       // Check if Tare Button is Pressed  (Flag_CalOpr_Stage2 == 0)&&
	{
		Cycle_TMR          = CALIB_CYCLE_TIME;		  // Set the Settling Cycle Timer Tare Operation
		Flag_Tare_Done     = 0;						 // Reset the Tare Done Flag

		while(Cycle_TMR)
		{
			Clear_Watchdog();
			Read_Adc();                               // Read Load Cell ADC count from CS5532
			Check_Stable();					          // Check if the Counts are Stable

			if(adc_stable)					         // if ADC is stable
			{
				TareCnt           = adc_count;		// Move current ADC count to the Tare Count
				Flag_Tare_Done    = 1;			   // Set the Tare Flag

				adc_stable        = 0;				// Rest the ADC stable Flag
				Tare_Flag=FALSE;
				Cycle_TMR         = 0;
				Clear_Watchdog();
			}
		}
		Tare_Send_Time = CAN_WAIT_TIME * CAN_SlaveID;
		Tare_Send_Flag = 1;
    }

	if((Calibeate_Flag==TRUE)&&(Cycle_TMR == 0))				// Check if Calib Button is Pressed
	{
		Cycle_TMR = CALIB_CYCLE_TIME;	  // Set the Settling Cycle Timer for Calib operation
		Flag_Calib_Done    = 0;			 // Reset the calibration Done Flag
		Flag_CalOpr_Stage3 = 1;			// Set the Inter-Lock of Calib Operation

		while(Cycle_TMR)
		{
			Read_Adc();			      // Read Load Cell ADC count from CS5532
			Check_Stable();		     // Check if the Counts are Stable
			CalibSlope=0;
			if(adc_stable)
			{
				CalibCnt          = adc_count;		// Move Current ADC count to the Calib Count
				Flag_Calib_Done   = TRUE;		   // Set the Calib Done Flag
				adc_stable        = 0;				         // Reset the ADC Stabel Flag
				CalibSlope = (CalibCnt - TareCnt)/1;		// 5 indicates 500gm
				Send_Calibcount();
				Calibeate_Flag    = FALSE;
				Cycle_TMR         = 0;
				Clear_Watchdog();
			}
		}
		Clear_Watchdog();
	}

}

void Auto_Tare()
{
	Clear_Watchdog();
	Cycle_TMR      = AUTO_TARE_CYCLE_TIME;						// Set the Settling Cycle Timer Tare Operation
	Flag_Tare_Done = 0;						// Reset the Tare Done Flag

	while(Cycle_TMR)
	{
		Read_Adc();						// Read Load Cell ADC count from CS5532
		Check_Stable();					// Check if the Counts are Stable
		if(adc_stable)					// if ADC is stable
		{
			TareCnt           = adc_count;		// Move current ADC count to the Tare Count
			Flag_Tare_Done    = 1;			// Set the Tare Flag
			Actual_Dump_Counter = 0;
			//gs_test_w_buff[1] = Flag_Tare_Done;
			adc_stable = 0;				// Rest the ADC stable Flag
		}
	}

	Auto_Tare_Flag = 0;

	Send_Tarecount();
	Tare_Send_Time = CAN_WAIT_TIME * CAN_SlaveID;
	Tare_Send_Flag = 1;
}
