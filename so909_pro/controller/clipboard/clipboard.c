#include "clipboard.h"
#include "../GLCD/GLCD.h"
#include "../GLCD/sequencer_visualisation.h"
#include "../sequencer/navigation.h"
#include "../sequencer/sequencer.h"
#include "../shared/types.h"
#include "../shared/global.h"


// Area might be 'inverted' (start after end) while CB_drawArea is
// ready for drawing (reduces complexity of writeStep()).
// CB_currDrawArea is the currently drawn CB_area, used since we need to know what
// steps to remove highlighting from.
struct Area CB_area;
struct Area CB_drawArea;
struct Area CB_currDrawArea;

//clipboard data and CB_area information
struct Area clipArea;

// REMOVED UNTILL MEMORY CAN TAKE ALL OF CLIPBOARD
//unsigned short clipDataVelocity[MAX_TRACKS][MAX_STEPS];
//short clipDataFlam[MAX_TRACKS][MAX_STEPS];

unsigned short clipDataVelocity[1][MAX_STEPS];
short clipDataFlam[1][MAX_STEPS];

  
short CB_cursorx;
short CB_cursory;

/*******************************************************************************
  FUNCTIONS
*******************************************************************************/

void CB_init(){
  G_CB_cursorOn = 0;
  CB_cursorx = 0;
  CB_cursory = 0;
  CB_area.x1=0;
  CB_area.x2=0;
  CB_area.y1=0;
  CB_area.y2=0;
  CB_drawArea.x1=0;
  CB_drawArea.x2=0;
  CB_drawArea.y1=0;
  CB_drawArea.y2=0;
  CB_currDrawArea.x1=0;
  CB_currDrawArea.x2=0;
  CB_currDrawArea.y1=0;
  CB_currDrawArea.y2=0;
  G_CB_areaOn = 0;

}

void CB_resetCursor(){
  G_CB_cursorOn = 0;
  CB_cursorx = 0;
  CB_cursory = 0;
  CB_resetArea();
}

void CB_cursorSet(unsigned short x, unsigned short y){
  CB_cursorx = x;
  CB_cursory = y;
  CB_areaStart(CB_cursorx, CB_cursory);
}

void CB_cursorLeft(){
  if(G_CB_cursorOn == 0){
    CB_areaStart(CB_cursorx, CB_cursory);
  } else {
    if(CB_cursorx>0) CB_cursorx--;
    if(G_NAV_shiftOn){
      CB_areaEnd(CB_cursorx, CB_cursory);
    } else {
      CB_areaStart(CB_cursorx, CB_cursory);
    }
  }
}

void CB_cursorRight(){
  if(G_CB_cursorOn == 0){
    CB_areaStart(CB_cursorx, CB_cursory);
  } else {
    if(CB_cursorx<patt->steps-1) CB_cursorx++;
    if(G_NAV_shiftOn){
      CB_areaEnd(CB_cursorx, CB_cursory);
    } else {
      CB_areaStart(CB_cursorx, CB_cursory);
    }
  }
}

void CB_cursorUp(){
  if(G_CB_cursorOn == 0){
    CB_areaStart(CB_cursorx, CB_cursory);
  } else {
    if(CB_cursory>0) CB_cursory--;
    if(G_NAV_shiftOn){
      CB_moveSelectionToTrack(CB_cursory);
      
      //REMOVED UNTILL MEMORY CAN TAKE ALL OF CLIPBOARD
      //CB_areaEnd(CB_cursorx, CB_cursory);
    } else {
      CB_areaStart(CB_cursorx, CB_cursory);
    }
  }
}

void CB_cursorDown(){
  if(G_CB_cursorOn == 0){
    CB_areaStart(CB_cursorx, CB_cursory);
  } else {
    if(CB_cursory<patt->trackcount-1) CB_cursory++;
    if(G_NAV_shiftOn){
      CB_moveSelectionToTrack(CB_cursory);

      //REMOVED UNTILL MEMORY CAN TAKE ALL OF CLIPBOARD
      //CB_areaEnd(CB_cursorx, CB_cursory);
    } else {
      CB_areaStart(CB_cursorx, CB_cursory);
    }
  }
}

// ADDED UNTILL MEMORY CAN TAKE ALL OF CLIPBOARD
// method used while clipboard does not support more than one track at the time
void CB_moveSelectionToTrack(char track){
  CB_area.y1 = track;
  CB_area.y2 = track+1;
  CB_drawArea.y1 = track;
  CB_drawArea.y2 = track+1;

  SEQ_VIS_updateHighlitArea();
}

/**
 * Copy/paste: clear current highlighted CB_area.
 **/
void CB_resetArea(){
  CB_area.x1=0;
  CB_area.x2=0;
  CB_area.y1=0;
  CB_area.y2=0;
  G_CB_areaOn = 0;

  CB_drawArea = CB_area;

  SEQ_VIS_updateHighlitArea();

}

void CB_resetCurrDrawArea(){
  CB_currDrawArea.x1 = 0;
  CB_currDrawArea.x2 = 0;
  CB_currDrawArea.y1 = 0;
  CB_currDrawArea.y2 = 0;
}

/**
 * Copy/paste: set highlight start, clears previous highlighting
 **/
void CB_areaStart(unsigned short x, unsigned short y){

  G_CB_cursorOn = 1;
  G_CB_areaOn = 0;
  CB_area.x1 = x;
  CB_area.y1 = y;
  CB_area.x2 = x+1;
  CB_area.y2 = y+1;

  CB_drawArea = CB_area;
  SEQ_VIS_updateHighlitArea();

}

/**
 * Copy/paste: calculate new CB_area size
 * x and y coordinates of ending cell. Function is inclusive, end cell is
 * part of selected CB_area.
 *
 * FIX: DO NOT SWAP START AND END if end < start. Do this in drawing routine
 *      instead!
 **/
void CB_areaEnd(unsigned short x, unsigned short y){

  //used for minimal updating of display
  CB_area.x2 = x+1;
  CB_area.y2 = y+1;
  G_CB_areaOn = 1;
  
  if( CB_area.x2 <= CB_area.x1 ) {
    CB_drawArea.x1 = CB_area.x2-1;
    CB_drawArea.x2 = CB_area.x1+1;
  } else {
    CB_drawArea.x1 = CB_area.x1;
    CB_drawArea.x2 = CB_area.x2;
  }

  if( y < CB_area.y1 ) {
    CB_drawArea.y1 = CB_area.y2-1;
    CB_drawArea.y2 = CB_area.y1+1;
  } else {
    CB_drawArea.y1 = CB_area.y1;
    CB_drawArea.y2 = CB_area.y2;
  }
  
  SEQ_VIS_updateHighlitArea();

}



//
// Copy/paste: copy highlighted CB_area to clipboard
//
void CB_copy(unsigned short clear){

  unsigned short track,step;

  for( track=CB_area.y1; track<CB_area.y2; track++) {
    for( step=CB_area.x1; step<CB_area.x2; step++ ) {
      // REMOVED UNTILL MEMORY CAN TAKE ALL OF CLIPBOARD
      //clipDataVelocity[track][step] = patt->tracks[track].steps[step];
      //clipDataFlam[track][step] = patt->tracks[track].flams[step];
      clipDataVelocity[0][step] = patt->tracks[track].steps[step];
      clipDataFlam[0][step] = patt->tracks[track].flams[step];
      
      if(clear){
        SEQ_stepOff(track, step);
      }
    }
  }
  clipArea = CB_area;

  G_CB_cursorOn = 0;
  CB_cursorx=CB_area.x1;
  CB_cursory=CB_area.y1;
  CB_resetArea();
  
}


//
// Copy/paste: paste highlighted CB_area from clipboard to given position and
//             remove highlighting
//
// If swap = 1, the contents of the cell pasted into will be inserted into the
// corresponding clipboard position.
//
void CB_paste(unsigned short swap){
  unsigned short track,step;
  unsigned short cbt,cbs; //clipboard track, clipboard step
  unsigned short swapValueVel;
  unsigned short swapValueFlam;

  // remove highlighting
  CB_resetArea();

  // paste data into pattern
  track = CB_cursory;
  for( cbt=clipArea.y1; cbt<clipArea.y2; cbt++) {
    if(track >= patt->trackcount){
      break;
    }
    step = CB_cursorx;
    for( cbs=clipArea.x1; cbs<clipArea.x2; cbs++ ) {
      if(step>=patt->steps){
        break;
      }

      swapValueVel = patt->tracks[track].steps[step];
      swapValueFlam = patt->tracks[track].steps[step];
      
      // REMOVED UNTILL MEMORY CAN TAKE ALL OF CLIPBOARD
      /*
      if(clipDataVelocity[cbt][cbs]==0){
        SEQ_stepOff(track, step);
      } else {
        SEQ_stepOn(track, step, clipDataVelocity[cbt][cbs]);
        SEQ_flamOn(track, step, clipDataFlam[cbt][cbs]);
      }

      if(swap){
        clipDataVelocity[cbt][cbs] = swapValueVel;
        clipDataFlam[cbt][cbs] = swapValueFlam;
      }
      */

      if(clipDataVelocity[0][cbs]==0){
        SEQ_stepOff(track, step);
      } else {
        SEQ_stepOn(track, step, clipDataVelocity[0][cbs]);
        SEQ_flamOn(track, step, clipDataFlam[0][cbs]);
      }

      if(swap){
        clipDataVelocity[0][cbs] = swapValueVel;
        clipDataFlam[0][cbs] = swapValueFlam;
      }

      step++;
    }
    track++;
  }

  // move cursor to after pasted area
  CB_cursorx += (clipArea.x2 - clipArea.x1);
  if(CB_cursorx > patt->steps -1){
    CB_cursorx = patt->steps-1;
  }
}