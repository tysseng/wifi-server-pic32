#include "global.h"
#include "definitions.h"

char GLOBAL_statusBits1;
char GLOBAL_statusBits2;
char GLOBAL_statusBits3;

char name[NAME_LENGTH+1]; // file name variable, reused several places

char G_MIDI_sysexIdByte1;
char G_MIDI_sysexIdByte2;

void GLOBAL_resetName(){
  char i;
  for(i=0; i<NAME_LENGTH+2; i++){
    name[i] = 0;
  }
}

void GLOBAL_nop(){}