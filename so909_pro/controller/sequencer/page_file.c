#include "page_file.h"
#include "page_file_private.h"

#include <built_in.h>
#include "navigation.h"
#include "sequencer.h"
#include "input.h"
#include "../keyboard/multitap.h"
#include "../shared/key_consts.h"
#include "../shared/types.h"
#include "../shared/global.h"
#include "../shared/definitions.h"
#include "../GLCD/sequencer_visualisation.h"
#include "../storage/external_storage.h"

// file page variables
unsigned short currFolder = 0;
short currFile = 0;

// Dialog box menu items
struct MenuItem dialogChangeFolderRename;
struct MenuItem dialogChangeFolderCancel;
struct MenuItem *dialogChangeFolderItems[2];

struct MenuItem dialogChangeFileLoad;
struct MenuItem dialogChangeFileRename;
struct MenuItem dialogChangeFileOverwrite;
struct MenuItem dialogChangeFileDelete;
struct MenuItem dialogChangeFileCancel;
struct MenuItem *dialogChangeFileItems[5];

struct MenuItem dialogSaveFileSave;
struct MenuItem dialogSaveFileCancel;
struct MenuItem *dialogSaveFileItems[2];

struct MenuItem dialogNameNameentry;
struct MenuItem dialogNameOkRename;
struct MenuItem dialogNameOkSaveFile;
struct MenuItem dialogNameCancel;
struct MenuItem *dialogNameRenameItems[3];
struct MenuItem *dialogNameNewFileItems[3];

void PGF_initPage(){
  GLOBAL_resetName();
  PGF_initDialogMenuItems();
}

void PGF_initDialogMenuItems(){

  //
  // Dialog box for changing folder names
  //
  dialogChangeFolderRename.col = POS_DIALOG_ITEM_LEFT;
  dialogChangeFolderRename.row = POS_DIALOG_CHANGE_FOLDER_TOP+1;
  dialogChangeFolderRename.keyListener = &PGF_listenerShowRenameDialog;

  dialogChangeFolderCancel.col = POS_DIALOG_ITEM_LEFT;
  dialogChangeFolderCancel.row = POS_DIALOG_CHANGE_FOLDER_TOP+2;
  dialogChangeFolderCancel.keyListener = &PGF_listenerCancel;
  
  dialogChangeFolderItems[0] = &dialogChangeFolderRename;
  dialogChangeFolderItems[1] = &dialogChangeFolderCancel;

  //
  // Dialog box for changing an existing file
  //
  dialogChangeFileLoad.col = POS_DIALOG_ITEM_LEFT;
  dialogChangeFileLoad.row = POS_DIALOG_CHANGE_FILE_TOP+1;
  dialogChangeFileLoad.keyListener = &PGF_listenerLoadFile;

  dialogChangeFileRename.col = POS_DIALOG_ITEM_LEFT;
  dialogChangeFileRename.row = POS_DIALOG_CHANGE_FILE_TOP+2;
  dialogChangeFileRename.keyListener = &PGF_listenerShowRenameDialog;

  dialogChangeFileOverwrite.col = POS_DIALOG_ITEM_LEFT;
  dialogChangeFileOverwrite.row = POS_DIALOG_CHANGE_FILE_TOP+3;
  dialogChangeFileOverwrite.keyListener = &PGF_listenerSaveFile;

  dialogChangeFileDelete.col = POS_DIALOG_ITEM_LEFT;
  dialogChangeFileDelete.row = POS_DIALOG_CHANGE_FILE_TOP+4;
  dialogChangeFileDelete.keyListener = &PGF_listenerDeleteFile;

  dialogChangeFileCancel.col = POS_DIALOG_ITEM_LEFT;
  dialogChangeFileCancel.row = POS_DIALOG_CHANGE_FILE_TOP+5;
  dialogChangeFileCancel.keyListener = &PGF_listenerCancel;

  dialogChangeFileItems[0] = &dialogChangeFileLoad;
  dialogChangeFileItems[1] = &dialogChangeFileRename;
  dialogChangeFileItems[2] = &dialogChangeFileOverwrite;
  dialogChangeFileItems[3] = &dialogChangeFileDelete;
  dialogChangeFileItems[4] = &dialogChangeFileCancel;

  //
  // Dialog box for saving a new file
  //
  dialogSaveFileSave.col = POS_DIALOG_ITEM_LEFT;
  dialogSaveFileSave.row = POS_DIALOG_SAVE_FILE_TOP+1;
  dialogSaveFileSave.keyListener = &PGF_listenerShowNameNewFileDialog;

  dialogSaveFileCancel.col = POS_DIALOG_ITEM_LEFT;
  dialogSaveFileCancel.row = POS_DIALOG_SAVE_FILE_TOP+2;
  dialogSaveFileCancel.keyListener = &PGF_listenerCancel;

  dialogSaveFileItems[0] = &dialogSaveFileSave;
  dialogSaveFileItems[1] = &dialogSaveFileCancel;

  //
  // Dialog box for naming/renaming a file
  //
  dialogNameNameentry.col = POS_DIALOG_ITEM_LEFT;
  dialogNameNameentry.row = POS_DIALOG_NAME_FILE_TOP+3;
  dialogNameNameentry.keyListener = &PGF_listenerEnterCharacter;

  dialogNameOkRename.col = POS_DIALOG_ITEM_LEFT;
  dialogNameOkRename.row = POS_DIALOG_NAME_FILE_TOP+5;
  dialogNameOkRename.keyListener = &PGF_listenerNameFileOrFolder;

  dialogNameOkSaveFile.col = POS_DIALOG_ITEM_LEFT;
  dialogNameOkSaveFile.row = POS_DIALOG_NAME_FILE_TOP+5;
  dialogNameOkSaveFile.keyListener = &PGF_listenerSaveFile;

  dialogNameCancel.col = POS_DIALOG_ITEM_LEFT;
  dialogNameCancel.row = POS_DIALOG_NAME_FILE_TOP+6;
  dialogNameCancel.keyListener = &PGF_listenerCancel;

  dialogNameRenameItems[0] = &dialogNameNameentry;
  dialogNameRenameItems[1] = &dialogNameOkRename;
  dialogNameRenameItems[2] = &dialogNameCancel;

  dialogNameNewFileItems[0] = &dialogNameNameentry;
  dialogNameNewFileItems[1] = &dialogNameOkSaveFile;
  dialogNameNewFileItems[2] = &dialogNameCancel;

}

void PGF_showPage(char resetPosition){
  currPage = PAGE_FILE;
  nextPatternAction = &GLOBAL_nop;
  
  IN_clearCurrentInputField();
  SEQ_VIS_clearFullGrid();
  SEQ_VIS_writeMenu();
  SEQ_VIS_showPageFileFolders();
  
  if(resetPosition){
    currFolder = 0; //to reset arrow
    currFile = NO_FILE_FOCUS;
    pageFocus = FOCUS_LEFT;
  }

  PGF_toggleFolder(currFolder);
  
  if(resetPosition == 0 && pageFocus == FOCUS_RIGHT){
    PGF_toggleFile(currFile);
  }
}

void PGF_toggleFileFirst(){
  PGF_toggleFile(0);
}
//
// Toggle focus on the right hand of the file page. Highlights a given
// 'file' in the current folder.
//
void PGF_toggleFile(short fileNum){

  if(currFile != NO_FILE_FOCUS){
    SEQ_VIS_clearArrow(POS_FILE_PATTERN_1 + currFile, POS_FILE_RIGHT);
  }

  if(fileNum == -1) {
    currFile = MEMMAP_FILES_PER_FOLDER - 1;
  } else if(fileNum > MEMMAP_FILES_PER_FOLDER - 1){
    currFile = 0;
  } else {
    currFile = fileNum;
  }

  currKeyListener = &PGF_listenerShowFileDialog;
  SEQ_VIS_writeArrow(POS_FILE_PATTERN_1 + currFile, POS_FILE_RIGHT);
}

void PGF_removeFileFocus(){
  SEQ_VIS_clearArrow(POS_FILE_PATTERN_1 + currFile, POS_FILE_RIGHT);
  currFile = NO_FILE_FOCUS;
}

//
// Show setup fields for a certain item
//
void PGF_toggleFolder(unsigned short folderNum){

  // skip a line between pattern folders and song/performances
  if(currFolder > MEMMAP_PATTERN_FOLDERS-1){
    SEQ_VIS_clearItemFocus(POS_FILE_FOLDER_1 + currFolder + 1);
  } else {
    SEQ_VIS_clearItemFocus(POS_FILE_FOLDER_1 + currFolder);
  }

  // if folderNum is decremented below 0, it overflows. This leads to a large
  // positive number, something that lets us set the focus to the last file
  // in the list.
  if(folderNum == 255) {
    currFolder = MEMMAP_PATTERN_FOLDERS + 1;
  } else if(folderNum > MEMMAP_PATTERN_FOLDERS + 1) {
    currFolder = 0;
  } else {
    currFolder = folderNum;
  }

  switch(currFolder){
  case FILE_ITEM_SONG:
    SEQ_VIS_showPageFileSongs(currFolder);
    currKeyListener = &NAV_listenerNOOP;
    break;
  case FILE_ITEM_PERFORMANCE:
    SEQ_VIS_showPageFilePerformances(currFolder);
    currKeyListener = &NAV_listenerNOOP;
    break;
  default:
    SEQ_VIS_showPageFilePatterns(currFolder);
    currKeyListener = &PGF_listenerShowChangeFolderDialog;
    break;
  }

  // skip a line between pattern folders and song/performances
  if(currFolder > MEMMAP_PATTERN_FOLDERS-1){
    SEQ_VIS_writeItemFocus(POS_FILE_FOLDER_1 + currFolder + 1);
  } else {
    SEQ_VIS_writeItemFocus(POS_FILE_FOLDER_1 + currFolder);
  }
}

void PGF_listenerShowFileDialog(char key){
  if(key == KEY_ENTER){
    GLOBAL_resetName();
    EEX_readFileName(currFolder, currFile, name);
    if(name[0] == 0){
      PGF_showSaveFileDialog();
    } else {
      PGF_showChangeFileDialog();
    }
  }
}

void PGF_showChangeFileDialog(){
  currDialog = DIALOG_CHANGE_FILE;
  SEQ_VIS_changeFileDialog();
  PGF_toggleDialogItem(0);
}

void PGF_showSaveFileDialog(){
  currDialog = DIALOG_SAVE_FILE;
  SEQ_VIS_saveFileDialog();
  PGF_toggleDialogItem(0);
}


void PGF_listenerShowChangeFolderDialog(char key){
  if(key == KEY_ENTER){
    GLOBAL_resetName();
    EEX_readFolderName(currFolder, name);
    currDialog = DIALOG_CHANGE_FOLDER;
    SEQ_VIS_changeFolderDialog();
    PGF_toggleDialogItem(0);
  }
}

void PGF_listenerShowRenameDialog(char key){
  short cursorPos;
  if(key == KEY_ENTER){
    currDialog = DIALOG_RENAME;
    cursorPos = MT_initMultiTap(name);
    SEQ_VIS_nameFileDialog();
    SEQ_VIS_nameFileDialogUpdateName(name, cursorPos);
    PGF_toggleDialogItem(0);
  }
}

void PGF_listenerShowNameNewFileDialog(char key){
  short cursorPos;
  if(key == KEY_ENTER){
    currDialog = DIALOG_NAME_NEW_FILE;
    GLOBAL_resetName();
    cursorPos = MT_initMultiTap(name);
    SEQ_VIS_nameFileDialog();
    SEQ_VIS_nameFileDialogUpdateName(name, cursorPos);
    PGF_toggleDialogItem(0);
  }
}

void PGF_listenerEnterCharacter(char key){
  short cursorPos;
  if(key == KEY_CUT){
    cursorPos = MT_removeChar(name);
  } else if(key == KEY_LEFT){
    cursorPos = MT_moveCursorLeft();
  } else if(key == KEY_RIGHT){
    cursorPos = MT_moveCursorRight(name);
  } else if(key >=KEY_0 && key <= KEY_9){
    cursorPos = MT_addChar(key-KEY_0, name);
  }
  SEQ_VIS_nameFileDialogUpdateName(name, cursorPos);
}

void PGF_listenerLoadFile(char key){
  if(key == KEY_ENTER){
    switch(currFolder){
    case FILE_ITEM_SONG:
      EEX_readSong(currFile, songElements);
      break;
    case FILE_ITEM_PERFORMANCE:
      EEX_readPerformance(currFile, patternKeyMap);
      break;
    default:
      EEX_readPattern(currFolder, currFile, nextPatt);
      SEQ_switchToNextPattern();
    }
    PGF_closeDialog();
  }
}

void PGF_listenerNameFileOrFolder(char key){
  if(key == KEY_ENTER && name[0] != 0){
    if(pageFocus == FOCUS_LEFT){
      EEX_writeFolderName(currFolder, name);
    } else {
      EEX_writeFileName(currFolder, currFile, name);
    }
    PGF_closeDialog();
  }
}

void PGF_listenerSaveFile(char key){
  if(key == KEY_ENTER && name[0] != 0){
    switch(currFolder){
    case FILE_ITEM_SONG:
      EEX_writeFileName(currFolder, currFile, name);
      delay_ms(5); //necessary to prevent program from crashing
      EEX_writeSong(currFile, songElements);
      break;
    case FILE_ITEM_PERFORMANCE:
      EEX_writeFileName(currFolder, currFile, name);
      delay_ms(5); //necessary to prevent program from crashing
      EEX_writePerformance(currFile, patternKeyMap);
      break;
    default:
      EEX_writeFileName(currFolder, currFile, name);
      delay_ms(5); //necessary to prevent program from crashing
      EEX_writePattern(currFolder, currFile, patt);
    }
    PGF_closeDialog();
  }
}

void PGF_listenerDeleteFile(char key){
  name[0] = 0;
  if(key == KEY_ENTER){
    EEX_writeFileName(currFolder, currFile, name[0]);
    PGF_closeDialog();
  }
}

void PGF_listenerCancel(char key){
  if(key == KEY_ENTER){
    PGF_closeDialog();
  }
}

void PGF_closeDialog(){
  currDialog = DIALOG_NO_DIALOG;
  currMenuItem = DIALOG_NO_MENU_ITEM;
  PGF_showPage(0);
}

//
// Dialog box event handler
//
void PGF_toggleDialogItem(unsigned short item){

  if(currDialog == DIALOG_CHANGE_FOLDER){
    if(item >= sizeof(dialogChangeFolderItems) / sizeof(struct MenuItem *)) return;
    NAV_setCurrentDialogItem(dialogChangeFolderItems[item]);
  } else if(currDialog == DIALOG_CHANGE_FILE){
    if(item >= sizeof(dialogChangeFileItems) / sizeof(struct MenuItem *)) return;
    NAV_setCurrentDialogItem(dialogChangeFileItems[item]);
  } else if(currDialog == DIALOG_SAVE_FILE){
    if(item >= sizeof(dialogSaveFileItems) / sizeof(struct MenuItem *)) return;
    NAV_setCurrentDialogItem(dialogSaveFileItems[item]);
  } else if(currDialog == DIALOG_NAME_NEW_FILE){
    if(item >= sizeof(dialogNameNewFileItems) / sizeof(struct MenuItem *)) return;
    if(item==0){
      SEQ_VIS_showCursor();
    } else {
      SEQ_VIS_hideCursor();
    }
    NAV_setCurrentDialogItem(dialogNameNewFileItems[item]);
  } else if(currDialog == DIALOG_RENAME){
    if(item >= sizeof(dialogNameRenameItems) / sizeof(struct MenuItem *)) return;
    if(item==0){
      SEQ_VIS_showCursor();
    } else {
      SEQ_VIS_hideCursor();
    }
    NAV_setCurrentDialogItem(dialogNameRenameItems[item]);
  }
  currDialogItem = item;
}

//
// Send a pressed key event to the current key receiver
//
void PGF_keyEventHandler(char key){
  if(currDialog == DIALOG_NO_DIALOG){
    switch(key){
    case KEY_UP:
      if(pageFocus == FOCUS_LEFT){
        PGF_toggleFolder(currFolder-1);
      } else {
        PGF_toggleFile(currFile-1);
      }
      break;
    case KEY_DOWN:
      if(pageFocus == FOCUS_LEFT){
        PGF_toggleFolder(currFolder+1);
      } else {
        PGF_toggleFile(currFile+1);
      }
      break;
    case KEY_LEFT:
    case KEY_RIGHT:
      NAV_toggleFocus();
      break;
    default:
      currKeyListener(key);
    }
  } else {
    switch(key){
    case KEY_UP:
      PGF_toggleDialogItem(currDialogItem-1);
      break;
    case KEY_DOWN:
      PGF_toggleDialogItem(currDialogItem+1);
      break;
    default:
      currKeyListener(key);
    }
  }
}