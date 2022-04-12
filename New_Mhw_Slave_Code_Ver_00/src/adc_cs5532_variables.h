/*
 * adc_cs5532_variables.h
 *
 *  Created on: Feb 28, 2022
 *      Author: Dnyaneshwar
 */

#ifndef ADC_CS5532_VARIABLES_H_
#define ADC_CS5532_VARIABLES_H_

#ifndef ADC_VAR
# define _ADC extern
# define _ADC_INIT(x)
#else
# define _ADC
# define _ADC_INIT(x)  = x
#endif

_ADC unsigned char SPEED _ADC_INIT(2);
_ADC unsigned char GAIN _ADC_INIT(7);
_ADC unsigned char adc_stable _ADC_INIT(0);
_ADC signed int	adc_count _ADC_INIT(0),last_adc_count _ADC_INIT(0);
_ADC unsigned long int asic_value _ADC_INIT(0),last_asic_value _ADC_INIT(0);
_ADC unsigned long int adc_buff[300+1],sort_buff[300+1];
_ADC unsigned int adptr _ADC_INIT(0);
_ADC unsigned int ADSAMPLES _ADC_INIT(4);
_ADC unsigned char Diff_Cnt_Val _ADC_INIT(3);
_ADC unsigned char stable_count _ADC_INIT(0);
_ADC unsigned char Stable_Count_Val _ADC_INIT(1);
_ADC unsigned char auto_zero_count _ADC_INIT(0);
_ADC uint32_t Disp _ADC_INIT(0),disp _ADC_INIT(0),disp2 _ADC_INIT(0);
_ADC signed int		CurrentWt _ADC_INIT(0);
//-----------------------------------FLAGS-------------------------------------//
struct bit_define
{
    char  f0:1;
    char  f1:1;
    char  f2:1;
    char  f3:1;
    char  f4:1;
    char  f5:1;
    char  f6:1;
    char  f7:1;
};
union  byte_define
{
  	struct bit_define f_bit;
  	unsigned char   f_byte;
};
_ADC union byte_define	CS5532_Flag;

#define	read_asic_f    		CS5532_Flag.f_bit.f0
#define	ad_time_out_flag	CS5532_Flag.f_bit.f1
#define	adflag				CS5532_Flag.f_bit.f2
//#define	adc_stable    		CS5532_Flag.f_bit.f3
#define	asicvarsign    		CS5532_Flag.f_bit.f4




#endif /* ADC_CS5532_VARIABLES_H_ */
