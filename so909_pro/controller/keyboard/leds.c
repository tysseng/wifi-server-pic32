#include "leds.h"
#include "leds_private.h"

#include "../sequencer/sequencer.h"

#include "../shared/definitions.h"
#include "../shared/led_consts.h"
#include "../shared/types.h"
#include "../../shared/bus_definitions.h"

unsigned short leds[10];
unsigned short ledmap[88];  // mapping between rows/columns and led numbers

char byte;
char bitForMask;
char mask;

/**
 * Reset LED variables
 **/
void LED_init(){
  unsigned short i=0;
  for(i=0; i<10; i++){
    leds[i]=0;
  }
  LED_ENABLE = 0;
  LEDMAP_init(ledmap);
}

void LED_on(unsigned short led){
  byte = led / 8;
  bitForMask = led % 8;
  mask = (1 << bitForMask);

  leds[byte] = leds[byte] | mask;

}

void LED_off(unsigned short led){
  byte = led / 8;
  bitForMask = led % 8;
  mask = 0xFF ^ (1 << bitForMask);

  leds[byte] = leds[byte] & mask;
  
}

void LED_onFromInterrupt(unsigned short led){
  byte = led / 8;
  bitForMask = led % 8;
  mask = (1 << bitForMask);

  leds[byte] = leds[byte] | mask;

}

void LED_offFromInterrupt(unsigned short led){
  byte = led / 8;
  bitForMask = led % 8;
  mask = 0xFF ^ (1 << bitForMask);

  leds[byte] = leds[byte] & mask;

}

char LED_state(unsigned short led){
   byte = led / 8;
   bitForMask = led % 8;
   mask = (1 << bitForMask);
  
  return leds[byte] & mask;

}

void LED_stepOn(unsigned short step){
  LED_on(LED_RED_SEQ01+step);
}

void LED_stepOff(unsigned short step){
  LED_off(LED_RED_SEQ01+step);
}

void LED_recordOn(){
  LED_on(LED_RED_REC);
}

void LED_recordOff(){
  LED_off(LED_RED_REC);
}

void LED_tempoBarOn(){
  LED_onFromInterrupt(LED_GREEN_TEMPO);
}

void LED_tempoQuarterOn(){
  LED_onFromInterrupt(LED_RED_TEMPO);
}

void LED_tempoAllOff(){
  LED_off(LED_RED_TEMPO);
  LED_off(LED_GREEN_TEMPO);
}

void LED_tempoAllOffFromInterrupt(){
  LED_offFromInterrupt(LED_RED_TEMPO);
  LED_offFromInterrupt(LED_GREEN_TEMPO);
}

//
// Turn on a single mode led, turn off all others.
//
void LED_updateModeLeds(char mode){
  unsigned short i;
  for(i=LED_GREEN_MODE_1; i<=LED_GREEN_MODE_5; i++){
    LED_off(i);
  }
  for(i=LED_RED_MODE_1; i<=LED_RED_MODE_5; i++){
    LED_off(i);
  }
  LED_on(LED_RED_MODE_1 + mode);
}

/**
 * Turn on the leds that should be active on the current row.
 **/
void LED_writeLeds(unsigned short row){

  DATA_BUS_DIRECTION = DATA_OUT;
  delay_us(1);
  ADDRESS_BUS = row | SEL_DATA_OUT; //set address and select data output/led enable

  DATA_BUS = LED_getLedState(row);
  delay_us(500);
  DATA_BUS = 0;
  LED_ENABLE = 0;
  delay_us(1);
}

/**
 * Look through led state array and compile a byte where the correct leds
 * are turned on
 **/
unsigned short LED_getLedState(unsigned short row){
  unsigned short state = 0;
  unsigned short col;
  unsigned short currLed = 0;

  //loop through all 8 bits.
  for( col=0; col < 8; col++){

    currLed = ledmap[row*8 + col];

    // if current led is one of the green step leds, check if sequencer is in
    // play mode. If so, light it even if it is not set in the leds[] array.
    if(LED_state(currLed) ||
       (currLed == sequencer.currStep && sequencer.playState != PLAY_STOPPED)){
      if(currLed >= LED_RED_SEQ01 && currLed <= LED_RED_SEQ32){

        // for red step leds: check if current sequencer step equals the current
        // diode, if so, don't light it.
        if(sequencer.currStep + LED_RED_OFFSET != currLed ||
           sequencer.playState == PLAY_STOPPED){
          state += (1<<col);
        }
      } else {
        state += (1<<col);
      }
    }
  }
  return state;
}