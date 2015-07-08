//TODO:
// korrekte verdider for commands
// korrekte output pins
// lage transfer-funksjon for calculate velociry
// reset CD4022 on init

#define RUNTESTS

// Mock ports when in test mode
#ifdef RUNTESTS
  #define OUTPUT_BUS mockedOutputBus
  #define READY_TO_SEND_PIN mockedReadyToSendPin.b0
  unsigned short mockedOutputBus;
  unsigned short mockedReadyToSendPin;
  unsigned short lastNoteSent;
  unsigned short lastVelocitySent;
#endif

// Real ports when not in test mode
#ifndef RUNTESTS
  #define OUTPUT_BUS portb
  #define READY_TO_SEND_PIN porta.f0
#endif

#include "PVScontroller.h"
#include "PVScontroller.test.h"

#define BASE_NOTE 24 //C0 = 0100100

// I/O pins

#define KEYS_START_ROW portc
#define KEYS_END_ROW portd

// commands to send to the master MCU
#define COMMAND_NOTE_ON 0b10000000
#define COMMAND_NOTE_OFF 0b00000000

unsigned short noteTimers[KEYCOUNT];
unsigned short noteVelocity[KEYCOUNT];

unsigned short noteStartSwitchStates[COLUMNS];
unsigned short noteEndSwitchStates[COLUMNS];

unsigned short readyToSendOff[COLUMNS];
unsigned short readyToSendOn[COLUMNS];

unsigned short currentColumn;
unsigned short cycleCounter;

void interrupt() {
  unsigned short column;

  // colScanTimer must run 8 times faster than the desired cycle counter speed
  // as we only update the cycleCounter once every key has been scanned.
  if(/*colScanTimer*/ 0){

    checkKeyStartSwitches(KEYS_START_ROW, currentColumn);
    checkKeyBottomSwitches(KEYS_END_ROW, currentColumn);

    //TODO: switch to next row - lets values settle untill next timeout.
    currentColumn = (currentColumn + 1) % COLUMNS;
    //TODO: Send clock pulse to counter

    // Increment counter before starting a new round.
    // Counter is only incremented once per round, not once per column
    if(currentColumn == 0){
      cycleCounter++;
    }
  }
}

void checkKeyStartSwitches(unsigned short newState, unsigned short column){

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

void checkKeyBottomSwitches(unsigned short newState, unsigned short column){

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

// blocking send, sets a data value on the output and waits until the receiver
// indicates that the value has been read.
void send(unsigned short value){

  OUTPUT_BUS = value;
  while(READY_TO_SEND_PIN == 0){
    delay_us(10);
  }
}

unsigned short calculateVelocity(unsigned short velocityTime){
  return 0;
}

void sendNoteOn(unsigned short noteIndex, unsigned short velocityTime){
  unsigned short velocity = calculateVelocity(velocityTime);
  unsigned short noteToSend = COMMAND_NOTE_ON | (BASE_NOTE + noteIndex);
  #ifdef RUNTESTS
    lastNoteSent = noteToSend;
    lastVelocitySent = velocity;
  #endif
  send(noteToSend);
  send(velocity);
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
        
        // clear readyToSend. Only the current bit may be cleared, as new
        // ones may have been set through interrupts while we are in this loop
        readyToSendOn[column] &= ~mask;
      }
      noteIndex++;
    }
    mask = mask << 1;
  }
}

void sendNoteOff(unsigned short noteIndex){
  send(COMMAND_NOTE_OFF | (BASE_NOTE + noteIndex));
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
        
        // clear readyToSend. Only the current bit may be cleared, as new
        // ones may have been set through interrupts while we are in this loop
        readyToSendOff[column] &= ~mask;
      }
      noteIndex++;
    }
    mask = mask << 1;
  }
}

void initKeyScanner(){
  unsigned short i;
  for(i=0; i<COLUMNS; i++){
    noteStartSwitchStates[i]=0;
    noteEndSwitchStates[i]=0;
    readyToSendOff[i]=0;
    readyToSendOn[i]=0;
  }
  currentColumn = 0;
  //TODO: Make sure counter is at 0, may read input from counter.
}

void initVelocityTiming(){
  unsigned short i;
  for(i=0; i<KEYCOUNT; i++){
    noteTimers[i]=0;
    noteVelocity[i]=0;
  }
  cycleCounter = 0;
}

void init(){
  initKeyScanner();
  initVelocityTiming();
}

#ifdef RUNTESTS
void main() {
  init();
  lastNoteSent = 0;
  lastVelocitySent = 0;
  runTests();
}
#endif

#ifndef RUNTESTS
void main() {
  init();
}
#endif