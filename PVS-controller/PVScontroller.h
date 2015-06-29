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

extern unsigned short calcNoteOnOff(unsigned short newState, unsigned short column);
extern unsigned short calcVelocity(unsigned short newState, unsigned short column);
extern void init();

#endif