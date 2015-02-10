#include "munit.h"
#include "asserts.h"
#include "../nodetypes.h"
#include "../matrix.private.h"
#include "../matrix.h"
#include "../definitions.h"

void testSum(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_SUM);
    aNode.params[0] = 1;
    aNode.params[1] = 2;
    aNode.params[2] = 4;
    aNode.paramsInUse = 3;
    aNode.paramIsConstant = 0b00000111;
    addNode(&aNode);
    
    runMatrix();
    
    assertEquals(7,aNode.result,"sum");
}

void testMultiply(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_MULTIPLY);
    aNode.params[0] = 2;
    aNode.params[1] = 4;
    aNode.params[2] = 8;
    aNode.paramsInUse = 3;
    aNode.paramIsConstant = 0b00000111;
    addNode(&aNode);
    
    runMatrix();

    assertEquals(64,aNode.result,"product");
}

void testInvertPositive(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_INVERT);
    aNode.params[0] = 10;
    aNode.paramIsConstant = 0b00000001;
    addNode(&aNode);

    runMatrix();

    assertEquals(-10,aNode.result,"invert positive");
}

void testInvertNegative(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_INVERT);
    aNode.params[0] = -10;
    aNode.paramIsConstant = 0b00000001;
    addNode(&aNode);

    runMatrix();

    assertEquals(10,aNode.result,"invert negative");
}

// special case as max positive is 1 less than max negative
void testInvertMaxNegative(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_INVERT);
    aNode.params[0] = MAX_NEGATIVE;
    aNode.paramIsConstant = 0b00000001;
    addNode(&aNode);

    runMatrix();

    assertEquals(MAX_POSITIVE,aNode.result,"invert max negative");
}

void testInvertZero(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_INVERT);
    aNode.params[0] = 0;
    aNode.paramIsConstant = 0b00000001;
    addNode(&aNode);

    runMatrix();

    assertEquals(0,aNode.result,"invert zero");
}

void testInvertEachSideZero(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_INVERT_EACH_SIDE);
    aNode.params[0] = 0;
    aNode.paramIsConstant = 0b00000001;
    addNode(&aNode);

    runMatrix();

    assertEquals(MAX_POSITIVE,aNode.result,"invert each side zero");
}

void testInvertEachSidePositive(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_INVERT_EACH_SIDE);
    aNode.params[0] = MAX_POSITIVE;
    aNode.paramIsConstant = 0b00000001;
    addNode(&aNode);

    runMatrix();

    assertEquals(0,aNode.result,"invert each side positive");
}

void testInvertEachSideNegative(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_INVERT_EACH_SIDE);
    aNode.params[0] = MAX_NEGATIVE;
    aNode.paramIsConstant = 0b00000001;
    addNode(&aNode);

    runMatrix();

    assertEquals(-1,aNode.result,"invert each side negative");
}

void testDelayLine(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_DELAY_LINE);
    aNode.params[0] = 10;
    aNode.paramIsConstant = 0b00000001;
    aNode.result = 0;
    addNode(&aNode);

    assertEquals(0,aNode.result,"delay line precondition");
    
    runMatrix();

    assertEquals(10,aNode.result,"delay line");
}

void testMemorySet(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_MEMORY);
    aNode.params[0] = 10;
    aNode.params[1] = 1; // should set
    aNode.params[2] = 0; // should not clear
    aNode.paramIsConstant = 0b00000111;
    addNode(&aNode);

    runMatrix();

    assertEquals(10,aNode.result,"memory set");
}

void testMemoryHold(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_MEMORY);
    aNode.result = 5; //value to hold - leave unchanged
    aNode.params[0] = 10;
    aNode.params[1] = 0; // should not set
    aNode.params[2] = 0; // should not clear
    aNode.paramIsConstant = 0b00000111;
    addNode(&aNode);

    runMatrix();

    assertEquals(5,aNode.result,"memory hold");
}

void testMemoryClear(){
    Node aNode;
    aNode.func = getFunctionPointer(NODE_MEMORY);
    aNode.result = 7; //initial value
    aNode.params[0] = 10;
    aNode.params[1] = 0; // should not set
    aNode.params[2] = 1; // should clear
    aNode.paramIsConstant = 0b00000111;
    addNode(&aNode);

    runMatrix();

    assertEquals(0,aNode.result,"memory hold");
}


// setup and run test suite
void runMatrixTests(){
    reset();
    add(&testSum);
    add(&testMultiply);
    
    add(&testInvertPositive);
    add(&testInvertNegative);
    add(&testInvertMaxNegative);
    add(&testInvertZero);

    add(&testInvertEachSideZero);
    add(&testInvertEachSidePositive);
    add(&testInvertEachSideNegative);
    
    add(&testDelayLine);

    add(&testMemorySet);
    add(&testMemoryHold);
    add(&testMemoryClear);
    
    run(resetMatrix);
}

// TODO void nodeFuncRamp(Node *aNode){
// TODO void nodeFuncLfoPulse(Node *aNode){