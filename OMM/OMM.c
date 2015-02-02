#define MAX_OPERATIONS 20

#define UP 1
#define DOWN 0

#define RUNNING 1
#define STOPPED 0

#include <built_in.h>

//function pointer to a matrix node function
typedef void (*nodeFunction)(struct matrixNode *);

//node in matrix
typedef struct matrixNode{
  // placeholder for result from Node function
    short result;

    // parameters passed to this node
    short params[8];

    // high res status variable for ramps etc, to reduce effects of rounding
    // errors.
    int highResState;

    // state, holds flags for
    short state;

    // Bitwise variable that indicates usage of params:
    // 1 signifies that param is a constant
    // 0 that it is the index of the Node to get result from
    unsigned short paramIsConstant;

    // How many of the input params are used
    unsigned short paramsInUse;

    // function to run when this Node is accessed
    nodeFunction func;
} Node;

// matrix nodes
Node *nodes[MAX_OPERATIONS];
short nodesInUse = 0;

short inputBuffer[8];
short outputBuffer[8];

short temp;

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




/*
void interrupt(){
    //reset timer etc
} */

// params can be pointers to the result of the previous Node in the matrix or
// they can be constants. This function figures out which one and returns its
// value.
short getParam(Node *aNode, unsigned short paramId){
    unsigned short type;

    type = (aNode->paramIsConstant >> paramId) & 0b00000001;
    if(type == 1){
        return aNode->params[paramId];
    } else {
        return nodes[aNode->params[paramId]]->result;
    }
}

// sum an arbitrary number of inputs.
void nodeFuncSum(Node *aNode){
    unsigned short i;
    aNode->result = 0;
    for(i=0; i<aNode->paramsInUse; i++){
      temp = getParam(aNode, i);
        aNode->result += temp;
    }
}

// accepts a single input and inverts it.
void nodeFuncInvert(Node *aNode){
    short param;
    param = getParam(aNode, 0);

    //hack to allow whole range to be inverted. chops off top as 128 is not
    //possible with an 8 bit signed variable.
    if(param == -128){
        aNode->result = 127;
    } else {
        aNode->result = -param;
    }
}

// accepts a single input and iverts it around the "center" (64 or -63) of the
// side of 0 that the input is: 1 becomes 127 but -1 becomes -126.
// usefull for inverting envelopes without turning them negative.
void nodeFuncInvertEachSide(Node *aNode){
    short param;
    param = getParam(aNode, 0);

    if(param >= 0){
        aNode->result = 127-param;
    } else {
        aNode->result = -129-param;
    }
}

// calculate increment necessary to get the requested ramp timing.
int calculateRampIncrement(short setting, short direction, short bipolar){
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
    short trigger, startPosition;
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
        if(aNode->state.B0){ // RUNNING
            if( direction == UP   && (aNode->highResState < 0 ||  32767 - aNode->highResState > increment) ||
                direction == DOWN && (aNode->highResState > 0 || -32768 - aNode->highResState <= increment)){
                aNode->highResState += increment;
            } else {
//                aNode->state.B0 = STOPPED;
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

// fetch  input from inputBuffer and add it as the result of a Node to be used
// in the matrix
void nodeFuncInput(Node *aNode){
    aNode->result = inputBuffer[getParam(aNode, 0)];
}

// write output to outputBuffer
void nodeFuncOutput(Node *aNode){
    outputBuffer[getParam(aNode, 0)] = getParam(aNode, 1);
}

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

void writeToDac(){
    unsigned int dacout;
    
     /**** DAC ****/
     SPI1_Init();
     TRISC = 0; //trisc as output
     LATC.B0 = 1;
     dacout=0;

     // The following code runs at about 89kHz.
     while(1){
         LATC.B0 = 0; //must write directly to latch (didn't work with PORTC.B0!)
         SPI1_write(hi(dacout));
         SPI1_write(lo(dacout) & 0b11111100);
         LATC.B0 = 1; //latches values in DAC.
         dacout += 2048;
     }
}

void main() {
    unsigned short iteration;
    Node aNode0, aNode1, aNode2, aNode3, aNode4, aNode5;

    iteration =0;

    // DEBUG STUFF
    Lcd_Init();                        // Initialize Lcd
    Lcd_Cmd(_LCD_CLEAR);               // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
    Lcd_Out(1,1,txt1);                 // Write text in first row

    inputBuffer[0] = 2;
    inputBuffer[1] = 4;

    aNode0.func = &nodeFuncInput;
    aNode0.params[0] = 0;
    aNode0.paramIsConstant = 0b0000001;
    addNode(&aNode0);

    aNode1.func = &nodeFuncInput;
    aNode1.params[0] = 1;
    aNode1.paramIsConstant = 0b0000001;
    addNode(&aNode1);

    aNode2.func = &nodeFuncSum;
    aNode2.params[0] = 0;
    aNode2.params[1] = 4;
    aNode2.paramsInUse = 2;
    aNode2.paramIsConstant = 0b00000000;
    addNode(&aNode2);

    aNode3.func = &nodeFuncOutput;
    aNode3.params[0] = 0; // write to 0
    aNode3.params[1] = 2; // value from 2;
    aNode3.paramIsConstant = 0b00000001;
    addNode(&aNode3);

    aNode4.func = &nodeFuncDelayLine;
    aNode4.result = 0; //set initial state to 0
    aNode4.params[0] = 2;
    aNode4.paramIsConstant = 0b00000000;
    addNode(&aNode4);

    runMatrix();
    printSignedShort(2,1,outputBuffer[0]);
    printSignedShort(2,12,iteration++);

    while(1){
        delay_ms(1000);
        runMatrix();
        printSignedShort(2,1,outputBuffer[0]);
        printSignedShort(2,12,iteration++);
    }
    
    // DEBUG STUFF
    printSignedShort(2,1,aNode3.result);


}