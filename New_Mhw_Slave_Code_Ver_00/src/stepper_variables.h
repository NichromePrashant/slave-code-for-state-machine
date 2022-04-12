/*
 * stepper.h
 *
 *  Created on: Feb 28, 2022
 *      Author: Dnyaneshwar
 */

#ifndef STEPPER_VARIABLES_H_
#define STEPPER_VARIABLES_H_


#ifndef STEPPER_VAR
# define _STEPPER extern
# define _STEPPER_INIT(x)
#else
# define _STEPPER
# define _STEPPER_INIT(x)  = x
#endif


_STEPPER _Bool Flag_FB_Trig _STEPPER_INIT(0);				// FeedBucket Run Flag
_STEPPER _Bool Flag_FB_Trig_Edge _STEPPER_INIT(0);		// Check edge of the feedBucket Run Flag
_STEPPER _Bool Flag_FB_OD_TMR _STEPPER_INIT(0);
_STEPPER _Bool Flag_FB_OT_TMR _STEPPER_INIT(0);
_STEPPER _Bool Flag_FB_Stage1 _STEPPER_INIT(0);
_STEPPER _Bool Flag_FB_Stage2 _STEPPER_INIT(0);
_STEPPER _Bool Flag_FB_Stage3 _STEPPER_INIT(0);
_STEPPER _Bool Flag_FB_HomIN _STEPPER_INIT(0);
_STEPPER _Bool Flag_FB_Status _STEPPER_INIT(0);


_STEPPER _Bool Flag_WB_Trig _STEPPER_INIT(0);				// Weighing Bucket Run Flag
_STEPPER _Bool Flag_WB_Trig_Edge _STEPPER_INIT(0);		// Check edge of the WeighingBucket Run Flag
_STEPPER _Bool Flag_WB_OD_TMR _STEPPER_INIT(0);
_STEPPER _Bool Flag_WB_OT_TMR _STEPPER_INIT(0);
_STEPPER _Bool Flag_WB_Stage1 _STEPPER_INIT(0);
_STEPPER _Bool Flag_WB_Stage2 _STEPPER_INIT(0);
_STEPPER _Bool Flag_WB_Stage3 _STEPPER_INIT(0);
_STEPPER _Bool Flag_WB_HomIN _STEPPER_INIT(0);

_STEPPER _Bool Flag_DumpCyc _STEPPER_INIT(0);
_STEPPER _Bool Flag_RechCyc _STEPPER_INIT(0);
_STEPPER _Bool Flag_WB_Status _STEPPER_INIT(0);

_STEPPER unsigned int FB_Angle_Ptr _STEPPER_INIT(0);
_STEPPER unsigned int WB_Angle_Ptr _STEPPER_INIT(0);
_STEPPER unsigned int homing_flag_fb _STEPPER_INIT(0),homing_flag_wb _STEPPER_INIT(0);
_STEPPER unsigned int fb_at_homing_flag _STEPPER_INIT(0),wb_at_homing_flag _STEPPER_INIT(0);

_STEPPER unsigned int WB_Clk_Pulses _STEPPER_INIT(0);			// Clock Pulses for the Weighing Bucket
_STEPPER unsigned int WB_OD_Time _STEPPER_INIT(10);		// Open Delay time for he WeighingBucket
_STEPPER unsigned int WB_OT_Time _STEPPER_INIT(10);		// Open Time time for the Weighing Bucket
_STEPPER unsigned int WB_OD_TMR _STEPPER_INIT(0);				// Open Delay Timer of the Weighing Bucket
_STEPPER unsigned int WB_OT_TMR _STEPPER_INIT(0);				// Open Time Timer of Weighing Bucket


_STEPPER unsigned int FB_Clk_Pulses _STEPPER_INIT(0);			// Clock Pulses for the Feed Bucket
_STEPPER unsigned int FB_OD_Time _STEPPER_INIT(10);		// Open Delay time for the FeedBucket
_STEPPER unsigned int FB_OT_Time _STEPPER_INIT(10);		// Open Time time for the FeedBucket
_STEPPER unsigned int FB_OD_TMR _STEPPER_INIT(0);				// Open Delay Timer of the Feed Bucket
_STEPPER unsigned int FB_OT_TMR _STEPPER_INIT(0);				// Open Time Timer of Feed Bucket


_STEPPER unsigned int DSidx _STEPPER_INIT(0), DSidx_FB _STEPPER_INIT(0);
_STEPPER unsigned int MotoPro _STEPPER_INIT(0);
_STEPPER unsigned int MotoPro_FB _STEPPER_INIT(0);
_STEPPER unsigned int shift_speed_row_WB _STEPPER_INIT(0),shift_speed_row_FB _STEPPER_INIT(0);
//_STEPPER unsigned int pulses_buff[100],v _STEPPER_INIT(0);

#endif /* STEPPER_VARIABLES_H_ */
