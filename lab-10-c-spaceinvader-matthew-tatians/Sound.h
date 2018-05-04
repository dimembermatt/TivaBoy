// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// November 17, 2014


void Sound_Init(void);
void Sound_Play(const uint8_t *pt, uint32_t count);

void PlaySFX(void);
void Button_Press(void);
void Pikachu_Cry(void);
void Victory_SFX(void);
void Loss_SFX(void);
void end_SFX(void);

void PlayBGM(void);
void set_BGM_1(void);
void set_BGM_2(void);
void end_BGM(void);
void SysTick_Handler(void);
void SysTick_Init(void);;
void Disable_BGM(void);
void Enable_BGM(void);

