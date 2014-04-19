#include "page_performance.h"
#include "page_performance_private.h"

#include "sequencer.h"
#include "navigation.h"
#include "input.h"

#include "../GLCD/sequencer_visualisation.h"
#include "../storage/external_storage.h"
#include "../midi/midi.h"

#include "../shared/key_consts.h"
#include "../shared/util.h"
#include "../shared/global.h"
#include "../shared/definitions.h"

struct InputField inputNextPattern;
struct InputField inputPerfPatternNum;

char perfCursorPos;
char perfCurrPos;
char perfFirstPatternOnPage;

struct MenuItem dialogPerfPatternNum;
struct MenuItem dialogPerfOk;
struct MenuItem dialogPerfCancel;
struct MenuItem *dialogPerfItems[3];

void PGPF_initPage(){

  inputNextPattern.maxValue = 812;
  inputNextPattern.currValue = NOT_SELECTED;
  inputNextPattern.labelLength = 0;
  inputNextPattern.length = 3;
  inputNextPattern.col = POS_PERF_LEFT_COL;
  inputNextPattern.row = POS_PERF_NEXT_PATT_TOP+2;
  inputNextPattern.action = &PGPF_inputActionNextPatternChanged;

  inputPerfPatternNum.maxValue = 812;
  inputPerfPatternNum.currValue = NOT_SELECTED;
  inputPerfPatternNum.labelLength = 0;
  inputPerfPatternNum.length = 3;
  inputPerfPatternNum.col = POS_DIALOG_ITEM_LEFT+10;
  inputPerfPatternNum.row = POS_DIALOG_CHANGE_PATTERN_TOP+3;
  inputPerfPatternNum.action = &PGPF_inputActionPatternChanged;

  // Trick to fool the linker. If this is not included, the linker will not
  // be able to find the addresses of any of the inputActions above!
  if(0>1){
    inputNextPattern.action(0, 0);
  }

  perfCurrPos = 0;
  perfCursorPos = 0;
  perfFirstPatternOnPage = 0;

  G_PGPF_patternLoadEnabled = 1;
  
  PGPF_initDialogMenuItems();
  PGPF_resetPatternKeyMap();
}

void PGPF_initDialogMenuItems(){

  dialogPerfPatternNum.col = POS_DIALOG_ITEM_LEFT;
  dialogPerfPatternNum.row = POS_DIALOG_CHANGE_PATTERN_TOP+3;
  dialogPerfPatternNum.keyListener = &PGPF_listenerEnterPatternNum;

  dialogPerfOk.col = POS_DIALOG_ITEM_LEFT;
  dialogPerfOk.row = POS_DIALOG_CHANGE_PATTERN_TOP+5;
  dialogPerfOk.keyListener = &PGPF_listenerUpdatePattern;

  dialogPerfCancel.col = POS_DIALOG_ITEM_LEFT;
  dialogPerfCancel.row = POS_DIALOG_CHANGE_PATTERN_TOP+5;
  dialogPerfCancel.keyListener = &PGPF_listenerCancel;

  dialogPerfItems[0] = &dialogPerfPatternNum;
  dialogPerfItems[1] = &dialogPerfOk;
  dialogPerfItems[2] = &dialogPerfCancel;

}

void PGPF_resetPatternKeyMap(){
  char i;
  for(i=0; i<MAX_KEY_MAP_SIZE; i++){
    patternKeyMap[i] = NOT_SELECTED;
  }
}

void PGPF_showPage(){
  currPage = PAGE_PERFORMANCE;
  nextPatternAction = &PGPF_swapPatterns;
  IN_clearCurrentInputField();
  
  pageFocus = FOCUS_LEFT;
  PGPF_redrawPage();
  PGPF_toggleNextPatternInput();
}

void PGPF_redrawPage(){
  int currPattern;

  SEQ_VIS_showPagePerformance();
  SEQ_VIS_writeExternalSynch(G_MIDI_isExternalSynchOn);

  GLOBAL_resetName();
  if(patt->folderNum != NOT_SELECTED && patt->fileNum != NOT_SELECTED){
    EEX_readFileName(patt->folderNum-1, patt->fileNum-1, name);
  }

  SEQ_VIS_writeCurrPatternName(name);
  currPattern = UTIL_getPrintablePatternNumFromFolderFile(patt->folderNum, patt->fileNum);
  SEQ_VIS_writeCurrPerformancePattern(currPattern);

  GLOBAL_resetName();
  if(nextPatt->folderNum != NOT_SELECTED && nextPatt->fileNum != NOT_SELECTED){
    EEX_readFileName(nextPatt->folderNum-1, nextPatt->fileNum-1, name);
  }

  SEQ_VIS_writeNextPatternName(name);
  IN_refreshInputField(&inputNextPattern);
  PGPF_writeKeyMapPatterns();

}

void PGPF_swapPatterns(){

  int currPattern;

  inputNextPattern.currValue = NOT_SELECTED;
  IN_refreshInputField(&inputNextPattern);
  SEQ_VIS_updateNextPatternSetup();

  GLOBAL_resetName();
  SEQ_VIS_writeNextPatternName(name);

  EEX_readFileName(patt->folderNum-1, patt->fileNum-1, name);
  SEQ_VIS_writeCurrPatternName(name);
  
  currPattern = UTIL_getPrintablePatternNumFromFolderFile(patt->folderNum, patt->fileNum);
  SEQ_VIS_writeCurrPerformancePattern(currPattern);
  
  SEQ_VIS_updateCurrPatternSetup();

}

void PGPF_togglePrevPage(char cursorOnTop){
  if(perfFirstPatternOnPage > 0){
    SEQ_VIS_clearKeyMapItemFocus(perfCursorPos);
    perfFirstPatternOnPage = perfFirstPatternOnPage - PERF_ELEMENTS_PER_PAGE;
    PGPF_writeKeyMapPatterns();
    if(cursorOnTop){
      perfCursorPos = 0;
    } else {
      perfCursorPos = PERF_ELEMENTS_PER_PAGE - 1;
    }
    perfCurrPos = perfFirstPatternOnPage + perfCursorPos;
    SEQ_VIS_writeKeyMapItemFocus(perfCursorPos);
  }
}

void PGPF_togglePrevPatt(){
  if(perfCurrPos > 0){
    perfCurrPos--;

    if(perfCursorPos>0){
      SEQ_VIS_clearKeyMapItemFocus(perfCursorPos);
      perfCursorPos--;
      SEQ_VIS_writeKeyMapItemFocus(perfCursorPos);
    } else {
      PGPF_togglePrevPage(0);
    }
  }
}

void PGPF_toggleNextPage(){
  if(perfFirstPatternOnPage < MAX_KEY_MAP_SIZE - PERF_ELEMENTS_PER_PAGE){
    SEQ_VIS_clearKeyMapItemFocus(perfCursorPos);
    perfCursorPos = 0;
    perfFirstPatternOnPage = perfFirstPatternOnPage + PERF_ELEMENTS_PER_PAGE;
    PGPF_writeKeyMapPatterns();
    perfCurrPos = perfFirstPatternOnPage;
    SEQ_VIS_writeKeyMapItemFocus(perfCursorPos);
  }
}

void PGPF_toggleNextPatt(){
  if(perfFirstPatternOnPage + perfCursorPos < MAX_KEY_MAP_SIZE - 1){
    perfCurrPos++;

    if(perfCursorPos<PERF_ELEMENTS_PER_PAGE-1){
      SEQ_VIS_clearKeyMapItemFocus(perfCursorPos);
      perfCursorPos++;
      SEQ_VIS_writeKeyMapItemFocus(perfCursorPos);
    } else {
      PGPF_toggleNextPage();
    }
  }
}

void PGPF_writeKeyMapPatterns(){

  char i;
  char currIndex;
  currIndex = perfFirstPatternOnPage;

  for(i=0;i<PERF_ELEMENTS_PER_PAGE; i++){
    if(perfFirstPatternOnPage + i < MAX_KEY_MAP_SIZE ){
      PGPF_getFileName(currIndex);
      SEQ_VIS_writeKeyMapPattern(i, i + perfFirstPatternOnPage + 1, name);
      currIndex++;
    } else {
      SEQ_VIS_writeEmptyKeyMapPattern(i);
    }
  }
}

void PGPF_getFileName(char index){
  char folderNum;
  char fileNum;

  GLOBAL_resetName();
  if(patternKeyMap[index] != NOT_SELECTED){
    folderNum = UTIL_getFolderNum(patternKeyMap[index]);
    fileNum = UTIL_getFileNum(patternKeyMap[index]);
    EEX_readFileName(folderNum-1, fileNum-1, name);
  }
}


void PGPF_loadNextPatt(char folderNum, char fileNum){
  EEX_readPattern(folderNum-1, fileNum-1, nextPatt);

  if(sequencer.playState == PLAY_STOPPED){
    SEQ_switchToNextPattern();
  } else {
    SEQ_VIS_updateNextPatternSetup();
  }
}

void PGPF_deletePattern(){
  patternKeyMap[perfCurrPos] = NOT_SELECTED;
  PGPF_writeKeyMapPatterns();
}


void PGPF_quickNextPatternInput(char key){
  char patternNum;
  patternNum = patternKeyMap[key];
  if(patternNum != NOT_SELECTED){
    inputNextPattern.currValue = UTIL_getPrintablePatternNum(patternNum);
    IN_refreshInputField(&inputNextPattern);
    PGPF_inputActionNextPatternChanged(&inputNextPattern, 0);
    PGPF_inputActionNextPatternChanged(&inputNextPattern, 1);
  }
}

void PGPF_inputActionNextPatternChanged(struct InputField *field, short enter){
  char folderNum;
  char fileNum;

  if(field->currValue > 99){
    folderNum = field->currValue / 100;
    fileNum = field->currValue - folderNum * 100;
    EEX_readFileName(folderNum-1, fileNum-1, name);
    if(enter){
      if(name[0] != 0 && G_PGPF_patternLoadEnabled){
        PGPF_loadNextPatt(folderNum, fileNum);
      }
    } else {
      SEQ_VIS_writeNextPatternName(name);
    }
  }
}

void PGPF_inputActionPatternChanged(struct InputField *field, short enter){
  char folderNum;
  char fileNum;

  if(field->currValue > 99){
    folderNum = field->currValue / 100;
    fileNum = field->currValue - folderNum * 100;
    EEX_readFileName(folderNum-1, fileNum-1, name);
    SEQ_VIS_changePerfPatternDialogUpdateName(name);
  }
}

void PGPF_listenerShowChangePatternDialog(char key){
  if(key == KEY_ENTER){
    currDialog = DIALOG_CHANGE_PATTERN;
    inputPerfPatternNum.currValue = UTIL_getPrintablePatternNum(patternKeyMap[perfCurrPos]);
    SEQ_VIS_changePerfPatternDialog();
    PGPF_getFileName(perfCurrPos);
    SEQ_VIS_changePerfPatternDialogUpdateName(name);
    IN_refreshInputField(&inputPerfPatternNum);
    PGPF_toggleDialogItem(0);
  }
}

void PGPF_listenerEnterNextPatternNum(char key){
  if(key >= KEY_0 && key <= KEY_9){
    IN_addNumberToPatternInputField(key-KEY_0);
  }
}

void PGPF_listenerEnterPatternNum(char key){
  currentInputField = &inputPerfPatternNum;
  if(key >= KEY_0 && key <= KEY_9){
    IN_addNumberToPatternInputField(key-KEY_0);
  }
}

//
// Update the current position with a new pattern. It is only possible to press
// OK if the currently entered number is a valid pattern number (but the pattern
// need not exist...)
//
void PGPF_listenerUpdatePattern(char key){
  if(key == KEY_ENTER && inputPerfPatternNum.currValue > 99){
    patternKeyMap[perfCurrPos] = UTIL_getPatternNumFromPrintable(inputPerfPatternNum.currValue);
    PGPF_closeDialog();
  }
}

void PGPF_listenerCancel(char key){
  if(key == KEY_ENTER){
    PGPF_closeDialog();
  }
}

void PGPF_togglePerfKeyMappingInput(){
  currKeyListener = &PGPF_listenerShowChangePatternDialog;
  IN_setCurrentInputField(0);
  SEQ_VIS_writeKeyMapItemFocus(perfCursorPos);
}

void PGPF_toggleNextPatternInput(){
  SEQ_VIS_clearKeyMapItemFocus(perfCursorPos);
  IN_setCurrentInputField(&inputNextPattern);
  currKeyListener = PGPF_listenerEnterNextPatternNum;
}

void PGPF_togglePlayPause(){
  SEQ_togglePlayPause(PLAY_MODE_PERFORMANCE);
  SEQ_VIS_writePlayState();
  SEQ_VIS_writePlayMode();
}

void PGPF_stop(){
  SEQ_stopPlayback();
  SEQ_VIS_writePlayState();
  SEQ_VIS_writePlayMode();
  SEQ_VIS_writeTime(1);
}

void PGPF_closeDialog(){
  currDialog = DIALOG_NO_DIALOG;
  currMenuItem = DIALOG_NO_MENU_ITEM;
  PGPF_redrawPage();
  PGPF_togglePerfKeyMappingInput();
}

//
// Dialog box event handler
//
void PGPF_toggleDialogItem(unsigned short item){

  if(currDialog == DIALOG_CHANGE_PATTERN){
    if(item >= sizeof(dialogPerfItems) / sizeof(struct MenuItem *)) return;
    NAV_setCurrentDialogItem(dialogPerfItems[item]);
  }
  currDialogItem = item;
}

//
// Send a pressed key event to the current key receiver
//
void PGPF_keyEventHandler(char key){

  if(currDialog == DIALOG_NO_DIALOG){
  
    if(key >= KEY_SEQ01 && key <= KEY_SEQ32){
      PGPF_quickNextPatternInput(key - KEY_SEQ01);
      return;
    }

    switch(key){
    case KEY_UP:
      if(pageFocus == FOCUS_RIGHT){
        if(G_NAV_shiftOn){
          PGPF_togglePrevPage(1);
        } else {
          PGPF_togglePrevPatt();
        }
      }
      break;
    case KEY_DOWN:
      if(pageFocus == FOCUS_RIGHT){
        if(G_NAV_shiftOn){
          PGPF_toggleNextPage();
        } else {
          PGPF_toggleNextPatt();
        }
      }
      break;
    case KEY_PLAY:
      PGPF_togglePlayPause();
      break;
    case KEY_STOP:
      PGPF_stop();
      break;
    case KEY_ENTER:
      if(pageFocus == FOCUS_LEFT){
        IN_enterInputField();
      } else {
        currKeyListener(key);
      }
      break;
    case KEY_LEFT:
    case KEY_RIGHT:
      NAV_toggleFocus();
      break;
    case KEY_CUT:
      PGPF_deletePattern();
      break;
    default:
      currKeyListener(key);
    }
  } else {
    switch(key){
    case KEY_UP:
      PGPF_toggleDialogItem(currDialogItem-1);
      break;
    case KEY_DOWN:
      PGPF_toggleDialogItem(currDialogItem+1);
      break;
    default:
      currKeyListener(key);
    }
  }
}