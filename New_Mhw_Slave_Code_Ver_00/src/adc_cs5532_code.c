/*
 * adc_cs5532_code.c
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

#include "adc_cs5532_variables.h"
#include "adc_cs5532_functions.h"

#include "delay_functions.h"

#include "calibration_variables.h"

const char  GainTable[8] = {0x00,0x08,0x10,0x18,0x20,0x28,0x30};
const char SpeedTableH[10] = {0x02,0x01,0x01,0x00,0x00,0x06,0x05,0x05,0x04,0x04};   /*Higher Byte of the Speed Table*/
const char SpeedTableL[10] = {0x00,0x80,0x00,0x80,0x00,0x00,0x80,0x00,0x80,0x00};   /*Lower Byte of the Speed Table*/

//***********************************************************************************************************************
// * Function Name	: void CS5532_Reset(void)
// * Description	: used to Reset the CS5532 Sigma Delta used to reset and clear watch dog timer.
//***********************************************************************************************************************
void Reset_Asic(void)
{
	unsigned int i;
	unsigned long int 	temp0;

	CS5532_WaitDelay(2);				//5
	for(i=0;i<16;i++)
	{
		Trans_byte(0xFF);			 	// Reset ADC
	}
	Trans_byte(0xFE);
	TransRec_5b(0x03,0x22,0,0,0);		// write to configuration register RS bit = 1
	for(i=0;i<1000;i++);				// Wait here for 1000 machine cycles
	TransRec_5b(0x03,0x02,0,0,0);		// write to configuration register RS bit = 0
	for(i=0;i<1000;i++);				// Wait here for 1000 machine cycles
	temp0 = 0;
	while((temp0&0x10)==0)
	{
		 temp0 = TransRec_5b(0x0B,0x02,0,0,0);			//Wait for RV  bit to set, i.e. end of reset
		 temp0>>=16;					//m_rcv[1]
		 Clear_Watchdog();			// Clear Watch dog timer and reset
	}
	statemc=3100;
}

//***********************************************************************************************************************
// * Function Name	: void CS5532_Init(void)
// * Description 	: used to initialize the CS5532 sigma delta ADC.
//***********************************************************************************************************************
void Initialize_Asic(void)
{
	unsigned int  idx;
	unsigned char var1,var2;
	var1=GainTable[GAIN-1]|SpeedTableH[SPEED-1];
	var2=SpeedTableL[SPEED-1]|0x40;
	TransRec_5b(0x05,var1,var2,var1,var2);	//Initialize ADC for Gain = 16 , word rate = 30sps (0000)
	for(idx=0;idx<1000;idx++);
//	TransRec_5b(0x01,0x00,0x00,0x00,0x00);	//Store Offset = FF F2 26 00
//	TransRec_5b(0x02,0x01,0xFF,0xFF,0xFF);	//Store Gain = 2.0000000
	TransRec_5b(0x02,0x01,0x00,0x00,0x00);	//Store Gain = 1.0000000
	  statemc=3200;
}

void Asic_Start(void)
{
	Trans_byte(0xC0);				 //Start Conversion r phase
	  statemc=4000;
}

void Trans_byte(char trnsbyte)
{
	char maskdata;

	SCS_Pin=0;
	SCLK_Pin=1;
	SDI_Pin=1;
	CS5532_WaitDelay(5);	//5
	SCS_Pin=1;
	CS5532_WaitDelay(3);	//2
	maskdata=0x80;
	while(maskdata)
	{
		Clear_Watchdog();			// Clear Watch Dog Timer and Reset
		if(trnsbyte & maskdata)	// Mask the output bit to be send over SPI
			SDI_Pin=0;			// Yes SDA="H"
		else
			SDI_Pin=1;			// No  SDA="L"
		maskdata >>= 1;
		SCLK_Pin=0;				// Set clock pin high
		CS5532_WaitDelay(5);	//5
		SCLK_Pin=1;				// Set clock pin low
		CS5532_WaitDelay(3);	//2
	}
	CS5532_WaitDelay(3);		//2
	SCS_Pin=0;				// Set chip select pin
}

unsigned long int TransRec_5b(unsigned char mtrs0, unsigned char mtrs1, unsigned char mtrs2, unsigned char mtrs3, unsigned char mtrs4)
{
	unsigned char 	i,maskdata,readdata;
	unsigned char 	m_rcv[5],m_trs[5],opr_data[4];
	unsigned long int 	temp0,temp1;
	m_trs[0]=mtrs0;
	m_trs[1]=mtrs1;
	m_trs[2]=mtrs2;
	m_trs[3]=mtrs3;
	m_trs[4]=mtrs4;
	//for(i=0; i<5; i++)
	{
		 m_rcv[0] = 0x00;		// Reset the receive buffer
		 m_rcv[1] = 0x00;
		 m_rcv[2] = 0x00;
		 m_rcv[3] = 0x00;
		 m_rcv[4] = 0x00;
	}
	asicvarsign = 0;
	SCS_Pin=0;		// 1
	SCLK_Pin=1;
	SDI_Pin=1;
	CS5532_WaitDelay(5);
	SCS_Pin=1;		// 0
	CS5532_WaitDelay(3);
	for(i=0;i<5;i++)
	{
		maskdata=0x80;
		while (maskdata)
		{
			readdata = m_rcv[i] | maskdata;
			if (m_trs[i]& maskdata) 		// Actual "H" output
			 SDI_Pin = 0;					// Actual SDI_Pin = "H"
			else
			 SDI_Pin = 1;					// Actual SDI_Pin = "L"
			if(SDO_Pin == 0)				// Actual SDO_Pin = "H"
			 m_rcv[i] = readdata;
			maskdata >>= 1;
			SCLK_Pin=0;						// Actual SCLK_Pin = "H"
			CS5532_WaitDelay(5);
			SCLK_Pin=1;						// Actual SCLK_Pin = "L"
			CS5532_WaitDelay(3);
		}
	}
	SCS_Pin=1;		// 0
	m_rcv[0]=0;				// Unknown byte received from the CS5532
	opr_data[3] = 0x00;
	opr_data[2] = m_rcv[1];
	opr_data[1] = m_rcv[2];
	opr_data[0] = m_rcv[3];
	temp1 = 0x00;
	temp1 = opr_data[3];
	temp1 <<=8;
	temp0 = opr_data[2];
	temp1 = temp1 | temp0;
	temp1 <<=8;
	temp0 = opr_data[1];
	temp1 = temp1 | temp0;
	temp1 <<=8;
	temp0 = opr_data[0];
	temp0 = temp1 | temp0;
	return temp0;
}

void Read_Adc(void)
{
		unsigned char 	ad_time_out_count;

		ad_time_out_count=250;
		ad_time_out_flag=0;
		read_asic_f=0;
		SCS_Pin=1;
		CS5532_WaitDelay(5);		//waits till the adc is ready

		while(SDO_Pin == 0)
		{
			if(ad_time_out_flag!=0)
			{
				ad_time_out_flag=0;
				ad_time_out_count--;

				if(ad_time_out_count==0)
				{
					SCS_Pin=0;
					Delay_Loop(6000);  //
					Reset_Asic();      //
					Delay_Loop(6000);  //
					Initialize_Asic();       //
					Asic_Start();      //
					Delay_Loop(6000);  //
					return;
				}
			}
			Clear_Watchdog();
		}
		//waits till the adc is ready
		read_asic_f=1;
		asic_value=Read_Asic();								// Read the ADC count for the Load Cell Operation.
		asic_value=asic_value>>0;				//4			// Changes Need to be done for the ADC resolution.
		read_asic_f=0;
		asic_value=asic_value*100;
		asic_value=asic_value/5269;
		adc_buff[adptr]=asic_value;
		adptr++;
		//last_asic_value=asic_value;
		if(adptr>(ADSAMPLES-1))
		{
			adptr=0;
		}
		sampling();
		adflag=1;		//find effective count and call check_stable
}
/*The function used to check wheather the ADC Count are in range*/
void Check_Stable(void)
{
	int diff_cnt;

	if(adflag==1)
	{
		adflag = 0;

		diff_cnt = absolute_diff(adc_count,last_adc_count);       //sg for test asic_value== adc count;

		if((diff_cnt < Diff_Cnt_Val)&&(adc_stable==0))
		{
			if(stable_count<250)
				stable_count++;

			if(stable_count > Stable_Count_Val)
			{
				adc_stable = 1;
			}
		}

		if((diff_cnt > (Diff_Cnt_Val))&&(adc_stable==1))        // written by Dnyaneshwar 1/12/21
		{
			adc_stable      = 0;
			auto_zero_count = 0;
			stable_count    = 0;
		}
		last_adc_count=adc_count;              //done asic_value to adc count
	}
}

/*The function used to figure out absolute difference of the ADC count.*/
unsigned long int  absolute_diff(unsigned long int v1,unsigned long int v2)
{
	if(v1>v2)
	 return v1-v2;
	else return v2-v1;
}

unsigned long int Read_Asic(void)
{
	unsigned long int temp0;
	temp0 = TransRec_5b(0,0,0,0,0);
	return temp0;
}


void sampling()
{
	int k;

	for(k=0;k<4;k++)
	{
		disp+=adc_buff[k];
	}

	disp  = disp/4;
	disp2 = disp;
	disp  = 0;
	//disp2=disp2-22368;
	adc_count = disp2;
}

void Calculate_Weight(void)
{
	CurrentWt = (signed int)(((adc_count - TareCnt)*3000)/CalibSlope);
}

void CS5532_WaitDelay(char delaycnt)
{
	unsigned char i;

	for(i=0;i<delaycnt;i++)
	{

		Clear_Watchdog();			// Clear watch dog timer
	}
}
