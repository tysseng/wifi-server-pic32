#ifndef _PVSCONTROLLER_H
#define _PVSCONTROLLER_H

#define KEYCOUNT 61

#define COLUMNS 8
#define ROWS 8

extern unsigned short cycleCounter;

extern unsigned short noteTimers[KEYCOUNT];
extern unsigned short noteVelocity[KEYCOUNT];

extern unsigned short noteStartSwitchStates[COLUMNS];
extern unsigned short noteEndSwitchStates[COLUMNS];

extern unsigned short readyToSendOff[COLUMNS];
extern unsigned short readyToSendOn[COLUMNS];

extern void checkKeyStartSwitches(unsigned short newState, unsigned short column);
extern void checkKeyBottomSwitches(unsigned short newState, unsigned short column);

extern void void send(unsigned short value);
extern unsigned short calculateVelocity(unsigned short velocityTime);
extern void void sendNoteOn(unsigned short noteIndex, unsigned short velocityTime);
extern void void sendNoteOns();
extern void void sendNoteOff(unsigned short noteIndex);
extern void void sendNoteOffs();

extern void init();

#endif