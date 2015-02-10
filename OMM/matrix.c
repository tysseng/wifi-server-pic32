#include "types.h"
#include "config.h"
#include "definitions.h"
#include "nodetypes.h"
#include "Output.h"

// matrix nodes
Node *nodes[MAX_OPERATIONS];
unsigned short nodesInUse = 0;

// place where matrix reads inputs from
matrixint inputBuffer[8];

// true if a matrix run has been completed and data is ready to be copied to
// the dac buffer before the next dac cycle.
unsigned short matrixCalculationCompleted;

// params can be pointers to the result of the previous Node in the matrix or
// they can be constants. This function figures out which one and returns its
// value.
matrixint getParam(Node *aNode, unsigned short paramId){
    unsigned short type;

    type = (aNode->paramIsConstant >> paramId) & 0b00000001;
    if(type == 1){
        return aNode->params[paramId];
    } else {
        //TODO will this work with a 16 bit param?
        return nodes[aNode->params[paramId]]->result;
    }
}

// sum an arbitrary number of inputs.
void nodeFuncSum(Node *aNode){
    unsigned short i;
    aNode->result = 0;
    for(i=0; i<aNode->paramsInUse; i++){
        aNode->result += getParam(aNode, i);
    }
}

// multiply an arbitrary number of inputs.
void nodeFuncMultiply(Node *aNode){
    unsigned short i;
    aNode->result = getParam(aNode, 0);
    for(i=1; i<aNode->paramsInUse; i++){
        aNode->result *= getParam(aNode, i);
    }
}

// accepts a single input and inverts it.
void nodeFuncInvert(Node *aNode){
    matrixint param;
    param = getParam(aNode, 0);

    //hack to allow whole range to be inverted. chops off top as 128 is not
    //possible with an 8 bit signed variable.
    if(param == MAX_NEGATIVE){
        aNode->result = MAX_POSITIVE;
    } else {
        aNode->result = -param;
    }
}

// accepts a single input and iverts it around the "center" (64 or -63) of the
// side of 0 that the input is: 1 becomes 127 but -1 becomes -126.
// usefull for inverting envelopes without turning them negative.
void nodeFuncInvertEachSide(Node *aNode){
    matrixint param;
    param = getParam(aNode, 0);

    if(param >= 0){
        aNode->result = MAX_POSITIVE - param;
    } else {
        aNode->result = MAX_NEGATIVE - 1 - param;
    }
}

// calculate increment necessary to get the requested ramp timing.
int calculateRampIncrement(matrixint setting, short direction, short bipolar){
    int highResSetting = setting;
    if(direction == 0){
        highResSetting = -highResSetting;
    }
    return highResSetting << 8;
}

// Ramp function, takes three parameters:
// - param[0]: timing
// - param[1]: trigger, ramp is reset when trigger != 0
// - param[2]: start position (may be runtime input or config), depends on paramIsConstant setting
// - param[3]: bitfield
//   - B0=shoudResetWhenFinished
//   - B1=direction, 0=down, 1=up
//   - B2=bipolar, 0=false (only positive numbers are used), 1=true (full range)
void nodeFuncRamp(Node *aNode){

    int increment;
    matrixint trigger, startPosition;
    bit shouldResetWhenFinished, direction, bipolar;

    trigger = getParam(aNode, 1);
    startPosition = getParam(aNode, 2);
    shouldResetWhenFinished = aNode->params[3].B0;

    //TODO: Implement usage of these
    direction = aNode->params[3].B1;
    bipolar = aNode->params[3].B2;

    if(trigger){
        aNode->highResState = startPosition << 8;
        aNode->state.B0 = RUNNING;
    } else {
        increment = calculateRampIncrement(getParam(aNode, 0), direction, bipolar);

        //TODO: This may never reach max, is that a problem?
        //      should possibly add max value before resetting
        if(aNode->state.B0){ // RUNNING
            if( direction == UP   && (aNode->highResState < 0 ||  32767 - aNode->highResState > increment) ||
                direction == DOWN && (aNode->highResState > 0 || -32768 - aNode->highResState <= increment)){
                aNode->highResState += increment;
            } else {
                aNode->state.B0 = STOPPED;
                if(shouldResetWhenFinished){
                    aNode->highResState = startPosition << 8;
                }
            }
        }
    }
    aNode->result = (aNode->highResState >> 8);
}

// A square/pulse wave LFO with settable speed, pulse width, positive and
// negative amplitude, retrigger and start position
void nodeFuncLfoPulse(Node *aNode){
    matrixint cyclelength = getParam(anode, 0);
    matrixint pulsewidth = getParam(aNode, 1);
    matrixint trigger = getParam(aNode, 2);

    //may be set to any value to limit amplitude and save using a scale shape.
    matrixint positive = getParam(aNode, 3);
    matrixint negative = getParam(aNode, 4);

    bit startPosition;
    startPosition = aNode->params[5].B0; // 0 = bottom, 1 = top;

    if(trigger){
        if(startPosition){
            aNode->result = positive;
        } else {
            aNode->result = negative;
        }
        aNode->highResState = 0;
    } else {
        aNode->highResState++;
        if(aNode->highResState == pulsewidth || aNode->highResState == cyclelength){
           //flip
           if(aNode->result > 0 ){
               aNode->result = negative;
           } else {
               aNode->result = positive;
           }
        }
        if(aNode->highResState == cyclelength){
            aNode->highResState = 0;
        }
    }
}

// "delay line", stores the input for one cycle and uses it as feedback in the
// next, makes it possible to make loops in the network (may not be necessary?
// as one can use the value of any Node's previous result in the next run).
//   NB: result should be set to 0 initially (or to whatever starting feedback
//   one wants.
void nodeFuncDelayLine(Node *aNode){
    aNode->result = getParam(aNode, 0);
}

// memory with set and clear, may be used as sample and hold
// Set if param 1 > 0,
// Clear if param 2 > 0 (resets to 0)
void nodeFuncMemory(Node *aNode){
    if(getParam(aNode, 2)){
        aNode->result = 0;
    } else if(getParam(aNode, 1)){
        aNode->result = getParam(aNode, 0);
    }
}

// switch, passes value on input 0 when input 1 is true, reverts to 0 if not
void nodeFuncSwitch(Node *aNode){
    if(getParam(aNode, 1)){
        aNode->result = getParam(aNode, 0);
    } else {
        aNode->result = 0;
    }
}

// compares parameter 0 to parameter 1. If 0 is larger, output is BINARY_TRUE,
// if, not it is BINARY_FALSE
void nodeFuncCompare(Node *aNode){
    if(getParam(aNode, 0) > getParam(aNode, 1)){
        aNode->result = BINARY_TRUE;
    } else {
        aNode->result = BINARY_FALSE;
    }
}

// returns the maximum of all inputs
void nodeFuncMax(Node *aNode){
    unsigned short i;
    matrixint temp;

    aNode->result = MAX_NEGATIVE;
    for(i = 0; i<aNode->paramsInUse; i++){
        temp = getParam(aNode, i);
        if(temp > aNode->result){
            aNode->result = temp;
        }
    }
}

// returns the minimum of all inputs
void nodeFuncMin(Node *aNode){
    unsigned short i;
    matrixint temp;

    aNode->result = MAX_POSITIVE;
    for(i = 0; i<aNode->paramsInUse; i++){
        temp = getParam(aNode, i);
        if(temp < aNode->result){
            aNode->result = temp;
        }
    }
}

// scales input 0 by input 1 / MAX_POSITIVE
void nodeFuncScale(Node *aNode){
    matrixlongint temp;

    temp = getParam(aNode, 0) * getParam(aNode, 1);
    temp = temp >> 8;

    aNode->result = temp;
}

// Generates a pulse (maximum output value) lasting for one iteration
// after the input changes from negative to positive.
void nodeFuncTrigger(Node *aNode){
    if(getParam(aNode, 0) > 0 ){
        if(aNode->state == 0){
            aNode->result = MAX_POSITIVE;
            aNode->state = MAX_POSITIVE;
        } else {
            aNode->result = 0;
        }
    } else {
        aNode->state = 0;
    }
}

// treat input as a binary values and binary AND them
void nodeFuncBinaryAnd(Node *aNode){
    unsigned short paramNum;
    aNode->result = BINARY_TRUE;
    for(paramNum = 0; paramNum < aNode->paramsInUse; paramNum++){
        if(getParam(aNode, paramNum) <= 0){
            aNode->result = BINARY_FALSE;
            break;
        }
    }
}

// treat input as a binary values and binary OR them
void nodeFuncBinaryOr(Node *aNode){
    unsigned short paramNum;
    aNode->result = BINARY_FALSE;
    for(paramNum = 0; paramNum < aNode->paramsInUse; paramNum++){
        if(getParam(aNode, paramNum) > 0){
            aNode->result = BINARY_TRUE;
            break;
        }
    }
}

// treat input as a binary values and binary XOR them
void nodeFuncBinaryXor(Node *aNode){
    unsigned short param0;
    unsigned short param1;

    param0 = getParam(aNode, 0) > 0;
    param1 = getParam(aNode, 1) > 0;

    if(param0 != param1){
      aNode->result = BINARY_TRUE;
    } else {
      aNode->result = BINARY_FALSE;
    }
}

// treat input as a binary value and binary INVERT it
void nodeFuncBinaryNot(Node *aNode){
    if(getParam(aNode,0) > 0){
        aNode->result = BINARY_FALSE;
    } else {
        aNode->result = BINARY_TRUE;
    }
}

// fetch  input from inputBuffer and add it as the result of a Node to be used
// in the matrix
void nodeFuncInput(Node *aNode){
    aNode->result = inputBuffer[getParam(aNode, 0)];
}

// write output to outputBuffer
void nodeFuncOutput(Node *aNode){
    outputBuffer[getParam(aNode, 0)] = getParam(aNode, 1);
}

// do nothing
void nodeFuncNoop(Node *aNode){}

// add Node to the matrix.
void addNode(Node *aNode){
    nodes[nodesInUse] = aNode;
    nodesInUse++;
}

// loop over the matrix array once and calculate all results
void runMatrix(){
    unsigned short i;
    Node *aNode;
    for(i = 0; i<nodesInUse; i++){
      aNode = nodes[i];
      aNode->func(aNode);
    }

    // all nodes have written their data to the output buffer, tell
    // dac loop that new data can be loaded when dac cycle restarts
    matrixCalculationCompleted = 1;
}

extern void resetMatrix(){
    unsigned short i;
    for(i=0; i<MAX_OPERATIONS; i++){
        nodes[i] = 0;
    }
    nodesInUse = 0;
}

nodeFunction getFunctionPointer(unsigned short function){
    switch(function){
        case NODE_SUM:
            return &nodeFuncSum;
        case NODE_INVERT:
            return &nodeFuncInvert;
        case NODE_INVERT_EACH_SIDE:
            return &nodeFuncInvertEachSide;
        case NODE_RAMP:
            return &nodeFuncRamp;
        case NODE_DELAY_LINE:
            return &nodeFuncDelayLine;
        case NODE_INPUT:
            return &nodeFuncInput;
        case NODE_OUTPUT:
            return &nodeFuncOutput;
        case NODE_MULTIPLY:
            return &nodeFuncMultiply;
        case NODE_MEMORY:
            return &nodeFuncMemory;
        default:
            return &nodeFuncNoop;
    }
}