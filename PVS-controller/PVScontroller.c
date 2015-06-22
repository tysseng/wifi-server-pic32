#define BASE_NOTE 164 //C0 = 0100100
#define KEYCOUNT 61

#define COLUMNS 8
#define ROWS 8

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

unsigned short calcNoteOnOff(unsigned short newState, unsigned short column){

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
    if(noteIndex == KEYCOUNT){
      break;
    }
  }
  
  noteStartSwitchStates[column] = newState;
}

unsigned short calcVelocity(unsigned short newState, unsigned short column){

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
        if(cycleCounter > noteTimers[noteIndex]){
          noteVelocity[noteIndex] = cycleCounter - noteTimers[noteIndex];
        } else {
          noteVelocity[noteIndex] = noteTimers[noteIndex] - cycleCounter; // will underflow and start counting from the top.
        }
        readyToSendOn[column] = readyToSendOn[column] | mask; // set bit at row position
      } 
    }

    //step to next note which is COLUMNS higher than the previous one.
    noteIndex += COLUMNS;

    mask = mask << 1;

    // last row does not contain a full set of keys.
    if(noteIndex == KEYCOUNT){
      break;
    }
  }
  
  noteEndSwitchStates[column] = newState;
}

void initSwitchStates(){
  unsigned short i;
  for(i=0; i<COLUMNS; i++){
    noteStartSwitchStates[i]=0;
    noteEndSwitchStates[i]=0;
  }
}

void main() {
  initSwitchStates();
}