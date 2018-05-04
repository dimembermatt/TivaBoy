// Sound.c
// Runs on any computer
// Sound assets based off the original Space Invaders 
// Import these constants into your SpaceInvaders.c for sounds!
// Jonathan Valvano
// November 17, 2014
#include <stdint.h>
#include "Sound.h"
#include "DAC.h"
#include "ADC.h"
#include "Timer1.h"
#include "Timer0.h"
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"

#define C1	19112
#define Cs1	18040
#define Df1	18040
#define D1	17028
#define Ds1	16072
#define Ef1	16072
#define E1	15168
#define F1	14316
#define Fs1	13512
#define Gf1	13512
#define G1	12756
#define Gs1	12040
#define Af1	1204
#define A1	11364
#define As1	10724
#define Bf1	10724
#define B1	10124

#define C2	9556
#define Cs2	9020
#define Df2	9020
#define D2	8514
#define Ds2	8036
#define Ef2	8036
#define E2	7584
#define F2	7158
#define Fs2	6756
#define Gf2	6756
#define G2	6378
#define Gs2	6020
#define Af2	6020
#define A2	5682
#define As2	5362
#define Bf2	5362
#define B2	5062

#define C3	4778
#define Cs3	4510
#define Df3	4510
#define D3	4257
#define Ds3	4018
#define Ef3	4018
#define E3	3792
#define F3	3579
#define Fs3	3378
#define Gf3	3378
#define G3	3189
#define Gs3	3010
#define Af3	3010
#define A3	2841
#define As3	2681
#define Bf3	2681
#define B3	2531

#define C4	2389
#define Cs4	2255
#define Df4	2255
#define D4	2129
#define Ds4	2009
#define Ef4	2009
#define E4	1896
#define F4	1790
#define Fs4	1689
#define Gf4	1689
#define G4	1595
#define Gs4	1505
#define Af4	1505
#define A4	1421
#define As4	1341
#define Bf4	1341
#define B4	1266

#define C5	1195

#define pause 0
#define null 0xFFFF

void Sound_Init(void);
void Sound_Play(const uint8_t *pt, uint32_t count);

void PlaySFX(void);
void Button_Press(void);
void end_SFX(void);

void PlayBGM(void);
void ChangeBGM(uint32_t *newBGM, uint32_t newSpeed, unsigned char *newInstr);
void end_BGM(void);

void SysTick_Handler(void);
void SysTick_Init(void);
void Disable_BGM(void);
void Enable_BGM(void);



//piano
//unsigned char sineArr2[32] = {32, 38, 44, 49, 54, 58, 61, 62, 63, 62, 61, 58, 54, 49, 44, 38, 32, 26, 20, 15, 10, 6, 3, 2, 1, 2, 3, 6, 10, 15, 20, 26};
//softer and centered around 32 to allow for amplification
unsigned char sineArr[32] = {32, 34, 36, 38, 39, 40, 41, 42, 42, 42, 41, 40, 39, 38, 36, 34, 32, 30, 28, 26, 25, 24, 23, 22, 22, 22, 23, 24, 25, 26, 28, 30};
unsigned char guitarArr[32] = {31, 31, 29, 27, 26, 29, 34, 37, 38, 35, 30, 28, 31, 37, 41, 40, 36, 33, 31, 31, 32, 34, 34, 33, 30, 28, 29, 29, 29, 30, 31, 31};
	
uint32_t JubilifeCity[] = {A2, A2, A2, A3, pause, A2, D3, pause, pause, pause, pause, pause, 
  G3, pause, pause, Fs3, pause, pause, D3, A2, pause, E3, pause, pause,
  E2, E2, E2, A3, pause, E2, D3, pause, pause, pause, pause, pause,
  E3, pause, pause, D3, pause, pause, A2, Fs2, pause, B3, pause, pause,
  A2, A2, D3, pause, pause, D3, E3, E3, D3, D3, A2, A2,
  As2, As2, B2, pause, pause, D3, E3, G2, G3, pause, pause,
  A2, A2, D3, pause, pause, pause, G2, Fs2, pause, D3, pause, B3,
  Fs3, Fs3, A2, G3, pause, pause, D3, D3, D3, E3, E3, D3,
  Cs3, Cs3, E2, D3, pause, pause, G3, Fs3, pause, D3, pause, A2,
  D3, pause, pause, A2, pause, pause, D3, pause, pause, E3, Fs3, G3,
  A3, A3, D3, D3, B3, B3, A3, A3, D3, D3, A2, A2,
  D3, pause, pause, pause, pause, pause, E3, pause, E3, Fs3, D3,
  Cs3, Cs3, Cs3, Cs3, Cs3, Cs3, B2, B2, B2, B2, B2, B2,
  Cs3, Cs3, Cs3, Cs3, Cs3, Cs3, D3, D3, D3, D3, D3, D3,
  pause, pause, null }; //eighth note smallest denom
uint32_t Oracion[] = {C3, C3, D3, D3,
  E3, E3, B3, B3, A3, A3, A3, A3, A3, A3, A3, A3, A3, A3, G3, G3,
  C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, D3, D3,
  E3, E3, C4, C4, B3, B3, B3, B3, B3, B3, B3, B3, B3, B3, G3, G3,
  E3, E3, E3, E3, E3, E3, E3, E3, E3, E3, E3, E3, pause, pause, A3, B3,
  C4, C4, E4, E4, B3, B3, E4, E4, A3, A3, A3, A3, A3, A3, G3, G3,
  G3, G3, G3, G3, C3, C3, C3, C3, C3, C3, C3, C3, pause, pause, D3, E3,
  D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, C3, C3,
  C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3,
  C3, C3, C3, C3, C3, C3, C3, C3, C2, C2, D2, D2, E2, E2, B2, B2,
  A2, A2, A2, A2, A2, A2, A2, A2, A2, A2, A2, A2, A2, A2, G2, G2,
  C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, D2, D2, E2, E2, C3, C3,
  B2, B2, B2, B2, B2, B2, B2, B2, B2, B2, B2, B2, B2, B2, G2, G2,
  E2, E2, E2, E2, E2, E2, E2, E2, E2, E2, E2, E2, E2, E2, E2, E2,
  E2, E2, E2, E2, E2, E2, E2, E2, A1, A1, A1, A1, B1, B1, B1, B1,
  C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2,
  B1, B1, B1, B1, B1, B1, B1, B1, G1, G1, G1, G1, A2, A2, B2, B2,
  C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, D3, D3,
  D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, D3, A2, B2,
  C3, C3, E3, E3, B2, B2, E3, E3, A2, A2, A2, A2, A2, A2, G2, G2,
  G2, G2, G2, G2, C2, C2, C2, C2, C2, C2, C2, C2, A2, A2, B2, B2,
  C3, C3, C3, C3, C3, C3, D3, D3, D3, D3, D3, D3, D3, D3, E3, E3,
  F3, F3, E3, E3, E3, E3, E3, E3, E3, E3, E3, E3, D3, D3, D3, D3,
  C3, C3, E3, E3, B2, B2, E3, E3, A2, A2, A2, A2, A2, A2, G2, G2,
  G2, G2, G2, G2, C2, C2, C2, C2, C2, C2, C2, C2, pause, D2, E2, E2,
  D2, D2, D2, D2, D2, D2, D2, D2, D2, D2, D2, D2, D2, D2, C2, C2,
  C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2,
  }; //16th notes smallest denom
uint32_t SeaofTime[] = {E3, E3, E3, D3, D3, D3, E3, D3,
  E3, E3, E3, D3, D3, D3, E3, D3,
  E3, E3, E3, D3, D3, D3, E3, D3,
  E3, E3, E3, D3, G3, B3, C4, D4,
  E4, E4, E4, E4, E4, E4, D4, C4,
  B3, B3, B3, B3, G3, G3, B3, B3,
  A3, A3, E3, E3, E3, E3, D3, D3,
  E3, E3, E3, E3, G3, B3, C4, D4,
  E4, E4, E4, E4, E4, E4, D4, C4,
  B3, B3, B3, B3, G3, G3, B3, B3,
  A3, A3, E4, E4, E4, E4, D4, D4,
  E4, E4, E4, E4, G3, B3, C4, D4,
  E4, E4, E4, E4, E4, E4, D4, C4,
  B3, B3, B3, B3, G3, G3, B3, B3,
  A3, A3, E3, E3, E3, E3, D3, D3,
  E3, E3, E3, E3, G3, B3, C4, D4,
  E4, E4, E4, E4, E4, E4, D4, C4,
  B3, B3, B3, B3, G3, G3, B3, B3,
  A3, A3, E4, E4, E4, E4, D4, D4,
  E4, E4, E4, E4, G4, G4, G4, G4,
  C4, C4, C4, C4, C4, C4, A3, A3,
  B3, B3, B3, B3, G3, G3, B3, B3,
  A3, A3, E3, E3, E3, E3, E3, E3,
  pause, pause, pause, pause, G4, G4, G4, G4,
  C4, C4, C4, C4, C4, C4, A3, A3,
  B3, B3, B3, B3, G3, G3, D3, D3,
  E3, A3, A3, A3, A3, A3, A3, A3,
  pause, pause, pause, pause, G3, B3, C4, D4, null}; //eighth note smallest denom, fast

uint32_t LittlerootTown[] = {pause, C2, F2, G2, 
  A2, A2, A2, G2, A2, G2, A2, Bf2, 
  C3, C3, C3, D3, A2, A2, A2, Cs3,
  D3, D3, E3, E3, Cs3, Cs3, A2, G2,
  F2, E2, F2, A2, D3, D3, D2, E2,
  F2, F2, F2, F2, C3, Bf2, Bf2, A2,
  F2, F2, F2, F2, D3, A2, A2, G2,
  F2, F2, F2, F2, F2, F2, E2, D2,
  E2, E2, E2, F2, G2, C2, C3, Bf2,
  A2, A2, A2, G2, A2, G2, A2, Bf2,
  pause, C3, C3, D3, A2, G2, A2, Cs3,
  D3, D3, E3, E3, F3, F3, A2, G2,
  F2, E2, F2, A2, D3, D3, D2, E2,
  F2, F2, F2, F2, C3, Bf2, Bf2, A2,
  F2, F2, F2, F2, D3, A2, A2, G2,
  F2, F2, F2, F2, F2, F2, E2, F2,
  G2, G2, G2, A2, Bf2, Bf2, A2, Bf2,
  C3, C3, C3, C3, Cs3, Bf2, A2, G2,
  F2, F2, F2, E2, D2, D2, F2, A2,
  G2, G2, G2, G2, pause, F2, D2, F2,
  E2, E2, E2, F2, G2, G2, A2, Bf2,
  C3, C3, C3, C3, pause, A2, Cs3, E3,
  G3, F3, E3, F3, D3, D3, F3, A3,
  G3, G3, G3, G3, pause, F3, D3, F3,
  E3, E3, E3, F3, G3, G3, Bf2, G2, null}; //eighth note lowest denom


uint32_t TwinleafTown[] = {G2, G2, C3, C3, B2, C3, C3, D3, 
  E3, E3, Af2, A2, A2, Bf2, Bf2, Bf2, 
  A2, A2, D3, D3, Df3, D3, D3, E3,
  F3, F3, F2, G2, G2, Af2, Af2,
  D3, D3, C3, C3, G2, C3, C3, C3, 
  B2, B2, A2, A2, D3, D3, Ef3, E3,
  E3, E3, F3, F3, A2, A2, C3, C3,
  B2, B2, C3, C3, A2, A2, Af2, Af2,
  G2, C3, B2, C3, C3, D3, E3, G2,
  A2, A2, D3, C3, C3, C3, C3, C3,
  G2, C3, B2, C3, C3, D3, E3, G3,
  C4, C4, D4, C4, C4, F3, E3, D3,
  C3, C3, G2, C3, C3, D3, D3, C3,
  C3, C3, G2, D3, D3, C3, C3, G2,
  A2, A2, C3, C3, A3, A3, F3, F3,
  B2, B2, D3, D3, B3, B3, A3, A3,
  G2, G2, C3, C3, B2, C3, C3, A3,
  G3, G3, E3, E3, Bf2, Bf2, Af2, Af2,
  A2, A2, D3, D3, Df3, D3, D3, C4,
  Af3, Af3, F3, F3, C3, C3, D3, D3,
  E3, E3, F3, E3, C3, G3, G3, G3,
  D3, D3, Ef3, D3, C3, Af2, Af2, Af2,
  C3, C3, C3, C3, C3, C3, C3, C3,
  C3, C3, C3, C3, C3, C3, C3, C3,
  F3, F3, F3, F3, C3, C3, C3, C3, 
  null}; //eighth note lowest denom	
	
	
	//uint32_t battleTheme[] = {Dlo, pause, Dlo, pause, Alo, Alo, pause, Dlo, pause, Dlo, pause, BFlo, BFlo, pause, Dlo, pause, Dlo, pause,
	//Alo, Alo, pause, Dlo, pause, Dlo, pause, GFlo, GFlo, pause, Dlo, pause, Dlo, pause, Alo, Alo, pause, Dlo, pause, Dlo, pause, DF, DF, pause,
	//D, D, D, D, pause, Alolo, Alolo, Alolo, Alolo, pause, Clo, Clo, Clo, Clo, pause, Clolo, Clolo, Clolo, Clolo, pause, Dlo, Dlo, pause, pause, null};

	





unsigned char *sfx;
	
uint32_t *bgm;
unsigned char *wavArr;
	
uint8_t mode, BGM_on;
uint8_t BGM_idx = 0;



void Sound_Init(void){
	//Timer1_Init(&PlaySFX, period);
	Timer0_Init(&PlayBGM, 0xFFFFFF);	//dummy value, interrupt reload value set by note frequency
	
	SysTick_Init();
};




/* BGM */

/**
 *PlayBGM is called by Timer0A_Handler
 *plays bgm until disabled by end_BGM
 */
void PlayBGM(void){
	//play a note of x frequency until end_SFX is called - 
	static uint8_t idx3 = 0; 
	int8_t mVal;
	uint8_t modifier = getVolumeModifier();
	//if modifier = 0, mVal = 0
	//if mod = 4 && val = 32, mVal would be 0
	//if mod = 4 && val = 22, mVal would be 2
	//if mod = 4 && val = 42, mVal would be 62
	//etc...
	mVal = (wavArr[idx3] - 32) * modifier / 2;
	if(modifier == 0)
		DAC_Out(wavArr[idx3] * modifier);
	else
		DAC_Out(wavArr[idx3] + mVal);
	
	//dump[idx3] = sineArr[idx3] + mVal;
	idx3 = (idx3 + 1) % 32;
}	

/**
 *ChangeBGM switches reference BGM that the interrupt handlers play.
 */
void ChangeBGM(uint32_t *newBGM, uint32_t newSpeed, unsigned char *newInstr){
	bgm = newBGM;
	wavArr = guitarArr;
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = newSpeed;					// reload value to 1ms bus clock
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;                                      // enable SysTick with core clock
  NVIC_ST_CTRL_R = 0x07;//NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
	BGM_on = 1;	
	BGM_idx = 0;
};

/**
*end_BGM disables Timer0A
*/
void end_BGM(void){
	TIMER0_CTL_R = 0x00000000;
};





/**
*Temporarily in charge of picking the notes and changing the frequency for PlayBGM
*/
void SysTick_Handler(void){
	//only play notes if BGM_on is active
	if(BGM_on == 1){
		//set frequency of interrupt based on note sequence
		//restart from beginning when null is reached
		if(bgm[BGM_idx] == null)
			BGM_idx = 0;
		//change sound frequency if next note to play isn't 0 or null
		if(bgm[BGM_idx] != pause){
			TIMER0_TAILR_R = bgm[BGM_idx]-1;
			TIMER0_TAPR_R = 0;
			TIMER0_CTL_R = 0x00000001; 
		}
		else
			end_BGM();
		BGM_idx ++;
	}
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 727240*15;					// reload value to 1ms bus clock
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;                                      // enable SysTick with core clock
  NVIC_ST_CTRL_R = 0x07;//NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
	BGM_on = 1;
}

void Disable_BGM(void){
	BGM_on = 0;
}

void Enable_BGM(void){
	BGM_on = 1;
}

