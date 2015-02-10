#ifndef _MATRIX_PRIVATE_H
#define _MATRIX_PRIVATE_H

#include "types.h"

matrixint getParam(Node *aNode, unsigned short paramId);
void nodeFuncSum(Node *aNode);
void nodeFuncMultiply(Node *aNode);
void nodeFuncInvert(Node *aNode);
void nodeFuncInvertEachSide(Node *aNode);
int calculateRampIncrement(matrixint setting, short direction, short bipolar);
void nodeFuncRamp(Node *aNode);
void nodeFuncLfoPulse(Node *aNode);
void nodeFuncDelayLine(Node *aNode);
void nodeFuncMemory(Node *aNode);
void nodeFuncSwitch(Node *aNode);
void nodeFuncCompare(Node *aNode);
void nodeFuncMax(Node *aNode);
void nodeFuncMin(Node *aNode);
void nodeFuncScale(Node *aNode);
void nodeFuncTrigger(Node *aNode);
void nodeFuncBinaryAnd(Node *aNode);
void nodeFuncBinaryOr(Node *aNode);
void nodeFuncBinaryXor(Node *aNode);
void nodeFuncBinaryNot(Node *aNode);
void nodeFuncInput(Node *aNode);
void nodeFuncOutput(Node *aNode);
void nodeFuncNoop(Node *aNode);
void addNode(Node *aNode);
void runMatrix();

#endif