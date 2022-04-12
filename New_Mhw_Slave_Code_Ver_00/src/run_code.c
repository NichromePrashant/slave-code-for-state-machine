/*
 * run_code.c
 *
 *  Created on: Feb 28, 2022
 *      Author: Dnyaneshwar
 */
#include "platform.h"
#include "r_cg_userdefine.h"
#include "r_cg_cmt.h"
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

void Run_Operation(void)
{
	Clear_Watchdog();

	if(Flag_Run == 1  && Auto_Tare_Flag == 0 ) // Check for the Run Flag
	{
		if((Head_In_Combi)&&(Flag_RunCyc == 0)&&(Flag_Run_Stage1 == 0))
		{

			//if((HIN_WB == 0 && ENB_WB == 0)|| (wb_at_homing_flag == 0))
			if((HIN_WB == 0 && ENB_WB == 0))
			{
				homing_flag_wb    = 0;
				Flag_WB_Trig 	  = 1;		// Trigger WeighBucket of the Systems
				Flag_WB_Trig_Edge = 0;
				CLK_WB            = 1;
				Flag_DumpCyc 	  = 1;		// Decode weather to Run the Weighing Bucket
				Actual_Dump_Counter++;

				Flag_Run_Stage5 = 0;		// Reset the Stage 5 complition Flag
				Head_In_Combi   = 0;

				Flag_WB_Status  = 0;
				Flag_FB_Status  = 0;



			if(Actual_Dump_Counter >= Auto_Zero_Interval)
			{
				Flag_Tare_Done      = 0;
				Flag_Run_Stage1     = 0;
				Flag_RunCyc         = 0;
				Auto_Tare_Flag      = 1;
				Flag_Run            = 0;     // 14/12/21
				Flag_RunOperation   = 0;     // 14/12/21
			}
			else
			{
				time_to_start_fb      = FB_WAIT_TIME;
				time_to_start_fb_flag = 1;
				Flag_Run_Stage1 = 1;		// Inter-lock of Stage 1 complition flag
				Flag_RunCyc 	= 1;		// Inter-lock of Run Cycle Trigger
			}
			}
			else
			{
				Flag_Run_Stage4 = 1;
				Flag_Run_Stage5 = 0;

				//if(wb_at_homing_flag == 1 && homing_flag_wb == 0 && Flag_WB_Trig == 0)
				if(homing_flag_wb == 0 && Flag_WB_Trig == 0)
				{
					homing_flag_wb = 1;
					Flag_WB_Trig   = 0;
				}
			}
		}

		if((Head_In_Recharge)&&(Flag_RunCyc == 0)&&(Flag_Run_Stage1 == 0)&&(Flag_Tare_Done))
		{
			Flag_WB_Trig_Edge     = 0;
			time_to_start_fb      = FB_WAIT_TIME;
			time_to_start_fb_flag = 1;

			Flag_DumpCyc 	      = 0;		// Decode weather to Run the Weighing Bucket
			Flag_RunCyc 	      = 1;		// Inter-lock of Run Cycle Trigger
			Flag_Run_Stage5       = 0;		// Reset the Stage 5 complition Flag
			Flag_Run_Stage1       = 1;		// Inter-lock of Stage 1 complition Flag
			Flag_Run_Stage2       = 0;

			Flag_WB_Status        = 0;
			Flag_FB_Status        = 0;
		}

		if((Flag_RunCyc == 1)&&(Flag_Run_Stage1 == 1)&&(Flag_Run_Stage2 == 0)&&(Flag_WB_Status == 1)&&(Flag_Tare_Done))
		{
			//if((HIN_FB == 0 && ENB_FB == 0)||(fb_at_homing_flag == 0))
			if((HIN_FB == 0 && ENB_FB == 0))
			{
				homing_flag_fb  = 0;
				Flag_FB_Trig    = 1;			  // Trigger FeedBucket of the System
				CLK_FB          = 1;
				Flag_Run_Stage2 = 1;		   // Inter-Lock of Stage 2 complition Flag
				Flag_Run_Stage3 = 0;
			}
			else
			{
				Flag_Run_Stage4 = 1;
				Flag_Run_Stage5 = 0;

				//if(fb_at_homing_flag == 1 && homing_flag_fb == 0 && Flag_FB_Trig == 0)
				if(homing_flag_fb == 0 && Flag_FB_Trig == 0)
				{
					homing_flag_fb = 1;
					Flag_FB_Trig   = 0;
				}
			}
		}
		if((Flag_RunCyc == 1)&&(Flag_Run_Stage2 == 1)&&(Flag_Run_Stage3 == 0)&&(Flag_FB_Status == 1))
		{
			SD_LF_TMR  = SD_LF_Time;		// Start Delay of the Linear Feeder
			Settle_TMR = Settle_Time;
			wt_calculated_flag = 0;
			calibration_wt_flag = 0;

			Flag_Run_Stage3 = 1;		// Inter-Lock of Stage 3 complition Flag
			Flag_Run_Stage4 = 0;
		}
		if((Flag_RunCyc == 1)&&(Flag_Run_Stage3 == 1)&&(Flag_Run_Stage4 == 0)&&(SD_LF_TMR == 0))
		{
			VBT_Cntr = VBRT_Cyl;		 // Trigger the Vibrator just by loading the Number of Firing Pulses
			Flag_Run_Stage4 = 1;		// Inter-Lock of Stage 4 complition Flag
			Flag_Run_Stage5 = 0;
		}
		if((Flag_RunCyc == 1)&&(Flag_Run_Stage4 == 1)&&(Flag_Run_Stage5 == 0))
		{
			Flag_WtReady          = 0;
			Start_Weight_Cal_Flag = 0;
			Flag_RunCyc	 	      = 0;		// Reset the RunCycle InterLcok of the Run Cycle
			Flag_Run_Stage1       = 0;		// Reset the All the Run Stage flag for Next Cycle
			Flag_Run_Stage2       = 0;
			Flag_Run_Stage3       = 0;
			Flag_Run_Stage4       = 0;
			Flag_Run_Stage5       = 1;
			Flag_RechCyc          = 0;
			Flag_FB_Status        = 0;
			Flag_WB_Status        = 0;
			Flag_RunOperation     = 0;

		}
	}

	Clear_Watchdog();


}
