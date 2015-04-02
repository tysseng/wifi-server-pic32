#include "dials.h"
#include "dials_private.h"

#include "../shared/dial_consts.h"
#include "../command_handler.h"
#include "../../shared/bus_definitions.h"

// mapping between columns and dial numbers
unsigned short dialmap[4];

// variables for 'debouncing' encoder clicks
unsigned short previousEncoderState[4];


/**
 * Reset dial flipflops etc
 **/
void DIAL_init(){
  unsigned short col;
  for(col=0; col<4; col++){
    previousEncoderState[col] = 0;
  }
  ADDRESS_BUS = 0xDf;
  DIALMAP_init(dialmap);
}

/**
 * Read and reset dials
 **/
void DIAL_read(){

  unsigned short events;

  DIAL_ENABLE = 1;
  DATA_BUS_DIRECTION = DATA_IN;

  DIAL_generateEvents(DATA_BUS);
  DIAL_ENABLE = 0;

  ADDRESS_BUS = 0x1f; //reset RS-flip flops, 0x1F gir output 0 på pinne 17 på 74HC154 = sel15
  delay_us(10);
  ADDRESS_BUS = 0x10;

}

/**
 * Handle turned dials
 **/
void DIAL_generateEvents(unsigned short events){

  unsigned short col;
  unsigned short dialEvent;
  unsigned short dialCode;

  //loop through all 8 bits.
  for( col=0; col < 4; col++){
    dialEvent = events & (1<<col);

    if( dialEvent ) {
      if( previousEncoderState[col] == 0 ){ //prevent multiple events from one click on the dial
        dialCode = dialmap[col]; //look up ID of dial event
        CMD_dialEventDispatcher(dialCode);
      }
      previousEncoderState[col] = 1;
    } else {
      previousEncoderState[col] = 0;
    }
  }
}

/**
 * map column/row to key number. indexes are [row * 8 + column] ([selX *8 + keysX])
 **/
void DIALMAP_init(unsigned short dialmap[]){
  dialmap[0] = DIAL_MODE_DOWN;
  dialmap[1] = DIAL_MODE_UP;
  dialmap[2] = DIAL_TEMPO_DOWN;
  dialmap[3] = DIAL_TEMPO_UP;
}