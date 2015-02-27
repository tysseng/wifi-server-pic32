#include "types.h"
#include "config.h"

#define DAC_TRIS TRISC
#define DAC_CS LATC.B0
#define DAC_CS_ON 0
#define DAC_CS_OFF 1

// place where matrix write outputs to
matrixint *outputBuffer;

// buffer that dac reads from. outputs should be moved here by swapping output
// buffers before dac starts writing outputs, to make it possible to write to
// output while simultaneously calculating the next matrix run.
matrixint *dacBuffer;

// buffers to hold output values, will be mapped to outputBuffer and dacBuffer
matrixint outputBuffer1[MAX_SH_OUTPUTS];
matrixint outputBuffer2[MAX_SH_OUTPUTS];

// dac update timing - interval between each dac update (= s&h acquisition time)
unsigned short dacIntervalTimerStartH;
unsigned short dacIntervalTimerStartL;

// what sample-and-hold output to update
unsigned short shToUpdate;

// Write output to DAC. NB: Only positive values are written!
// TODO: Does not work once we switch to 16 bit.
void writeToDac(unsigned short output){/*
    unsigned short positiveOut;
    if(output > 0) {
        positiveOut = output; //7 to 8 bit, as input is signed. NB: this means that the maximum value is 254 (as the LSB is 0).
        positiveOut = positiveOut << 1;
    } else {
        positiveOut = 0;
    }*/

    DAC_CS = DAC_CS_ON;  //must write directly to latch (didn't work with PORTC.B0!)

//    SPI1_write(positiveOut);
    SPI1_write(output);
    SPI1_write(0);       // 0 as long as we are working with 8 bit numbers.

    DAC_CS = DAC_CS_OFF; //latches values in DAC.
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

// initialize output buffers and set buffer pointers
void outputBufferInit(){
    unsigned short i;
    outputBuffer = &outputBuffer1;
    dacBuffer    = &outputBuffer2;

    for(i=0; i<MAX_SH_OUTPUTS; i++){
        outputBuffer[i] = 0;
        dacBuffer[i]    = 0;
    }
}