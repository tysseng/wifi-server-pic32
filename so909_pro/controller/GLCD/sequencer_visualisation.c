#include "sequencer_visualisation.h"
#include "GLCD.h"
#include "../../shared/char_addrs.h"

#include "../sequencer/navigation.h"
#include "../sequencer/sequencer.h"
#include "../sequencer/page_file.h"
#include "../shared/definitions.h"
#include "../clipboard/clipboard.h"
#include "../storage/external_storage.h"
#include "../shared/types.h"
#include "../shared/global.h"

/**
 * Higher level visualisation functions used for drawing sequencer details
 **/

void SEQ_VIS_showPageSong(){
  char i;
  
  SEQ_VIS_writePattDataRow();
  SEQ_VIS_writePlayMode();
  SEQ_VIS_writeTime(1);
  SEQ_VIS_writePlayState();
  SEQ_VIS_writeBPM();
  SEQ_VIS_writeSwing();
  SEQ_VIS_writeMenu();
  
  GLCD_CTRL_PrtAdr(POS_SONG_TOP-1, 0);
  SEQ_VIS_drawLineOfChars(setupLineTopDotted, 40);

  for(i=0; i<12; i++){
    GLCD_CTRL_PrtAdr(POS_SONG_TOP+i, POS_SONG_NAME_END);
    SEQ_VIS_drawLineOfChars(0x00, 14);
  }
}

void SEQ_VIS_writeSongPattern(char line, char patternPos, char repeat, char *name){
  GLCD_CTRL_PrtAdr(POS_SONG_TOP+line, 0);
  GLCD_CTRL_PrtCharInc(0x00);
  GLCD_CTRL_LL_writeNum3(patternPos);
  GLCD_CTRL_PrtTxt(": ");
  GLCD_CTRL_PrtCharInc(0x10 + repeat+1);
  GLCD_CTRL_PrtTxt(" x ");
  SEQ_VIS_writeName(name);
}

void SEQ_VIS_writeEmptyPattern(char line){
  GLCD_CTRL_PrtAdr(POS_SONG_TOP+line, 0);
  SEQ_VIS_drawLineOfChars(0x00, 40);
}

void SEQ_VIS_showPagePerformance(){

  char i;

  SEQ_VIS_writePattDataRow();
  SEQ_VIS_writePlayMode();
  SEQ_VIS_writeTime(1);
  SEQ_VIS_writePlayState();
  SEQ_VIS_writeBPM();
  SEQ_VIS_writeSwing();
  SEQ_VIS_writeMenu();

  GLCD_CTRL_PrtAdr(POS_PERF_PAGE_TOP-1, 0);
  SEQ_VIS_drawLineOfChars(setupLineTopDotted, POS_PERF_LEFT_COL_WIDTH);
  GLCD_CTRL_PrtCharInc(setupLineTopTDotted);
  SEQ_VIS_drawLineOfChars(setupLineTopDotted, POS_PERF_RIGHT_COL_WIDTH);
  
  GLCD_CTRL_PrtAdr(POS_PERF_NEXT_PATT_TOP, 0); //OK
  GLCD_CTRL_PrtTxt("Next pattern");
  SEQ_VIS_drawLineOfChars(0x00, POS_PERF_LEFT_COL_WIDTH - 12);
  GLCD_CTRL_PrtCharInc(setupLine);
  GLCD_CTRL_PrtTxt("Pattern key map");
  SEQ_VIS_drawLineOfChars(0x00, POS_PERF_RIGHT_COL_WIDTH - 15);

  //wraps to next line
  for(i=0; i<5; i++){
    SEQ_VIS_drawLineOfChars(0x00, POS_PERF_LEFT_COL_WIDTH);
    GLCD_CTRL_PrtCharInc(setupLine);
    SEQ_VIS_drawLineOfChars(0x00, POS_PERF_RIGHT_COL_WIDTH);
  }
  
  SEQ_VIS_drawLineOfChars(setupLineTopDotted, POS_PERF_LEFT_COL_WIDTH);
  GLCD_CTRL_PrtCharInc(setupLine);
  SEQ_VIS_drawLineOfChars(0x00, POS_PERF_RIGHT_COL_WIDTH);
  
  GLCD_CTRL_PrtAdr(POS_PERF_CURR_PATT_TOP, 0); //OK
  GLCD_CTRL_PrtTxt("Current pattern");
  SEQ_VIS_drawLineOfChars(0x00, POS_PERF_LEFT_COL_WIDTH - 15);
  GLCD_CTRL_PrtCharInc(setupLine);
  SEQ_VIS_drawLineOfChars(0x00, POS_PERF_RIGHT_COL_WIDTH);

  //wraps to next line
  for(i=0; i<5; i++){
    SEQ_VIS_drawLineOfChars(0x00, POS_PERF_LEFT_COL_WIDTH);
    GLCD_CTRL_PrtCharInc(setupLine);
    SEQ_VIS_drawLineOfChars(0x00, POS_PERF_RIGHT_COL_WIDTH);
  }

  SEQ_VIS_updateCurrPatternSetup();
  SEQ_VIS_updateNextPatternSetup();
  
  /*
  | Current pattern   | Next pattern       |
  |                   |                    |
  | 225               |  101               |
  | My fine pattern   |  Some other patt   |
  |                   |                    |
  | Tracks.......: 11 |  Tracks.......: 11 |
  | Steps........: 32 |  Steps........: 11 |
  | Resolution...:  4 |  Resolution...:  4 |
  | Beats per bar:  4 |  Beats per bar:  4 |
  | Swing balance: 50 |  Swing balance: 50 |
  */

  /*
  | Next pattern      | Pattern key map    |
  |                   |                    |
  | 225 - loaded      |  1 <empty>         |
  | My fine pattern   |  2 Some other patt |
  | 24 steps          |  3 <empty>         |
  |-------------------|  4 <empty>         |
  | Current pattern   |  6 <empty>         |
  |                   |  7 <empty>         |
  | 101               |  8 <empty>         |
  | Some other patt   |  9 <empty>         |
  | 32 steps          | 10 <empty>         |
  |                   |  5 <empty>         |
  */

}

void SEQ_VIS_writeKeyMapPattern(char line, char patternPos, char *name){

  GLCD_CTRL_PrtAdr(POS_PERF_KEY_MAP_TOP+line, POS_PERF_RIGHT_COL+1); //OK
  GLCD_CTRL_LL_writeNum2(patternPos);
  GLCD_CTRL_PrtCharInc(0x00);
  SEQ_VIS_writeName(name);
}

void SEQ_VIS_writeEmptyKeyMapPattern(char line){
  GLCD_CTRL_PrtAdr(POS_PERF_KEY_MAP_TOP+line, POS_PERF_RIGHT_COL+1); //OK
  SEQ_VIS_drawLineOfChars(0x00, POS_PERF_RIGHT_COL_WIDTH);
}

void SEQ_VIS_updateCurrPatternSetup(){
  if(patt->folderNum != NOT_SELECTED && patt->fileNum != NOT_SELECTED){
    SEQ_VIS_writePatternSteps(patt, POS_PERF_CURR_PATT_TOP+4, POS_PERF_LEFT_COL);
  } else {
    SEQ_VIS_clearPatternSteps(POS_PERF_CURR_PATT_TOP+4, POS_PERF_LEFT_COL);
  }
}

void SEQ_VIS_updateNextPatternSetup(){
  if(nextPatt->folderNum != NOT_SELECTED && nextPatt->fileNum != NOT_SELECTED){
    SEQ_VIS_writePatternSteps(nextPatt, POS_PERF_NEXT_PATT_TOP+4, POS_PERF_LEFT_COL);
  } else {
    SEQ_VIS_clearPatternSteps(POS_PERF_NEXT_PATT_TOP+4, POS_PERF_LEFT_COL);
  }
}

/**
 * remove arrows/focus for items in the right column of the performance page
 **/
void SEQ_VIS_clearKeyMapItemFocus(char currRow){
  GLCD_CTRL_PrtAdr(POS_PERF_KEY_MAP_TOP+currRow,POS_PERF_RIGHT_COL); //OK
  GLCD_CTRL_PrtCharInc( 0x00 );
}

/**
 * write arrows/focus for items in the right column of the performance page
 **/
void SEQ_VIS_writeKeyMapItemFocus(char currRow){
  GLCD_CTRL_PrtAdr(POS_PERF_KEY_MAP_TOP+currRow,POS_PERF_RIGHT_COL); //OK
  GLCD_CTRL_PrtCharInc( rightArrow );
}

void SEQ_VIS_writePatternSteps(struct Pattern *patt, char row, char col){
  GLCD_CTRL_PrtAdr(row, col);
  GLCD_CTRL_LL_writeNum2(patt->steps);
  GLCD_CTRL_PrtTxt(" steps");
}

void SEQ_VIS_clearPatternSteps(char row, char col){
  GLCD_CTRL_PrtAdr(row, col);
  SEQ_VIS_drawLineOfChars(0x00, 8);
}

void SEQ_VIS_writeCurrPerformancePattern(int patternNum){
  GLCD_CTRL_PrtAdr(POS_PERF_CURR_PATT_TOP+2, POS_PERF_LEFT_COL);
  GLCD_CTRL_LL_writeNum3(patternNum);
}

void SEQ_VIS_writeCurrPatternName(char* name){
  GLCD_CTRL_PrtAdr(POS_PERF_CURR_PATT_TOP+3, POS_PERF_LEFT_COL);
  SEQ_VIS_writeName(name);
}

void SEQ_VIS_writeNextPatternName(char* name){
  GLCD_CTRL_PrtAdr(POS_PERF_NEXT_PATT_TOP+3, POS_PERF_LEFT_COL);
  SEQ_VIS_writeName(name);
}

//
// Toggle page; show step sequencer
//
void SEQ_VIS_showPagePatt(){
  SEQ_VIS_writeFullGrid();
  SEQ_VIS_writeAllStates();
  SEQ_VIS_writeGridHeader();
  SEQ_VIS_writeTrackNames();
  SEQ_VIS_writePattDataRow();
  SEQ_VIS_writePlayMode();
  SEQ_VIS_writeTime(1);
  SEQ_VIS_writePlayState();
  SEQ_VIS_writeBPM();
  SEQ_VIS_writeSwing();
  SEQ_VIS_writeMenu();
}

//
// Toggle page; show velocity bar graph
//
void SEQ_VIS_showPageBars(){

  SEQ_VIS_clearFullGrid();
  SEQ_VIS_writeSingleGrid(0, sequencer.currTrack);
  SEQ_VIS_writeVelocityBars();
  SEQ_VIS_writeFlams();

  SEQ_VIS_drawHorizontalSetupLine(POS_VELOCITY_BARS+4);

  SEQ_VIS_drawHorizontalSetupLine(POS_VELOCITY_BARS+6);
  GLCD_CTRL_PrtAdr(POS_VELOCITY_BARS+7,POS_LEFT+2);
  GLCD_CTRL_PrtTxt("Velocity:");
  GLCD_CTRL_PrtAdr(POS_VELOCITY_BARS+8,POS_LEFT+2);
  GLCD_CTRL_PrtTxt("Flam....:");
}

void SEQ_VIS_showPageFileFolders(){
  char i;

  GLOBAL_resetName();
  GLCD_CTRL_PrtAdr(2,0);
  GLCD_CTRL_PrtTxt("Folders            "); GLCD_CTRL_PrtCharInc(setupLine);
  GLCD_CTRL_PrtAdr(3,0);
  GLCD_CTRL_PrtTxt("                   "); GLCD_CTRL_PrtCharInc(setupLine);

  for(i=0; i<8; i++){
    EEX_readFolderName(i, name);
    SEQ_VIS_writeFolderName(i, name);
  }

  GLCD_CTRL_PrtAdr(12,0);
  GLCD_CTRL_PrtTxt("                   "); GLCD_CTRL_PrtCharInc(setupLine);
  GLCD_CTRL_PrtAdr(13,0);
  GLCD_CTRL_PrtTxt(" Songs             "); GLCD_CTRL_PrtCharInc(setupLine);
  GLCD_CTRL_PrtAdr(14,0);
  GLCD_CTRL_PrtTxt(" Performances      "); GLCD_CTRL_PrtCharInc(setupLine);
  GLCD_CTRL_PrtAdr(15,0);
  GLCD_CTRL_PrtTxt("                   "); GLCD_CTRL_PrtCharInc(setupLine);
}


void SEQ_VIS_showPageFilePatterns(char folderNum){
  GLCD_CTRL_PrtAdr(2,POS_FILE_RIGHT);
  GLCD_CTRL_PrtTxt("Patterns            ");
  GLCD_CTRL_PrtAdr(3,POS_FILE_RIGHT);
  GLCD_CTRL_PrtTxt("                    ");
  SEQ_VIS_writeFileNames(folderNum);
}

void SEQ_VIS_showPageFileSongs(char folderNum){
  GLCD_CTRL_PrtAdr(2,POS_FILE_RIGHT);
  GLCD_CTRL_PrtTxt("Songs               ");
  GLCD_CTRL_PrtAdr(3,POS_FILE_RIGHT);
  GLCD_CTRL_PrtTxt("                    ");
  SEQ_VIS_writeFileNames(folderNum);
}

void SEQ_VIS_showPageFilePerformances(char folderNum){
  GLCD_CTRL_PrtAdr(2,POS_FILE_RIGHT);
  GLCD_CTRL_PrtTxt("Performances        ");
  GLCD_CTRL_PrtAdr(3,POS_FILE_RIGHT);
  GLCD_CTRL_PrtTxt("                    ");
  SEQ_VIS_writeFileNames(folderNum);
}

void SEQ_VIS_writeFileNames(char folderNum){
  char i;

  GLOBAL_resetName();
  for(i=0; i<12; i++){
    EEX_readFileName(folderNum, i, name);
    SEQ_VIS_writePatternName(i, name);
  }
}

void SEQ_VIS_drawDialogBox(char row, char col, char height, char width){
  char i;
  
  GLCD_CTRL_PrtAdr(row, col);
  GLCD_CTRL_PrtCharInc( borderUL );
  SEQ_VIS_drawLineOfChars(borderUM, width-2);
  GLCD_CTRL_PrtCharInc( borderUR );

  for(i=1; i<height-1; i++){
    GLCD_CTRL_PrtAdr(row+i, col);
    GLCD_CTRL_PrtCharInc( borderL );
    SEQ_VIS_drawLineOfChars(0, width-2);
    GLCD_CTRL_PrtCharInc( borderR );
  }

  GLCD_CTRL_PrtAdr(row+height-1, col);
  GLCD_CTRL_PrtCharInc( borderLL );
  SEQ_VIS_drawLineOfChars(borderLM, width-2);
  GLCD_CTRL_PrtCharInc( borderLR );
}

void SEQ_VIS_changePatternDialog(){

  SEQ_VIS_drawDialogBox(POS_DIALOG_CHANGE_PATTERN_TOP, POS_DIALOG_ITEM_LEFT-1, 9, 20);

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_PATTERN_TOP+3, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Pattern: ");

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_PATTERN_TOP+4, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Repeat :");

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_PATTERN_TOP+6, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Ok");

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_PATTERN_TOP+7, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Cancel");
}

void SEQ_VIS_changePatternDialogUpdateName(char *filename){
  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_PATTERN_TOP+1, POS_DIALOG_ITEM_LEFT+1);
  SEQ_VIS_writeName(filename);
}

void SEQ_VIS_changePerfPatternDialog(){

  SEQ_VIS_drawDialogBox(POS_DIALOG_CHANGE_PATTERN_TOP, POS_DIALOG_ITEM_LEFT-1, 8, 20);

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_PATTERN_TOP+3, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Pattern: ");

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_PATTERN_TOP+5, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Ok");

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_PATTERN_TOP+6, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Cancel");
}

void SEQ_VIS_changePerfPatternDialogUpdateName(char *filename){
  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_PATTERN_TOP+1, POS_DIALOG_ITEM_LEFT+1); //OK
  SEQ_VIS_writeName(filename);
}

void SEQ_VIS_changeFolderDialog(){

  SEQ_VIS_drawDialogBox(POS_DIALOG_CHANGE_FOLDER_TOP, POS_DIALOG_ITEM_LEFT-1, 4, 20);

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_FOLDER_TOP+1, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Rename folder");

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_FOLDER_TOP+2, POS_DIALOG_ITEM_LEFT+1); //OK
  GLCD_CTRL_PrtTxt("Cancel");
}

void SEQ_VIS_changeFileDialog(){

  SEQ_VIS_drawDialogBox(POS_DIALOG_CHANGE_FILE_TOP, POS_DIALOG_ITEM_LEFT-1, 7, 20);

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_FILE_TOP+1, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Load");

  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_FILE_TOP+2, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Rename");
  
  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_FILE_TOP+3, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Overwrite");
  
  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_FILE_TOP+4, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Delete");
  
  GLCD_CTRL_PrtAdr(POS_DIALOG_CHANGE_FILE_TOP+5, POS_DIALOG_ITEM_LEFT+1); //OK
  GLCD_CTRL_PrtTxt("Cancel");
}

void SEQ_VIS_saveFileDialog(){
  SEQ_VIS_drawDialogBox(POS_DIALOG_SAVE_FILE_TOP, POS_DIALOG_ITEM_LEFT-1, 4, 20);
  
  GLCD_CTRL_PrtAdr(POS_DIALOG_SAVE_FILE_TOP+1, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Save");
  
  GLCD_CTRL_PrtAdr(POS_DIALOG_SAVE_FILE_TOP+2, POS_DIALOG_ITEM_LEFT+1); //OK
  GLCD_CTRL_PrtTxt("Cancel");
}

void SEQ_VIS_nameFileDialog(){
  SEQ_VIS_drawDialogBox(POS_DIALOG_NAME_FILE_TOP, POS_DIALOG_ITEM_LEFT-1, 8, 20);

  GLCD_CTRL_PrtAdr(POS_DIALOG_NAME_FILE_TOP+1, POS_DIALOG_ITEM_LEFT);
  GLCD_CTRL_PrtTxt("Enter name:");

  GLCD_CTRL_PrtAdr(POS_DIALOG_NAME_FILE_TOP+5, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Ok            ");

  GLCD_CTRL_PrtAdr(POS_DIALOG_NAME_FILE_TOP+6, POS_DIALOG_ITEM_LEFT+1);
  GLCD_CTRL_PrtTxt("Cancel          ");
}

void SEQ_VIS_nameFileDialogUpdateName(char *filename, short cursorPos){
  GLCD_CTRL_PrtAdr(POS_DIALOG_NAME_FILE_TOP+3, POS_DIALOG_ITEM_LEFT+1);
  SEQ_VIS_writePaddedName(filename);

  GLCD_CTRL_CursorPtr(POS_DIALOG_NAME_FILE_TOP+3, POS_DIALOG_ITEM_LEFT+1+cursorPos); //OK
  GLCD_CTRL_CursorOn();
}

//
// Draw a horizontal line in the sequencer using setupTopLine
// Line is the line number from the top of the display
//
void SEQ_VIS_drawHorizontalSetupLine(unsigned short line){
  GLCD_CTRL_PrtAdr(line,POS_LEFT);
  SEQ_VIS_drawLineOfChars(setupLineTop, patt->steps);
}

//
// Draw a line with the supplied character
//
void SEQ_VIS_drawLineOfChars(char character, char repetitions){
  unsigned short i;

  for(i=0; i<repetitions; i++){
    GLCD_CTRL_PrtCharInc(character);
  }
}

//
// Write all bar graph bars.
//
void SEQ_VIS_writeVelocityBars(){
  unsigned short i;
  for(i=0; i<patt->steps; i++){
    SEQ_VIS_writeVelocityBar(i);
  }
}

/**
 * Write 'bar graph' for the velocity of a single step
 **/
void SEQ_VIS_writeVelocityBar(unsigned short step){
  unsigned short lcdi;
  unsigned short vel;

  lcdi = step+POS_LEFT;

  GLCD_CTRL_PrtAdr(POS_VELOCITY_BARS,lcdi);
  vel = (patt->tracks[sequencer.currTrack].steps[step] + 3)/ 4;
  if( vel > 24 ) {
    GLCD_CTRL_PrtCharInc( gridBar1 + ( vel - 25 ));
  } else {
    GLCD_CTRL_PrtCharInc( 0x00 );
  }

  GLCD_CTRL_PrtAdr(POS_VELOCITY_BARS+1,lcdi);
  if( vel > 24 ) {
    GLCD_CTRL_PrtCharInc( gridBar8 );
  } else if( vel > 16 ) {
    GLCD_CTRL_PrtCharInc( gridBar1 + ( vel - 17) );
  } else {
    GLCD_CTRL_PrtCharInc( 0x00 );
  }

  GLCD_CTRL_PrtAdr(POS_VELOCITY_BARS+2,lcdi);
  if( vel > 16 ) {
    GLCD_CTRL_PrtCharInc( gridBar8 );
  } else if( vel > 8 ) {
    GLCD_CTRL_PrtCharInc( gridBar1 + ( vel - 9) );
  } else {
    GLCD_CTRL_PrtCharInc( 0x00 );
  }

  GLCD_CTRL_PrtAdr(POS_VELOCITY_BARS+3,lcdi);
  if( vel > 8 ) {
    GLCD_CTRL_PrtCharInc( gridBar8 );
  } else if( vel > 0 ) {
    GLCD_CTRL_PrtCharInc( gridBar1 + (vel-1) );
  } else {
    GLCD_CTRL_PrtCharInc( 0x00 );
  }
}

/**
 * Write track/setup item names
 **/
void SEQ_VIS_writeSetupItems(){

  unsigned short i;

  GLCD_CTRL_PrtAdr(2,0);
  GLCD_CTRL_PrtCharInc(0x00);
  SEQ_VIS_writeTrackName( TIDgeneral );
  GLCD_CTRL_PrtTxt("   ");
  GLCD_CTRL_PrtCharInc(setupLine);

  GLCD_CTRL_PrtAdr(3,0);
  GLCD_CTRL_PrtCharInc(0x00);
  SEQ_VIS_writeTrackName( TIDpattern );
  GLCD_CTRL_PrtTxt("   ");
  GLCD_CTRL_PrtCharInc(setupLine);

  for( i=0; i<patt->trackcount; i++){
    GLCD_CTRL_PrtAdr(POS_TOP+i,0);
    GLCD_CTRL_PrtCharInc(0x00);
    SEQ_VIS_writeTrackName(patt->tracks[i].name);
    SEQ_VIS_drawLineOfChars(0x00, 3);
    GLCD_CTRL_PrtCharInc(setupLine);
  }
  for( i; i<12; i++){
    GLCD_CTRL_PrtAdr(POS_TOP+i,0);
    SEQ_VIS_drawLineOfChars(0x00, 9);
    GLCD_CTRL_PrtCharInc(setupLine);
  }
}

/**
 write blanks in right column of setup page
 **/
void SEQ_VIS_clearSetupArea(){
  unsigned short i,j;
  for( j=2; j<16; j++) {
    GLCD_CTRL_PrtAdr(j,10);
    for( i=0; i<30; i++) GLCD_CTRL_PrtCharInc(0x00);
  }
}

/**
 * remove arrows/focus for items in the left column
 **/
void SEQ_VIS_clearItemFocus(char currRow){
  GLCD_CTRL_PrtAdr(currRow,0);
  GLCD_CTRL_PrtCharInc( 0x00 );
}

/**
 * write arrows/focus for items in the left column
 **/
void SEQ_VIS_writeItemFocus(char currRow){
  GLCD_CTRL_PrtAdr(currRow,0);
  GLCD_CTRL_PrtCharInc( rightArrow );
}

/**
 * (re)write step states for one track.
 **/
void SEQ_VIS_writeSingleGrid(unsigned short line, unsigned short track){
  unsigned short j;
  unsigned short blanks;

  GLCD_CTRL_PrtAdr(POS_TOP+line,POS_LEFT);
  for( j=0; j<patt->steps; j++) {
    SEQ_VIS_writeStep(track,j,1);
  }
  blanks = MAX_STEPS - j;
  SEQ_VIS_drawLineOfChars(0x00, blanks);
}

/**
 * (re)write all step states
 **/
void SEQ_VIS_writeFullGrid(){

  unsigned short i;

  for( i=0; i<patt->trackcount; i++) {
    GLCD_CTRL_PrtAdr(POS_TOP+i,POS_LEFT);
    SEQ_VIS_writeSingleGrid(i, i);
  }
}

/**
 * clear step sequencer display
 **/
void SEQ_VIS_clearFullGrid(){
  unsigned short i,j;
  for( i=0; i<12; i++) {
    GLCD_CTRL_PrtAdr(POS_TOP+i,POS_LEFT);
    for( j=0; j<32; j++) {
      GLCD_CTRL_PrtCharInc(0x00);
    }
  }
}

//
// Update step and velocity bar when in velocity mode
//
void SEQ_VIS_writeStepBar(unsigned short step){
  SEQ_VIS_writeVelocityBar(step);
  SEQ_VIS_writeSingleStep(0, sequencer.currTrack, step);
  SEQ_VIS_writeStepFlam(step);
}

//
// Write a symbol indicating flam notes
//
void SEQ_VIS_writeStepFlam(unsigned short step){
  struct Flam *currentFlam;
  short flamindex;
  flamindex = patt->tracks[sequencer.currTrack].flams[step];
  
  GLCD_CTRL_PrtAdr(POS_VELOCITY_BARS+5,step+POS_LEFT);
  if(flamindex >= 0){
    currentFlam = flams[flamindex];
    GLCD_CTRL_PrtCharInc(currentFlam->symbol);
  } else {
    GLCD_CTRL_PrtCharInc( 0x00 );
  }
}

//
// Write flam symbol for all current page flams
//
void SEQ_VIS_writeFlams(){
  unsigned short step;
  short flamindex;
  struct Flam *currentFlam;

  GLCD_CTRL_PrtAdr(POS_VELOCITY_BARS+5,POS_LEFT);
  for(step=0; step<patt->steps; step++){
    flamindex = patt->tracks[sequencer.currTrack].flams[step];
    if(flamindex >= 0){
      currentFlam = flams[flamindex];
      GLCD_CTRL_PrtCharInc(currentFlam->symbol);
    } else {
      GLCD_CTRL_PrtCharInc( 0x00 );
    }
  }
}

//
// Write a single step from an aribtrary track at the sequencer line selected
// by line.
//
void SEQ_VIS_writeSingleStep(unsigned short line, unsigned short track, unsigned short step){
  GLCD_CTRL_PrtAdr(POS_TOP+line,POS_LEFT+step);
  SEQ_VIS_writeStep(track,step,1);
}

//
// write a single step
//
void SEQ_VIS_writeStep(unsigned short track, unsigned short step, unsigned short inc){

  unsigned short resGrd;
  unsigned short hilite;
  resGrd = patt->res-1;
  hilite = 0;

  if(!inc)GLCD_CTRL_PrtAdr(POS_TOP+track,POS_LEFT+step); //set pointer

  //TODO: Clipboard
  //is step within highlighted CB_area?
  if( track >= CB_drawArea.y1 &&
      track <  CB_drawArea.y2 &&
      step  >= CB_drawArea.x1 &&
      step  <  CB_drawArea.x2 ) hilite = 1;

  if( patt->tracks[track].steps[step] > 0 ){
    if( step % patt->res == resGrd) { //gridline
      if(hilite){
        GLCD_CTRL_PrtCharInc(gridROnInv);
      } else {
        GLCD_CTRL_PrtCharInc(gridROn);
      }
    } else {
      if(hilite){
        GLCD_CTRL_PrtCharInc(gridMOnInv);
      } else {
        GLCD_CTRL_PrtCharInc(gridMOn);
      }
    }
  } else {
    if( step % patt->res == resGrd) { //gridline
      if(hilite){
        GLCD_CTRL_PrtCharInc(gridROffInv);
      } else {
        GLCD_CTRL_PrtCharInc(gridROff);
      }
    } else {
      if(hilite){
        GLCD_CTRL_PrtCharInc(gridMOffInv);
      } else {
        GLCD_CTRL_PrtCharInc(gridMOff);
      }
    }
  }
}


//
// write/update all track names. Also clears 'missing' track lines
//
void SEQ_VIS_writeTrackNames(){

  unsigned short i;

  for( i=0; i<patt->trackcount; i++){
    GLCD_CTRL_PrtAdr(POS_TOP+i,0);
    GLCD_CTRL_PrtCharInc(0x00);
    SEQ_VIS_writeTrackName(patt->tracks[i].name);
  }
  for( i; i<12; i++){
    GLCD_CTRL_PrtAdr(POS_TOP+i,0);
    SEQ_VIS_drawLineOfChars(0x00, 40);
  }
}

//
// writes the label specified by trackID at current position
//
void SEQ_VIS_writeTrackName(unsigned short trackID){

  if(trackID >= TIDtrack1 ){
    GLCD_CTRL_PrtCharInc(insTrack1);
    GLCD_CTRL_PrtCharInc(insTrack2);
    GLCD_CTRL_PrtCharInc(insTrack3);
  }

  switch(trackID){
    case TIDsnare:
      GLCD_CTRL_PrtCharInc(insSnare1);
      GLCD_CTRL_PrtCharInc(insSnare2);
      GLCD_CTRL_PrtCharInc(insSnare3);
      GLCD_CTRL_PrtCharInc(insSnare4);
      GLCD_CTRL_PrtCharInc(0x00);
      break;
    case TIDbass:
      GLCD_CTRL_PrtCharInc(insBass1);
      GLCD_CTRL_PrtCharInc(insBass2);
      GLCD_CTRL_PrtCharInc(insBass3);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      break;
    case TIDhi:
      GLCD_CTRL_PrtCharInc(insHigh1);
      GLCD_CTRL_PrtCharInc(insHigh2);
      GLCD_CTRL_PrtCharInc(insHigh3);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      break;
    case TIDmid:
      GLCD_CTRL_PrtCharInc(insMid1);
      GLCD_CTRL_PrtCharInc(insMid2);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      break;
    case TIDlo:
      GLCD_CTRL_PrtCharInc(insLow1);
      GLCD_CTRL_PrtCharInc(insLow2);
      GLCD_CTRL_PrtCharInc(insLow3);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      break;
    case TIDcrash:
      GLCD_CTRL_PrtCharInc(insCrash1);
      GLCD_CTRL_PrtCharInc(insCrash2);
      GLCD_CTRL_PrtCharInc(insCrash3);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      break;
    case TIDride:
      GLCD_CTRL_PrtCharInc(insRide1);
      GLCD_CTRL_PrtCharInc(insRide2);
      GLCD_CTRL_PrtCharInc(insRide3);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      break;
    case TIDhhopen:
      GLCD_CTRL_PrtCharInc(insHH1);
      GLCD_CTRL_PrtCharInc(insHH2);
      GLCD_CTRL_PrtCharInc(insHH3);
      GLCD_CTRL_PrtCharInc(insHH4);
      GLCD_CTRL_PrtCharInc(0x00);
      break;
    case TIDhhclosed:
      GLCD_CTRL_PrtCharInc(insHH1);
      GLCD_CTRL_PrtCharInc(insHH5);
      GLCD_CTRL_PrtCharInc(insHH6);
      GLCD_CTRL_PrtCharInc(insHH7);
      GLCD_CTRL_PrtCharInc(insHH8);
      break;
    case TIDrimshot:
      GLCD_CTRL_PrtCharInc(insRim1);
      GLCD_CTRL_PrtCharInc(insRim2);
      GLCD_CTRL_PrtCharInc(insRim3);
      GLCD_CTRL_PrtCharInc(insRim4);
      GLCD_CTRL_PrtCharInc(insRim5);
      break;
    case TIDhandclap:
      GLCD_CTRL_PrtCharInc(insHand1);
      GLCD_CTRL_PrtCharInc(insHand2);
      GLCD_CTRL_PrtCharInc(insHand3);
      GLCD_CTRL_PrtCharInc(insHand4);
      GLCD_CTRL_PrtCharInc(insHand5);
      break;
    case TIDtrack1:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(ins1);
      break;
    case TIDtrack2:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(ins2);
      break;
    case TIDtrack3:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(ins3);
      break;
    case TIDtrack4:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(ins4);
      break;
    case TIDtrack5:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(ins5);
      break;
    case TIDtrack6:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(ins6);
      break;
    case TIDtrack7:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(ins7);
      break;
    case TIDtrack8:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(ins8);
      break;
    case TIDtrack9:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(ins9);
      break;
    case TIDtrack10:
      GLCD_CTRL_PrtCharInc(ins1);
      GLCD_CTRL_PrtCharInc(ins0);
      break;
    case TIDtrack11:
      GLCD_CTRL_PrtCharInc(ins1);
      GLCD_CTRL_PrtCharInc(ins1);
      break;
    case TIDtrack12:
      GLCD_CTRL_PrtCharInc(ins1);
      GLCD_CTRL_PrtCharInc(ins2);
      break;
    case TIDpattern:
      GLCD_CTRL_PrtCharInc(insPatt1);
      GLCD_CTRL_PrtCharInc(insPatt2);
      GLCD_CTRL_PrtCharInc(insPatt3);
      GLCD_CTRL_PrtCharInc(insPatt4);
      GLCD_CTRL_PrtCharInc(insPatt5);
      break;
    case TIDgeneral:
      GLCD_CTRL_PrtCharInc(insGen1);
      GLCD_CTRL_PrtCharInc(insGen2);
      GLCD_CTRL_PrtCharInc(insGen3);
      GLCD_CTRL_PrtCharInc(insGen4);
      GLCD_CTRL_PrtCharInc(insGen5);
      break;
    default:
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      GLCD_CTRL_PrtCharInc(0x00);
      break;
  }
}

//
// write states (none/mute/solo) for all tracks
//
void SEQ_VIS_writeAllStates(){
  unsigned short i;
  for( i=0; i<patt->trackcount; i++){
    SEQ_VIS_writeState(i);
  }
}

//
// write state (none/mute/solo) for a single track
//
void SEQ_VIS_writeState(unsigned short track){
  GLCD_CTRL_PrtAdr(POS_TOP+track, 6);
  if(sequencer.trackStates[track] == STATE_NONE ) {
    GLCD_CTRL_PrtCharInc(0x00);
    GLCD_CTRL_PrtCharInc(gridL);
  } else if( sequencer.trackStates[track] == STATE_MUTE ) {
    GLCD_CTRL_PrtCharInc(mute1);
    GLCD_CTRL_PrtCharInc(mute2);
  } else {
    GLCD_CTRL_PrtCharInc(solo1);
    GLCD_CTRL_PrtCharInc(solo2);
  }
}

//
// Write top line and 'resolution' above grid
//
void SEQ_VIS_writeGridHeader(){

  unsigned short blanks;
  unsigned short i;
  char barNum;

  GLCD_CTRL_PrtAdr(3, 0);
  SEQ_VIS_drawLineOfChars(0x00, 7);

  GLCD_CTRL_PrtCharInc(gridHeadC);

  for( i=0; i<patt->steps; i++){
    if( i%patt->res == 0 ) {
      barNum = (i/patt->res) % 10;
      GLCD_CTRL_PrtCharInc(gridHead1+barNum);
    } else {
      GLCD_CTRL_PrtCharInc(gridHead);
    }
  }

  blanks = MAX_STEPS - i;
  SEQ_VIS_drawLineOfChars(0x00, blanks);
}

/////// TIME/BEAT FUNCITONS ///////

void SEQ_VIS_writePattDataRow(){
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW,0);
  GLCD_CTRL_PrtTxt("               B:");//   .  .   @     S:  /  ");
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW,20);
  GLCD_CTRL_PrtCharInc('.' - 0x20);
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW,23);
  GLCD_CTRL_PrtCharInc('.' - 0x20);
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW,26);
  GLCD_CTRL_PrtTxt(" @ ");
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW,32);
  GLCD_CTRL_PrtTxt(" S:");
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW,37);
  GLCD_CTRL_PrtCharInc('/' - 0x20);
}

void SEQ_VIS_writePatternStatus(char patternLoaded){
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_PATT_STATUS);
  if(patternLoaded){
    GLCD_CTRL_PrtCharInc(0x00);
  } else {
    GLCD_CTRL_PrtCharInc(notok);
  }
}

void SEQ_VIS_writeExternalSynch(char synchOn){
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_SYNCH);
  if(synchOn){
    GLCD_CTRL_PrtCharInc(extSynch);
  } else {
    GLCD_CTRL_PrtCharInc(0x00);
  }
}

void SEQ_VIS_writeLoop(char loopOn){
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_LOOP);
  if(loopOn){
    GLCD_CTRL_PrtCharInc(loop);
  } else {
    GLCD_CTRL_PrtCharInc(0x00);
  }
}

void SEQ_VIS_writePlayMode(){
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_MODE);
  if(sequencer.playState != PLAY_STOPPED){
    switch(sequencer.playMode){
    case PLAY_MODE_PATTERN:
      GLCD_CTRL_PrtTxt("Patt");
      break;
    case PLAY_MODE_PERFORMANCE:
      GLCD_CTRL_PrtTxt("Perf");
      break;
    case PLAY_MODE_SONG:
      GLCD_CTRL_PrtTxt("Song");
      break;
    }
  } else {
    GLCD_CTRL_PrtTxt("    ");
  }
}

void SEQ_VIS_writePlayState(){

  GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_ICON);

  if(G_SEQ_isRecording){
    GLCD_CTRL_PrtCharInc( rec );
  } else {
    switch(sequencer.playState){
      case PLAY_STOPPED:
        GLCD_CTRL_PrtCharInc( stop );
        break;
      case PLAY_PLAYING:
        GLCD_CTRL_PrtCharInc( play );
        break;
      case PLAY_PAUSED:
        GLCD_CTRL_PrtCharInc( pause );
        break;
    }
  }
}

//
// print the current Beat pr. Minute rate.
//
void SEQ_VIS_writeBPM(){
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_BPM);
  GLCD_CTRL_LL_writeNum3(patt->bpm);
}

void SEQ_VIS_writeSwing(){
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_SWING);
  GLCD_CTRL_LL_writeNum2(patt->swingBalance);
  GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_SWING + 3);
  GLCD_CTRL_LL_writeNum2(100 - patt->swingBalance);
}

//
//  update time in display
//
void SEQ_VIS_writeTime(char forceUpdate){

  if(forceUpdate || sequencer.currSub == 1){

    GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_BARS);
    //Bar hundreds
    if( sequencer.currBar > 99 ) GLCD_CTRL_PrtCharInc(0x10 + sequencer.currBar / 100);
    else GLCD_CTRL_PrtCharInc(0x10);

    //Bar tenths
    if( sequencer.currBar > 9 ) GLCD_CTRL_PrtCharInc(0x10 + (sequencer.currBar % 100)/ 10);
    else GLCD_CTRL_PrtCharInc(0x10);

    //Bar ones
    GLCD_CTRL_PrtCharInc(0x10 + (sequencer.currBar%10));

    GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_BEATS);
    //Beat tenths
    if( sequencer.currBeat > 9 ) GLCD_CTRL_PrtCharInc(0x10 + sequencer.currBeat / 10 );
    else GLCD_CTRL_PrtCharInc(0x10);

    //Bar ones
    GLCD_CTRL_PrtCharInc(0x10 + (sequencer.currBeat%10));
  }

  GLCD_CTRL_PrtAdr(POS_PLAY_ROW, POS_PLAY_SUB_BEATS);
  //Sub beat tenths
  if( sequencer.currSub > 9 ) GLCD_CTRL_PrtCharInc(0x10 + sequencer.currSub / 10 );
  else GLCD_CTRL_PrtCharInc(0x10);

  //Sub beat ones
  GLCD_CTRL_PrtCharInc(0x10 + (sequencer.currSub%10));
}

//
// write menu items and highlight active item.
//
void SEQ_VIS_writeMenu(){

  GLCD_CTRL_PrtAdr(0,0);
  GLCD_CTRL_PrtTxt(" Steps ");
  GLCD_CTRL_PrtCharInc( divider );
  GLCD_CTRL_PrtTxt(" Song ");
  GLCD_CTRL_PrtCharInc( divider );
  GLCD_CTRL_PrtTxt(" Perform ");
  GLCD_CTRL_PrtCharInc( divider );
  GLCD_CTRL_PrtTxt(" File ");
  GLCD_CTRL_PrtCharInc( divider );
  GLCD_CTRL_PrtTxt(" Setup ");

  GLCD_CTRL_PrtAdr(1,0);
  SEQ_VIS_drawLineOfChars(setupLineTop, 40);

  switch(currPage){
    case PAGE_PATT:
      GLCD_CTRL_PrtAdr(0,0);
      GLCD_CTRL_PrtCharInc( rightArrow );
      GLCD_CTRL_PrtAdr(0,6);
      GLCD_CTRL_PrtCharInc( leftArrow );
      break;
    case PAGE_SONG:
      GLCD_CTRL_PrtAdr(0,8);
      GLCD_CTRL_PrtCharInc( rightArrow );
      GLCD_CTRL_PrtAdr(0,13);
      GLCD_CTRL_PrtCharInc( leftArrow );
      break;
    case PAGE_PERFORMANCE:
      GLCD_CTRL_PrtAdr(0,15);
      GLCD_CTRL_PrtCharInc( rightArrow );
      GLCD_CTRL_PrtAdr(0,23);
      GLCD_CTRL_PrtCharInc( leftArrow );
      break;
    case PAGE_FILE:
      GLCD_CTRL_PrtAdr(0,25);
      GLCD_CTRL_PrtCharInc( rightArrow );
      GLCD_CTRL_PrtAdr(0,30);
      GLCD_CTRL_PrtCharInc( leftArrow );
      break;
    case PAGE_SETUP:
      GLCD_CTRL_PrtAdr(0,32);
      GLCD_CTRL_PrtCharInc( rightArrow );
      GLCD_CTRL_PrtAdr(0,38);
      GLCD_CTRL_PrtCharInc( leftArrow );
      break;
  }
}

void SEQ_VIS_writeSetupGeneral(){

  SEQ_VIS_clearSetupArea();

  GLCD_CTRL_PrtAdr(2,10);
  GLCD_CTRL_PrtTxt("General settings");

  GLCD_CTRL_PrtAdr(4,11);
  GLCD_CTRL_PrtTxt("External sync..: ");  //internal or external

  GLCD_CTRL_PrtAdr(5,11);
  GLCD_CTRL_PrtTxt("Sysex addr 1...: ");

  GLCD_CTRL_PrtAdr(6,11);
  GLCD_CTRL_PrtTxt("Sysex addr 2...: ");

  GLCD_CTRL_PrtAdr(7,11);
  GLCD_CTRL_PrtTxt("Export memory..: ");

  GLCD_CTRL_PrtAdr(9,11);
  GLCD_CTRL_PrtTxt("Reset steps....: ");

  GLCD_CTRL_PrtAdr(10,11);
  GLCD_CTRL_PrtTxt("Reset song.....: ");

  GLCD_CTRL_PrtAdr(11,11);
  GLCD_CTRL_PrtTxt("Reset perform..: ");

  GLCD_CTRL_PrtAdr(13,11);
  GLCD_CTRL_PrtTxt("Reset system...: ");
}

void SEQ_VIS_showSysexDumpDialog(){
  SEQ_VIS_drawDialogBox(POS_DIALOG_SYSEX_DUMP_TOP, POS_DIALOG_ITEM_LEFT-4, 4, 26);

  GLCD_CTRL_PrtAdr(POS_DIALOG_SYSEX_DUMP_TOP+1, POS_DIALOG_ITEM_LEFT-2);
  GLCD_CTRL_PrtTxt("Please wait, exporting");

  GLCD_CTRL_PrtAdr(POS_DIALOG_SYSEX_DUMP_TOP+2, POS_DIALOG_ITEM_LEFT-2);
  GLCD_CTRL_PrtTxt("   memory content     ");

}

//
// write settings for current track
//
void SEQ_VIS_writeSetupPattern(){

  SEQ_VIS_clearSetupArea();

  GLCD_CTRL_PrtAdr(2,10);
  GLCD_CTRL_PrtTxt("Pattern settings");

  SEQ_VIS_writeSetupPatternLabels(4, 11);
}

void SEQ_VIS_writeSetupPatternLabels(char row, char col){

  GLCD_CTRL_PrtAdr(row, col);
  GLCD_CTRL_PrtTxt("Tracks.........:");

  GLCD_CTRL_PrtAdr(row+1, col);
  GLCD_CTRL_PrtTxt("Steps..........:");

  GLCD_CTRL_PrtAdr(row+2, col);
  GLCD_CTRL_PrtTxt("Resolution.....:");

  GLCD_CTRL_PrtAdr(row+3, col);
  GLCD_CTRL_PrtTxt("Beats per bar..:");

  GLCD_CTRL_PrtAdr(row+4, col);
  GLCD_CTRL_PrtTxt("Swing balance..:");

  GLCD_CTRL_PrtAdr(row+6, col);
  GLCD_CTRL_PrtTxt("Set bpm on load:");

}



//
// write labels for track setup
//
void SEQ_VIS_writeSetupTrack(char track){

  SEQ_VIS_clearSetupArea();

  GLCD_CTRL_PrtAdr(2,10);
  GLCD_CTRL_PrtTxt("Track settings");

  GLCD_CTRL_PrtAdr(POS_SETUP_TOP,POS_SETUP_LABEL_LEFT);
  GLCD_CTRL_PrtTxt("Name...........: "); //five chars long
  SEQ_VIS_writeTrackName(patt->tracks[track].name);

  GLCD_CTRL_PrtAdr(POS_SETUP_TOP+1,POS_SETUP_LABEL_LEFT);
  GLCD_CTRL_PrtTxt("Velocity.......: ");

  GLCD_CTRL_PrtAdr(POS_SETUP_TOP+2,POS_SETUP_LABEL_LEFT);
  GLCD_CTRL_PrtTxt("Accent.........: ");

  GLCD_CTRL_PrtAdr(POS_SETUP_TOP+3,POS_SETUP_LABEL_LEFT);
  GLCD_CTRL_PrtTxt("Base note......: ");

  GLCD_CTRL_PrtAdr(POS_SETUP_TOP+4,POS_SETUP_LABEL_LEFT);
  GLCD_CTRL_PrtTxt("Midi channel...: ");
}

//
// Print on or off at current position
//
void SEQ_VIS_writeOnOff(unsigned short row, unsigned short col, unsigned short length, unsigned short toggle){
  GLCD_CTRL_PrtAdr(row,col + length - 3);

  if(toggle){
    GLCD_CTRL_PrtTxt(" on");
  }else{
    GLCD_CTRL_PrtTxt("off");
  }
}

//
// Remove active track arrow
//
void SEQ_VIS_clearTrackArrow(){
  SEQ_VIS_clearArrow(POS_TOP+sequencer.currTrack, 0);
}

//
// Show active track arrow
//
void SEQ_VIS_writeTrackArrow(){
  SEQ_VIS_writeArrow(POS_TOP+sequencer.currTrack, 0);
}

//
// Place arrow
//
void SEQ_VIS_writeArrow(unsigned short row, unsigned short col){
  GLCD_CTRL_PrtAdr(row, col);
  GLCD_CTRL_PrtCharInc(rightArrow);
}

//
// Clear arrow
//
void SEQ_VIS_clearArrow(unsigned short row, unsigned short col){
  GLCD_CTRL_PrtAdr(row, col);
  GLCD_CTRL_PrtCharInc(0x00);
}

//
// Copy/paste: set highlight end and update display if necessary
//
void SEQ_VIS_updateHighlitArea() {
  if(currPage == PAGE_PATT){
    SEQ_VIS_updateHighlitAreaAll();
  } else if(currPage == PAGE_BARS){
    SEQ_VIS_updateHighlitAreaSingleTrack(0, sequencer.currTrack);
  }
}

void SEQ_VIS_updateHighlitAreaAll() {

  unsigned char i,j;
  unsigned char startXS, endXS;
  unsigned char startXE, endXE;
  unsigned char startYO, endYO;
  unsigned char startYI, endYI;

  //clear/highlight start
  if( CB_drawArea.x1 < CB_currDrawArea.x1 ) {
    startXS = CB_drawArea.x1;
    endXS = CB_currDrawArea.x1;
  } else {
    startXS = CB_currDrawArea.x1;
    endXS = CB_drawArea.x1;
  }

  //clear/highlight end
  if( CB_drawArea.x2 > CB_currDrawArea.x2 ) {
    startXE = CB_currDrawArea.x2;
    endXE = CB_drawArea.x2;
  } else {
    startXE = CB_drawArea.x2;
    endXE = CB_currDrawArea.x2;
  }

  //affected rows, start/end
  if( CB_drawArea.y1 < CB_currDrawArea.y1 ){
    startYO = CB_drawArea.y1;
    startYI = CB_currDrawArea.y1;
  } else {
    startYO = CB_currDrawArea.y1;
    startYI = CB_drawArea.y1;
  }
  if( CB_area.y2 > CB_currDrawArea.y2 ) {
    endYO = CB_drawArea.y2;
    endYI = CB_currDrawArea.y1;
  } else {
    endYO = CB_currDrawArea.y2;
    endYI = CB_drawArea.y2;
  }

  //update display
  for( i = startYI; i<endYI; i++) {
    for( j=startXS; j<endXS; j++){
      SEQ_VIS_writeStep(i, j, 0);
    }
    for( j=startXE; j<endXE; j++){
      SEQ_VIS_writeStep(i, j, 0);
    }
  }

  for( i=startYO; i<startYI; i++){
    GLCD_CTRL_PrtAdr(POS_TOP+i,POS_LEFT+startXS);
    for( j=startXS; j<endXE; j++){
      SEQ_VIS_writeStep(i, j, 1);
    }
  }

  for( i=endYI; i<endYO; i++){
    GLCD_CTRL_PrtAdr(POS_TOP+i,POS_LEFT+startXS);
    for( j=startXS; j<endXE; j++){
      SEQ_VIS_writeStep(i, j, 1);
    }
  }

  CB_currDrawArea = CB_drawArea;
}

void SEQ_VIS_updateHighlitAreaSingleTrack(unsigned short line, unsigned short track) {

  unsigned char j;
  unsigned char startXS, endXS;
  unsigned char startXE, endXE;

  //clear/highlight start
  if( CB_drawArea.x1 < CB_currDrawArea.x1 ) {
    startXS = CB_drawArea.x1;
    endXS = CB_currDrawArea.x1;
  } else {
    startXS = CB_currDrawArea.x1;
    endXS = CB_drawArea.x1;
  }

  //clear/highlight end
  if( CB_drawArea.x2 > CB_currDrawArea.x2 ) {
    startXE = CB_currDrawArea.x2;
    endXE = CB_drawArea.x2;
  } else {
    startXE = CB_drawArea.x2;
    endXE = CB_currDrawArea.x2;
  }

  //update display
  GLCD_CTRL_PrtAdr(POS_TOP+line,POS_LEFT+startXS);
  for( j=startXS; j<endXE; j++){
    SEQ_VIS_writeStep(track, j, 1);
  }

  CB_currDrawArea = CB_drawArea;
}

//
// Write a maximum three digit short with blank padding.
//
void SEQ_VIS_writeNum(unsigned short row, unsigned short col, unsigned short length, int number){
  unsigned short i;
  GLCD_CTRL_PrtAdr(row,col);
  if(number < 0){
    for(i = 0; i<length; i++){
      GLCD_CTRL_PrtCharInc(0x00);
    }
  } else {
    if(length>2){
      if( number > 99 ) {
        GLCD_CTRL_PrtCharInc(0x10 + number / 100);
      } else {
        GLCD_CTRL_PrtCharInc(0x00);
      }
    }
    if(length>1){
      if( number > 9 ) {
        GLCD_CTRL_PrtCharInc(0x10 + (number %100) / 10);
      } else {
        GLCD_CTRL_PrtCharInc(0x00);
      }
    }
    GLCD_CTRL_PrtCharInc(0x10 + number % 10);
  }
}

void SEQ_VIS_loadCharsFromStorage(){
  int i;
  char charData[8];
  for(i=CHAR_FIRST_ADDRESS; i<CHAR_FIRST_ADDRESS+128; i++){
    EEX_readDisplayCharacter(i, charData);
    GLCD_CTRL_LoadChar(i, charData);
    delay_ms(10);
  }
}

void SEQ_VIS_splashScreen(){
  GLCD_CTRL_PrtAdr(7,9);
  GLCD_CTRL_PrtTxt("M A C H I N E B E A T S");
  GLCD_CTRL_PrtAdr(8,9);
  GLCD_CTRL_PrtTxt("      v 1.0 rc 1       ");
}

//
// Write the name of a single folder, including folder number
//
void SEQ_VIS_writeFolderName(char folderNum, char* name){
  GLCD_CTRL_PrtAdr(POS_FILE_FOLDER_1+folderNum,0);
  GLCD_CTRL_PrtCharInc(0);
  GLCD_CTRL_PrtCharInc(0x11 + folderNum);
  GLCD_CTRL_PrtCharInc(0);
  SEQ_VIS_writeName(name);
  GLCD_CTRL_PrtCharInc(setupLine);
}

//
// Write the name of a single pattern, including pattern number
//
void SEQ_VIS_writePatternName(char patternNum, char* name){
  GLCD_CTRL_PrtAdr(POS_FILE_PATTERN_1+patternNum,POS_FILE_RIGHT);
  GLCD_CTRL_LL_writeNum3(patternNum+1);
  GLCD_CTRL_PrtCharInc(0);
  SEQ_VIS_writeName(name);
}

void SEQ_VIS_writeName(char* name){
  if(name[0] == 0){
    GLCD_CTRL_PrtTxt("<empty>");
    SEQ_VIS_drawLineOfChars(0, 9);
  } else {
    SEQ_VIS_writePaddedName(name);
  }
}

void SEQ_VIS_writePaddedName(char* name){
  char nameLength = 0;

  while(name[nameLength] != 0){
    nameLength++;
  }
  GLCD_CTRL_PrtTxt(name);
  SEQ_VIS_drawLineOfChars(0, NAME_LENGTH - nameLength);
}

void SEQ_VIS_showCursor(){
  GLCD_CTRL_CursorOn();
}

void SEQ_VIS_hideCursor(){
  GLCD_CTRL_CursorOff();
}