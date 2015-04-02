#ifndef _SEQUENCER_H
#define _SEQUENCER_H

#include "../shared/definitions.h"

extern struct SongElement songElements[MAX_SONG_PATTERNS];
extern char patternKeyMap[MAX_KEY_MAP_SIZE];
extern struct Sequencer sequencer;
extern struct Pattern *patt;
extern struct Pattern *nextPatt;

extern struct Flam *flams[7];

extern unsigned short pulseInterval;
extern unsigned short pulseTimerStartH;
extern unsigned short pulseTimerStartL;
extern unsigned int pulseTimerStartFirstSwing;
extern unsigned int pulseTimerStartSecondSwing;
extern short currSoloTrack;

extern void (*nextPatternAction)();

extern unsigned short lastToggledTrack;
extern unsigned short lastToggledStep;

extern short currentTap;

void SEQ_init();
void SEQ_resetSequencer();
void SEQ_setDefaultSetup();

// step related inc velocity & flam
void SEQ_recordStep(unsigned short track);
void SEQ_toggleStep(char step);
void SEQ_stepOn(char track, char step, unsigned short velocity); // used in clipboard
void SEQ_stepOff(char track, char step);                         // used in clipboard
void SEQ_flamOn(unsigned short track, unsigned short step, short flam); // used in clipboard

void SEQ_setVelocity(unsigned short velocity);
void SEQ_setFlam(short flam);
void SEQ_setAccent();

void SEQ_updateMaxTrackAndStep();
void SEQ_setCurrentTrack(unsigned short track);
void SEQ_refreshStepLeds();
void SEQ_switchToNextPattern();

// Tempo/swing related
void SEQ_increaseBPM();
void SEQ_increaseSwing();
void SEQ_decreaseBPM();
void SEQ_decreaseSwing();
void SEQ_tempoTapped();

// output
void SEQ_playNote(unsigned short channel, unsigned short note, unsigned short velocity );
void SEQ_playNotes(unsigned short pulse);
void SEQ_incrementStep();
void SEQ_performEndOfNoteActions();

// record and play control
void SEQ_setPulseTimerStart();
void SEQ_togglePlayPause(char playMode);
void SEQ_stopPlayback();
void SEQ_toggleRecord();
void SEQ_toggleSoloUnsoloPrevious();
void SEQ_toggleSolo();
void SEQ_toggleMute();

void SEQ_test();
#endif