#include "sequencer.h"
#include "sequencer_private.h"

#include <built_in.h>
#include "navigation.h"
#include "page_song.h" //Get rid of this, only needed for pattern change!
#include "../controller.h"           // needed for pulse timer control

#include "../midi/midi.h"
#include "../keyboard/leds.h"
#include "../clipboard/clipboard.h"
#include "../storage/external_storage.h"
#include "../GLCD/sequencer_visualisation.h"

#include "../shared/types.h"
#include "../shared/global.h"
#include "../shared/definitions.h"

#include "../../shared/char_addrs.h" // needed for flam characters

// sequencer state
struct Sequencer sequencer;

// Two set of tracks and pattern definitions are needed to be able to do
// background loading of a pattern while playing another.
struct Track traxx1[MAX_TRACKS];
struct Track traxx2[MAX_TRACKS];
struct Pattern pattern1;
struct Pattern pattern2;

// Performance mode - key/pattern mapping
char patternKeyMap[MAX_KEY_MAP_SIZE];

// Song elements - pattern addresses, repetitions and order.
struct SongElement songElements[MAX_SONG_PATTERNS];

// Pointer to the current and next pattern
struct Pattern *patt;
struct Pattern *nextPatt;

// Timing variables
unsigned short pulseInterval;          // number of pulses between sequencer steps.

// A crude swing mode is implemented. Each second 16th may be moved slightly early or late
// using a percentage value to achieve a swing rythm. The total duration of two 16ths are
// still an 8th.
unsigned int pulseTimerStartFirstSwing = 0;
unsigned int pulseTimerStartSecondSwing = 0;

unsigned short pulseTimerStartH = 0;       // High bit, timer start position at the current BPM (= length of 1 pulse)
unsigned short pulseTimerStartL = 0;       // Low bit

short currSoloTrack = NO_SOLO;

// action to execute when the swapping to next pattern
void (*nextPatternAction)();

// Accent/velocity/flam-related variables
unsigned short lastToggledTrack = 0;
unsigned short lastToggledStep = 0;

// Flam notes. Flam is read from right to left.
struct Flam flamEightTriplet1; // is also first part of quarter triplet
struct Flam flamEightTriplet2; // is also third part of quarter triplet
struct Flam flamEightTriplet3; // is also second part of quarter triplet
struct Flam flamSixteenth;
struct Flam flamSixteenthTriplet1;
struct Flam flamSixteenthTriplet2;
struct Flam flamThirtySecond;
struct Flam flamThirtySecondTriplet;

struct Flam *flams[7];

short currentTap = -1;
unsigned int tapTimings[TAPS_REQUIRED];

//
// Tap tempo-functionality. The user has to tap at least five times (indicating
// four intervals) before bpm calculation is done. After that, recalculation is
// preformed for each new tap as long as the user taps faster than about 36 bpm
// (even so, the lowest possible bpm is 60).
//
void SEQ_tempoTapped(){
  unsigned short i;
  unsigned long averageTicks;

  unsigned int bpm;
  unsigned short low, high;

  low = TMR0L;
  high = TMR0H;

  // first tap
  if(currentTap == -1){
    currentTap++;
    MCU_startTapTempoTimer();
    return;
  } else {
    tapTimings[currentTap] = low | (high << 8);
    currentTap++;
    MCU_startTapTempoTimer();
  }

  // we keep counting even after taps required is reached, in order to let the
  // user keep tapping to change the tempo.
  if(currentTap == TAPS_REQUIRED){
    G_SEQ_enoughTaps = 1;
    currentTap = 0;
  }

  // set the new bpm when enough taps have been recorded.
  if(G_SEQ_enoughTaps){
    averageTicks = 0;
    for(i=0; i<TAPS_REQUIRED; i++){
      averageTicks += tapTimings[i];
    }
    averageTicks = averageTicks / TAPS_REQUIRED;

    // calculate bpm. bpm = 60 / averageDelay
    // averageDelay = averageTicks / (ticks per second / prescaler)
    // ticks per second @ 20MHz = 5 000 000
    // averageDelay = averageTicks / (5 000 000 / 128 )
    // bpm = 60 / ( averageTicks / (5 000 000 / 128 ) )
    bpm = 2343750 / averageTicks;
    if(bpm > BPM_MAX) bpm = BPM_MAX;
    SEQ_setBPM(bpm); // TODO: we may get a rounding error here.);
  }
}



/**
 * Setup default sequencer settings
 **/
void SEQ_init(){

  nextPatternAction = &GLOBAL_nop;
  G_SEQ_resetBpmOnLoad = 1;
  SEQ_initSequencer();
  SEQ_resetSteps();
  EEX_readSetup(patt);

  //timing
  pulseInterval = 48 / patt->res; //48 ppqn
  SEQ_setPulseTimerStart();
  SEQ_initFlams();

}

void SEQ_resetSequencer(){
  SEQ_stopPlayback();
  SEQ_initSequencer();
  SEQ_resetSteps();
  SEQ_refreshStepLeds();
}

//
// init the parts of the sequencer setup not loaded from storage
//
void SEQ_initSequencer(){
  unsigned short i;

  G_SEQ_isRecording = 0;
  G_SEQ_enoughTaps = 0;
  sequencer.currTrack = 0;
  sequencer.playState = PLAY_STOPPED;
  sequencer.currStep = 0;
  sequencer.currSub = 1;
  sequencer.currBeat = 1;
  sequencer.currBar = 1;

  pattern1.tracks = traxx1;
  pattern2.tracks = traxx2;
  
  patt = &pattern1;
  nextPatt = &pattern2;

  patt->folderNum = NOT_SELECTED;
  patt->fileNum = NOT_SELECTED;

  nextPatt->folderNum = NOT_SELECTED;
  nextPatt->fileNum = NOT_SELECTED;
  
  //init tracks and states
  for( i=0; i<MAX_TRACKS; i++) {
    sequencer.trackStates[i] = STATE_NONE;
  }
  
}

void SEQ_resetSteps(){
  short i, j;
  //init tracks and states
  for( i=0; i<MAX_TRACKS; i++) {
    for( j=0; j<MAX_STEPS; j++) {
      patt->tracks[i].steps[j]=0;
      patt->tracks[i].flams[j]=NO_FLAM;
    }
  }
}

//
// Set flam notes. Flam is read from right to left. A one indicates that a
// note should be played at this pulse position.
//
void SEQ_initFlams(){

  //pulses per note:
  // 24: quarter
  // 16: quarter triplet
  // 12: eight
  //  8: eight triplet
  //  6: sixteenth
  //  4: sixteenth triplet
  //  3: thirty-second
  //  2: thirty-second triplet

  //Sequence of two eight  triplets: 100000 001000 000010 000000 100000 001000 000010 000000
  //Sequence of one quarter triplet: 100000 000000 000010 000000 000000 001000 000000 000000

  flamSixteenth.vel = 0b000001;
  flamSixteenth.acc = 0;
  flamSixteenth.symbol = flam0;

  flamEightTriplet2.vel = 0b000100;
  flamEightTriplet2.acc = 0;
  flamEightTriplet2.symbol = flam1;
  
  flamEightTriplet3.vel = 0b010000;
  flamEightTriplet3.acc = 0;
  flamEightTriplet3.symbol = flam2;

  flamSixteenthTriplet1.vel = 0b010001;
  flamSixteenthTriplet1.acc = 0;
  flamSixteenthTriplet1.symbol = flam3;
  
  flamSixteenthTriplet2.vel = 0b000100;
  flamSixteenthTriplet2.acc = 0;
  flamSixteenthTriplet2.symbol = flam4;
  
  flamThirtySecond.vel = 0b001001;
  flamThirtySecond.acc = 0;
  flamThirtySecond.symbol = flam5;
  
  flamThirtySecondTriplet.vel = 0b010101;
  flamThirtySecondTriplet.acc = 0;
  flamThirtySecondTriplet.symbol = flam6;

  flams[0] = &flamSixteenth;
  flams[1] = &flamEightTriplet2;
  flams[2] = &flamEightTriplet3;
  flams[3] = &flamSixteenthTriplet1;
  flams[4] = &flamSixteenthTriplet2;
  flams[5] = &flamThirtySecond;
  flams[6] = &flamThirtySecondTriplet;

}

void SEQ_toggleStep(unsigned short step){
  if(step >= patt->steps){
    return;
  }
  
  if(patt->tracks[sequencer.currTrack].steps[step] > 0){
    SEQ_stepOff(sequencer.currTrack, step);
  } else {
    SEQ_stepOn(sequencer.currTrack, step, patt->tracks[sequencer.currTrack].velocity);
    lastToggledTrack = sequencer.currTrack;
    lastToggledStep = step;
  }
}

void SEQ_recordStep(unsigned short track){
  if(track < patt->trackcount){
    SEQ_stepOn(track, sequencer.currStep, patt->tracks[track].velocity);
  }
}

void SEQ_stepOn(unsigned short track, unsigned short step, unsigned short velocity){

  if(velocity == 0 ){
    SEQ_stepOff(track, step);
    return;
  }

  patt->tracks[track].steps[step] = velocity;
  patt->tracks[track].flams[step] = 0;

  if(track == sequencer.currTrack){
    LED_stepOn(step);
  }

  if(currPage == PAGE_PATT){
    SEQ_VIS_writeStep(track, step,0);
  } else if(currPage == PAGE_BARS && track == sequencer.currTrack){
    SEQ_VIS_writeStepBar(step);
  }
}

void SEQ_stepOff(unsigned short track, unsigned short step){
  patt->tracks[track].steps[step] = 0;
  patt->tracks[track].flams[step] = NO_FLAM;

  if(track == sequencer.currTrack){
    LED_stepOff(step);
  }

  if(currPage == PAGE_PATT){
    SEQ_VIS_writeStep(track, step,0);
  } else if(currPage == PAGE_BARS){
    SEQ_VIS_writeStepBar(step);
  }
}

//
// change flam for a specified step
//
void SEQ_flamOn(unsigned short track, unsigned short step, short flam){

  patt->tracks[track].flams[step] = flam;

  if(currPage == PAGE_BARS){
    if(track == sequencer.currTrack){
      SEQ_VIS_writeStepBar(step);
    }
  }
}


//
// Accent the last pressed or currently highlit note. Sets velocity to
// track.velocity + track.accent
//
void SEQ_setAccent(){
  unsigned short velocity;
  velocity = patt->tracks[lastToggledTrack].accent + patt->tracks[lastToggledTrack].velocity;
  velocity = (velocity.F7 == 1 ? 127 : velocity);
  SEQ_setVelocity(velocity);
}



void SEQ_setVelocity(unsigned short velocity){
  unsigned short track, step;

  if(G_CB_cursorOn){
    for(step=CB_area.x1; step<CB_area.x2; step++){
      for(track=CB_area.y1; track<CB_area.y2; track++){
        if(patt->tracks[track].steps[step]>0){
          SEQ_stepOn(track, step, velocity);
        }
      }
    }
  } else if(patt->tracks[lastToggledTrack].steps[lastToggledStep] > 0){
    SEQ_stepOn(lastToggledTrack, lastToggledStep, velocity);
  }
}

void SEQ_setFlam(short flam){
  unsigned short track, step;

  if(G_CB_cursorOn){
    for(step=CB_area.x1; step<CB_area.x2; step++){
      for(track=CB_area.y1; track<CB_area.y2; track++){
        if(patt->tracks[track].steps[step]>0){
          SEQ_flamOn(track, step, flam);
        }
      }
    }
  } else if(patt->tracks[lastToggledTrack].steps[lastToggledStep] > 0){
    SEQ_flamOn(lastToggledTrack, lastToggledStep, flam);
  }
}

void SEQ_updateMaxTrackAndStep(){
  CB_resetCurrDrawArea();
  CB_resetCursor();
  if(sequencer.currTrack >= patt->trackcount){
    sequencer.currTrack = patt->trackcount - 1;
  }
  SEQ_refreshStepLeds();
}

void SEQ_setCurrentTrack(unsigned short track){

  SEQ_VIS_clearTrackArrow();

  if(track < patt->trackcount && track >= 0){
    sequencer.currTrack = track;
    SEQ_refreshStepLeds();
  }
  
  SEQ_VIS_writeTrackArrow();
}

void SEQ_refreshStepLeds(){

  char i;

  // update step leds
  for(i=0; i<patt->steps; i++){
    if(patt->tracks[sequencer.currTrack].steps[i] == 0){
      LED_stepOff(i);
    } else {
      LED_stepOn(i);
    }
  }
  
  //steps outside pattern length should be turned off.
  for(;i<MAX_STEPS; i++){
    LED_stepOff(i);
  }
}

/**
 * Increase rate of playback
 **/
void SEQ_increaseBPM(){
  if(patt->bpm < BPM_MAX){
    patt->bpm++;
    SEQ_setPulseTimerStart();
  }
}

void SEQ_increaseSwing(){
  if(patt->swingBalance < SWING_MAX){
    patt->swingBalance++;
    SEQ_setPulseTimerStart();
  }
}

/**
 * Decrease rate of playback
 **/
void SEQ_decreaseBPM(){
  if(patt->bpm > BPM_MIN){
    patt->bpm--;
    SEQ_setPulseTimerStart();
  }
}

void SEQ_decreaseSwing(){
  if(patt->swingBalance > SWING_MIN){
    patt->swingBalance--;
    SEQ_setPulseTimerStart();
  }
}

void SEQ_setBPM(unsigned short bpm){
  if(bpm < BPM_MIN){
    patt->bpm = BPM_MIN;
  } else if(bpm > BPM_MAX){
    patt->bpm = BPM_MAX;
  } else {
    patt->bpm = bpm;
  }
  
  SEQ_setPulseTimerStart();
  if(currPage == PAGE_PATT){
    SEQ_VIS_writeBPM();
  }
}

//
// Increment current step, returns to start when overflowing
//
void SEQ_incrementStep(){
  if(sequencer.currStep > patt->steps -2){
    sequencer.currStep = 0;
  } else {
    sequencer.currStep++;
  }
  
  sequencer.currSub  = (sequencer.currStep % patt->res)+1;
  sequencer.currBeat = (sequencer.currStep / patt->res ) % patt->bpb + 1;
  
  if(sequencer.currSub == 1 && sequencer.currBeat == 1){
    if(sequencer.currBar < 999){
      sequencer.currBar++;
    } else {
      sequencer.currBar = 1;
    }
  }
}

//
// At the end of each note, check if anything should be done (like
// switching to the next pattern.
//
void SEQ_performEndOfNoteActions(){

  // Disable loading of new next pattern if we are close to the end of the
  // current pattern. This will prevent reading operations from taking place
  // while we swap patterns.
  if(patt->steps > 2 && sequencer.currStep == patt->steps - 2){
    G_PGPF_patternLoadEnabled = 0;
  }
  
  // end of pattern reached:
  if(sequencer.currStep > patt->steps - 2){
    if(sequencer.playMode == PLAY_MODE_PERFORMANCE){
      if(nextPatt->folderNum != NOT_SELECTED && nextPatt->fileNum != NOT_SELECTED){
        SEQ_switchToNextPattern();
      }
    } else if(sequencer.playMode == PLAY_MODE_SONG){
      if(PGSO_shouldSwitchToNextPattern()){
        SEQ_switchToNextPattern();
      }
      PGSO_incrementSongPosition();
    }
    G_PGPF_patternLoadEnabled = 1;
  }
}

//
// Record/playback functions
//

//
// Set timer start for time-between-pulses timer.
// Assumes a clock frequency of 20MHz and a prescaler of 4.
//
// See pulseTimerStart.txt for explanations of how to recalculate the params.
//
void SEQ_setPulseTimerStart(){

  // we're using a long as a temp. variable since the result of the
  // multiplication may exceed the size of an int
  unsigned long interval;
  unsigned long firstSwingInterval;
  unsigned long secondSwingInterval;

  // interval = 3125000 / patt->bpm; // 24 ppqn interval
  interval = 1562500 / patt->bpm; //48 ppqn interval
  firstSwingInterval = ( interval * patt->swingBalance ) / 50;
  secondSwingInterval = 2 * interval - firstSwingInterval;

  // The pulse timer is using a prescaler of 4, which means that one tick
  // is equal to four clock cycles. Thus, any number cycles to substract
  // must be divisible by 4.
  // The interrupt routine uses 20 instructions to reset the timer. This means
  // That we have to substract 20 / 4 = 5 ticks from the timer.
  pulseTimerStartFirstSwing = 65536 - firstSwingInterval - TICKS_USED_BY_INTERRUPT;
  pulseTimerStartSecondSwing = 65536 - secondSwingInterval - TICKS_USED_BY_INTERRUPT;
}

void SEQ_togglePlayPause(char playMode){

  if(sequencer.playState == PLAY_STOPPED){
    MCU_resetPulseTimer();
    sequencer.playMode = playMode;
  }

  if(sequencer.playState == PLAY_PAUSED || sequencer.playState == PLAY_STOPPED){
    MCU_startPulseTimer();
    sequencer.playState = PLAY_PLAYING;
  } else if( sequencer.playState == PLAY_PLAYING ){
    MCU_stopPulseTimer();
    sequencer.playState = PLAY_PAUSED;
    LED_tempoAllOff();
  }
}

void SEQ_stopPlayback(){
  MCU_stopPulseTimer();
  MCU_resetPulseTimer();
  sequencer.playState = PLAY_STOPPED;
  sequencer.currStep = 0;
  sequencer.currSub  = 1;
  sequencer.currBeat = 1;
  sequencer.currBar = 1;
  LED_tempoAllOff();
}

void SEQ_toggleRecord(){
  if(G_SEQ_isRecording){
    G_SEQ_isRecording = 0;
    LED_recordOff();
  } else {
    G_SEQ_isRecording = 1;
    LED_recordOn();
  }
}

void SEQ_toggleSoloUnsoloPrevious(){
  if(sequencer.trackStates[sequencer.currTrack] != STATE_SOLO && currSoloTrack != NO_SOLO){
    sequencer.trackStates[currSoloTrack] = STATE_NONE;
  }
}

void SEQ_toggleSolo(){
  if(sequencer.trackStates[sequencer.currTrack] != STATE_SOLO){
    currSoloTrack = sequencer.currTrack;
    sequencer.trackStates[sequencer.currTrack] = STATE_SOLO;
  } else {
    sequencer.trackStates[sequencer.currTrack] = STATE_NONE;
    currSoloTrack = NO_SOLO;
  }
}

void SEQ_toggleMute(){
  if(sequencer.trackStates[sequencer.currTrack] != STATE_MUTE){
    if(currSoloTrack == sequencer.currTrack){
      currSoloTrack = NO_SOLO;
    }
    sequencer.trackStates[sequencer.currTrack] = STATE_MUTE;
  } else {
    sequencer.trackStates[sequencer.currTrack] = STATE_NONE;
  }
}

void SEQ_updateTempoLed(){
  if(sequencer.currSub == 1){
    if(sequencer.currBeat == 1){
      LED_tempoBarOn();
    } else {
      LED_tempoQuarterOn();
    }
  } else {
    LED_tempoAllOffFromInterrupt();
  }
}


//
// Play the current (sub) note. Takes care of checking for flam entries
// (sub note divisions).
//
void SEQ_playNotes(unsigned short pulse){

  //check if note should be played.
  struct Flam *flam;

  char flamIndex;
  char flamVel;
  char flamAcc;
  char playVelocity;
  char playAccent;
  char velocity;
  char flamMask;
  char i;

  if(pulse==0){
    MIDI_sendAllOff();
  }

  for(i=0; i<patt->trackcount; i++){
  
    if(sequencer.trackStates[i] == STATE_MUTE ||
      (currSoloTrack != NO_SOLO && currSoloTrack != i)){
      continue;
    }
  
    flamIndex = patt->tracks[i].flams[sequencer.currStep];
    
    if(flamIndex == NO_FLAM){
      continue;
    }

    flam = flams[flamIndex];
    flamVel = flam->vel;
    flamAcc = flam->acc;
    flamMask = (1 << pulse);
    playVelocity = flamVel & flamMask;
    playAccent = flamAcc & flamMask;
    
    velocity = (playVelocity == 0 ? 0 : patt->tracks[i].steps[sequencer.currStep]);
    velocity = (playAccent == 0 ? velocity : patt->tracks[i].accent);

    if(velocity > 0 ){
      SEQ_playNote(patt->tracks[i].channel, patt->tracks[i].basenote, velocity);
    }
  }
  
  SEQ_updateTempoLed();

}

void SEQ_playNote(unsigned short channel, unsigned short note, unsigned short velocity ) {

  unsigned short correctedVelocity;
  correctedVelocity = (velocity.F7 == 1 ? 127 : velocity); // velocity > 127
  MIDI_sendNote(channel, note, correctedVelocity);

}

void SEQ_switchToNextPattern(){
  struct Pattern *swapPatt;
  swapPatt = patt;
  patt = nextPatt;
  nextPatt = swapPatt;
  nextPatt->folderNum = NOT_SELECTED;
  nextPatt->fileNum = NOT_SELECTED;

  // recalculate pulse timer, as bpm may have changed
  SEQ_setPulseTimerStart();
  SEQ_refreshStepLeds();

  if(nextPatternAction){
    nextPatternAction();
  }
}