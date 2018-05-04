// Graphics.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play animations
// Matthew Yu and Tatian Flores
// 4/26/18


void Grab_Image(char * name);

void Graphics_Init(void);		//write this, connect to timer3A, write timer3A.c, .h
void Start_Refresh(void);
void Stop_Refresh(void);

void Clear_Buffer(uint16_t newColor);
void Set_Screen(uint8_t in);
void Refresh(void);

void Draw_Sprite(int32_t x, int32_t y, const unsigned short sprite[], uint16_t width, uint16_t height);
void unDraw_Sprite(int32_t x, int32_t y, uint16_t width, uint16_t height);

//general animations
void jump(uint16_t x, uint16_t y, const unsigned short sprite[], uint16_t width, uint16_t height, uint8_t speed);
void attackAnimation(const unsigned short sprite[], uint16_t width, uint16_t height, uint8_t side);

//specific animations
void pkmnTrainerRedAnimation(void);
void mainScreenAnimation(void);

// *************************** Images ***************************
extern unsigned short Pikachu2[];
extern unsigned short Charizard2[];
extern unsigned short Venusaur2[];
extern const unsigned short Red2[];
extern const unsigned short pokeball2[];
extern unsigned short fireSprite2[];
extern unsigned short air_slash[];
extern unsigned short focus_blast[];
extern unsigned short dragon_claw[];
extern unsigned short sludge_bomb[];
extern unsigned short power_whip[];
extern unsigned short earthquake[];
extern unsigned short skull_bash[];
extern unsigned short volt_tackle[];
extern unsigned short brick_break[];

