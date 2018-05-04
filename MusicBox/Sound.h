// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// November 17, 2014


void Sound_Init(void);
void SysTick_Init(void);

void PlaySFX(void);									//timer 1A interrupt call (inactive)
void end_SFX(void); 								//disables timer 1A (inactive)


void PlayBGM(void);									//timer 0A interrupt call
void ChangeBGM(uint32_t *newBGM, uint32_t newSpeed, unsigned char *newInstr);
																		//changes BGM reference
void end_BGM(void);									//disables timer 0A

void SysTick_Handler(void);					//sysTick interrupt call
void Disable_BGM(void);							//prevents sysTick from enabling timer 0A
void Enable_BGM(void);							//allows sysTick to enable timer 0A

//instruments
extern unsigned char sineArr[];
extern unsigned char guitarArr[];
//BGMs
extern uint32_t JubilifeCity[];
extern uint32_t battleTheme[];
extern uint32_t Oracion[];
extern uint32_t SeaofTime[];
extern uint32_t LittlerootTown[];
extern uint32_t TwinleafTown[];
