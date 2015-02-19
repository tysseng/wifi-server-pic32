#ifndef _OUTPUT_H
#define _OUTPUT_H
#include "config.h"

extern matrixint *outputBuffer;
extern matrixint *dacBuffer;
extern unsigned short dacIntervalTimerStartH;
extern unsigned short dacIntervalTimerStartL;
extern unsigned short shToUpdate;

void writeToDac(unsigned short output);
void dacTimerInit();
void dacTimerStart();
void dacTimerStop();
void dacInit();
void outputBufferInit();

#endif