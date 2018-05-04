// SDCFile.c
// Runs on TM4C123
// This program is a simple demonstration of the SD card,
// file system, and ST7735 LCD.  It will read from a file,
// print some of the contents to the LCD, and write to a
// file.
// Daniel Valvano
// Feb 22, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2016
   Program 4.6, Section 4.3
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2016
   Program 2.10, Figure 2.37

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// hardware connections
// **********ST7735 TFT and SDC*******************
// ST7735
// 1  ground
// 2  Vcc +3.3V
// 3  PA7 TFT reset
// 4  PA6 TFT data/command
// 5  PD7/PB0 SDC_CS, active low to enable SDC
// 6  PA3 TFT_CS, active low to enable TFT
// 7  PA5 MOSI SPI data from microcontroller to TFT or SDC
// 8  PA2 Sclk SPI clock from microcontroller to TFT or SDC
// 9  PA4 MISO SPI data from SDC to microcontroller
// 10 Light, backlight connected to +3.3 V

// **********wide.hk ST7735R*******************
// Silkscreen Label (SDC side up; LCD side down) - Connection
// VCC  - +3.3 V
// GND  - Ground
// !SCL - PA2 Sclk SPI clock from microcontroller to TFT or SDC
// !SDA - PA5 MOSI SPI data from microcontroller to TFT or SDC
// DC   - PA6 TFT data/command
// RES  - PA7 TFT reset
// CS   - PA3 TFT_CS, active low to enable TFT
// *CS  - PD7/PB0 SDC_CS, active low to enable SDC
// MISO - PA4 MISO SPI data from SDC to microcontroller
// SDA  – (NC) I2C data for ADXL345 accelerometer
// SCL  – (NC) I2C clock for ADXL345 accelerometer
// SDO  – (NC) I2C alternate address for ADXL345 accelerometer
// Backlight + - Light, backlight connected to +3.3 V

#include <stdint.h>
#include "diskio.h"
#include "ff.h"
#include "PLL.h"
#include "ST7735.h"
#include "../inc/tm4c123gh6pm.h"

void EnableInterrupts(void);
static FATFS g_sFatFs;
FIL Handle,Handle2;
FRESULT MountFresult;
FRESULT Fresult;
#define MAXBLOCKS 100
#define maxImSize 30*54 //largest image size
#define maxSdSize 10000 //largest sound size

extern unsigned short image1[maxImSize];
extern unsigned char	sound1[maxSdSize];
extern uint16_t width;
extern uint16_t height;

const char ImageAssets[] = "ImageAssets.txt";
const char SoundAssets[] = "SoundAssets.txt";

/*
*Grab_Image takes in an id to find and modifies image1, as well width and height.
*param *id reference to id to find
*/
void Grab_Image(char *id){
}
/*
*Grab_Sound takes in an id to find and returns an array size.
*It also modifies sound1.
*param *id reference to id to find
*returns array size
*/
uint32_t Grab_Sound(char *id){
	return 0;
}


