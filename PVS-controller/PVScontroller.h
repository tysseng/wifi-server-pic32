#ifndef _PVSCONTROLLER_H
#define _PVSCONTROLLER_H

#define KEYCOUNT 61

#define COLUMNS 8
#define ROWS 8

extern unsigned short currentColumn;
extern unsigned short cycleCounter;

extern unsigned short noteTimers[KEYCOUNT];
extern unsigned short noteVelocity[KEYCOUNT];

extern unsigned short noteStartSwitchStates[COLUMNS];
extern unsigned short noteEndSwitchStates[COLUMNS];

extern unsigned short readyToSendOff[COLUMNS];
extern unsigned short readyToSendOn[COLUMNS];
      
extern void interruptBody();
extern void checkKeyStartSwitches(unsigned short newState, unsigned short column, unsigned short savedCycleCounter);
extern void checkKeyEndSwitches(unsigned short newState, unsigned short column, unsigned short savedCycleCounter);
void send(unsigned short value, unsigned short resetAfter);
extern unsigned short calculateVelocity(unsigned short velocityTime);
extern void void sendNoteOn(unsigned short noteIndex, unsigned short velocityTime);
extern void void sendNoteOns();
extern void void sendNoteOff(unsigned short noteIndex);
extern void void sendNoteOffs();

extern void init();

#endif