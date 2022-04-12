/***********************************************************************
*
*  FILE        : New_Mhw_Slave_Code_Ver_00.c
*  DATE        : 2022-02-25
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/

#include "includes.h"

void main(void)
{
	if (statemc==0)
	{
	Initialize_Pins();
	}
	if (statemc==1000)
	{
	Init_Peripherals();
	}

	mS_Delay(1000);

	/*-----------------------------------------Instruction below is to generate CAN Slave ID for Each Slave Dynamically*/
	if(statemc==1100)
	{
	if(DIP1 == 0)	CAN_SlaveID = CAN_SlaveID|0x01;
	if(DIP2 == 0)	CAN_SlaveID = CAN_SlaveID|0x02;
	if(DIP3 == 0)	CAN_SlaveID = CAN_SlaveID|0x04;
	if(DIP4 == 0)	CAN_SlaveID = CAN_SlaveID|0x08;
	statemc=2000;
	}
	/*-----------------------------------------------------------------------------------------------------------------*/

 if (statemc==2000)
 {
	Power_On_Homing();	// At Power On take stepper to Homing position
 }
	mS_Delay(1000);		// Wait here
	if (statemc==3000)
	{
    Reset_Asic();		// Reset to the CS5532
	}
	if (statemc==3100)
	{
	Initialize_Asic();	// Initialize the CS5532 LoadCell mV to ADC converter
	}
	 if( statemc==3200)
	 {
	Asic_Start();		// Starts the CS5532 LoadCell mV to ADC conversion
	 }
	mS_Delay(1000);		// Wait here

	while(1)
	{
		Clear_Watchdog();

    if(  statemc==4000)
    {
		if(Settle_TMR > Settle_Time )
		{
			Settle_TMR = 0;
		}

		if((calibration_wt_flag == 1 || Settle_TMR == 0))
		{
			Read_Adc();						// Read Load Cell ADC count from CS5532
			Check_Stable();					// Check if the Counts are Stable

			if(adc_stable)
			{
				Calculate_Weight();
				Flag_WtReady = 1;
				wt_calculated_flag = 1;
			}
			else
			{
				Flag_WtReady=0;
				wt_calculated_flag = 1;
			}
			  statemc=5000;
		}
		}
       if ( statemc==5000)
       {
		if(Flag_WB_Trig == 0 &&  Auto_Tare_Flag == 1)
		{
			Auto_Tare();
			Read_Adc();
		}
		  statemc=6000;
       }

       if(statemc ==6000)
       {
		if(Flag_ManualOperation == 1)
		{
			Clear_Watchdog();
			Read_Adc();						// Read Load Cell ADC count from CS5532
			Check_Stable();					// Check if the Counts are Stable
			Manual_Operation();
		//	Flag_ManualOperation = 0;
		}
		statemc =7000;
       }

       if(statemc == 7000)
       {
		if(Flag_CalibOperation == 1)
		{
			Clear_Watchdog();
			Read_Adc();						// Read Load Cell ADC count from CS5532
			Check_Stable();					// Check if the Counts are Stable
			Head_In_Recharge = 0;
			Calibration_Operation();

			if(Flag_Calib_Done == TRUE)
			{
				Clear_Watchdog();
				Read_Adc();				// Continuous read ADC in run time
				Check_Stable();			// Check weather ADC count stable

				if(adc_stable)
				{
					Calculate_Weight();		// Continuously calculate the weight above ADC count   4194.572
				}
			}
		}
		statemc = 8000;
       }
  if(statemc == 8000)
  {
		if(Flag_RunOperation == 1)
		{
			Clear_Watchdog();
			Read_Adc();						// Read Load Cell ADC count from CS5532
			Check_Stable();					// Check if the Counts are Stable
			Run_Operation();
		}

		statemc = 9000;
  }

      if(statemc == 9000)
      {
		if(LongTime_Not_In_Combi_Count>=MAX_LONGTIME_NOT_IN_COMBI_COUNT)
		{
			LongTime_Not_In_Combi_flag  = 1;
			LongTime_Not_In_Combi_Count = 0;

		}
		 statemc = 10000;
      }

      if(statemc == 10000)
      {
		if((Tare_Send_Flag == 1) && (Tare_Send_Time == 0))
		{
			Send_Tarecount();
			Tare_Send_Flag = 0;
		}
		 statemc = 4000;
      }

	}

}


void Init_Peripherals(void)
{
	/*The Instruction below used to Initiate CAN Communication*/
	Initialize_CAN_Communication();
	R_Config_CMT0_Start();			// Start the Compare Match Timer0 used for Interval Timer
	R_Config_CMT1_Start();
	R_Config_CMT2_Start();			// Start the Compare Match Timer1 used for FeedBucket
	R_Config_CMT3_Start();			// Start the Compare Match Timer2 used for Weighing Bucket
	R_Config_ICU_IRQ0_Start();		// Start the External Port Pin Interrupt
	statemc = 1100;
}

void Clear_Watchdog()
{
	R_Config_WDT_Restart();
}


void Timer0_Interval_Timer_Isr()
{
	if(Tare_Send_Time)
		Tare_Send_Time--;

	if(time_to_start_fb_flag)
		time_to_start_fb--;

	if(time_to_start_fb_flag == 1 && time_to_start_fb == 0 )
	{
		Head_In_Recharge      = 0;

		if(Flag_Tare_Done)
		{
			Flag_WB_Status    = 1;
		}

		time_to_start_fb_flag = 0;
	}

	if((Settle_TMR))
		Settle_TMR--;

	if(msflag == 0)
	{
	 msflag = 1;
	}
	else if(msflag == 1)
	{
		msflag = 0;
		ad_time_out_flag = 1;

		if(mS_Cnt) 			mS_Cnt--;
		if(WB_OD_TMR)		WB_OD_TMR--;
		if(WB_OT_TMR)		WB_OT_TMR--;
		if(FB_OD_TMR)		FB_OD_TMR--;
		if(FB_OT_TMR)		FB_OT_TMR--;
		if(Cycle_TMR)		Cycle_TMR--;
		if(SD_LF_TMR)		SD_LF_TMR--;
		if(t00)	t00--;
		if(t01)	t01--;
		if(t02)	t02--;
		if(t03)	t03--;

	}
}

void Initialize_Pins(void)
{
	R_Config_PORT_Create();	// Initialize all the SFRs of the PortPin
	DIR_FB = 0;		// Direction pin of the stepper motor1, set to default	(double inversion)
	DIR_WB = 0;		// Direction pin of the stepper motor2, set to default	(double inversion)
	ENB_FB = 0;		// Enable pin of the stepper motor1, set to default as disable (double inversion)
	ENB_WB = 0;		// Enable pin of the stepper motor2, set to default as disable (double inversion)
	CLK_FB = 1;		// Clock pin of the stepper motor1, set to default as Low (invertion is there)
	CLK_WB = 1;		// Clock pin of the stepper motor2, set to default as Low (invertion is there)
	HOP_FB = 0;		// Photo Interrupter is On, PNP Logic
	HOP_WB = 0;		// Photo Interrupter is On, PNP Logic
	statemc=1000;
}



