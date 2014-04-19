#include "multitap.h"
#include "multitap_private.h"
#include "../sequencer/sequencer.h"
#include "../sequencer/navigation.h"
#include "../sequencer/page_file.h"
#include "../shared/types.h"
#include "../shared/global.h"
#include "../shared/definitions.h"

short lastKey;
char tapNum;
short tapCursorPos;

short MT_initMultiTap(char *currName){
  lastKey = -1;
  tapCursorPos = -1;

  while(currName[tapCursorPos+1] != 0){
    tapCursorPos++;
  }

  return tapCursorPos+1;

}

//
// Add a new character - executed everytime a key is pressed. Places a character
// at the correct place in 'string' and increments the position if apppropriate.
//
short MT_addChar(char key, char *currName){
  char character;

  if(key == lastKey){
    tapNum++;
  } else {
    MT_setPositionNext();
  }

  character = MT_getChar(key);
  currName[tapCursorPos] = character;

  lastkey = key;
  
  return tapCursorPos;
}

//
// Increment the cursor position (if possible) and reset multi tap parameters.
// Should be done whenever a different key than the previous one is pressed or
// if multitap 'times out'.
//
void MT_setPositionNext(){
  if(tapCursorPos < NAME_LENGTH-1){
    tapCursorPos++;
  }
  lastKey = -1;
  tapNum = 0;
}

short MT_moveCursorLeft(){
  if(lastKey != -1 && tapCursorPos > 0){
    tapCursorPos--;
  }
  if(tapCursorPos>-1){
    tapCursorPos--;
    lastKey = -1;
    tapNum = 0;
  }
  return tapCursorPos+1;
}

short MT_moveCursorRight(char *currName){
  if(tapCursorPos < NAME_LENGTH-1){
  
    if(currName[tapCursorPos] != 0){
      if(lastKey != -1){
        lastKey=-1;
      } else if(currName[tapCursorPos+1] != 0){
        MT_setPositionNext();
      }
      return tapCursorPos+1;
    } else {
      return tapCursorPos;
    }
  } else {
    return NAME_LENGTH-1;
  }

}

short MT_removeChar(char *currName){

  if(tapCursorPos > -1){
    currName[tapCursorPos] = 0;
    tapCursorPos--;
    lastKey = -1;
    tapNum = 0;
  }
  
  return tapCursorPos+1;
}

//
// Get a character based on what numeric key has been pressed and how many
// times it has been pressed.
//
// Multi tap characters are mapped to the correct position but on the wrong
// numbers, as the numpad on a keyboard does not match the numpad on a cellphone
//
char MT_getChar(char key){
  char clickIndex;
  char offset = 32;
  
  if(G_NAV_shiftOn){
    offset = 0;
  }
  
  switch(key){
  case 0:
    clickIndex = tapNum % 5;
    switch(clickIndex){
      case 0:
        return ' ';
      case 1:
        return '-';
      case 2:
        return '&';
      case 3:
        return '!';
      case 4:
        return '0';
    }
  case 1:
    clickIndex = tapNum % 5;
    switch(clickIndex){
      case 0:
        return '/';
      case 1:
        return '@';
      case 2:
        return '.';
      case 3:
        return ',';
      case 4:
        return '1';
    }
  case 2:
    clickIndex = tapNum % 4;
    switch(clickIndex){
      case 0:
        return 'A' + offset;
      case 1:
        return 'B' + offset;
      case 2:
        return 'C' + offset;
      case 3:
        return '2';
    }
  case 3:
    clickIndex = tapNum % 4;
    switch(clickIndex){
      case 0:
        return 'D' + offset;
      case 1:
        return 'E' + offset;
      case 2:
        return 'F' + offset;
      case 3:
        return '3';
    }
  case 4:
    clickIndex = tapNum % 4;
    switch(clickIndex){
      case 0:
        return 'G' + offset;
      case 1:
        return 'H' + offset;
      case 2:
        return 'I' + offset;
      case 3:
        return '4';
    }
  case 5:
    clickIndex = tapNum % 4;
    switch(clickIndex){
      case 0:
        return 'J' + offset;
      case 1:
        return 'K' + offset;
      case 2:
        return 'L' + offset;
      case 3:
        return '5';
    }
  case 6:
    clickIndex = tapNum % 4;
    switch(clickIndex){
      case 0:
        return 'M' + offset;
      case 1:
        return 'N' + offset;
      case 2:
        return 'O' + offset;
      case 3:
        return '6';
    }
  case 7:
    clickIndex = tapNum % 5;
    switch(clickIndex){
      case 0:
        return 'P' + offset;
      case 1:
        return 'Q' + offset;
      case 2:
        return 'R' + offset;
      case 3:
        return 'S' + offset;
      case 4:
        return '7';
    }
  case 8:
    clickIndex = tapNum % 4;
    switch(clickIndex){
      case 0:
        return 'T' + offset;
      case 1:
        return 'U' + offset;
      case 2:
        return 'V' + offset;
      case 3:
        return '8';
    }
  case 9:
    clickIndex = tapNum % 5;
    switch(clickIndex){
      case 0:
        return 'W' + offset;
      case 1:
        return 'X' + offset;
      case 2:
        return 'Y' + offset;
      case 3:
        return 'Z' + offset;
      case 4:
        return '9';
    }
  }
}