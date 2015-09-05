#ifndef _PVSCONTROLLER_H
#define _PVSCONTROLLER_H

#define KEYCOUNT 61

#define COLUMNS 8
#define ROWS 8

// Lowest possible note, C0 = 0100100
#define BASE_NOTE 36

// Longest allowed delay between start and end of keypress
#define MAX_VELOCITY_TIME 70

// Commands to send to the master MCU. note off is 0 so no need to use a command
#define COMMAND_NOTE_ON 0b10000000

extern unsigned short currentColumn;
extern unsigned short cycleCounter;

extern unsigned short noteTimers[KEYCOUNT];
extern unsigned short noteVelocity[KEYCOUNT];

extern unsigned short noteStartSwitchStates[COLUMNS];
extern unsigned short noteEndSwitchStates[COLUMNS];

extern unsigned short hasSentOn[COLUMNS];
      
void checkKeyStartSwitches(unsigned short newState, unsigned short column, unsigned short savedCycleCounter);
void checkKeyEndSwitches(unsigned short newState, unsigned short startKeyState, unsigned short column, unsigned short savedCycleCounter);

unsigned short calculateVelocity(unsigned short velocityTime);

void send(unsigned short value, unsigned short resetAfter);
void sendNoteOn(unsigned short noteIndex, unsigned short velocityTime);
void sendNoteOff(unsigned short noteIndex);

void scanColumn();

void init();

#endif