/*
 * delay.c
 *
 *  Created on: Feb 11, 2022
 *      Author: Dnyaneshwar
 */

#include "Config_CMT0.h"
#include "Config_CMT1.h"
#include "Config_CMT2.h"
#include "Config_CMT3.h"
#include "Config_ICU.h"

#include "delay_functions.h"
#include "global_functions.h"
#include "global_variables.h"

#include "delay_variables.h"

void mS_Delay(unsigned int mS)
{
	mS_Cnt = mS;
	while(mS_Cnt)
	{
		Clear_Watchdog();
	}

}

void Delay_Loop(unsigned long int Cntr)
{
	unsigned long int i,j;
	for(i=0; i<=Cntr; i++)
	 for(j = 0; j <= 100; j++);
}
