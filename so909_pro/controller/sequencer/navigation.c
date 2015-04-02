#include "navigation.h"
#include "navigation_private.h"
#include "input.h"
#include "page_performance.h"
#include "page_song.h"
#include "page_file.h"
#include "page_setup.h"
#include "page_flam_vel.h"
#include "page_pattern.h"
#include "sequencer.h"
#include "../keyboard/leds.h"
//TODO: move input functions out of this
#include "../GLCD/sequencer_visualisation.h"
#include "../shared/types.h"
#include "../shared/global.h"
#include "../shared/definitions.h"

unsigned short mode; //current sequencer mode, almost the same as page but excluding flam/velocity
unsigned short pageFocus;
unsigned short currPage;
unsigned short currDialog = DIALOG_NO_DIALOG;
void (*currKeyListener)(char);

// selected dialog box
unsigned short currDialogItem = 0;
struct MenuItem *currMenuItem;

void NAV_init(){
  currPage  = PAGE_NONE;
  G_NAV_shiftOn = 0;
  pageFocus = FOCUS_LEFT;
  mode = 0;
  NAV_updateMode();
}

void NAV_shiftOn(){
  G_NAV_shiftOn = 1;
}

void NAV_shiftOff(){
  G_NAV_shiftOn = 0;
}

void NAV_previousMode(){
  if(currDialog == DIALOG_NO_DIALOG){
    if(mode > 0){
      mode--;
    } else {
      mode = NUMBER_OF_MODES - 1;
    }
    NAV_updateMode();
  }
}

void NAV_nextMode(){
  if(currDialog == DIALOG_NO_DIALOG){
    if(mode < NUMBER_OF_MODES - 1){
      mode++;
    } else {
      mode = 0;
    }
    NAV_updateMode();
  }
}

void NAV_updateMode(){
  LED_updateModeLeds(mode);
  switch(mode){
    case PAGE_PATT:
      PGP_showPage(DONT_RESET_POSITIONS);
      break;
    case PAGE_SONG:
      PGSO_showPage();
      break;
    case PAGE_PERFORMANCE:
      PGPF_showPage();
      break;
    case PAGE_FILE:
      PGF_showPage(1);
      break;
    case PAGE_SETUP:
      PGS_showPage();
      break;
  }
}

//
// Toggle if keypresses should affect navigation on the left or right side
// of the screen.
//
void NAV_toggleFocus(){
  if(pageFocus == FOCUS_LEFT){
    pageFocus = FOCUS_RIGHT;
    if(currPage == PAGE_SETUP){
      PGS_toggleSetupParamFirst();
    } else if(currPage == PAGE_FILE){
      PGF_toggleFileFirst();
    } else if(currPage == PAGE_PERFORMANCE){
      PGPF_togglePerfKeyMappingInput();
    }
  } else {
    pageFocus = FOCUS_LEFT;
    if(currPage == PAGE_SETUP){
      IN_setCurrentInputField(NO_INPUT_FOCUS);
    } else if(currPage == PAGE_FILE){
      PGF_removeFileFocus();
    } else if(currPage == PAGE_PERFORMANCE){
      PGPF_toggleNextPatternInput();
    }
  }
}

//
// Set current menu item in a dialog box and move arrows indicating
// current item.
//
// To turn off focus, send a 0 pointer.
//
void NAV_setCurrentDialogItem(struct MenuItem *item){
  if(currMenuItem){
    SEQ_VIS_clearArrow(currMenuItem->row, currMenuItem->col);
    currKeyListener = 0;
  }
  if(item){
    currMenuItem = item;
    currKeyListener = item->keyListener;
    SEQ_VIS_writeArrow(item->row, item->col);
  }
}

void NAV_listenerNOOP(char key){}
