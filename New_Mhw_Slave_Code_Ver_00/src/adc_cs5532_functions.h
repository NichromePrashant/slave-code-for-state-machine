/*
 * adc_cs5532_functions.h
 *
 *  Created on: Feb 28, 2022
 *      Author: Dnyaneshwar
 */

#ifndef ADC_CS5532_FUNCTIONS_H_
#define ADC_CS5532_FUNCTIONS_H_

void Reset_Asic();                        // Used to reset the CS5532 Sigma Delta ADC IC
void Asic_Start();                        // Used as indication of the start convertion for the CS5532
void Initialize_Asic();                  // Used to initiate CS5532
void CS5532_WaitDelay(char);			// Software Delay routine for 32Mhz crystal
void Trans_byte(char trnsbyte);			// Used to transmit a byte of data to the CS5532
void Read_Adc(void);					// Used to read ADC value from the CS5532
void Check_Stable(void);				// Check the stability ADC count received from the CS5532
unsigned long int Read_Asic(void);		// Read the Data from CS5532
unsigned long int TransRec_5b(unsigned char mtrs0, unsigned char mtrs1, unsigned char mtrs2, unsigned char mtrs3, unsigned char mtrs4);
unsigned long int absolute_diff(unsigned long int v1,unsigned long int v2);		// Calculate the absolute count of ADC values.
void Calculate_Weight(void);
void sampling();


#endif /* ADC_CS5532_FUNCTIONS_H_ */
