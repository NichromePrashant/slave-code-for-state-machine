/*
 * global_functions.h
 *
 *  Created on: Feb 25, 2022
 *      Author: Dnyaneshwar
 */

#ifndef GLOBAL_FUNCTIONS_H_
#define GLOBAL_FUNCTIONS_H_

void Init_Peripherals();
void Clear_Watchdog();
void Timer0_Interval_Timer_Isr();
void Initialize_Pins();


void Initial_Values();
extern void PWM_Stop(void);
extern void PWM_Start(void);

#endif /* GLOBAL_FUNCTIONS_H_ */
