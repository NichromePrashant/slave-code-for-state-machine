/*
 * runcycle_variables.h
 *
 *  Created on: Feb 25, 2022
 *      Author: Dnyaneshwar
 */

#ifndef RUNCYCLE_VARIABLES_H_
#define RUNCYCLE_VARIABLES_H_

#ifndef RUN_VAR
# define _RUN extern
# define _RUN_INIT(x)
#else
# define _RUN
# define _RUN_INIT(x)  = x
#endif

_RUN _Bool Head_In_Combi;
_RUN _Bool Head_In_Recharge;
_RUN _Bool Flag_RunOperation;
_RUN _Bool Flag_Run;
_RUN _Bool Flag_RunCyc;
_RUN _Bool Flag_Run_Stage1;
_RUN _Bool Flag_Run_Stage2;
_RUN _Bool Flag_Run_Stage3;
_RUN _Bool Flag_Run_Stage4;
_RUN _Bool Flag_Run_Stage5;
_RUN _Bool Flag_WtReady;
_RUN _Bool wt_calculated_flag;
_RUN _Bool Start_Weight_Cal_Flag;

_RUN unsigned int Auto_Tare_Flag;
_RUN unsigned int Actual_Dump_Counter;
_RUN unsigned int Auto_Zero_Interval;
_RUN unsigned int time_to_start_fb,time_to_start_fb_flag;
_RUN unsigned int calibration_wt_flag;
_RUN unsigned int a,Dump,Recharge;
_RUN unsigned int LongTime_Not_In_Combi_Count,LongTime_Not_In_Combi_flag;

_RUN signed int Tare_Cnt_1,Tare_Cnt_2,Tare_Cnt_3,Tare_Cnt_4,Tare_Cnt_5,Tare_Cnt_6,Tare_Cnt_7,Tare_Cnt_8,Tare_Cnt_9,Tare_Cnt_10;
_RUN signed int Calib_Cnt_1,Calib_Cnt_2,Calib_Cnt_3,Calib_Cnt_4,Calib_Cnt_5,Calib_Cnt_6,Calib_Cnt_7,Calib_Cnt_8,Calib_Cnt_9,Calib_Cnt_10;
_RUN signed int Calib_Slope_1,Calib_Slope_2,Calib_Slope_3,Calib_Slope_4,Calib_Slope_5,Calib_Slope_6,Calib_Slope_7,Calib_Slope_8,Calib_Slope_9,Calib_Slope_10;

_RUN unsigned int Run_Cycle_Time_Flag;
_RUN unsigned int Run_Cycle_Counter;
_RUN unsigned int fb_time_taken_flag,wb_time_taken_flag,lf_time_taken_flag,FB_OfD_TMR_flag;
_RUN unsigned int fb_time_taken_counter,wb_time_taken_counter,lf_time_taken_counter;


#endif /* RUNCYCLE_VARIABLES_H_ */
