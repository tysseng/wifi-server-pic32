#ifndef _MATRIX_H
#define _MATRIX_H

#include "types.h"

extern matrixint inputBuffer[8];
extern unsigned short matrixCalculationCompleted;
extern void addNode(Node *aNode);
extern void runMatrix();
extern void resetMatrix();
nodeFunction getFunctionPointer(unsigned short function);

#endif