/*
 * vibrator_code.c
 *
 *  Created on: Mar 18, 2022
 *      Author: Dnyaneshwar
 */

#ifndef VIBRATOR_CODE_C_
#define VIBRATOR_CODE_C_

#include "platform.h"
#include "r_cg_userdefine.h"
#include "r_cg_cmt.h"
#include "Config_CMT1.h"
#include "Config_ICU.h"

#include "global_functions.h"
#include "global_variables.h"

#include "manual_functions.h"
#include "manual_variables.h"

#include "vibrator_functions.h"

void Timer1_FAngle_ISR(void)
{
	Clear_Watchdog();		         // Clear and Reset the Watch Timer

	R_Config_CMT1_Stop();		 // The function used to stop Compare match Timer1
	Pulses_SCR = 0;				// Set the Firing Pulse count to Zero.

	if((Initial_Vibrator_Flag==1) && ((VBT_Cntr>0) && VBT_Cntr<= (VBRT_Cyl-2)))
		PWM_Start();				// Start the PWM triggering logic
	else
	{
		R_Config_ICU_Create();
		R_Config_ICU_IRQ0_Start();
	}

	Initial_Vibrator_Flag = 1;
}

void ISR_ZCIP(void)
{
	/*The below pin used to check the status of the IRQ on port pin used to check the noise*/

	Clear_Watchdog();

	if(VBT_Cntr>VBRT_Cyl)
	{
		VBT_Cntr = 0;
	}

//	CPin      ^= 1;					// Reflect the status of External Interrupt Request on port
	VBRT_Half ^= 1;

	if((VBRT_Half == 1) && (VBT_Cntr <= VBRT_Cyl) )
	{
		if(VBT_Cntr > 0)
		{
			VBT_Cntr--;							// Decrease the vibration counter
			CMT1.CMCOR = Firing_Angle;			// The value of the Timer as of Firing angle of Vibrater
			R_Config_ICU_IRQ0_Stop();			// Stop the External Interrupt Request
			R_Config_CMT1_Start();				// Start the Compare and Match Timer
		}
	}

	Clear_Watchdog();
}

void PWM_ISR(void)
{
	Clear_Watchdog();				// Clear watch dog timer and reset

	if(Pulses_SCR == 1)			// must fire atleast two pulses
	{
		Pulses_SCR = 0;			// Reset the Number of pulses triggered to SCR
		PWM_Stop();				// Stop PWM block
		R_Config_ICU_Create();
		R_Config_ICU_IRQ0_Start();		// Start the external zcip ISR block
	}
	Pulses_SCR++;
}

#endif /* VIBRATOR_CODE_C_ */
