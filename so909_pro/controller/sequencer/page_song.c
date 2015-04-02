#include "page_song.h"
#include "page_song_private.h"

#include "sequencer.h"
#include "navigation.h"
#include "input.h"

#include "../storage/external_storage.h"
#include "../GLCD/sequencer_visualisation.h"
#include "../GLCD/GLCD.h"
#include "../midi/midi.h"

#include "../shared/types.h"
#include "../shared/util.h"
#include "../shared/key_consts.h"
#include "../shared/global.h"
#include "../shared/definitions.h"

/**
shift-play v. load setter song i loop mode
shift-stop resetter loop mode, sang stopper da etter siste pattern. Kan gjøres mens man spiller
shift-up/down hopper 1 side opp/ned
paste: insert new pattern (if songEnd < 127)
cut: delete pattern
Tillatt med tomme patterns, de hoppes over.
**/

// as a song is a linked list, and elements may be inserted _before_ the first
// element, we need to know what pattern is the first and last one.
char songEnd;
char songCurrPos;
char songCurrRepeat;
char songCursorPos;
char songFirstPatternOnPage;
char patternLoaded;

// dialog box items
struct MenuItem dialogPatternNum;
struct MenuItem dialogPatternRepeat;
struct MenuItem dialogOk;
struct MenuItem dialogCancel;
struct MenuItem *dialogItems[4];
struct InputField inputPatternNum;
struct InputField inputPatternRepeat;

void PGSO_initPage(){
  unsigned short i;

  songEnd=0;
  songCurrPos=0;
  songCurrRepeat=0;
  songCursorPos=0;
  G_PGSO_songLoop=0;
  songFirstPatternOnPage=0;
  patternLoaded=0;

  songElements[0].pattern = UTIL_getPatternNum(NOT_SELECTED, NOT_SELECTED);
  songElements[0].repeat = 0;

  for(i=1; i<MAX_SONG_PATTERNS - 1; i++){
    songElements[i].pattern = UTIL_getPatternNum(NOT_SELECTED, NOT_SELECTED);
    songElements[i].repeat = 0;
  }

  songElements[MAX_SONG_PATTERNS - 1].pattern = UTIL_getPatternNum(NOT_SELECTED, NOT_SELECTED);
  songElements[MAX_SONG_PATTERNS - 1].repeat = 0;
  
  inputPatternNum.maxValue = 812;
  inputPatternNum.currValue = NOT_SELECTED;
  inputPatternNum.labelLength = 0;
  inputPatternNum.length = 3;
  inputPatternNum.col = POS_DIALOG_ITEM_LEFT+10;
  inputPatternNum.row = POS_DIALOG_CHANGE_PATTERN_TOP+3;
  inputPatternNum.action = &PGSO_inputActionPatternChanged;

  inputPatternRepeat.maxValue = 4;
  inputPatternRepeat.currValue = NOT_SELECTED;
  inputPatternRepeat.labelLength = 0;
  inputPatternRepeat.length = 3;
  inputPatternRepeat.col = POS_DIALOG_ITEM_LEFT+10;
  inputPatternRepeat.row = POS_DIALOG_CHANGE_PATTERN_TOP+4;
  inputPatternRepeat.action = &PGSO_inputActionNoop;

  // Trick to fool the linker. If this is not included, the linker will not
  // be able to find the addresses of any of the inputActions above!
  if(0>1){
    inputPatternNum.action(0, 0);
  }

  PGSO_initDialogMenuItems();
}

void PGSO_initDialogMenuItems(){

  dialogPatternNum.col = POS_DIALOG_ITEM_LEFT;
  dialogPatternNum.row = POS_DIALOG_CHANGE_PATTERN_TOP+3;
  dialogPatternNum.keyListener = &PGSO_listenerEnterPatternNum;

  dialogPatternRepeat.col = POS_DIALOG_ITEM_LEFT;
  dialogPatternRepeat.row = POS_DIALOG_CHANGE_PATTERN_TOP+4;
  dialogPatternRepeat.keyListener = &PGSO_listenerEnterPatternRepeat;

  dialogOk.col = POS_DIALOG_ITEM_LEFT;
  dialogOk.row = POS_DIALOG_CHANGE_PATTERN_TOP+6;
  dialogOk.keyListener = &PGSO_listenerUpdatePattern;

  dialogCancel.col = POS_DIALOG_ITEM_LEFT;
  dialogCancel.row = POS_DIALOG_CHANGE_PATTERN_TOP+7;
  dialogCancel.keyListener = &PGSO_listenerCancel;

  dialogItems[0] = &dialogPatternNum;
  dialogItems[1] = &dialogPatternRepeat;
  dialogItems[2] = &dialogOk;
  dialogItems[3] = &dialogCancel;
  
}

void PGSO_showPage(){
  currPage = PAGE_SONG;
  patternLoaded = 2; // trigger re-check of pattern status.
  nextPatternAction = &GLOBAL_nop;
  SEQ_VIS_showPageSong();
  SEQ_VIS_writeExternalSynch(G_MIDI_isExternalSynchOn);
  SEQ_VIS_writeLoop(G_PGSO_songLoop);

  PGSO_writeSongPatterns();
  SEQ_VIS_writeItemFocus(songCursorPos+POS_SONG_TOP);
  currKeyListener = &PGSO_listenerShowChangePatternDialog;
  PGSO_updatePatternStatus();
}

void PGSO_updatePatternStatus(){
  int patternNum;
  char folderNum, fileNum;

  patternNum = songElements[songCurrPos].pattern;
  folderNum = UTIL_getFolderNum(patternNum);
  fileNum = UTIL_getFileNum(patternNum);

  if(patternNum != NOT_SELECTED && patt->folderNum == folderNum && patt->fileNum == fileNum){
    if(patternLoaded != 1){
      patternLoaded = 1;
      SEQ_VIS_writePatternStatus(patternLoaded);
    }
  } else {
    if(patternLoaded){
      patternLoaded = 0;
      SEQ_VIS_writePatternStatus(patternLoaded);
    }
  }
}

char PGSO_shouldSwitchToNextPattern(){
  if(songCurrRepeat == songElements[songCurrPos].repeat){
    return 1;
  } else {
    return 0;
  }
}

void PGSO_loadFirstNonEmptyPattern(char pattToLoad){
  char folderNum;
  char fileNum;
  int patternNum;

  // Reset to beginning if we've passed the end of the song.
  // Also handles special case of 1 pattern songs.
  if(pattToLoad>songEnd){
    pattToLoad = 0;
  }

  //skip any blank patterns
  do{
    patternNum = songElements[pattToLoad].pattern;
    pattToLoad++;
  } while( pattToLoad <= songEnd && patternNum == NOT_SELECTED );
  
  if(patternNum != NOT_SELECTED){
    folderNum = UTIL_getFolderNum(patternNum);
    fileNum = UTIL_getFileNum(patternNum);
    EEX_readPattern(folderNum-1, fileNum-1, nextPatt);
  }

}

void PGSO_incrementSongPosition(){

  if(songCurrRepeat == songElements[songCurrPos].repeat){

    if(songCurrPos < songEnd){
      songCurrRepeat = 0;

      //skip any blank patterns
      do{
        PGSO_toggleNextPatt();
      } while( songElements[songCurrPos].pattern == NOT_SELECTED && songCurrPos < songEnd);

      //load the next pattern
      PGSO_loadFirstNonEmptyPattern(songCurrPos+1);
      PGSO_updatePatternStatus();
      
    } else {
      if(G_PGSO_songLoop){
        songCurrRepeat = 0;
        songCurrPos=0;
        songCursorPos=0;
        songFirstPatternOnPage=0;
        if(currPage == PAGE_SONG){
          PGSO_showPage();
        }
      } else {
        SEQ_stopPlayback();
      }
      PGSO_loadFirstNonEmptyPattern(1);
    }
  } else {
    songCurrRepeat++;
  }
}

void PGSO_insertPattern(){
  char i;
  struct SongElement blankElement;
  blankElement.pattern = NOT_SELECTED;
  blankElement.repeat = 0;
  
  // check if a blank pattern is available at the end of the list
  if(songElements[MAX_SONG_PATTERNS-1].pattern == NOT_SELECTED){

    for(i = MAX_SONG_PATTERNS-1; i > songCurrPos; i--){
      songElements[i] = songElements[i-1];
    }
    songElements[songCurrPos] = blankElement;

    PGSO_writeSongPatterns();
    SEQ_VIS_writeItemFocus(songCursorPos+POS_SONG_TOP);
    songEnd++;
  }
}

void PGSO_deletePattern(){
  char i;

  if(songCurrPos <= songEnd){
    for(i = songCurrPos; i< MAX_SONG_PATTERNS-1; i++){
      songElements[i] = songElements[i+1];
    }
    songElements[MAX_SONG_PATTERNS-1].pattern = NOT_SELECTED;
    songElements[MAX_SONG_PATTERNS-1].repeat = 0;

    PGSO_writeSongPatterns();
    SEQ_VIS_writeItemFocus(songCursorPos+POS_SONG_TOP);
    songEnd--;
  }
}

void PGSO_writeSongPatterns(){

  char i;
  char currIndex;
  currIndex = songFirstPatternOnPage;

  for(i=0;i<SONG_ELEMENTS_PER_PAGE; i++){
    if(songFirstPatternOnPage + i < MAX_SONG_PATTERNS ){
      PGSO_getFileName(currIndex);
      SEQ_VIS_writeSongPattern(i, i + songFirstPatternOnPage + 1, songElements[currIndex].repeat, name);
      currIndex++;
    } else {
      SEQ_VIS_writeEmptyPattern(i);
    }
  }
}

void PGSO_getFileName(char index){
  char folderNum;
  char fileNum;

  GLOBAL_resetName();
  if(songElements[index].pattern != NOT_SELECTED){
    folderNum = UTIL_getFolderNum(songElements[index].pattern);
    fileNum = UTIL_getFileNum(songElements[index].pattern);
    EEX_readFileName(folderNum-1, fileNum-1, name);
  }
}

void PGSO_togglePrevPage(char cursorOnTop){
  if(songFirstPatternOnPage > 0){
    songFirstPatternOnPage = songFirstPatternOnPage - SONG_ELEMENTS_PER_PAGE;
    PGSO_writeSongPatterns();
    if(cursorOnTop){
      songCursorPos = 0;
    } else {
      songCursorPos = SONG_ELEMENTS_PER_PAGE - 1;
    }
    songCurrPos = songFirstPatternOnPage + songCursorPos;
    SEQ_VIS_writeItemFocus(songCursorPos+POS_SONG_TOP);
    PGSO_updatePatternStatus();
  }
}

void PGSO_togglePrevPatt(){
  if(songCurrPos > 0){
    songCurrPos--;

    if(songCursorPos>0){
      SEQ_VIS_clearItemFocus(songCursorPos+POS_SONG_TOP);
      songCursorPos--;
      SEQ_VIS_writeItemFocus(songCursorPos+POS_SONG_TOP);
      PGSO_updatePatternStatus();
    } else {
      PGSO_togglePrevPage(0);
    }
  }
}

void PGSO_toggleNextPage(){
  if(songFirstPatternOnPage < MAX_SONG_PATTERNS - SONG_ELEMENTS_PER_PAGE){
    songCursorPos = 0;
    songFirstPatternOnPage = songFirstPatternOnPage + SONG_ELEMENTS_PER_PAGE;
    PGSO_writeSongPatterns();
    songCurrPos = songFirstPatternOnPage;
    SEQ_VIS_writeItemFocus(songCursorPos+POS_SONG_TOP);
    PGSO_updatePatternStatus();
  }
}

void PGSO_toggleNextPatt(){
  if(songFirstPatternOnPage + songCursorPos < MAX_SONG_PATTERNS - 1){
    songCurrPos++;

    if(songCursorPos<SONG_ELEMENTS_PER_PAGE-1){
      SEQ_VIS_clearItemFocus(songCursorPos+POS_SONG_TOP);
      songCursorPos++;
      SEQ_VIS_writeItemFocus(songCursorPos+POS_SONG_TOP);
      PGSO_updatePatternStatus();
    } else {
      PGSO_toggleNextPage();
    }
  }
}

void PGSO_listenerShowChangePatternDialog(char key){
  if(key == KEY_ENTER){
    currDialog = DIALOG_CHANGE_PATTERN;
    inputPatternNum.currValue = UTIL_getPrintablePatternNum(songElements[songCurrPos].pattern);
    inputPatternRepeat.currValue = songElements[songCurrPos].repeat + 1;
    SEQ_VIS_changePatternDialog();
    PGSO_getFileName(songCurrPos);
    SEQ_VIS_changePatternDialogUpdateName(name);
    IN_refreshInputField(&inputPatternNum);
    IN_refreshInputField(&inputPatternRepeat);
    PGSO_toggleDialogItem(0);
  }
}

void PGSO_listenerEnterPatternNum(char key){
  currentInputField = &inputPatternNum;
  if(key >= KEY_0 && key <= KEY_9){
    IN_addNumberToPatternInputField(key-KEY_0);
  }
}

void PGSO_listenerEnterPatternRepeat(char key){
  currentInputField = &inputPatternRepeat;
  if(key >= KEY_1 && key <= KEY_5){
    IN_addNumberToInputField(key-KEY_0);
  }
}

//
// Update the current position with a new pattern. It is only possible to press
// OK if the currently entered number is a valid pattern number and the pattern
// exists (has a file name)
//
void PGSO_listenerUpdatePattern(char key){
  if(key == KEY_ENTER && inputPatternNum.currValue > 99 && name[0] != 0){
    songElements[songCurrPos].pattern = UTIL_getPatternNumFromPrintable(inputPatternNum.currValue);
    songElements[songCurrPos].repeat = inputPatternRepeat.currValue - 1;
    PGSO_closeDialog();
    if(songCurrPos > songEnd) songEnd = songCurrPos;
  }
}

void PGSO_listenerCancel(char key){
  if(key == KEY_ENTER){
    PGSO_closeDialog();
  }
}

void PGSO_inputActionNoop(struct InputField *field, short enter){
}

void PGSO_inputActionPatternChanged(struct InputField *field, short enter){
  char folderNum;
  char fileNum;

  if(field->currValue > 99){
    folderNum = field->currValue / 100;
    fileNum = field->currValue - folderNum * 100;
    EEX_readFileName(folderNum-1, fileNum-1, name);
    SEQ_VIS_changePatternDialogUpdateName(name);
  }
}

void PGSO_playOrLoad(){
  int patternNum;
  char folderNum;
  char fileNum;

  if(sequencer.playState == PLAY_STOPPED && patternLoaded<1){
    patternNum = songElements[songCurrPos].pattern;
    if(patternNum != NOT_SELECTED){
      folderNum = UTIL_getFolderNum(patternNum);
      fileNum = UTIL_getFileNum(patternNum);
      EEX_readPattern(folderNum-1, fileNum-1, patt);

      //prepare next pattern
      PGSO_loadFirstNonEmptyPattern(songCurrPos+1);
      PGSO_updatePatternStatus();
      SEQ_refreshStepLeds();
    }
  } else {
    SEQ_togglePlayPause(PLAY_MODE_SONG);
    SEQ_VIS_writePlayMode();
    SEQ_VIS_writePlayState();
  }
}

void PGSO_stopAndReset(){
  SEQ_stopPlayback();
  songCurrRepeat=0;
  songCurrPos=0;
  songCursorPos=0;
  songFirstPatternOnPage=0;
  PGSO_showPage();
}

void PGSO_stopOrReset(){

  songCurrRepeat=0;
  if(sequencer.playState == PLAY_STOPPED){
    songCurrPos=0;
    songCursorPos=0;
    songFirstPatternOnPage=0;
    PGSO_showPage();
  } else {
    SEQ_stopPlayback();
    SEQ_VIS_writePlayState();
    SEQ_VIS_writePlayMode();
    SEQ_VIS_writeTime(1);
  }
}

void PGSO_closeDialog(){
  currDialog = DIALOG_NO_DIALOG;
  currentInputField = NO_INPUT_FOCUS;
  currMenuItem = DIALOG_NO_MENU_ITEM;
  PGSO_showPage();

}

//
// Dialog box event handler
//
void PGSO_toggleDialogItem(unsigned short item){

  if(currDialog == DIALOG_CHANGE_PATTERN){
    if(item >= sizeof(dialogItems) / sizeof(struct MenuItem *)) return;
    NAV_setCurrentDialogItem(dialogItems[item]);
  }
  currDialogItem = item;
}

//
// Send a pressed key event to the current key receiver
//
void PGSO_keyEventHandler(char key){
  if(currDialog == DIALOG_NO_DIALOG){
    switch(key){
    case KEY_UP:
      if(G_NAV_shiftOn){
        PGSO_togglePrevPage(1);
      } else {
        PGSO_togglePrevPatt();
      }
      break;
    case KEY_DOWN:
      if(G_NAV_shiftOn){
        PGSO_toggleNextPage();
      } else {
        PGSO_toggleNextPatt();
      }
      break;
    case KEY_PASTE:
      PGSO_insertPattern();
      break;
    case KEY_CUT:
      PGSO_deletePattern();
      break;
    case KEY_PLAY:
      if(G_NAV_shiftOn){
        G_PGSO_songLoop = 1;
        SEQ_VIS_writeLoop(G_PGSO_songLoop);
      } else {
        PGSO_playOrLoad();
      }
      break;
    case KEY_STOP:
      if(G_NAV_shiftOn){
        G_PGSO_songLoop = 0;
        SEQ_VIS_writeLoop(G_PGSO_songLoop);
      } else {
        PGSO_stopOrReset();
      }
      break;
    default:
      currKeyListener(key);
    }
  } else {
    switch(key){
    case KEY_UP:
      PGSO_toggleDialogItem(currDialogItem-1);
      break;
    case KEY_DOWN:
      PGSO_toggleDialogItem(currDialogItem+1);
      break;
    default:
      currKeyListener(key);
    }
  }
}