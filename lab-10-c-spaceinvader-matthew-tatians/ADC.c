// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/28/2018 
// Student names: Matthew and Tatians
// Last modification date: 4/25/18

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer2.h"
#include "ST7735.h"


uint32_t Convert(uint32_t input);
void VolumeAdjust(void);
uint8_t getVolumeModifier(void);

uint8_t modifier = 0;

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
	SYSCTL_RCGCGPIO_R |= 0x08;		//turn on clock for port D
	SYSCTL_RCGCADC_R |= 0x0001;		//turn on ADC0

	volatile uint8_t delay = 0;
	delay = 1;
	
	GPIO_PORTD_DIR_R &= ~0x04;	  //set PD2 input (0)
	GPIO_PORTD_AFSEL_R |= 0x04;
	GPIO_PORTD_AMSEL_R |= 0x04;
	GPIO_PORTD_DEN_R &= ~0x04;		//disable digital IO on PD2
	
	
	ADC0_PC_R &= ~0xF;            //clear max sample rate field
  ADC0_PC_R |= 0x1;             //configure for 125K samples/sec
	
  ADC0_SSPRI_R = 0x0123;        //Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;      //disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;       //seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;     //clear SS3 field
  ADC0_SSMUX3_R += 5;           //set channel
  ADC0_SSCTL3_R = 0x0006;       //no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;         //disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;       //enable sample sequencer 3

	//Init Timer2 to interrupt every 200ms//half second
	Timer2_Init(&VolumeAdjust, 727240);
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	uint32_t result;
	ADC0_PSSI_R =0x0008;
	while((ADC0_RIS_R & 0x08) == 00){}	//do nothing until RIS_R bit 3 is 1
	result = ADC0_SSFIFO3_R & 0xFFF;		//read data
	ADC0_ISC_R =0x0008;									//acknowledge sample is read
  return result;
}

/**
*Convert changes voltage in to a uint32_t
*param - input, data to convert
*returns a value roughly between 400-2300. Rounded to 0-2.
*/
uint32_t Convert(uint32_t input){
  return (input * 4149 / 10000 + 350); 
}

/**
*Volume Adjust is called by Timer2A_Handler
*reads value of current volume slide/ slide potentiometer and converts into a
*3 decimal point number. Is rounded and adjusts a modifier for playing 
*sound array to enhance/reduce volume
*/
void VolumeAdjust(void){
	uint32_t Data = ADC_In();
	uint32_t Position = Convert(Data);
	modifier = (int) (Position / 1300); //volume 0-4
	//test output of modifier
	//ST7735_SetCursor(0, 9);
	//ST7735_OutUDec(modifier);
}	

/**
*getVolumeModifier is a get method for the play BGM/SFX methods in Sound.c
*returns the static modifier value changed by Volume Adjust interrupt.
*/
uint8_t getVolumeModifier(void){
	return modifier;
}
