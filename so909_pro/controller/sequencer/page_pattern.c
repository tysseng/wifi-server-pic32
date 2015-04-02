#include "page_setup.h"
#include "page_setup_private.h"
#include "sequencer.h"
#include "navigation.h"
#include "page_flam_vel.h"
#include "input.h"
#include "../clipboard/clipboard.h"
#include "../GLCD/sequencer_visualisation.h"
#include "../midi/midi.h"
#include "../shared/key_consts.h"
#include "../shared/dial_consts.h"
#include "../shared/global.h"
#include "../shared/definitions.h"

void PGP_showPage(char resetPositions){
  currPage = PAGE_PATT;
  nextPatternAction = &GLOBAL_nop;
  IN_clearCurrentInputField();
  
  SEQ_VIS_showPagePatt();
  SEQ_VIS_writeExternalSynch(G_MIDI_isExternalSynchOn);
  if(resetPositions){
    sequencer.currTrack = 0;
  }
  SEQ_setCurrentTrack(sequencer.currTrack);
}

void PGP_togglePlayPause(){
  SEQ_togglePlayPause(PLAY_MODE_PATTERN);
  SEQ_VIS_writePlayState();
  SEQ_VIS_writePlayMode();
}

void PGP_stop(){
  SEQ_stopPlayback();
  SEQ_VIS_writePlayState();
  SEQ_VIS_writePlayMode();
  SEQ_VIS_writeTime(1);
}

//
// Send a pressed key event to the current key receiver
//
void PGP_keyEventHandler(char key){

  if(key >= KEY_BASS && key <= KEY_TRACK_12){
    if(G_SEQ_isRecording){
      SEQ_recordStep(key - KEY_BASS);
    } else {
      SEQ_setCurrentTrack(key - KEY_BASS);
    }
    return;
  }
  
  if(key >= KEY_SEQ01 && key <= KEY_SEQ32){
    SEQ_toggleStep(key - KEY_SEQ01);
    return;
  }

  switch(key){
  case KEY_UP:
    if(G_SEQ_isRecording){
      SEQ_setCurrentTrack(sequencer.currTrack - 1);
    } else {
      CB_CursorUp();
    }
    break;
  case KEY_DOWN:
    if(G_SEQ_isRecording){
      SEQ_setCurrentTrack(sequencer.currTrack + 1);
    } else {
      CB_CursorDown();
    }
    break;
  case KEY_LEFT:
    if(!G_SEQ_isRecording){
      CB_CursorLeft();
    }
    break;
  case KEY_RIGHT:
    if(!G_SEQ_isRecording){
      CB_CursorRight();
    }
    break;
  case KEY_CUT:
    CB_copy(1);
    break;
  case KEY_COPY:
    CB_copy(0);
    break;
  case KEY_PASTE:
    CB_Paste(0);
    break;
  case KEY_SWAP:
    CB_Paste(1);
    break;

  // sequencer related, may be moved later
  case KEY_PLAY:
    PGP_togglePlayPause();
    break;
  case KEY_STOP:
    PGP_stop();
    break;
  case KEY_REC:
    SEQ_toggleRecord();
    SEQ_VIS_writePlayState();
    break;
  case KEY_MUTE:
    SEQ_toggleMute();
    SEQ_VIS_writeState(sequencer.currTrack);
    break;
  case KEY_SOLO:
    SEQ_toggleSoloUnsoloPrevious();
    if(currSoloTrack != NO_SOLO){
      SEQ_VIS_writeState(currSoloTrack);
    }
    SEQ_toggleSolo();
    SEQ_VIS_writeState(sequencer.currTrack);
    break;
  case KEY_FLAM :
    PGFV_showFlamPage();
    break;
  case KEY_VELOCITY :
    PGFV_showVelocityPage();
    break;
  case KEY_ACCENT:
    SEQ_setAccent();
    break;
  case KEY_TAP_TEMPO:
    SEQ_tempoTapped();
    break;

  default:
    break;
  }
}

void PGP_dialEventHandler(char event){
  switch(event){
  case DIAL_TEMPO_DOWN :
    if(G_NAV_shiftOn){
      SEQ_decreaseSwing();
      SEQ_VIS_writeSwing();
    } else {
      SEQ_decreaseBPM();
      SEQ_VIS_writeBPM();
    }
    break;
  case DIAL_TEMPO_UP :
    if(G_NAV_shiftOn){
      SEQ_increaseSwing();
      SEQ_VIS_writeSwing();
    } else {
      SEQ_increaseBPM();
      SEQ_VIS_writeBPM();
    }
    break;
  }
}
