#include "matrix.h"
#include "test/matrix.test.h"
#include "output.h"
#include "types.h"
#include "config.h"
#include "nodetypes.h"

#include <built_in.h>

#define RUNTESTS

// The number of dac updates finished since last time the matrix were run.
// This is checked before a new runMatrix is called as timing is done through
// a timer interrupt that increments what dac to update.
// NB: As dacs may have been updated multiple times since the matrix run started,
// any ramp increments should be multiplied by this number to get correct timing.
// This is done by using intervalMultiplier, which is a copy of dacUpdatesFinished
// that stays constant through a whole run of the matrix.
unsigned short dacUpdatesFinished;
unsigned short intervalMultiplier;  //TODO: set og reset denne

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
    matrixint *tempOutputBuffer;
    
    if(PIR1.TMR1IF){
        //restart timer
        TMR1H = dacIntervalTimerStartH;
        TMR1L = dacIntervalTimerStartL;
        PIE1.TMR1IE = 1;
        PIR1.TMR1IF = 0;

        if(shToUpdate == 0){
        
            if(matrixCalculationCompleted){
                // swap output buffer with dac buffer to make sure nothing 
                // changes while updating dacs.
                tempOutputBuffer = outputBuffer;
                outputBuffer = dacBuffer;
                dacBuffer = tempOutputBuffer;
                matrixCalculationCompleted = 0;
            }

            // signal that data has been copied and that next matrix calculation
            // may start.
            dacUpdatesFinished++;
        }

        //writeToDac(dacBuffer[shToUpdate]);

        // step to next sample and hold output
        // TODO: Fix sh shift register
        shToUpdate++;
        
        // finished cycling through all outputs
        if(shToUpdate == MAX_SH_OUTPUTS){
            shToUpdate = 0;
        }
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

#ifdef RUNTESTS
void main() {
    runMatrixTests();
}
#endif

#ifndef RUNTESTS
void main() {
    unsigned short iteration;
    Node aNode0, aNode1, aNode2, aNode3, aNode4, aNode5;

    iteration = 0;
    outputBufferInit();
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

    aNode0.func = getFunctionPointer(NODE_INPUT);
    aNode0.params[0] = 0;
    aNode0.paramIsConstant = 0b0000001;
    addNode(&aNode0);

    aNode1.func = getFunctionPointer(NODE_INPUT);
    aNode1.params[0] = 1;
    aNode1.paramIsConstant = 0b0000001;
    addNode(&aNode1);

    aNode2.func = getFunctionPointer(NODE_SUM);
    aNode2.params[0] = 0;
    aNode2.params[1] = 4;
    aNode2.paramsInUse = 2;
    aNode2.paramIsConstant = 0b00000000;
    addNode(&aNode2);

    aNode3.func = getFunctionPointer(NODE_OUTPUT);
    aNode3.params[0] = 0; // write to 0
    aNode3.params[1] = 2; // value from 2;
    aNode3.paramIsConstant = 0b00000001;
    addNode(&aNode3);

    aNode4.func = getFunctionPointer(NODE_DELAY_LINE);
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
    /*
    while(1){
        if(dacUpdatesFinished){
            dacUpdatesFinished = 0;
            intervalMultiplier = 0;
            runMatrix();
        }
        printSignedShort(2,1,outputBuffer[0]);
        printSignedShort(2,12,iteration++);
    } */
}
#endif

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
- set og reset denne intervalMultiplier

Mathematical expressions
- divide
- average

Outputs
- CV
- trigger pulse (for analog envelope)
- gate (for analog envelope)
- other binary pins (controlling switches etc)
*/