#include "page_flam_vel.h"
#include "page_flam_vel_private.h"
#include "page_pattern.h"
#include "input.h"
#include "navigation.h"
#include "sequencer.h"
#include "../GLCD/sequencer_visualisation.h"
#include "../clipboard/clipboard.h"
#include "../midi/midi.h"
#include "../shared/global.h"
#include "../shared/key_consts.h"
#include "../shared/definitions.h"

struct InputField inputVelocity;
struct InputField inputFlam;

void PGFV_initPage(){
  PGFV_initInputFields();
}

void PGFV_initInputFields(){
  inputVelocity.labelLength = 10;
  inputVelocity.length = 3;
  inputVelocity.maxValue = 127;
  inputVelocity.currValue = 0;
  inputVelocity.col = 20;
  inputVelocity.row = POS_VELOCITY_BARS + 7;
  inputVelocity.action = &PGFV_inputActionVelocity;

  inputFlam.labelLength = 10;
  inputFlam.length = 3;
  inputFlam.maxValue = 6;
  inputFlam.currValue = 0;
  inputFlam.col = 20;
  inputFlam.row = POS_VELOCITY_BARS + 8;
//  inputFlam.action = &PGFV_inputActionFlam;

  // Trick to fool the linker. If this is not included, the linker will not
  // be able to find the addresses of any of the inputActions above!
  if(0>1){
    inputVelocity.action(0, 0);
  }
}

void PGFV_inputActionVelocity(struct InputField *field, short enter){
  if(enter){
//    SEQ_setVelocity(field->currValue);
  }
}
/*
void PGFV_inputActionFlam(struct InputField *field, short enter){
  SEQ_setFlam(field->currValue);
}

void PGFV_showVelocityPage(){
  PGFV_showPage();
  IN_setCurrentInputField(&inputVelocity);
}

void PGFV_toggleVelocityPage(){
  if(currentInputField == &inputFlam){
    IN_setCurrentInputField(&inputVelocity);
  } else {
    IN_setCurrentInputField(NO_INPUT_FOCUS);
    PGP_showPage(DONT_RESET_POSITIONS);
    CB_ResetCursor();
  }
}

void PGFV_showFlamPage(){

  PGFV_showPage();
  IN_clearCurrentInputField();
  IN_setCurrentInputField(&inputFlam);
}

void PGFV_toggleFlamPage(){
  if(currentInputField == &inputVelocity){
    IN_setCurrentInputField(&inputFlam);
  } else {
    IN_setCurrentInputField(NO_INPUT_FOCUS);
    PGP_showPage(DONT_RESET_POSITIONS);
    CB_ResetCursor();
  }
}

void PGFV_showPage(){
  currPage = PAGE_BARS;
  SEQ_VIS_showPageBars();
  SEQ_VIS_writeExternalSynch(G_MIDI_isExternalSynchOn);
  if(lastToggledTrack == sequencer.currTrack){
    PGFV_moveCursorToStep(lastToggledStep);
  } else {
    PGFV_moveCursorToStep(0);
  }
}

//
// Move cursor to a step while in velocity page mode
//
void PGFV_moveCursorToStep(unsigned short step){
  if(G_CB_areaOn==0){
    CB_cursorSet(step, sequencer.currTrack);
    PGFV_updateVelocityPageFields(step);
  }
}

void PGFV_updateVelocityPageFields(unsigned short step){
  inputVelocity.currValue = patt->tracks[sequencer.currTrack].steps[step];
  IN_refreshInputField(&inputVelocity);

  inputFlam.currValue = patt->tracks[sequencer.currTrack].flams[step];
  IN_refreshInputField(&inputFlam);
}

void PGFV_keyEventHandler(char key){
  if(key >= KEY_0 && key <= KEY_9){
    IN_addNumberToInputField(key-KEY_0);
    return;
  }

  if(key >= KEY_BASS && key <= KEY_TRACK_12){
    if(G_SEQ_isRecording){
      SEQ_recordStep(key - KEY_BASS);
    } else {
      SEQ_setCurrentTrack(key - KEY_BASS);
      PGFV_showPage();
    }
    return;
  }
  
  if(key >= KEY_SEQ01 && key <= KEY_SEQ32){
    SEQ_toggleStep(key - KEY_SEQ01);
    PGFV_moveCursorToStep(lastToggledStep);
    return;
  }

  switch(key){
  case KEY_LEFT:
    if(!G_SEQ_isRecording){
      CB_CursorLeft();
      PGFV_updateVelocityPageFields(CB_cursorx);
    }
    break;
  case KEY_RIGHT:
    if(!G_SEQ_isRecording){
      CB_CursorRight();
      PGFV_updateVelocityPageFields(CB_cursorx);
    }
    break;
  case KEY_UP:
    if(G_SEQ_isRecording){
      SEQ_setCurrentTrack(sequencer.currTrack - 1);
      PGFV_showPage();
    }
    break;
  case KEY_DOWN:
    if(G_SEQ_isRecording){
      SEQ_setCurrentTrack(sequencer.currTrack + 1);
      PGFV_showPage();
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

  case KEY_ENTER:
    IN_enterInputField();
    break;
    
  // sequencer functions, may be moved
  case KEY_PLAY:
    SEQ_togglePlayPause(PLAY_MODE_PATTERN);
    SEQ_VIS_writePlayState();
    SEQ_VIS_writePlayMode();
    break;
  case KEY_STOP:
    SEQ_stopPlayback();
    SEQ_VIS_writePlayState();
    SEQ_VIS_writeTime(1);
    SEQ_VIS_writePlayMode();
    break;
  case KEY_REC:
    SEQ_toggleRecord();
    SEQ_VIS_writePlayState();
    break;
  case KEY_FLAM :
    PGFV_toggleFlamPage();
    break;
  case KEY_VELOCITY :
    PGFV_toggleVelocityPage();
    break;
  case KEY_ACCENT:
    SEQ_setAccent();
    break;

  default:
    break;
  }
}
*/