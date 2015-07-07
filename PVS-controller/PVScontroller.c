#include "PVScontroller.h"
#include "PVScontroller.test.h"

#define RUNTESTS

#define BASE_NOTE 164 //C0 = 0100100


#define NOTE_ON b0
#define NOTE_ON_SENT b1
#define NOTE_OFF_SENT b1

unsigned short noteTimers[KEYCOUNT];
unsigned short noteVelocity[KEYCOUNT];

unsigned short noteStartSwitchStates[COLUMNS];
unsigned short noteEndSwitchStates[COLUMNS];

unsigned short readyToSendOff[COLUMNS];
unsigned short readyToSendOn[COLUMNS];

unsigned short currentColumn = 0;

unsigned short cycleCounter = 0;

void interrupt() {
  if(/*colScanTimer*/ 0){
    cycleCounter++;
  }
}

void calcNoteOnOff(unsigned short newState, unsigned short column){

  unsigned short changes;
  unsigned short row = 0;
  unsigned short mask = 0b00000001;
  unsigned short noteIndex = column;
  changes = noteStartSwitchStates[column] ^ newState;

  if(!changes){
    return;
  }

  for(row=0; row < ROWS; row++){
    if(changes & mask){ // row has changed
      if(newState & mask){ // row has been turned on, store start time
        noteTimers[noteIndex] = cycleCounter;
      } else { // row has been turned off
        readyToSendOff[column] = readyToSendOff[column] | mask; // set bit at row position
      }
    }
    
    //step to next note which is COLUMNS higher than the previous one.
    noteIndex += COLUMNS;
    
    mask = mask << 1;
    
    // last row does not contain a full set of keys.
    if(noteIndex >= KEYCOUNT){
      break;
    }
  }
  
  noteStartSwitchStates[column] = newState;
}

void calcVelocity(unsigned short newState, unsigned short column){

  unsigned short changes;
  unsigned short row = 0;
  unsigned short mask = 0b00000001;
  unsigned short noteIndex = column;
  changes = noteEndSwitchStates[column] ^ newState;

  if(!changes){
    return;
  }

  for(row=0; row < ROWS; row++){
    if(changes & mask){ // row has changed
      if(newState & mask){ // row has been turned on, calculate velocity
      
        // since we're using unsigned shorts, we will get a mod 256 effect,
        // so the calculation will be correct even if cycleCounter is less
        // than noteTimer
        noteVelocity[noteIndex] = cycleCounter - noteTimers[noteIndex];
        readyToSendOn[column] = readyToSendOn[column] | mask; // set bit at row position
      } 
    }

    //step to next note which is COLUMNS higher than the previous one.
    noteIndex += COLUMNS;

    mask = mask << 1;

    // last row does not contain a full set of keys.
    if(noteIndex >= KEYCOUNT){
      break;
    }
  }
  
  noteEndSwitchStates[column] = newState;
}

void sendNoteOn(unsigned short noteIndex, unsigned short velocityTime){
  // must block untill note has been accepted
}

void sendNoteOns(){
  unsigned short column, row;
  unsigned short mask;
  unsigned short noteIndex = 0;

  for(row=0; row < ROWS; row++){
    mask = 1;
    for(column=0; column < COLUMNS; column++){
      if(readyToSendOn[column] & mask){
        sendNoteOn(noteIndex, noteVelocity[noteIndex]);
        readyToSendOn[column] & ~mask; // clear readyToSend
      }
      noteIndex++;
    }
    mask = mask << 1;
  }
}

void sendNoteOff(unsigned short noteIndex, unsigned short velocityTime){
  // must block untill note has been accepted
}

void sendNoteOffs(){
  unsigned short column, row;
  unsigned short mask;
  unsigned short noteIndex = 0;

  for(row=0; row < ROWS; row++){
    mask = 1;
    for(column=0; column < COLUMNS; column++){
      if(readyToSendOff[column] & mask){
        sendNoteOff(noteIndex);
        readyToSendOff[column] & ~mask; // clear readyToSend
      }
      noteIndex++;
    }
    mask = mask << 1;
  }
}

void initSwitchStates(){
  unsigned short i;
  for(i=0; i<COLUMNS; i++){
    noteStartSwitchStates[i]=0;
    noteEndSwitchStates[i]=0;
    readyToSendOff[COLUMNS]=0;
    readyToSendOn[COLUMNS]=0;

  }
}

void initVelocity(){
  unsigned short i;
  for(i=0; i<KEYCOUNT; i++){
    noteTimers[i]=0;
    noteVelocity[i]=0;
  }
}

void init(){
  initSwitchStates();
  initVelocity();
}

#ifdef RUNTESTS
void main() {
  init();
  runTests();
}
#endif

#ifndef RUNTESTS
void main() {
  init();
}
#endif
