#ifndef _SEQUENCER_PRIVATE_H
#define _SEQUENCER_PRIVATE_H
void SEQ_initFlams();
void SEQ_initSequencer();
void SEQ_resetSteps();
void SEQ_setBPM(unsigned short bpm);
void SEQ_updateTempoLed();
#endif