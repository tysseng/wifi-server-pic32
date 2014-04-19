#include "command_handler.h"

#include "controller.h"
#include "sequencer/navigation.h"
#include "sequencer/page_file.h"
#include "sequencer/page_song.h"
#include "sequencer/page_setup.h"
#include "sequencer/page_flam_vel.h"
#include "sequencer/page_pattern.h"
#include "sequencer/page_performance.h"

#include "shared/types.h"
#include "shared/global.h"
#include "shared/key_consts.h"
#include "shared/dial_consts.h"

char CMD_keyToRepeat;

void CMD_init(){
  G_CMD_repeatLastKey = 0;
}

void CMD_dispatchKeyDown(char key){
  if(currPage == PAGE_BARS){
    PGFV_keyEventHandler(key);
  } else if(currPage == PAGE_SETUP){
    PGS_keyEventHandler(key);
  } else if(currPage == PAGE_FILE){
    PGF_keyEventHandler(key);
  } else if(currPage == PAGE_PATT){
    PGP_keyEventHandler(key);
  } else if(currPage == PAGE_PERFORMANCE){
    PGPF_keyEventHandler(key);
  } else if(currPage == PAGE_SONG){
    PGSO_keyEventHandler(key);
  }
}

void CMD_keyEventDispatcher(char akey, unsigned short keydirection){

  //workaround for non-functioning left button
  char key;

  if(akey == KEY_SEQ01){
    key = KEY_LEFT;
  } else {
    key = akey;
  }

  // init/disable automatic repetition of arrow keys
  if(key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT){
    if(keydirection == KEYDIR_UP){
      MCU_stopKeyRepeatTimer();
    } else {
      CMD_keyToRepeat = key;
      MCU_startKeyRepeatTimer();
    }
  }

  // handle shift pressed/released
  if(key == KEY_SHIFT){
    if(keydirection == KEYDIR_UP){
      NAV_shiftOff();
    } else {
      NAV_shiftOn();
    }
  }

  if(keydirection == KEYDIR_DOWN){
    // block playback keys in external synch mode
    if(G_MIDI_isExternalSynchOn){
      if(key == KEY_PLAY || key == KEY_STOP){
        return;
      }
    }
    CMD_dispatchKeyDown(key);
  }
}

void CMD_dialEventDispatcher(char key){
  switch(key){
    case DIAL_MODE_DOWN :
      NAV_previousMode();
      return;
    case DIAL_MODE_UP :
      NAV_nextMode();
      return;
  }
  
  if(currPage == PAGE_PATT){
    PGP_dialEventHandler(key);
  }
}

void CMD_repeatLastKey(){
  G_CMD_repeatLastKey = 0;
  CMD_dispatchKeyDown(CMD_keyToRepeat);
}