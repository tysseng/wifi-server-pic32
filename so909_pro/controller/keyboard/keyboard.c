#include "keyboard.h"
#include "keyboard_private.h"

#include "../shared/key_consts.h"
#include "../command_handler.h"
#include "../../shared/bus_definitions.h"


unsigned short keymap[88];  // mapping between rows/columns and key numbers

// variables for detecting button changes.
unsigned short previousState[11];
unsigned short currentState[11];

/**
 * Read initial keyboard states
 **/
void KBD_init(){
  unsigned short row=0;
  for(row = 0; row < ROWCOUNT; row++){
    DATA_BUS_DIRECTION = DATA_IN;
    ADDRESS_BUS = row | SEL_DATA_IN; //Set address and select data input
    delay_ms(1);
    currentState[row] = DATA_BUS;
    previousState[row] = currentState[row];
  }
  KBD_DISABLE = 1;
  KEYMAP_init(keymap);
}

/**
 * Read all keys on the current row and raise a key pressed/released event
 **/
void KBD_read(unsigned short row){

  //read the keyboard
  DATA_BUS_DIRECTION = DATA_IN;
  
  KBD_DISABLE = 0;

  ADDRESS_BUS.F0 = row.F0;
  ADDRESS_BUS.F1 = row.F1;
  ADDRESS_BUS.F2 = row.F2;
  ADDRESS_BUS.F3 = row.F3;
  delay_ms(1);
  currentState[row] = DATA_BUS;
  KBD_DISABLE = 1;

  //check if a button is pressed or released.
  if( currentState[row] != previousState[row]) {
    KBD_generateEvents( row, ~currentState[row] & previousState[row], KEYDIR_DOWN ); //press
    KBD_generateEvents( row, currentState[row] & ~previousState[row], KEYDIR_UP ); //release
  }
  previousState[row] = currentState[row];
}

/**
 * Handle pressed keys. If press = 1, a keyPressed event is added, else a
 * keyReleased is added.
 **/
void KBD_generateEvents( unsigned short row, unsigned short states, unsigned short keydirection ) {

  unsigned short col;
  unsigned short keyPressed;
  unsigned short keyCode;

  //loop through all 8 bits.
  for( col=0; col < 8; col++){
    keyPressed = states & (1<<col);

    if( keyPressed ) {
      keyCode = keymap[row*8+col]; //lookup ID of key pressed.
      CMD_keyEventDispatcher(keyCode, keydirection);
    }
  }
}

/**
 * map column/row to key number. indexes are [row * 8 + column] ([selX *8 + keysX])
 **/
void KEYMAP_init(unsigned short keymap[]){
  keymap[0*8+0] = KEY_TAP_TEMPO;
  keymap[0*8+1] = KEY_REC;
  keymap[0*8+2] = KEY_STOP;
  keymap[0*8+3] = KEY_PLAY;
  keymap[0*8+4] = KEY_SOLO;
  keymap[0*8+5] = KEY_MUTE;

  keymap[1*8+0] = KEY_BASS;
  keymap[1*8+1] = KEY_SNARE;
  keymap[1*8+2] = KEY_LOW;
  keymap[1*8+3] = KEY_MID;
  keymap[1*8+4] = KEY_HI;
  keymap[1*8+5] = KEY_RIMSHOT;
  keymap[1*8+6] = KEY_CUT;
  keymap[1*8+7] = KEY_COPY;

  keymap[2*8+0] = KEY_HANDCLAP;
  keymap[2*8+1] = KEY_HH_CLOSED;
  keymap[2*8+2] = KEY_HH_OPEN;
  keymap[2*8+3] = KEY_CYMBAL_CRASH;
  keymap[2*8+4] = KEY_CYMBAL_RIDE;
  keymap[2*8+5] = KEY_TRACK_12;
  keymap[2*8+6] = KEY_PASTE;
  keymap[2*8+7] = KEY_SWAP;

  keymap[3*8+0] = KEY_SEQ01;
  keymap[3*8+1] = KEY_SEQ02;
  keymap[3*8+2] = KEY_SEQ03;
  keymap[3*8+3] = KEY_SEQ04;
  keymap[3*8+4] = KEY_SEQ05;
  keymap[3*8+5] = KEY_SEQ06;
  keymap[3*8+6] = KEY_SEQ07;
  keymap[3*8+7] = KEY_SEQ08;

  keymap[4*8+0] = KEY_SEQ17;
  keymap[4*8+1] = KEY_SEQ18;
  keymap[4*8+2] = KEY_SEQ19;
  keymap[4*8+3] = KEY_SEQ20;
  keymap[4*8+4] = KEY_SEQ21;
  keymap[4*8+5] = KEY_SEQ22;
  keymap[4*8+6] = KEY_SEQ23;
  keymap[4*8+7] = KEY_SEQ24;

  keymap[5*8+0] = KEY_SEQ09;
  keymap[5*8+1] = KEY_SEQ10;
  keymap[5*8+2] = KEY_SEQ11;
  keymap[5*8+3] = KEY_SEQ12;
  keymap[5*8+4] = KEY_SEQ13;
  keymap[5*8+5] = KEY_SEQ14;
  keymap[5*8+6] = KEY_SEQ15;
  keymap[5*8+7] = KEY_SEQ16;

  keymap[6*8+0] = KEY_SEQ25;
  keymap[6*8+1] = KEY_SEQ26;
  keymap[6*8+2] = KEY_SEQ27;
  keymap[6*8+3] = KEY_SEQ28;
  keymap[6*8+4] = KEY_SEQ29;
  keymap[6*8+5] = KEY_SEQ30;
  keymap[6*8+6] = KEY_SEQ31;
  keymap[6*8+7] = KEY_SEQ32;

  keymap[8*8+1] = KEY_1;
  keymap[8*8+0] = KEY_2;
  keymap[8*8+3] = KEY_3;
  keymap[8*8+2] = KEY_VELOCITY;
  keymap[8*8+5] = KEY_ACCENT;

  keymap[7*8+1] = KEY_4;
  keymap[7*8+0] = KEY_5;
  keymap[7*8+3] = KEY_6;
  keymap[7*8+2] = KEY_FLAM;
  keymap[7*8+5] = KEY_ENTER;

  keymap[9*8+1] = KEY_7;
  keymap[9*8+0] = KEY_8;
  keymap[9*8+3] = KEY_9;
  keymap[9*8+2] = KEY_UP;

  keymap[10*8+1] = KEY_SHIFT;
  keymap[10*8+0] = KEY_0;
  keymap[10*8+3] = KEY_LEFT;
  keymap[10*8+2] = KEY_DOWN;
  keymap[10*8+5] = KEY_RIGHT;
}