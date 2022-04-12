/*
 * global_variables.h
 *
 *  Created on: Feb 25, 2022
 *      Author: Dnyaneshwar
 */

#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_

#ifndef GLOBAL_VAR
# define _GLOBAL extern
# define _GLOBAL_INIT(x)
#else
# define _GLOBAL
# define _GLOBAL_INIT(x)  = x
#endif

#define TRUE        (1)
#define FALSE       (0)

#define MAXPULSES   400

#define FB_WAIT_TIME 520

#define CAN_WAIT_TIME 5

#define FB_CYCLE_TIME   2000
#define WB_CYCLE_TIME   2000
#define VIB_CYCLE_TIME  2000
#define SS_CYCLE_TIME   2000
#define CS_CYCLE_TIME   670

#define CALIB_CYCLE_TIME       5000
#define AUTO_TARE_CYCLE_TIME   2000

#define MAX_LONGTIME_NOT_IN_COMBI_COUNT 20

 _GLOBAL volatile unsigned int CAN_SlaveID;
 _GLOBAL volatile unsigned int HEADID;

//_GLOBAL  unsigned int CAN_SlaveID _GLOBAL_INIT(0);
_GLOBAL  unsigned int   statemc _GLOBAL_INIT(0);
_GLOBAL  unsigned int   t00 _GLOBAL_INIT(0),t01 _GLOBAL_INIT(0),t02 _GLOBAL_INIT(0),t03 _GLOBAL_INIT(0);
_GLOBAL  unsigned int 	VBT_Cntr _GLOBAL_INIT(0);		   // Actual variable to decide number cycle to be fired
_GLOBAL  unsigned char 	Pulses_SCR _GLOBAL_INIT(0);				          // Number of Pulses of PWM fired to trigger SCR
_GLOBAL  unsigned int	SD_LF_TMR _GLOBAL_INIT(0);				         // Linear feeder Start Delay Timer
_GLOBAL  unsigned int	SD_LF_Time _GLOBAL_INIT(5);		// Start Delay of Time of the Linear feeder
_GLOBAL  unsigned int 	VBRT_Cyl _GLOBAL_INIT(5);	// Number cycle to be fired
_GLOBAL  signed int 	Firing_Angle _GLOBAL_INIT(5500); // Decide firing angle of the SCR, in multiple of 1 microseconds
_GLOBAL  _Bool          VBRT_Half _GLOBAL_INIT(0);

_GLOBAL  unsigned int   Cycle_TMR _GLOBAL_INIT(0);		// Single Cycle Timer
_GLOBAL  unsigned int   Settle_TMR _GLOBAL_INIT(0);
_GLOBAL  unsigned int   Settle_Time _GLOBAL_INIT(200);	// Settling Time in Run Cycle for Weight calculation
_GLOBAL  unsigned int   Initial_Vibrator_Flag _GLOBAL_INIT(0);
//_GLOBAL  unsigned int  pulses_buff[20],v;
#endif /* GLOBAL_VARIABLES_H_ */
