/**
 * MusicBox.c
 * C, ARM assembly
 * @author: Matthew Yu
 * @version 1.1
 * Last Modified: 11/28/18
 * 	11/28/18 - comment clean, beginning to switch into C++ OOP design
 *
 * This program is a music box with a simple UI, allowing users
 * to navigate and play through a simple set of songs preprogrammed
 * into the TivaBoy. See documentation on hardware here:
 * TODO: increase song list, enable polyphony - chords, multiple notes played at once
 * develop a standard and program to automatically translate discrete note sound files (MIDI?) into key sequences
 * Songs preprogrammed are:
 *      Pokemon DPPT 		- Jubilife City
 *      Pokemon Movie		- Oracion
 *			Pokemon PMD EoS - Through the Sea of Time
 *			Pokemon RSE			-	Littleroot Town
 *			Pokemon DPPT		- Twinleaf Town
 **/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "ADC.h"
#include "DAC.h"
#include "Sound.h"

#define playlistSize 15

typedef struct Song{
	char *name;
	uint32_t *song;
	uint32_t speed;
	unsigned short *coverImage;
	//assume each cover image is 50x50 pixels
}Song_t;

/**
 * the struct Playlist contains an array of songs,
 * with a reference to its array and its name.
 **/
Song_t Playlist[] = {
	{"Jubilife City      ", JubilifeCity, 727240*15},
	{"Hearthome City     ", },
	{"Pokemon FR/LG op   ", },
	{"Oracion            ", Oracion, 727240*13},
	{"Sea of Time        ", SeaofTime, 727240*13},
	{"Littleroot Town    ", LittlerootTown, 727240*20},
	{"Twinleaf Town      ", TwinleafTown, 727240*20},
	{"dummy8             ", },
	{"dummy9             ", },
	{"dummy10            ", },
	{"dummy11            ", },
	{"dummy12            ", },
	{"dummy13            ", },
	{"dummy14            ", },
	{"dummy15            ", },
};

//init functions
void LED_Init(void);
void Switch_Init(void);

//general functions
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

//program specific functions
uint8_t optionMenu(void);
void playMenu(uint8_t selection);

int mainSandbox(void){
	uint8_t selection = 0;

	PLL_Init(Bus80MHz);       // Bus clock is 80 MHz
	Output_Init();						//LCD init
	LED_Init();							//PF0-2 init, onboard LED indicators
	Switch_Init();						//PE0-5 init, D-pad, A/B buttons
	ADC_Init();							//ADC0, PD2, timer 2 init, slide pot
	DAC_Init();							//DAC PB0-5 init, speaker, audio jack
	Sound_Init();						//timer 0, 1 init

	Disable_BGM();
	end_BGM();
	EnableInterrupts();
	while(1){
		playMenu(selection);
	}
}
int main(void){
	uint8_t selection = 0;

	PLL_Init(Bus80MHz);       // Bus clock is 80 MHz
  	Output_Init();						//LCD init
	LED_Init();							//PF0-2 init, onboard LED indicators
	Switch_Init();						//PE0-5 init, D-pad, A/B buttons
	ADC_Init();							//ADC0, PD2, timer 2 init, slide pot
	DAC_Init();							//DAC PB0-5 init, speaker, audio jack
	Sound_Init();						//timer 0, 1 init

	Disable_BGM();
	end_BGM();
	EnableInterrupts();
	do{
		//main engine
		//intro screen
		//display intro graphics here
		ST7735_FillScreen(0);            // set screen to black
		ST7735_DrawCharS(22, 0, 'J', 0xF81F, 0x0000, 2);
		ST7735_DrawCharS(34, 0, 'U', 0x07FF, 0x0000, 2);
		ST7735_DrawCharS(46, 0, 'K', 0x001F, 0x0000, 2);
		ST7735_DrawCharS(58, 0, 'E', 0x07E0, 0x0000, 2);
		ST7735_DrawCharS(70, 0, 'B', 0xFE0F, 0x0000, 2);
		ST7735_DrawCharS(82, 0, 'O', 0xF800, 0x0000, 2);
		ST7735_DrawCharS(94, 0, 'X', 0xFFFF, 0x0000, 2);

		ST7735_SetCursor(3, 15);
		ST7735_OutString("PRESS A TO START");
		while(!(GPIO_PORTE_DATA_R & 0x10));

		//option menu
			//display list of music here
		selection = optionMenu();
		if(selection != 0xFF){
			//play menu
			playMenu(selection);
		}

	}while(selection != 0xFF);			//exit when exit flag is raised in option menu
	ST7735_FillScreen(0);
	GPIO_PORTF_DATA_R ^= 0x02;			//turn on PF1 green LED for on
	return 1;
}

/**
 * Option menu is a function that displays all of the songs available in
 * microcontroller. It allows for up and down scrolling and clicking on songs to play.
 * return selection song to play
 **/
uint8_t optionMenu(void){
	uint8_t selection = 0;

	ST7735_FillScreen(0);            // set screen to black
	ST7735_DrawCharS(2, 0, 'P', 0xF81F, 0x0000, 2);
	ST7735_DrawCharS(14, 0, 'l', 0x07FF, 0x0000, 2);
	ST7735_DrawCharS(26, 0, 'a', 0x001F, 0x0000, 2);
	ST7735_DrawCharS(38, 0, 'y', 0x07E0, 0x0000, 2);
	ST7735_DrawCharS(50, 0, 'l', 0xFE0F, 0x0000, 2);
	ST7735_DrawCharS(62, 0, 'i', 0xF800, 0x0000, 2);
	ST7735_DrawCharS(74, 0, 's', 0xFFFF, 0x0000, 2);
	ST7735_DrawCharS(86, 0, 't', 0xF81F, 0x0000, 2);
	//shift cursor up and down to select songs
	do{

		//display songs up until amount that fit the screen
		for(uint8_t i = 0; i < 7; i++){
			ST7735_SetCursor(1, i*2 + 3);
			if(i + selection < playlistSize)
				ST7735_OutString(Playlist[i + selection].name);
			else
				ST7735_OutString("                   ");
		}


		if((GPIO_PORTE_DATA_R & 0x02) && (selection > 0)){	//PE1 (UP) is pressed
			selection --;
		}else if((GPIO_PORTE_DATA_R & 0x04) && ((selection < playlistSize) && (selection < 14))){	//PE2 (DOWN) is pressed
			selection ++;
		}else if(GPIO_PORTE_DATA_R & 0x20){	//PF5 (B) is pressed
			Delay100ms(1);
			return 0xFF;
		}else{	//display cursor
			ST7735_SetCursor(0, 3);
			ST7735_OutString("~");
		}
		Delay100ms(1);

	}while(!(GPIO_PORTE_DATA_R & 0x10));	//loop until PE4 (A) is pressed
	Delay100ms(1);
	return selection;
};

/**
 * playMenu takes the selection from option menu and draws a screen with
 * the name of the song selected and its current note/total notes.
 * the song is played and the A button can pause/unpause the song
 * while the B button exits from the menu.
 * param - selection selection from the Playlist
 */
void playMenu(uint8_t selection){
	uint8_t toggle = 0;							 //0 - off, 1 - on

	ST7735_FillScreen(0);            // set screen to black
	ST7735_SetCursor(3, 10);
	ST7735_OutString(Playlist[selection].name);

	//change bgm to selection and enable song play
	ChangeBGM(Playlist[selection].song, Playlist[selection].speed, sineArr);
	Enable_BGM();

	do{
		if(toggle == 0){
			ST7735_SetCursor(3, 13);
			ST7735_OutString("Play ");
			//turn off BGM interrupt
			Disable_BGM();
			end_BGM();
		}else{
			ST7735_SetCursor(3, 13);
			ST7735_OutString("Pause");
			//turn on BGM interrupt
			Enable_BGM();
		}
		Delay100ms(2);
		if(GPIO_PORTE_DATA_R & 0x10)
			toggle = (toggle + 1) % 2;
	}while(!(GPIO_PORTE_DATA_R & 0x20));
	Disable_BGM();
	end_BGM();
};


/**
 * Delay100ms function delays program for count * 100 ms.
 * param: count - how many 100 ms to delay for
 */
void Delay100ms(uint32_t count){
	uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

/***********************************************************
 *init functions
 ************************************************************/

/**
 * LED Init - sets status LEDs port initialization for PF0, 1, 2
 **/
void LED_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000020;	//turn on port F
	volatile uint8_t delay = 0;
	delay = 1;
	delay = 2;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R |= 0x0E;
	GPIO_PORTF_AMSEL_R = 0x00;
	GPIO_PORTF_AFSEL_R = 0x00;
	GPIO_PORTF_DIR_R |= 0x0E;					//dir output
	GPIO_PORTF_DEN_R |= 0x0E;


	GPIO_PORTF_DATA_R |= 0x02;				//turn on PF1 green LED for on
}

/**
 * Switch Init - sets Switch port initialization for PE0-5
 **/
void Switch_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000010;	//turn on port E clock
	while((SYSCTL_PRGPIO_R & 0x00000010) == 0){};
	GPIO_PORTE_CR_R |= 0x3F;
	GPIO_PORTE_AMSEL_R = 0x00;
	GPIO_PORTE_AFSEL_R = 0x00;
	GPIO_PORTE_DIR_R &= ~0x3F;				//dir input
	GPIO_PORTE_DEN_R |= 0x3F;
}
