#include "page_setup.h"
#include "page_setup_private.h"
#include "input.h"
#include "navigation.h"
#include "sequencer.h"
#include "page_song.h"
#include "page_performance.h"
#include "../midi/midi.h"
#include "../storage/external_storage.h"
#include "../GLCD/sequencer_visualisation.h"
#include "../shared/key_consts.h"
#include "../shared/global.h"
#include "../../shared/sequencer_setup.h"

// Setup parameters
struct InputField inputGeneralExtSync;
struct InputField inputGeneralSysex1;
struct InputField inputGeneralSysex2;
struct InputField inputGeneralSysexDump;
struct InputField inputGeneralResetSteps;
struct InputField inputGeneralResetSong;
struct InputField inputGeneralResetPerformance;
struct InputField inputGeneralResetSystem;
struct InputField *setupGeneralFields[8];

struct InputField inputPatternSteps;
struct InputField inputPatternRes;
struct InputField inputPatternBpb;
struct InputField inputPatternTracks;
struct InputField inputPatternSwing;
struct InputField inputPatternSetBpm;
struct InputField *setupPatternFields[6];

struct InputField inputTrackVelocity;
struct InputField inputTrackAccent;
struct InputField inputTrackBasenote;
struct InputField inputTrackMidiChannel;
struct InputField *setupTrackFields[4];

unsigned short currSetupParam  = 0; //parameter in focus
unsigned short currSetupItem   = 0;

void PGS_initPage(){
  PGS_initInputFields();
}

void PGS_initInputFields(){

  inputTrackVelocity.labelLength = POS_LABEL_LENGTH;
  inputTrackVelocity.length = 3;
  inputTrackVelocity.maxValue = 127;
  inputTrackVelocity.currValue = 0;
  inputTrackVelocity.col = POS_SETUP_LEFT;
  inputTrackVelocity.row = POS_SETUP_TOP+1;
  inputTrackVelocity.action = &PGS_inputActionSetupVelocity;

  inputTrackAccent.labelLength = POS_LABEL_LENGTH;
  inputTrackAccent.length = 3;
  inputTrackAccent.maxValue = 127;
  inputTrackAccent.currValue = 0;
  inputTrackAccent.col = POS_SETUP_LEFT;
  inputTrackAccent.row = POS_SETUP_TOP+2;
  inputTrackAccent.action = &PGS_inputActionSetupAccent;

  inputTrackBasenote.labelLength = POS_LABEL_LENGTH;
  inputTrackBasenote.length = 3;
  inputTrackBasenote.maxValue = 127;
  inputTrackBasenote.currValue = 0;
  inputTrackBasenote.col = POS_SETUP_LEFT;
  inputTrackBasenote.row = POS_SETUP_TOP+3;
  inputTrackBasenote.action = &PGS_inputActionSetupBasenote;

  inputTrackMidiChannel.labelLength = POS_LABEL_LENGTH;
  inputTrackMidiChannel.length = 3;
  inputTrackMidiChannel.maxValue = 16;
  inputTrackMidiChannel.currValue = 0;
  inputTrackMidiChannel.col = POS_SETUP_LEFT;
  inputTrackMidiChannel.row = POS_SETUP_TOP+4;
  inputTrackMidiChannel.action = &PGS_inputActionSetupMidiChannel;

  setupTrackFields[0] = &inputTrackVelocity;
  setupTrackFields[1] = &inputTrackAccent;
  setupTrackFields[2] = &inputTrackBasenote;
  setupTrackFields[3] = &inputTrackMidiChannel;

  inputPatternTracks.labelLength = POS_LABEL_LENGTH;
  inputPatternTracks.length = 3;
  inputPatternTracks.maxValue = 12;
  inputPatternTracks.currValue = 0;
  inputPatternTracks.col = POS_SETUP_LEFT;
  inputPatternTracks.row = POS_SETUP_TOP;
  inputPatternTracks.action = &PGS_inputActionSetupTracks;

  inputPatternSteps.labelLength = POS_LABEL_LENGTH;
  inputPatternSteps.length = 3;
  inputPatternSteps.maxValue = 32;
  inputPatternSteps.currValue = 0;
  inputPatternSteps.col = POS_SETUP_LEFT;
  inputPatternSteps.row = POS_SETUP_TOP+1;
  inputPatternSteps.action = &PGS_inputActionSetupSteps;

  inputPatternRes.labelLength = POS_LABEL_LENGTH;
  inputPatternRes.length = 3;
  inputPatternRes.maxValue = 4;
  inputPatternRes.currValue = 0;
  inputPatternRes.col = POS_SETUP_LEFT;
  inputPatternRes.row = POS_SETUP_TOP+2;
  inputPatternRes.action = &PGS_inputActionSetupRes;

  inputPatternBpb.labelLength = POS_LABEL_LENGTH;
  inputPatternBpb.length = 3;
  inputPatternBpb.maxValue = 8;
  inputPatternBpb.currValue = 0;
  inputPatternBpb.col = POS_SETUP_LEFT;
  inputPatternBpb.row = POS_SETUP_TOP+3;
  inputPatternBpb.action = &PGS_inputActionSetupBpb;

  inputPatternSwing.labelLength = POS_LABEL_LENGTH;
  inputPatternSwing.length = 3;
  inputPatternSwing.maxValue = 100;
  inputPatternSwing.currValue = 0;
  inputPatternSwing.col = POS_SETUP_LEFT;
  inputPatternSwing.row = POS_SETUP_TOP+4;
  inputPatternSwing.action = &PGS_inputActionSetupSwing;

  inputPatternSetBpm.labelLength = POS_LABEL_LENGTH;
  inputPatternSetBpm.length = 3;
  inputPatternSetBpm.maxValue = 1;
  inputPatternSetBpm.currValue = 0;
  inputPatternSetBpm.col = POS_SETUP_LEFT;
  inputPatternSetBpm.row = POS_SETUP_TOP+6;
  inputPatternSetBpm.action = &PGS_inputActionSetupSetBpm;

  setupPatternFields[0] = &inputPatternTracks;
  setupPatternFields[1] = &inputPatternSteps;
  setupPatternFields[2] = &inputPatternRes;
  setupPatternFields[3] = &inputPatternBpb;
  setupPatternFields[4] = &inputPatternSwing;
  setupPatternFields[5] = &inputPatternSetBpm;

  inputGeneralExtSync.labelLength = POS_LABEL_LENGTH;
  inputGeneralExtSync.length = 3;
  inputGeneralExtSync.maxValue = 1;
  inputGeneralExtSync.currValue = 0;
  inputGeneralExtSync.col = POS_SETUP_LEFT;
  inputGeneralExtSync.row = POS_SETUP_TOP;
  inputGeneralExtSync.action = &PGS_inputActionSetupExternalSynch;

  inputGeneralSysex1.labelLength = POS_LABEL_LENGTH;
  inputGeneralSysex1.length = 3;
  inputGeneralSysex1.maxValue = 127;
  inputGeneralSysex1.currValue = 0;
  inputGeneralSysex1.col = POS_SETUP_LEFT;
  inputGeneralSysex1.row = POS_SETUP_TOP+1;
  inputGeneralSysex1.action = &PGS_inputActionSetupSysexAddr1;

  inputGeneralSysex2.labelLength = POS_LABEL_LENGTH;
  inputGeneralSysex2.length = 3;
  inputGeneralSysex2.maxValue = 127;
  inputGeneralSysex2.currValue = 0;
  inputGeneralSysex2.col = POS_SETUP_LEFT;
  inputGeneralSysex2.row = POS_SETUP_TOP+2;
  inputGeneralSysex2.action = &PGS_inputActionSetupSysexAddr2;

  inputGeneralSysexDump.labelLength = POS_LABEL_LENGTH;
  inputGeneralSysexDump.length = 3;
  inputGeneralSysexDump.maxValue = 1;
  inputGeneralSysexDump.currValue = 0;
  inputGeneralSysexDump.col = POS_SETUP_LEFT;
  inputGeneralSysexDump.row = POS_SETUP_TOP+3;
  inputGeneralSysexDump.action = &PGS_inputActionSetupSysexDump;

  inputGeneralResetSteps.labelLength = POS_LABEL_LENGTH;
  inputGeneralResetSteps.length = 3;
  inputGeneralResetSteps.maxValue = 1;
  inputGeneralResetSteps.currValue = 0;
  inputGeneralResetSteps.col = POS_SETUP_LEFT;
  inputGeneralResetSteps.row = POS_SETUP_TOP+5;
  inputGeneralResetSteps.action = &PGS_inputActionSetupResetSteps;

  inputGeneralResetSong.labelLength = POS_LABEL_LENGTH;
  inputGeneralResetSong.length = 3;
  inputGeneralResetSong.maxValue = 1;
  inputGeneralResetSong.currValue = 0;
  inputGeneralResetSong.col = POS_SETUP_LEFT;
  inputGeneralResetSong.row = POS_SETUP_TOP+6;
  inputGeneralResetSong.action = &PGS_inputActionSetupResetSong;

  inputGeneralResetPerformance.labelLength = POS_LABEL_LENGTH;
  inputGeneralResetPerformance.length = 3;
  inputGeneralResetPerformance.maxValue = 1;
  inputGeneralResetPerformance.currValue = 0;
  inputGeneralResetPerformance.col = POS_SETUP_LEFT;
  inputGeneralResetPerformance.row = POS_SETUP_TOP+7;
  inputGeneralResetPerformance.action = &PGS_inputActionSetupResetPerformance;

  inputGeneralResetSystem.labelLength = POS_LABEL_LENGTH;
  inputGeneralResetSystem.length = 3;
  inputGeneralResetSystem.maxValue = 1;
  inputGeneralResetSystem.currValue = 0;
  inputGeneralResetSystem.col = POS_SETUP_LEFT;
  inputGeneralResetSystem.row = POS_SETUP_TOP+9;
  inputGeneralResetSystem.action = &PGS_inputActionSetupResetSystem;

  setupGeneralFields[0] = &inputGeneralExtSync;
  setupGeneralFields[1] = &inputGeneralSysex1;
  setupGeneralFields[2] = &inputGeneralSysex2;
  setupGeneralFields[3] = &inputGeneralSysexDump;
  setupGeneralFields[4] = &inputGeneralResetSteps;
  setupGeneralFields[5] = &inputGeneralResetSong;
  setupGeneralFields[6] = &inputGeneralResetPerformance;
  setupGeneralFields[7] = &inputGeneralResetSystem;
  
  //Trick to fool the linker. If this is not included, the linker will not
  // be able to find the addresses of any of the inputActions above!
  if(0>1){
    inputTrackVelocity.action(0, 0);
  }
}

void PGS_showPage(){
  currPage = PAGE_SETUP;
  nextPatternAction = &GLOBAL_nop;
  IN_clearCurrentInputField();
  
  currSetupItem = SETUP_ITEM_GENERAL;
  SEQ_VIS_writeMenu();
  SEQ_VIS_writeSetupItems();
  PGS_toggleSetupItem(2);

  pageFocus = FOCUS_LEFT;
}

void PGS_updateSetupTrack(){
  unsigned short track;
  track = currSetupItem-2;
  inputTrackVelocity.currValue = patt->tracks[currSetupItem-2].velocity;
  inputTrackAccent.currValue = patt->tracks[currSetupItem-2].accent;
  inputTrackBasenote.currValue = patt->tracks[currSetupItem-2].basenote;
  inputTrackMidiChannel.currValue = patt->tracks[currSetupItem-2].channel + 1;

  IN_refreshInputField(&inputTrackVelocity);
  IN_refreshInputField(&inputTrackAccent);
  IN_refreshInputField(&inputTrackBasenote);
  IN_refreshInputField(&inputTrackMidiChannel);
}

void PGS_updateSetupPattern(){
  inputPatternTracks.currValue = patt->trackcount;
  inputPatternSteps.currValue = patt->steps;
  inputPatternRes.currValue = patt->res;
  inputPatternBpb.currValue = patt->bpb;
  inputPatternSwing.currValue = patt->swingBalance;
  inputPatternSetBpm.currValue = G_SEQ_resetBpmOnLoad;

  IN_refreshInputField(&inputPatternTracks);
  IN_refreshInputField(&inputPatternSteps);
  IN_refreshInputField(&inputPatternRes);
  IN_refreshInputField(&inputPatternBpb);
  IN_refreshInputField(&inputPatternSwing);
  IN_refreshInputField(&inputPatternSetBpm);
}

void PGS_updateSetupGeneral(){
  inputGeneralExtSync.currValue = G_MIDI_isExternalSynchOn;
  inputGeneralSysex1.currValue = G_MIDI_sysexIdByte1;
  inputGeneralSysex2.currValue = G_MIDI_sysexIdByte2;
  inputGeneralSysexDump.currValue = 0;
  inputGeneralResetSteps.currValue = 0;
  inputGeneralResetSong.currValue = 0;
  inputGeneralResetPerformance.currValue = 0;
  inputGeneralResetSystem.currValue = 0;

  IN_refreshInputField(&inputGeneralExtSync);
  IN_refreshInputField(&inputGeneralSysex1);
  IN_refreshInputField(&inputGeneralSysex2);
  IN_refreshInputField(&inputGeneralSysexDump);
  IN_refreshInputField(&inputGeneralResetSteps);
  IN_refreshInputField(&inputGeneralResetSong);
  IN_refreshInputField(&inputGeneralResetPerformance);
  IN_refreshInputField(&inputGeneralResetSystem);
}

void PGS_inputActionSetupVelocity(struct InputField *field, short enter){
  if(enter){
    patt->tracks[currSetupItem - 2].velocity = field->currValue;
  }
}

void PGS_inputActionSetupAccent(struct InputField *field, short enter){
  if(enter){
    patt->tracks[currSetupItem - 2].accent = field->currValue;
  }
}

void PGS_inputActionSetupBasenote(struct InputField *field, short enter){
  if(enter){
    patt->tracks[currSetupItem - 2].basenote = field->currValue;
  }
}

void PGS_inputActionSetupMidiChannel(struct InputField *field, short enter){
  if(enter){
    patt->tracks[currSetupItem - 2].channel = field->currValue -1;
  }
}

void PGS_inputActionSetupSteps(struct InputField *field, short enter){
  if(enter){
    patt->steps = field->currValue;
    SEQ_updateMaxTrackAndStep();
  }
}

void PGS_inputActionSetupRes(struct InputField *field, short enter){
  if(enter){
    patt->res = field->currValue;
  }
}

void PGS_inputActionSetupBpb(struct InputField *field, short enter){
  if(enter){
    patt->bpb = field->currValue;
  }
}

void PGS_inputActionSetupTracks(struct InputField *field, short enter){
  if(enter){
    if(field->currValue == 0){
      field->currValue = 1;
      IN_refreshInputField(field);
    }
    patt->trackcount = field->currValue;
    SEQ_updateMaxTrackAndStep();
    SEQ_VIS_writeSetupItems();
    SEQ_VIS_writeItemFocus(currSetupItem+2);
  }
}

void PGS_inputActionSetupSwing(struct InputField *field, short enter){
  if(enter){
    patt->swingBalance = field->currValue;
    SEQ_setPulseTimerStart();
  }
}

void PGS_inputActionSetupSetBpm(struct InputField *field, short enter){
  if(enter){
    G_SEQ_resetBpmOnLoad = field->currValue;
  }
}

void PGS_inputActionSetupExternalSynch(struct InputField *field, short enter){
  if(enter){
    G_MIDI_isExternalSynchOn = field->currValue;
    if(G_MIDI_isExternalSynchOn == 0){
      G_MIDI_isInPlayMode = 0;
    }
  }
}

void PGS_inputActionSetupSysexAddr1(struct InputField *field, short enter){
  if(enter){
    G_MIDI_sysexIdByte1 = field->currValue;
  }
}

void PGS_inputActionSetupSysexAddr2(struct InputField *field, short enter){
  if(enter){
    G_MIDI_sysexIdByte2 = field->currValue;
  }
}

void PGS_inputActionSetupSysexDump(struct InputField *field, short enter){
  if(enter){
    if(field->currValue == 1){
      SEQ_VIS_showSysexDumpDialog();
      MIDI_sendSysexStart();
      EEX_exportMemoryAsMidi(MEMMAP_CHIP1);
      EEX_exportMemoryAsMidi(MEMMAP_CHIP2);
      MIDI_sendSysexEnd();
      field->currValue = 0;
      PGS_showPage();
    }
  }
}

void PGS_inputActionSetupResetSteps(struct InputField *field, short enter){
  if(enter){
    if(field->currValue == 1){
      SEQ_resetSequencer();
      field->currValue = 0;
      IN_refreshInputField(field);
    }
  }
}

void PGS_inputActionSetupResetSong(struct InputField *field, short enter){
  if(enter){
    if(field->currValue == 1){
      SEQ_stopPlayback();
      PGSO_initPage();
      field->currValue = 0;
      IN_refreshInputField(field);
    }
  }
}

void PGS_inputActionSetupResetPerformance(struct InputField *field, short enter){
  if(enter){
    if(field->currValue == 1){
      SEQ_stopPlayback();

      patt->folderNum = NOT_SELECTED;
      patt->fileNum = NOT_SELECTED;
      nextPatt->folderNum = NOT_SELECTED;
      nextPatt->fileNum = NOT_SELECTED;

      PGPF_initPage();
      field->currValue = 0;
      IN_refreshInputField(field);
    }
  }
}

void PGS_inputActionSetupResetSystem(struct InputField *field, short enter){
  if(enter){
    if(field->currValue == 1){
      SEQ_SETUP_setDefaultSetup(patt);
      EEX_writeSetup(patt);
      field->currValue = 0;
      IN_refreshInputField(field);
    }
  }
}

void PGS_toggleSetupParamFirst(){
  PGS_toggleSetupParam(0);
}

void PGS_toggleSetupParamNext(){
  PGS_toggleSetupParam(currSetupParam+1);
}

void PGS_toggleSetupParamPrevious(){
  PGS_toggleSetupParam(currSetupParam-1);
}

//
// Toggle focus on the right hand of the setup page. Highlights a given
// input field.
//
void PGS_toggleSetupParam(unsigned short param){

  if(currSetupItem == SETUP_ITEM_GENERAL){
    if(param >= sizeof(setupGeneralFields) / sizeof(struct InputField *)) return;
    PGS_updateSetupGeneral(); //update in case the current value has not been saved
    IN_refreshInputField(currentInputField);
    currSetupParam = param;
    IN_setCurrentInputField(setupGeneralFields[param]);
  } else if(currSetupItem == SETUP_ITEM_PATTERN){
    if(param >= sizeof(setupPatternFields) / sizeof(struct InputField *)) return;
    PGS_updateSetupPattern(); //update in case the current value has not been saved
    IN_refreshInputField(currentInputField);
    currSetupParam = param;
    IN_setCurrentInputField(setupPatternFields[param]);
  } else {
    if(param >= sizeof(setupTrackFields) / sizeof(struct InputField *)) return;
    PGS_updateSetupTrack(); //update in case the current value has not been saved
    IN_refreshInputField(currentInputField);
    currSetupParam = param;
    IN_setCurrentInputField(setupTrackFields[param]);
  }
}

void PGS_toggleSetupItemNext(){
  PGS_toggleSetupItem(currSetupItem+1);
}

void PGS_toggleSetupItemPrevious(){
  PGS_toggleSetupItem(currSetupItem-1);
}

//
// Show setup fields for a certain item
//
void PGS_toggleSetupItem(unsigned short newItem){

  SEQ_VIS_clearItemFocus(currSetupItem+2);

  if(newItem == 255){
    newItem = patt->trackcount + 1;
  } else if(newItem >= patt->trackcount + 2){
    newItem = 0;
  }

  switch(newItem){
  case SETUP_ITEM_GENERAL:
    currSetupItem = newItem;
    SEQ_VIS_writeSetupGeneral();
    PGS_updateSetupGeneral();
    break;
  case SETUP_ITEM_PATTERN:
    currSetupItem = newItem;
    SEQ_VIS_writeSetupPattern();
    PGS_updateSetupPattern();
    break;
  default:
    currSetupItem = newItem;
    SEQ_VIS_writeSetupTrack(currSetupItem-2);
    PGS_updateSetupTrack();
    break;
  }

  SEQ_VIS_writeItemFocus(currSetupItem+2);
}

//
// Send a pressed key event to the current key receiver
//
void PGS_keyEventHandler(char key){
  if(key >= KEY_0 && key <= KEY_9){
    IN_addNumberToInputField(key-KEY_0);
    return;
  }

  switch(key){
  case KEY_UP:
    if(pageFocus == FOCUS_LEFT){
      PGS_toggleSetupItemPrevious();
    } else {
      PGS_toggleSetupParamPrevious();
    }
    break;
  case KEY_DOWN:
    if(pageFocus == FOCUS_LEFT){
      PGS_toggleSetupItemNext();
    } else {
      PGS_toggleSetupParamNext();
    }
    break;
  case KEY_LEFT:
  case KEY_RIGHT:
    NAV_toggleFocus();
    break;
  case KEY_ENTER:
    if(pageFocus == FOCUS_RIGHT){
      IN_enterInputField();
      EEX_writeSetupParameter(currSetupItem, currSetupParam, currentInputField->currValue);
    }
    break;
  default:
    break;
  }
}