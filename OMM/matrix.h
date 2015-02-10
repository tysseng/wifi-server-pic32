#ifndef _MATRIX_H
#define _MATRIX_H

#include "types.h"

extern matrixint inputBuffer[8];
extern unsigned short matrixCalculationCompleted;
void addNode(Node *aNode);
void runMatrix();
nodeFunction getFunctionPointer(unsigned short function);

#endif