//SDCFile.h
//Runs on LM4F120/TM4C123
//Provides functions that grab images and sound for Graphics and DAC

//Matthew Yu and Tatian Flores
//4/29/18
#ifndef SDCFile_H
#define SDCFile_H
#include <stdint.h>

/*
*Grab_Image takes in an id to find and modifies image1, as well width and height.
*param *id reference to id to find
*/
void Grab_Image(char *id);
/*
*Grab_Sound takes in an id to find and returns an array size.
*It also modifies sound1.
*param *id reference to id to find
*returns array size
*/
uint32_t Grab_Sound(char *id);
#endif