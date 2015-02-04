#define MAX_OPERATIONS 20
#define MAX_SH_OUTPUTS 8

// ramp etc
#define UP 1
#define DOWN 0
#define RUNNING 1
#define STOPPED 0

//binary values. Anything from 0 and down is treated as false
#define BINARY_TRUE 127
#define BINARY_FALSE 0

// datatype to use for matrix, makes it easier to switch between 8 and 16 bit
// operations later.
#define matrixint short
#define matrixlongint int
#define MAX_POSITIVE 127
#define MAX_NEGATIVE -128

// dac
#define DAC_TRIS TRISC
#define DAC_CS LATC.B0
#define DAC_CS_ON 0
#define DAC_CS_OFF 1

// functions
#define FUNC_SUM 0
#define FUNC_INVERT 1
#define FUNC_INVERT_EACH_SIDE 2
#define FUNC_RAMP 3
#define FUNC_DELAY_LINE 4
#define FUNC_INPUT 5
#define FUNC_OUTPUT 6

#include <built_in.h>

//function pointer to a matrix node function
typedef void (*nodeFunction)(struct matrixNode *);

//node in matrix
typedef struct matrixNode{
    // function to run when this Node is accessed
    nodeFunction func;

    // parameters passed to this node
    matrixint params[8];

    // Bitwise variable that indicates usage of params:
    // 1 signifies that param is a constant
    // 0 that it is the index of the Node to get result from
    unsigned short paramIsConstant;

    // How many of the input params are used
    unsigned short paramsInUse;

    // high res status variable for ramps etc, to reduce effects of rounding
    // errors.
    int highResState;

    // state, holds flags for
    short state;
    
    // placeholder for result from Node function
    matrixint result;

} Node;

// matrix nodes
Node *nodes[MAX_OPERATIONS];
unsigned short nodesInUse = 0;

// The number of dac updates finished since last time the matrix were run.
// This is checked before a new runMatrix is called as timing is done through
// a timer interrupt that increments what dac to update.
// NB: As dacs may have been updated multiple times since the matrix run started,
// any ramp increments should be multiplied by this number to get correct timing.
unsigned short dacUpdatesFinished;

// what sample-and-hold output to update
unsigned short shToUpdate;

// true if a matrix run has been completed and data is ready to be copied to
// the dac buffer before the next dac cycle.
unsigned short outputBufferComplete;

// dac update timing - interval between each dac update (= s&h acquisition time)
unsigned short dacIntervalTimerStartH;
unsigned short dacIntervalTimerStartL;

// place where matrix reads inputs from
matrixint inputBuffer[8];

// place where matrix write outputs to
matrixint outputBuffer[MAX_SH_OUTPUTS];

// buffer that dac reads from. outputs should be copied here before dac starts
// writing them out, to make it possible to write to output while simultaneously
// calculating the next run.
matrixint dacBuffer[MAX_SH_OUTPUTS];

// Lcd module connections
sbit LCD_RS at LATB2_bit;
sbit LCD_EN at LATB3_bit;
sbit LCD_D4 at LATB4_bit;
sbit LCD_D5 at LATB5_bit;
sbit LCD_D6 at LATB6_bit;
sbit LCD_D7 at LATB7_bit;

sbit LCD_RS_Direction at TRISB2_bit;
sbit LCD_EN_Direction at TRISB3_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;

char txt1[] = "OMM illustrator";
char txt2[] = "EasyPIC7";


void interrupt() {
    unsigned short i;

    if(PIR1.TMR1IF){
        //restart timer
        TMR1H = dacIntervalTimerStartH;
        TMR1L = dacIntervalTimerStartL;
        PIE1.TMR1IE = 1;
        PIR1.TMR1IF = 0;

        // copies output buffer to dac buffer to make sure nothing changes
        // while updating dacs.
        if(shToUpdate == 0){
            if(outputBufferComplete){
                for(i = 0; i < MAX_SH_OUTPUTS; i++){
                    dacBuffer[i] = outputBuffer[i];
                }
                outputBufferComplete = 0;
            }

            // signal that data has been copied and that next matrix calculation
            // may start.
            dacUpdatesFinished++;
        }

        // TODO: Write to dac here
        // writeToDac(dacBuffer[shToUpdate]);

        // step to next sample and hold output
        shToUpdate++;
        
        // finished cycling through all outputs //TODO: timing!
        if(shToUpdate == MAX_SH_OUTPUTS){
            shToUpdate = 0;
        }
    }
}

// params can be pointers to the result of the previous Node in the matrix or
// they can be constants. This function figures out which one and returns its
// value.
matrixint getParam(Node *aNode, unsigned short paramId){
    unsigned short type;

    type = (aNode->paramIsConstant >> paramId) & 0b00000001;
    if(type == 1){
        return aNode->params[paramId];
    } else {
        return nodes[aNode->params[paramId]]->result; //TODO will this work with a 16 bit param?
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
    aNode->result = 0;
    for(i=0; i<aNode->paramsInUse; i++){
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
    bipolar= aNode->params[3].B2;

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
    outputBufferComplete = 1;
}

nodeFunction getFunctionPointer(unsigned short function){
    switch(function){
        case FUNC_SUM:
            return &nodeFuncSum;
        case FUNC_INVERT:
            return &nodeFuncInvert;
        case FUNC_INVERT_EACH_SIDE:
            return &nodeFuncInvertEachSide;
        case FUNC_RAMP:
            return &nodeFuncRamp;
        case FUNC_DELAY_LINE:
            return &nodeFuncDelayLine;
        case FUNC_INPUT:
            return &nodeFuncInput;
        case FUNC_OUTPUT:
            return &nodeFuncOutput;
        default:
            return &nodeFuncNoop;
    }
}

// Write output to DAC. NB: Only positive values are written!
// TODO: Does not work once we switch to 16 bit.
void writeToDac(short output){
    unsigned short positiveOut;
    if(output > 0) {
        positiveOut = output; //7 to 8 bit, as input is signed. NB: this means that the maximum value is 254 (as the LSB is 0).
        positiveOut = positiveOut << 1;
    } else {
        positiveOut = 0;
    }
    
    DAC_CS = DAC_CS_ON;  //must write directly to latch (didn't work with PORTC.B0!)

    SPI1_write(positiveOut);
    SPI1_write(0);       // 0 as long as we are working with 8 bit numbers.

    DAC_CS = DAC_CS_OFF; //latches values in DAC.
}

void printSignedShort(unsigned short row, unsigned short col, short in){
  unsigned short inExpanded;
  unsigned short rest;
  if(in < 0){
    Lcd_Chr(row, col, '-');
    inExpanded = -in;
  } else {
    Lcd_Chr(row, col, ' ');
    inExpanded = in;
  }

  rest = inExpanded % 10;
  Lcd_Chr(row, col+3, 48 + rest);

  rest = (inExpanded / 10) % 10;
  Lcd_Chr(row, col+2, 48 + rest);

  rest = (inExpanded / 100) % 10;
  Lcd_Chr(row, col+1, 48 + rest);
}

void dacTimerInit(){

  // Enable GIE (all interrupt sources)
  // Enable PEIE (all peripheral interrupt sources - timer1, 2, 3 etc)
  // Enable T0IE (enable TMRO interrupt)
  // Disable T0IF (clear TMR0 interrupt)
  INTCON = 0xE0;

  // Timer1 - DAC clock
  TMR1H = dacIntervalTimerStartH;
  TMR1L = dacIntervalTimerStartL;

  T1CON = 0xA4; // Set to 16 bit, prescaler of 4 and timer stopped
  PIR1.TMR1IF = 0; // clear timer 1 interrupt
  PIE1.TMR1IE = 1; // enable timer 1 interrupt
}

// Start dac timer, should be run after everything else is ready to go to prevent
// writing bogus data to outputs.
void dacTimerStart(){
  shToUpdate = 0;
  T1CON.TMR1ON = 1;
}

void dacTimerStop(){
  T1CON.TMR1ON = 0;
}

void dacInit(){
    SPI1_Init();
    DAC_TRIS = 0; //output
    DAC_CS = DAC_CS_OFF;
}


void main() {
    unsigned short iteration;
    Node aNode0, aNode1, aNode2, aNode3, aNode4, aNode5;

    iteration = 0;
	dacInit();
    dacUpdatesFinished = 0;             //necessary to start runMatrix.
    dacTimerInit();
    
    // DEBUG STUFF
    Lcd_Init();                        // Initialize Lcd
    Lcd_Cmd(_LCD_CLEAR);               // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
    Lcd_Out(1,1,txt1);                 // Write text in first row

    inputBuffer[0] = 2;
    inputBuffer[1] = 4;

    aNode0.func = getFunctionPointer(FUNC_INPUT);
    aNode0.params[0] = 0;
    aNode0.paramIsConstant = 0b0000001;
    addNode(&aNode0);

    aNode1.func = getFunctionPointer(FUNC_INPUT);
    aNode1.params[0] = 1;
    aNode1.paramIsConstant = 0b0000001;
    addNode(&aNode1);

    aNode2.func = getFunctionPointer(FUNC_SUM);
    aNode2.params[0] = 0;
    aNode2.params[1] = 4;
    aNode2.paramsInUse = 2;
    aNode2.paramIsConstant = 0b00000000;
    addNode(&aNode2);

    aNode3.func = getFunctionPointer(FUNC_OUTPUT);
    aNode3.params[0] = 0; // write to 0
    aNode3.params[1] = 2; // value from 2;
    aNode3.paramIsConstant = 0b00000001;
    addNode(&aNode3);

    aNode4.func = getFunctionPointer(FUNC_DELAY_LINE);
    aNode4.result = 0; //set initial state to 0
    aNode4.params[0] = 2;
    aNode4.paramIsConstant = 0b00000000;
    addNode(&aNode4);

    // calculate initial state. dacUpdatesFinished will be 0, so any ramps
    // will not be incremented.
    runMatrix();
    
    // start writing to outputs
    dacTimerStart();

    // tight loop that runs at most once for every dacUpdate cycle.
    while(1){
        if(dacUpdatesFinished){
            dacUpdatesFinished = 0;
            runMatrix();
        }
        printSignedShort(2,1,outputBuffer[0]);
        printSignedShort(2,12,iteration++);
    }
}

/*
TODO:
- tuning lookup
- exponential lookup
- JSON parsing
- SPI input
- distance-in-samples-per-cents
- multi stage envelopes (inc looping?)
- LFOs
- Trigger (sends trigger pulse if input is high)

Mathematical expressions
- divide
- average

Logical operators, all with adjustable thresholds (with defaults)
- xor

Outputs
- CV
- trigger pulse (for analog envelope)
- gate (for analog envelope)
- other binary pins (controlling switches etc)
*/