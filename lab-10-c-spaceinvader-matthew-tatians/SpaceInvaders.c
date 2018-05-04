// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 11/21/2017 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2017

 Copyright 2017 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "DAC.h"
#include "Sound.h"
#include "Graphics.h"
#include <time.h>

/**
*Move_t contains data of move, namely base power, name, moveSprite, and moveSFX.
*/
typedef struct{
	//uint32_t moveSpriteAddr;
	char name[20];
	uint8_t base_power;
	uint8_t type;
	unsigned short *moveSprite;
	uint8_t spriteWidth;
	uint8_t spriteHeight;
	unsigned char *moveSFX;
} Move_t;

/**
*Pokemon_t contains data of Pokemon, namely name, typing, hp, atk, def, spd, and a set list of moves
*/
typedef struct Pokemon{
    char name[20];
		uint8_t active;		//0 inactive, 1 active
		uint8_t type1;
		uint8_t type2;
		uint16_t hp;
		uint8_t atk;
		uint16_t def;
		uint8_t spd;
		Move_t * moves[4];
		unsigned short *sprite;
		uint16_t spriteWidth;
		uint16_t spriteHeight;
		unsigned char crySFX;
} Pokemon_t;

/**
*Trainer_t contains data of Trainer, name name, and set list of Pokemon
*/
typedef struct Trainer{
		Pokemon_t pokemon[3];
		//wait for debugging to set up full team
    char *name;
} Trainer_t;


//init functions
void LED_Init(void);
void Switch_Init(void);

//general functions
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void IO_Touch(void);
uint8_t Button_Movement(void);

//battle functions
void PokemonBattle(uint8_t mode);
void Display_Pokemon(uint16_t turn, Trainer_t *Player1, Trainer_t *Player2);
uint8_t Choose_Option(Trainer_t *Player1);
uint8_t NPC_Choose_Option(Trainer_t *Player1, Trainer_t *Player2);
uint8_t Calculation(uint8_t option, uint8_t NPCoption, Trainer_t *Player1, Trainer_t *Player2);
void Move_Animation(uint8_t option, uint8_t NPCoption, Trainer_t *Player1, Trainer_t *Player2, uint8_t first);
uint8_t Update(Trainer_t *Player1, Trainer_t *Player2);


//type chart: no secondary - 0, normal - 1, fighting - 2, flying - 3, 
//poison - 4, ground - 5, rock - 6, bug - 7, ghost - 8, fire - 9, 
//water - 10, grass - 11, electric - 12, psychic - 13, ice - 14, dragon - 15
const uint8_t typeChart[16][16] = {  //row index - attacking type, column index - defending type
  {2, 2 ,2	,2	,2	,2	,2	,2	,2	,2	,2	,2	,2	,2	,2	,2}	,		//shouldn't be accessed by attacking type
	{2, 2 ,2  ,2  ,2  ,2  ,1	,2  ,0  ,2  ,2  ,2  ,2  ,2  ,2  ,2} ,
  {2, 4 ,2  ,1	,1	,1  ,8  ,1	,0  ,2  ,2  ,2  ,2  ,1 	,4  ,2} ,
  {2, 2 ,4  ,2  ,2  ,2  ,1	,4  ,2  ,2  ,2  ,4  ,1 	,2  ,2  ,2} ,
  {2, 2 ,2  ,2  ,1	,1	,1	,4  ,1 	,2  ,2  ,4  ,2  ,2  ,2  ,2} ,
  {2, 2 ,2  ,0  ,4  ,2  ,4  ,1	,2  ,4  ,2  ,1 	,4  ,2  ,2  ,2} ,
  {2, 2 ,1	,4  ,2  ,1	,2  ,4	,2  ,4  ,2  ,2  ,2  ,2  ,4  ,2} ,
  {2, 2 ,1	,1	,4  ,2  ,2  ,2  ,1 	,1 	,2  ,4  ,2  ,4  ,2  ,2} ,
  {2, 0 ,2  ,2  ,2  ,2  ,2  ,2  ,4  ,2  ,2  ,2  ,2  ,0  ,2  ,2} ,
  {2, 2 ,2  ,2  ,2  ,2  ,1	,4	,2  ,1 	,1 	,4  ,2  ,2  ,4  ,1} ,
  {2, 2 ,2  ,2  ,2  ,4  ,4  ,2  ,2  ,4  ,1 	,1 	,2  ,2  ,2  ,1} ,
  {2, 2 ,2  ,1	,1	,4  ,4  ,1 	,2  ,1 	,4  ,1 	,2  ,2  ,2  ,1} ,
  {2, 2 ,2  ,4  ,2  ,0  ,2  ,2  ,2  ,2  ,4  ,1 	,1 	,2  ,2  ,1} ,
  {2, 2 ,4  ,2  ,4  ,2  ,2  ,2  ,2  ,2  ,2  ,2  ,2  ,1 	,2  ,2} ,
  {2, 2 ,2  ,4  ,2  ,4  ,2  ,2  ,2  ,2  ,1 	,4  ,2  ,2  ,1 	,4} ,
  {2, 2 ,2  ,2  ,2  ,2  ,2  ,2  ,2  ,2  ,2  ,2  ,2  ,2  ,2  ,4}
};



//define moves
  Move_t Spark = {
  	"Spark",
  	65,
  	12, 
  	fireSprite2,
  	10,
  	18,
  };
  
  Move_t Flamethrower = {
  	"Flamethrower",
  	70,
    9,
  	fireSprite2,
    10, 18,
  };

  Move_t Air_Slash = {
   "Air Slash",
   75,
   3,
   air_slash,
   10, 18,
  };

  Move_t Focus_Blast = {
   "Focus Blast",
   120,
   2,
	 focus_blast,
   15, 15,
  };

  Move_t Dragon_Claw = {
   "Dragon Claw",
   80,
   15,
	 dragon_claw,
   15, 14,
  };
	
  Move_t Sludge_Bomb = {
   "Sludge Bomb",
   90,
   4,
	 sludge_bomb,
   13, 16,
  };  

  Move_t Power_Whip = {
   "Power Whip",
   120,
   11,
	 power_whip,
   13, 18,
  };   
	
  Move_t Skull_Bash = {
   "Skull Bash",
   130,
   1,
   skull_bash,
   16, 15,
  };

  Move_t Earthquake = {
   "Earthquake",
   100,
   5,
	 earthquake,
   10, 18,
  };	

  Move_t Brick_Break = {
   "Brick Break",
   70,
   2,
	 brick_break,
   16, 10,
  };	

  Move_t Volt_Tackle = {
   "Volt Tackle",
   120,
   12,
	 volt_tackle,
   23, 10,
  };		
	
Pokemon_t PkmnList[] = {
	{"Pikachu", 0, 12, 0, 70, 68, 72, 90,	//hp 35, atk 55, def 40 previously
		{&Spark, &Brick_Break, &Volt_Tackle, &Skull_Bash},
		Pikachu2,
		23, 23,
	},
	{"Charizard", 0, 3, 11, 78, 84, 78, 100, 
		{&Flamethrower, &Air_Slash, &Focus_Blast, &Dragon_Claw},
		Charizard2,
		23, 23,
	},
	{"Venusaur", 0, 4, 11, 80, 82, 83, 80,
		{&Sludge_Bomb, &Power_Whip, &Skull_Bash, &Earthquake},
		Venusaur2, 
		23, 23,
	},
};



// *************************** Capture image dimensions out of BMP**********


int main(void){
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);						//random number gen init
  Output_Init();						//LCD init
	LED_Init();								//PF0-2 init, onboard LED indicators
	Switch_Init();						//PE0-5 init, D-pad, A/B buttons
	ADC_Init();								//ADC0, PD2, timer 2 init, slide pot
	DAC_Init();								//DAC PB0-5 init, speaker, audio jack
	Sound_Init();							//timer 0, 1 init
	Graphics_Init();					//Enabling buffer refresh interrupt

  while(1){
		Stop_Refresh();
		end_BGM();
		Disable_BGM();
		end_SFX();
		ST7735_FillScreen(0xFFFF);            // set screen to black
  
		EnableInterrupts();
		
		//grab random seed from ADC
		Random_Init(Convert(ADC_In()));
		
		/**
		* Intro screen: 
		* pikachu jumps up and runs off animation
		* Pokemon title appears, then menu options (solo, multiplayer)
		* 		subtext: "U/D TO SELECT\nPRESS A TO START"
		* loop menu while polling for specific button press
		*/
		mainScreenAnimation();
		Delay100ms(2);
		ST7735_FillScreen(0x0000); 
		//set start screen
		set_BGM_2();
		Enable_BGM();
		ST7735_DrawCharS(2, 0, 'P', 0xF81F, 0x0000, 2);
		ST7735_DrawCharS(14, 0, 'O', 0x07FF, 0x0000, 2);
		ST7735_DrawCharS(26, 0, 'K', 0x001F, 0x0000, 2);
		ST7735_DrawCharS(38, 0, 'E', 0x07E0, 0x0000, 2);
		ST7735_DrawCharS(50, 0, 'M', 0xFE0F, 0x0000, 2);
		ST7735_DrawCharS(62, 0, 'O', 0xF800, 0x0000, 2);
		ST7735_DrawCharS(74, 0, 'N', 0xFFFF, 0x0000, 2);
		ST7735_SetCursor(2, 3);
		ST7735_OutString("Solo");
		ST7735_SetCursor(2, 6);
		ST7735_OutString("Multiplayer");
		ST7735_SetCursor(1, 3);
		ST7735_OutString("~");
		ST7735_SetCursor(4, 13);
		ST7735_OutString("U/D TO SELECT");
		ST7735_SetCursor(3, 15);
		ST7735_OutString("PRESS A TO START");
	
		//checking for input
		//button press sfx upon polling detects button press.
		
		uint8_t selection = 1;
		while((GPIO_PORTE_DATA_R & 0x10) >> 4 == 0){
		
			//if up switch is pressed, move ~ up if ~ is at multiplayer
			if(((GPIO_PORTE_DATA_R & 0x04) >> 2 != 1) && ((GPIO_PORTE_DATA_R & 0x02) >> 1 == 1)){
				ST7735_SetCursor(1, selection*3);
				ST7735_OutString(" ");
				selection = 1;	
				Button_Press();
				Delay100ms(1);
			}
			//if down switch is pressed, move ~ down if ~ is at solo
			else if(((GPIO_PORTE_DATA_R & 0x04) >> 2 == 1) && ((GPIO_PORTE_DATA_R & 0x02) >> 1 != 1)){
				ST7735_SetCursor(1, selection*3);
				ST7735_OutString(" ");
				selection = 2;
				Button_Press();
				Delay100ms(1);
			}else if(GPIO_PORTE_DATA_R && 1){
				Button_Press();
				Delay100ms(1);	
			}else;
			//update image
			ST7735_SetCursor(1, selection*3);
			ST7735_OutString("~");
		}
		Disable_BGM();
		end_BGM();
		
		/**
		* Solo battle screen: 
		* Red challenges player animation
		* Pokemon title appears, then menu options (solo, multiplayer)
		* 		subtext: "U/D TO SELECT\nPRESS A TO START"
		* continues upon pressing A.
		*/
		if(selection == 1){
			ST7735_FillScreen(0);            // set screen to white
			ST7735_SetCursor(3, 5);
			ST7735_OutString("PKMN Trainer Red");	
			ST7735_SetCursor(3, 6);
			ST7735_OutString("wants to battle!");
			pkmnTrainerRedAnimation();
			
			while(!(GPIO_PORTE_DATA_R & 0x10));
			Button_Press();
			Delay100ms(1);
			
			ST7735_FillScreen(0xFFFF);            // set screen to white
			PokemonBattle(0);
		}
		/**
		* Multiplayer battle screen:
		* screen only shows "Still Implementing", exits upon pressing A.
		*/
		else{
			ST7735_FillScreen(0x0000);            // set screen to black
			ST7735_SetCursor(1, 15);
			ST7735_OutString("Still Implementing");
			while((GPIO_PORTE_DATA_R & 0x10) >> 4 == 0);
			Button_Press();
			Delay100ms(1);
			end_SFX();
		}
		
		Delay100ms(12);
	}//runloop
	
}

//sandboxmain
int mainSandbox(void){
	PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);						//random number gen init
  Output_Init();						//LCD init
	LED_Init();								//PF0-2 init, onboard LED indicators
	Switch_Init();						//PE0-5 init, D-pad, A/B buttons
	ADC_Init();								//ADC0, PD2, timer 2 init, slide pot
	DAC_Init();								//DAC PB0-5 init, speaker, audio jack
	Sound_Init();							//timer 0, 1 init
	Graphics_Init();					//Enabling buffer refresh interrupt
  Stop_Refresh();
	
	while(1){
		EnableInterrupts();
		end_BGM();
		end_SFX();
		//Clear_Buffer();
		//Set_Screen(0);
		//Start_Refresh();
		
		GPIO_PORTF_DATA_R = 0x02;
		
		//prints out x right y up, bottom left corner
		//Draw_Sprite(60, 20, fireSprite, 15, 27);
		//Draw_Sprite(0, 0, fireSprite2, 10, 18);
		set_BGM_2();
		Enable_BGM();
		while(1){
			GPIO_PORTF_DATA_R ^= 0x02;
		}
		
		//test mainSreenAnimation
		mainScreenAnimation();

		//test pkmnTrainerRedAnimation
		ST7735_SetCursor(3, 5);
		ST7735_OutString("PKMN Trainer Red");	
		ST7735_SetCursor(3, 6);
		ST7735_OutString("wants to battle!");
		while((GPIO_PORTE_DATA_R & 0x10) >> 4 == 0);
		for(int i = 0 ; i < 145448; i ++);
		Delay100ms(2);
		pkmnTrainerRedAnimation();
		while((GPIO_PORTE_DATA_R & 0x10) >> 4 == 0);
		Delay100ms(2);
		
		Clear_Buffer(0xFFFF);			//set buffer to Black
		Set_Screen(1);						//set top screen
		Start_Refresh();
		Delay100ms(1);
		//test positionings of strings
		ST7735_SetCursor(1, 0);
		ST7735_OutString("Player");			
		ST7735_SetCursor(14, 0);
		ST7735_OutString("EE319K");		
		//left pokemon name, hp
		ST7735_SetCursor(1, 8);
		ST7735_OutString("Pika");			
		ST7735_SetCursor(1, 9);
		ST7735_OutString("HP: ");	
		ST7735_OutString("999");	
		//right pokemon name, hp
		ST7735_SetCursor(12, 8);
		ST7735_OutString("Chu");			
		ST7735_SetCursor(12, 9);
		ST7735_OutString("HP: ");	
		ST7735_OutString("500");
		//option menu
		ST7735_SetCursor(3, 12);
		ST7735_OutString("Fight");			
		ST7735_SetCursor(3, 14);
		ST7735_OutString("Switch");	
		ST7735_SetCursor(12, 12);
		ST7735_OutString("Forfeit");			
		ST7735_SetCursor(12, 14);
		ST7735_OutString("Option4");
		
		//test positionings of pokemon - move pikachu back and forth
		uint8_t i = 0;
		while(1){
			Draw_Sprite(10, 5, Pikachu2, 23, 23);
			Draw_Sprite(80, 25, Pikachu2, 23, 23);
			/**
			for(int8_t i = -3; i < 3; i++){
				Draw_Sprite(10 + i, 5, pikachuSprite, 30, 30);
				Draw_Sprite(80 + i, 25, pikachuSprite, 30, 30);
				Delay100ms(1);
				unDraw_Sprite(10 + i, 5, 30, 30);
				unDraw_Sprite(80 + i, 25, 30, 30);
			}
			for(int8_t i = 3; i > -3; i--){
				Draw_Sprite(10 + i, 5, pikachuSprite, 30, 30);
				Draw_Sprite(80 + i, 25, pikachuSprite, 30, 30);				
				Delay100ms(1);
				unDraw_Sprite(10 + i, 5, 30, 30);
				unDraw_Sprite(80 + i, 25, 30, 30);
			}**/
			if(i == 1){
				attackAnimation(fireSprite2, 10, 18, 0);
			}
			if(i == 3){
				attackAnimation(fireSprite2, 10, 18, 1);
			}
			i = (i+1)%5;
		}
		
		//test PkmnTrainerRedAnimationBattle
		//Red moving offscreen during battle animation
		/**
		for(uint8_t i = 0; i < 35; i += 2){
			Draw_Sprite(5 - i, 0, pkmnTrainerRed, 30, 54);
			Delay100ms(1);
			unDraw_Sprite(5 - i, 0, 30, 54);
		}**/
	}	
}

 /***********************************************************
 *gameplay functions
 ************************************************************/

/**
*Solo mode PokemonBattle
*gets npc data and generates player data then initiates fight
*/
void PokemonBattle(uint8_t mode){
	//randomly generate a list of pokemon for player
	uint16_t pkmnIdx, turn = 0;
	uint8_t endFlag = 2, option, NPCoption, first;
	if(mode == 0){
		//setup NPC and player
		Trainer_t Player1;		
		Trainer_t Player2;

		for(uint8_t slot = 0; slot <6; slot++){
			pkmnIdx = Random() %3;
			switch(slot){
				case 0: Player1.pokemon[0] = PkmnList[pkmnIdx];
					break;
				case 1: Player1.pokemon[1] = PkmnList[pkmnIdx];
					break;
				case 2: Player1.pokemon[2] = PkmnList[pkmnIdx];
					break;
				case 3: Player2.pokemon[0] = PkmnList[pkmnIdx];
					break;
				case 4: Player2.pokemon[1] = PkmnList[pkmnIdx];
					break;
				case 5: Player2.pokemon[2] = PkmnList[pkmnIdx];
					break;
				default: Player1.pokemon[0] = PkmnList[pkmnIdx];
			}
		}
		Player1.name = "Player";
		Player2.name = "Red";
		//Player1.pokemon[0] = PkmnList[1];
		//Player2.pokemon[0] = PkmnList[1];
		//set init actives
		Player2.pokemon[0].active = 1;
		Player1.pokemon[0].active = 1;
		
		Graphics_Init();					//Enabling buffer refresh interrupt
		Clear_Buffer(0xFFFF);
		Start_Refresh();
	
		set_BGM_1();
		Enable_BGM();
		//Red moving offscreen during battle animation
		for(uint8_t i = 0; i < 80; i += 1){
			Draw_Sprite(45 - i, 0, Red2, 25, 45);
			for(int i = 72724*3/2; i > 0; i--);
			unDraw_Sprite(45 - i, 0, 30, 54);
		}
		//battle sequence
		while(endFlag == 2){
			Display_Pokemon(turn, &Player1, &Player2);								//draw sprites of pokemon on field
			option = Choose_Option(&Player1);													//Player selects option (attack, switch, forfeit)
			NPCoption = NPC_Choose_Option(&Player1, &Player2);				//NPC rudimentary ai selects option
			first = Calculation(option, NPCoption, &Player1, &Player2);				//calculate who moves first, damage calculations, hp remaining
			Move_Animation(option, NPCoption, &Player1, &Player2, first);		//display moves if applicable (e.g. if someone dies, don't display their move)
			endFlag = Update(&Player1, &Player2);											//check end condition and display result if end
			turn++;
		}
		Stop_Refresh();
		Disable_BGM();
		end_BGM();
		if(endFlag == 0){
			Clear_Buffer(0xFFFF);	//clear screen to white
			ST7735_FillScreen(0xFFFF);
			Delay100ms(1);
			Stop_Refresh();
			ST7735_SetCursor(3, 5);
			ST7735_OutString("PKMN Trainer Red");
			ST7735_SetCursor(6, 6);
			ST7735_OutString("is defeated!");
			while(!(GPIO_PORTE_DATA_R & 0x10)){};
			Button_Press();
			
			Victory_SFX();
			ST7735_SetCursor(7, 8);
			ST7735_OutString("You Win!");		
			Delay100ms(3);	
			ST7735_FillScreen(0xFFFF);
			Delay100ms(3);
			//end display if Player1 wins
		}
		else{
			Clear_Buffer(0xFFFF);	//clear screen to white
			ST7735_FillScreen(0xFFFF);
			Delay100ms(1);
			Stop_Refresh();
			ST7735_SetCursor(6, 5);
			ST7735_OutString("Player");
			ST7735_SetCursor(4, 6);
			ST7735_OutString("is defeated!");
			while(!(GPIO_PORTE_DATA_R & 0x10)){};
			Button_Press();
			
			Loss_SFX();
			ST7735_SetCursor(6, 8);
			ST7735_OutString("You Lose!");		
			Delay100ms(3);	
			ST7735_FillScreen(0xFFFF);
			Delay100ms(3);		
			//end display if Player2 wins
		}
	}
	//exit back to main test
}

/*
*Display_Pokemon displays the active pokemon on the screen (both player and opponent)
*param turn turn of battle
*param Player1 pass by reference of player1
*param Player2 pass by reference of player2
*/
void Display_Pokemon(uint16_t turn, Trainer_t *Player1, Trainer_t *Player2){
	//if turn == 0, display pokemon calls, pokemon, new pokemon sound
	if(turn == 0){
		Set_Screen(1);
		//intro statements
		Stop_Refresh();
		ST7735_FillScreen(0);
		Start_Refresh();
		
		ST7735_SetCursor(4, 9);
		ST7735_OutString(Player2->name);	
		ST7735_OutString(" sent out ");	
		ST7735_SetCursor(6, 10);
		ST7735_OutString(Player2->pokemon[0].name);
		ST7735_OutString("!");
		
		//display pokemon
		//fix with correct param
		//New_Pokemon();
		Draw_Sprite(80, 25, Player2->pokemon[0].sprite, Player2->pokemon[0].spriteWidth, Player2->pokemon[0].spriteHeight);
		Delay100ms(3);	//delay to allow sprite to appear
		//end_SFX();
		//clear intro statements
		Stop_Refresh();
		ST7735_FillScreen(0);
		Start_Refresh();
		
		ST7735_SetCursor(3, 9);
		ST7735_OutString("Go! ");	
		ST7735_OutString(Player1->pokemon[0].name);
		ST7735_OutString("!");
		
		//fix with correct param
		//New_Pokemon();
		Draw_Sprite(10, 5, Player1->pokemon[0].sprite, Player1->pokemon[0].spriteWidth, Player1->pokemon[0].spriteHeight);
		Delay100ms(3);
		
		Stop_Refresh();
		ST7735_FillScreen(0);
		Start_Refresh();
		
		ST7735_SetCursor(1, 8);
		ST7735_OutString(Player1->pokemon[0].name);	
		ST7735_SetCursor(1, 9);
		ST7735_OutString("HP: ");					
		ST7735_OutUDec(Player1->pokemon[0].hp);	

		ST7735_SetCursor(12, 8);
		ST7735_OutString(Player2->pokemon[0].name);	
		ST7735_SetCursor(12, 9);
		ST7735_OutString("HP: ");					
		ST7735_OutUDec(Player2->pokemon[0].hp);
		
	}
	else{
		//display left pokemon and status bar
		uint8_t activePokemon;
		//get active pokemon
		for(activePokemon = 0; activePokemon < 3; activePokemon++){
			if(Player1->pokemon[activePokemon].active == 1)
				break;
		}
		//draw active pokemon
		Draw_Sprite(10, 5, Player1->pokemon[activePokemon].sprite, Player1->pokemon[activePokemon].spriteWidth, Player1->pokemon[activePokemon].spriteHeight);
		Delay100ms(3);

		ST7735_SetCursor(1, 8);
		ST7735_OutString(Player1->pokemon[activePokemon].name);	
		ST7735_SetCursor(1, 9);
		ST7735_OutString("HP: ");					
		ST7735_OutUDec(Player1->pokemon[activePokemon].hp);
	
		//display right pokemon and status bar
		for(activePokemon = 0; activePokemon < 3; activePokemon++){
			if(Player2->pokemon[activePokemon].active == 1)
				break;
		}
		//draw active pokemon
		Draw_Sprite(80, 25, Player2->pokemon[activePokemon].sprite, Player2->pokemon[activePokemon].spriteWidth, Player2->pokemon[activePokemon].spriteHeight);
		Delay100ms(3);

		ST7735_SetCursor(12, 8);
		ST7735_OutString(Player2->pokemon[activePokemon].name);	
		ST7735_SetCursor(12, 9);
		ST7735_OutString("HP: ");					
		ST7735_OutUDec(Player2->pokemon[activePokemon].hp);
	}
}
/*
*Choose_Option opens a menu below the action screen that polls for user response.
*param Player1 pass by reference of player1
*returns user response
*/
uint8_t Choose_Option(Trainer_t *Player1){
	//set to default value
	uint8_t selection, move, switchPok, activePokemon, forfeit, availablePkmn;
	do{
		selection = 0xFF, move = 0xFF, switchPok = 0xFF, availablePkmn = 0, forfeit = 0xFF;
		//option menu
		//Stop_Refresh();
		//ST7735_FillScreen(0);
		//Start_Refresh();
		ST7735_SetCursor(3, 11);
		ST7735_OutString("Fight");			
		ST7735_SetCursor(3, 13);
		ST7735_OutString("Forfeit");	
		ST7735_SetCursor(9, 12);
		ST7735_OutString("Switch");			
		//ST7735_SetCursor(9, 14);
		//ST7735_OutString("Option4");
		
		//select option, disable B button output
		do{
			selection = Button_Movement();
		}while(selection == 2 || selection == 0x30);
		Delay100ms(2);
		
		//clear option menu
		Stop_Refresh();
		ST7735_FillScreen(0);
		Start_Refresh();
		
		//get active pokemon
		for(activePokemon = 0; activePokemon < 3; activePokemon++){
			if(Player1->pokemon[activePokemon].active == 1)
				break;
		}
		//if selection = 0, go to fight menu, if forfeit, return 0, if switch, go to switch menu
		switch(selection){
			case 0: //load pokemon move option menu
				ST7735_SetCursor(3, 11);
				ST7735_OutString(Player1->pokemon[activePokemon].moves[0]->name);			
				ST7735_SetCursor(9, 12);
				ST7735_OutString(Player1->pokemon[activePokemon].moves[1]->name);	
				ST7735_SetCursor(9, 14);
				ST7735_OutString(Player1->pokemon[activePokemon].moves[2]->name);			
				ST7735_SetCursor(3, 13);
				ST7735_OutString(Player1->pokemon[activePokemon].moves[3]->name);		
				move = Button_Movement();
				Delay100ms(2);
				break;
			case 1: //load pokemon selection menu
					for(uint8_t i = 0; i < 3; i++)
						if(Player1->pokemon[i].hp != 0)
							availablePkmn ++;
						
					if(availablePkmn > 1){
						ST7735_SetCursor(3, 11);
						if(Player1->pokemon[0].hp != 0)
							ST7735_OutString(Player1->pokemon[0].name);					
						ST7735_SetCursor(9, 12);
						if(Player1->pokemon[1].hp != 0)
							ST7735_OutString(Player1->pokemon[1].name);			
						ST7735_SetCursor(9, 14);
						if(Player1->pokemon[2].hp != 0)
							ST7735_OutString(Player1->pokemon[2].name);	
						do{
							switchPok = Button_Movement();
							//repeat if pokemon is fainted, is the same as active, or default 0xFF
						}while((switchPok == activePokemon) || (Player1->pokemon[switchPok].hp == 0) || (switchPok == 0x03));
					}else{
						ST7735_SetCursor(0, 12);						
						ST7735_OutString("No pokemon available!");
						switchPok = 0xEF;
						while(!(GPIO_PORTE_DATA_R & 0x10)){};
						Button_Press();
					}
				Stop_Refresh();
				ST7735_FillScreen(0);
				Start_Refresh();					
				Delay100ms(2);
				break;
			case 2: break;
			case 3:	//load forfeit prompt
					ST7735_SetCursor(4, 9);
					ST7735_OutString("Do you really");	
					ST7735_SetCursor(3, 10);
					ST7735_OutString("want to forfeit?");
					ST7735_SetCursor(3, 11);
					ST7735_OutString("Yes");			
					ST7735_SetCursor(9, 12);
					ST7735_OutString("No");		
					do{
						forfeit = Button_Movement();
					}while(forfeit == 2 || forfeit == 3); 
					Stop_Refresh();
					ST7735_FillScreen(0);
					Start_Refresh();
					Delay100ms(2);
					break;
			default: break;
		}
	//clear menu
	Stop_Refresh();
	ST7735_FillScreen(0);
	Start_Refresh();	
	//back button manipulation
	}while(((selection == 0) && (move == 0x30)) || ((selection == 1) && (switchPok == 0x30)) || ((selection == 3) && (forfeit == 1 || forfeit == 0x30)));

	
	//return codes
	//0 - mov1, 1 - mov2, 2 - mov4, 3 - mov3, 4 - pok1, 5 - pok2, 6 - pok3, 7 - forfeited
	if(selection == 0)
		return move;
	if(selection == 1)
		return switchPok + 4;
	if(selection == 3 && forfeit == 0)
		return 7;
	else
		return 0;
}
/*
*NPC_Choose_Option chooses option based on what is available to it.
*current algorithm is to calculate theoretical max damage (no crit no miss no flinch)
*of each move on the current pokemon on the field and choose that.
*param Player1 pass by reference of player1
*param Player2 pass by reference of player2
*returns NPC response
*/
uint8_t NPC_Choose_Option(Trainer_t *Player1, Trainer_t *Player2){
	//get active pokemon of each side
	uint8_t activePokemon, type1, type2, bMove;
	uint32_t dmg, bDmg, def;
	for(activePokemon = 0; activePokemon < 3; activePokemon++){
		if(Player1->pokemon[activePokemon].active == 1)
			break;
	}
	type1 = Player1->pokemon[activePokemon].type1;
	type2 = Player1->pokemon[activePokemon].type2;
	def = Player1->pokemon[activePokemon].def;

	for(activePokemon = 0; activePokemon < 3; activePokemon++){
		if(Player2->pokemon[activePokemon].active == 1)
			break;
	}	
	//computer assumes move does max damage (1.2 modifier from Random)
	dmg = 1.2 * Player2->pokemon[activePokemon].moves[0]->base_power * Player2->pokemon[activePokemon].atk / def * 
		typeChart[Player2->pokemon[activePokemon].moves[0]->type][type1] * typeChart[Player2->pokemon[activePokemon].moves[0]->type][type2] / 5;
	bDmg = dmg;
	bMove = 0;
	dmg = 1.2 * Player2->pokemon[activePokemon].moves[1]->base_power * Player2->pokemon[activePokemon].atk / def * 
		typeChart[Player2->pokemon[activePokemon].moves[1]->type][type1] * typeChart[Player2->pokemon[activePokemon].moves[1]->type][type2] / 5;	
	if(dmg > bDmg){
		bDmg = dmg;
		bMove = 1;				
	}
	dmg = 1.2 * Player2->pokemon[activePokemon].moves[2]->base_power * Player2->pokemon[activePokemon].atk / def * 
		typeChart[Player2->pokemon[activePokemon].moves[2]->type][type1] * typeChart[Player2->pokemon[activePokemon].moves[2]->type][type2] / 5;			
	if(dmg > bDmg){
		bDmg = dmg;
		bMove = 3;				
	}
	dmg = 1.2 * Player2->pokemon[activePokemon].moves[3]->base_power * Player2->pokemon[activePokemon].atk / def * 
		typeChart[Player2->pokemon[activePokemon].moves[3]->type][type1] * typeChart[Player2->pokemon[activePokemon].moves[3]->type][type2] / 5;	
	if(dmg > bDmg){
		bDmg = dmg;
		bMove = 2;				
	}		
	return bMove;
}
/*
*Calculation takes into account both players' response and calculates damage.
*It calculates who goes first, what move does what damage, how hp is lowered.
*param option user selected option
*param NPCoption NPC selected option
*param Player1 pass by reference of player1
*param Player2 pass by reference of player2
*returns first, who went first: 0 = player1, 1 = player2, 2 = player1 forfeit
*/
uint8_t Calculation(uint8_t option, uint8_t NPCoption, Trainer_t *Player1, Trainer_t *Player2){
	uint8_t aPkmnP1, aPkmnP2, first = 0, randMod1, randMod2;
	uint32_t dmg;
	//option codes
	//0 - mov1, 1 - mov2, 2 - mov4, 3 - mov3, 4 - pok1, 5 - pok2, 6 - pok3, 7 - forfeited
	
	//get idx of active pokemon
	for(aPkmnP1 = 0; aPkmnP1 < 3; aPkmnP1++){
		if(Player1->pokemon[aPkmnP1].active == 1)
			break;
	}
	for(aPkmnP2 = 0; aPkmnP2 < 3; aPkmnP2++){
		if(Player2->pokemon[aPkmnP2].active == 1)
			break;
	}

	//check switch
	if(option == 4){
		Player1->pokemon[aPkmnP1].active = 0;
		Player1->pokemon[0].active = 1;
		aPkmnP1 = 0;
	}else if(option == 5){
		Player1->pokemon[aPkmnP1].active = 0;
		Player1->pokemon[1].active = 1;
		aPkmnP1 = 1;
	}else	if(option == 6){
		Player1->pokemon[aPkmnP1].active = 0;
		Player1->pokemon[2].active = 1;
		aPkmnP1 = 2;
	}else if(option == 7){	//edge case forfeit
		for(uint8_t i = 0; i < 3; i++){
			Player1->pokemon[i].hp = 0;
		}		
		return 2;
	}else;
	
	//speed calculations
	if(option == 4 || option == 5 || option == 6)
		first = 1;
	else if(Player2->pokemon[aPkmnP2].spd > Player1->pokemon[aPkmnP2].spd)
		first = 1;
	else
		first = 0;
	//damage calculation of first move
	if(first == 1){
		//random modifier is from .8 - 1.2 (4/5), (6/5)
		randMod1 = Random() % 5 + 8;
		randMod2 = 10;
		//get dmg
		//implement crit chance, flinch, miss later
		dmg = ((randMod1 / randMod2) * Player2->pokemon[aPkmnP2].moves[NPCoption]->base_power * 
			Player2->pokemon[aPkmnP2].atk / Player1->pokemon[aPkmnP1].def * 
			(typeChart[Player2->pokemon[aPkmnP2].moves[NPCoption]->type][Player1->pokemon[aPkmnP1].type1] * 
			typeChart[Player2->pokemon[aPkmnP2].moves[NPCoption]->type][Player1->pokemon[aPkmnP1].type2])/4 / 3); //typechart  = 2*2/4 = 1
		//reduce hp
		if(Player1->pokemon[aPkmnP1].hp <= dmg){
			//ignore second move
			dmg = 0;
			//prevent hp underflow
			Player1->pokemon[aPkmnP1].hp = 0;
		}else{
			Player1->pokemon[aPkmnP1].hp -= dmg;
			dmg = 0;
			//damage calculation of second move
			if(option < 4){
				randMod1 = Random() % 5 + 8;
				dmg = (randMod1 / randMod2) * Player1->pokemon[aPkmnP1].moves[option]->base_power * 
					Player1->pokemon[aPkmnP1].atk / Player2->pokemon[aPkmnP2].def * 
					(typeChart[Player1->pokemon[aPkmnP1].moves[option]->type][Player2->pokemon[aPkmnP2].type1] * 
					typeChart[Player1->pokemon[aPkmnP1].moves[option]->type][Player2->pokemon[aPkmnP2].type2])/4 / 3;
			}
		}
		if(Player2->pokemon[aPkmnP2].hp <= dmg){
			Player2->pokemon[aPkmnP2].hp = 0;
		}else{
			Player2->pokemon[aPkmnP2].hp -= dmg;
		}
	}else{
		//random modifier is from .8 - 1.2 (4/5), (6/5)
		randMod1 = Random() % 5 + 8;
		randMod2 = 10;
		
		//get dmg
		//implement crit chance, flinch, miss later
		dmg = (int)(double)((randMod1 / randMod2) * Player1->pokemon[aPkmnP1].moves[option]->base_power * 
			Player1->pokemon[aPkmnP1].atk / Player2->pokemon[aPkmnP2].def * 
			(typeChart[Player1->pokemon[aPkmnP1].moves[option]->type][Player2->pokemon[aPkmnP2].type1] * 
			typeChart[Player1->pokemon[aPkmnP1].moves[option]->type][Player2->pokemon[aPkmnP2].type2])/4 / 3);
		//reduce hp
		if(Player2->pokemon[aPkmnP2].hp <= dmg){
			//ignore second move
			dmg = 0;
			//prevent hp underflow
			Player2->pokemon[aPkmnP2].hp = 0;
		}
		else{
			Player2->pokemon[aPkmnP2].hp -= dmg;
			dmg = 0;
			//damage calculation of second move
			randMod1 = Random() % 5 + 8;
			dmg = (randMod1 / randMod2) * Player2->pokemon[aPkmnP2].moves[NPCoption]->base_power * 
				Player2->pokemon[aPkmnP2].atk / Player1->pokemon[aPkmnP1].def * 
				(typeChart[Player2->pokemon[aPkmnP2].moves[NPCoption]->type][Player1->pokemon[aPkmnP1].type1] * 
				typeChart[Player2->pokemon[aPkmnP2].moves[NPCoption]->type][Player1->pokemon[aPkmnP1].type2])/4 / 3;
		}
		if(Player1->pokemon[aPkmnP1].hp <= dmg){
			Player1->pokemon[aPkmnP1].hp = 0;
		}else{
			Player1->pokemon[aPkmnP1].hp -= dmg;
		}
	}
	return first;
}
/*
*Move_Animation displays the moves of each pokemon.
*It first checks who moves first and if second pokemon has 0 hp, assume second move has not been executed and removes pokemon from field.
*Alternatively, Calculation can, upon making a pokemon faint, null the option to 0 and Move_Animation reads that.
*param option user selected option
*param NPCoption NPC selected option
*param Player1 pass by reference of player1
*param Player2 pass by reference of player2
*param first who goes first
*/
void Move_Animation(uint8_t option, uint8_t NPCoption, Trainer_t *Player1, Trainer_t *Player2, uint8_t first){
	//find active pokemon
	uint8_t aPkmnP1, aPkmnP2;
	for(aPkmnP1 = 0; aPkmnP1 < 3; aPkmnP1++){
		if(Player1->pokemon[aPkmnP1].active == 1)
			break;
	}	
	for(aPkmnP2 = 0; aPkmnP2 < 3; aPkmnP2++){
		if(Player2->pokemon[aPkmnP2].active == 1)
			break;
	}	
	//check who goes first
	if(first == 0){
		//display animation of first move
		attackAnimation(Player1->pokemon[aPkmnP1].moves[option]->moveSprite, Player1->pokemon[aPkmnP1].moves[option]->spriteWidth, 
			Player1->pokemon[aPkmnP1].moves[option]->spriteHeight, 0);
		//play hitsound
		
		//text result
		ST7735_SetCursor(3, 12);
		ST7735_OutString(Player1->pokemon[aPkmnP1].name);
		ST7735_OutString(" used ");
		ST7735_SetCursor(3, 13);		
		ST7735_OutString(Player1->pokemon[aPkmnP1].moves[option]->name);
		ST7735_OutString("!");
		
		while(!(GPIO_PORTE_DATA_R & 0x10)){};
		Button_Press();
		Delay100ms(1);

		//refresh text screen
		Stop_Refresh();
		ST7735_FillScreen(0);
		Start_Refresh();			
		
			//check who goes second, if hp == 0, don't display animation of first move.
		if(Player2->pokemon[aPkmnP2].hp != 0){
			//display animation of second move
			attackAnimation(Player2->pokemon[aPkmnP2].moves[NPCoption]->moveSprite, Player2->pokemon[aPkmnP2].moves[NPCoption]->spriteWidth, 
				Player2->pokemon[aPkmnP2].moves[NPCoption]->spriteHeight, 1);
			//play hitsound		
			
			//text result
			ST7735_SetCursor(3, 12);
			ST7735_OutString(Player2->pokemon[aPkmnP2].name);
			ST7735_OutString(" used ");
			ST7735_SetCursor(3, 13);			
			ST7735_OutString(Player2->pokemon[aPkmnP2].moves[NPCoption]->name);
			ST7735_OutString("!");		

			while(!(GPIO_PORTE_DATA_R & 0x10)){};
			Button_Press();
		}else{
			ST7735_SetCursor(3, 12);
			ST7735_OutString(Player2->pokemon[aPkmnP2].name);			
			ST7735_OutString(" fainted!");
			while(!(GPIO_PORTE_DATA_R & 0x10)){};
			Button_Press();
		}
	}else if(first == 1){
		//display animation of first move
		attackAnimation(Player2->pokemon[aPkmnP2].moves[NPCoption]->moveSprite, Player2->pokemon[aPkmnP2].moves[NPCoption]->spriteWidth, 
			Player2->pokemon[aPkmnP2].moves[NPCoption]->spriteHeight, 1);		
		
		//play hitsound

		//text result
		ST7735_SetCursor(3, 12);
		ST7735_OutString(Player2->pokemon[aPkmnP2].name);
		ST7735_OutString(" used ");
		ST7735_SetCursor(3, 13);		
		ST7735_OutString(Player2->pokemon[aPkmnP2].moves[NPCoption]->name);
		ST7735_OutString("!");					
		while(!(GPIO_PORTE_DATA_R & 0x10)){};
		Button_Press();
		Delay100ms(1);

		//refresh text screen
		Stop_Refresh();
		ST7735_FillScreen(0);
		Start_Refresh();
			
		//check who goes second, if hp == 0, don't display animation of first move.
		if(Player1->pokemon[aPkmnP1].hp != 0){
			if(option < 4){
				//display animation of second move
				attackAnimation(Player1->pokemon[aPkmnP1].moves[option]->moveSprite, Player1->pokemon[aPkmnP1].moves[option]->spriteWidth, 
					Player1->pokemon[aPkmnP1].moves[option]->spriteHeight, 0);
				//play hitsound		

				//text result
				ST7735_SetCursor(3, 12);
				ST7735_OutString(Player1->pokemon[aPkmnP1].name);
				ST7735_OutString(" used ");
				ST7735_SetCursor(3, 13);
				ST7735_OutString(Player1->pokemon[aPkmnP1].moves[option]->name);
				ST7735_OutString("!");	
			
				while(!(GPIO_PORTE_DATA_R & 0x10)){};
				Button_Press();
			}
		}else{
			ST7735_SetCursor(3, 12);
			ST7735_OutString(Player1->pokemon[aPkmnP1].name);			
			ST7735_OutString(" fainted!");
			while(!(GPIO_PORTE_DATA_R & 0x10)){};
			Button_Press();
		}
	}else{
		//text result
		ST7735_SetCursor(3, 12);
		ST7735_OutString(Player1->name);
		ST7735_OutString(" forfeited.");	
		while(!(GPIO_PORTE_DATA_R & 0x10)){};
		Button_Press();
	}
	//refresh text screen
	Stop_Refresh();
	ST7735_FillScreen(0);
	Start_Refresh();	
}
/*
*Update checks the status of both pokemon, updates hp, and if no pokemon is available on one side
*declares winner and sets endFlag to 1.
*param Player1 pass by reference of player1
*param Player2 pass by reference of player2
*/
uint8_t Update(Trainer_t *Player1, Trainer_t *Player2){
	//find active pokemon
	uint8_t aPkmnP1, aPkmnP2, newActiveP1, newActiveP2;
	for(aPkmnP1 = 0; aPkmnP1 < 3; aPkmnP1++){
		//check for active
		if(Player1->pokemon[aPkmnP1].active == 1)
			break;
	}	
	//if active has fainted, find next non fainted pokemon and activate
	if(Player1->pokemon[aPkmnP1].hp == 0){
		Player1->pokemon[aPkmnP1].active = 0;
		newActiveP1 = 3;
		for(uint8_t i = 0; i < 3; i++){
			if(Player1->pokemon[i].hp != 0){
				Player1->pokemon[i].active = 1;
				newActiveP1 = i;
				break;
			}
		}
		//if no non fainted pokemon are available, call game
		if(newActiveP1 == 3){
			//game over display
			unDraw_Sprite(10, 5, 30, 30);
			while(!(GPIO_PORTE_DATA_R & 0x10)){};
			Button_Press();
			return 1; //player2 wins
		}else{
			aPkmnP1 = newActiveP1;
		}
	}
	//update name and hp of player1 pokemon here, if required
	
	for(aPkmnP2 = 0; aPkmnP2 < 3; aPkmnP2++){
		//check for active
		if(Player2->pokemon[aPkmnP2].active == 1)
			break;
	}	
	//if active has fainted, find next non fainted pokemon and activate
	if(Player2->pokemon[aPkmnP2].hp == 0){
		Player2->pokemon[aPkmnP2].active = 0;
		newActiveP2 = 0;
		for(uint8_t i = 0; i < 3; i++){
			if(Player2->pokemon[i].hp != 0){
				Player2->pokemon[i].active = 1;
				newActiveP2 = i;
				break;
			}
		}
		//if no non fainted pokemon are available, call game
		if(newActiveP2 == 0){
			unDraw_Sprite(80, 25, 30, 30);
			while(!(GPIO_PORTE_DATA_R & 0x10)){};
			Button_Press();
			return 0; //player1 wins
		}else{
			aPkmnP2 = newActiveP2;
		}
	}
	//update name and hp of player2 pokemon here, if required
	
	return 2;	//no winner yet, keep playing
}
 /***********************************************************
 *misc functs
 ************************************************************/

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

/**
* IO Touch - debounces the switch in software
*/
void IO_Touch(void) {
 // --UUU-- wait for release; delay for 20ms; and then wait for press
	uint32_t current;
	do{ //check release
		current = GPIO_PORTE_DATA_R & 0x3F;
	}while(current  == 0x01 || current  == 0x02 || current  == 0x04 || 
		current  == 0x08 || current  == 0x10 || current  == 0x20);
	//delay
	//blindwait delay
	for(int i = 0 ; i < 145448; i ++);
	do{ //check press
		current = GPIO_PORTE_DATA_R & 0x3F;
	}while(current == 0);
	//blindwait delay
	for(int i = 0 ; i < 145448; i ++);
}  

/**
*Button_Movement manages cursor on lower screen among options.
*returns selection upon A button press.
*Depends on caller to error check selection.
*/
uint8_t Button_Movement(void){
	uint8_t selection = 0;
	ST7735_SetCursor(2, 11);
	ST7735_OutString("~");		
	//while A button has not been pressed
	while(!(GPIO_PORTE_DATA_R & 0x10)){
		//selection 0 = top left (fight), 1 = top right (forfeit), 2 = bottom right (opt4), 3 = bottom left (switch)
		//check if up is pressed
		if((GPIO_PORTE_DATA_R & 0x02) && (selection == 2 || selection == 3)){
			if(selection == 3){
				//if at switch
				ST7735_SetCursor(2, 13);
				ST7735_OutString(" ");
				ST7735_SetCursor(2, 11);
				ST7735_OutString("~");				
				selection = 0;	
				Button_Press();
				Delay100ms(1);
			}else{
				//if at opt4
				ST7735_SetCursor(8, 14);
				ST7735_OutString(" ");
				ST7735_SetCursor(8, 12);
				ST7735_OutString("~");				
				selection = 1;	
				Button_Press();
				Delay100ms(1);		
			}
		//check if down is pressed
		}else if((GPIO_PORTE_DATA_R & 0x04) && (selection == 0 || selection == 1)){
			if(selection == 0){
				//if at fight
				ST7735_SetCursor(2, 11);
				ST7735_OutString(" ");
				ST7735_SetCursor(2, 13);
				ST7735_OutString("~");
				selection = 3;	
				Button_Press();
				Delay100ms(1);
			}else{
				//if at forfeit
				ST7735_SetCursor(8, 12);
				ST7735_OutString(" ");
				ST7735_SetCursor(8, 14);
				ST7735_OutString("~");				
				selection = 2;	
				Button_Press();
				Delay100ms(1);	
			}
		//check if right is pressed
		}else if((GPIO_PORTE_DATA_R & 0x08) && (selection == 0 || selection == 3)){
			if(selection == 0){
				ST7735_SetCursor(2, 11);
				ST7735_OutString(" ");
				ST7735_SetCursor(8, 12);
				ST7735_OutString("~");
				selection = 1;	
				Button_Press();
				Delay100ms(1);
			}else{
				ST7735_SetCursor(2, 13);
				ST7735_OutString(" ");
				ST7735_SetCursor(8, 14);
				ST7735_OutString("~");
				selection = 2;	
				Button_Press();
				Delay100ms(1);
			}
		//check if left is pressed
		}else if((GPIO_PORTE_DATA_R & 0x01) && (selection == 1 || selection == 2)){
			if(selection == 1){
				ST7735_SetCursor(8, 12);
				ST7735_OutString(" ");
				ST7735_SetCursor(2, 11);
				ST7735_OutString("~");
				selection = 0;	
				Button_Press();
				Delay100ms(1);
			}else{
				ST7735_SetCursor(8, 14);
				ST7735_OutString(" ");
				ST7735_SetCursor(2, 13);
				ST7735_OutString("~");
				selection = 3;	
				Button_Press();
				Delay100ms(1);
			}
		}else if(GPIO_PORTE_DATA_R & 0x20){
			//if B button is pressed only, exit out with code 0xff.
			return 0x30;
		}else;
	}
	return selection;
}

 /***********************************************************
 *init functions
 ************************************************************/

/**
* LED Init - sets status LEDs port initialization for PF0, 1, 2
*/
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

	
	GPIO_PORTF_DATA_R |= 0x01;				//turn on PF1 green LED for on
}

/**
* Switch Init - sets Switch port initialization for PE0-5
*/
void Switch_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000010;	//turn on port E clock
	while((SYSCTL_PRGPIO_R & 0x00000010) == 0){};
	GPIO_PORTE_CR_R |= 0x3F;
	GPIO_PORTE_AMSEL_R = 0x00;
	GPIO_PORTE_AFSEL_R = 0x00;
	GPIO_PORTE_DIR_R &= ~0x3F;				//dir input
	GPIO_PORTE_DEN_R |= 0x3F;
}


