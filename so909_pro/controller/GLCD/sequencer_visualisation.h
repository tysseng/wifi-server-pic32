#ifndef _SEQUENCER_VISUALISATION_H
#define _SEQUENCER_VISUALISATION_H

//#include "../shared/types.h"

// Sequencer display positions
#define POS_TOP 4
#define POS_LEFT 8

// Top position, velocity bars
#define POS_VELOCITY_BARS 6

// Performance page
#define POS_PERF_NEXT_PATT_TOP 4
#define POS_PERF_CURR_PATT_TOP 11
#define POS_PERF_PAGE_TOP 4
#define POS_PERF_LEFT_COL 1
#define POS_PERF_RIGHT_COL 20
#define POS_PERF_LEFT_COL_WIDTH 19
#define POS_PERF_RIGHT_COL_WIDTH 20

#define POS_PERF_KEY_MAP_TOP 6

// Song page
#define POS_SONG_TOP 4
#define POS_SONG_NAME_END 26

// Setup items
#define POS_LABEL_LENGTH 17
#define POS_SETUP_LABEL_LEFT 11
#define POS_SETUP_LEFT 28
#define POS_SETUP_TOP 4

// File page items
#define POS_FILE_RIGHT 20
#define POS_FILE_FOLDER_1 4
#define POS_FILE_PATTERN_1 4

// Play status positions
#define POS_PLAY_ROW 2
#define POS_PLAY_MODE 1
#define POS_PLAY_PATT_STATUS 7
#define POS_PLAY_LOOP 9
#define POS_PLAY_SYNCH 11
#define POS_PLAY_ICON 13
#define POS_PLAY_BARS 17
#define POS_PLAY_BEATS 21
#define POS_PLAY_SUB_BEATS 24
#define POS_PLAY_BPM 29
#define POS_PLAY_SWING 35
#define POS_PLAY_PATTERN_STATUS 1

#define POS_DIALOG_SYSEX_DUMP_TOP 5
#define POS_DIALOG_ITEM_LEFT 11
#define POS_DIALOG_CHANGE_PATTERN_TOP 5
#define POS_DIALOG_CHANGE_FOLDER_TOP 6
#define POS_DIALOG_CHANGE_FILE_TOP 5
#define POS_DIALOG_SAVE_FILE_TOP 6
#define POS_DIALOG_NAME_FILE_TOP 4


void SEQ_VIS_showPageSong();
void SEQ_VIS_writeSongPattern(char line, char patternPos, char repeat, char *name);
void SEQ_VIS_writeEmptyPattern(char line);
void SEQ_VIS_showPagePerformance();
void SEQ_VIS_updateCurrPatternSetup();
void SEQ_VIS_updateNextPatternSetup();
void SEQ_VIS_clearKeyMapItemFocus(char currRow);
void SEQ_VIS_writeKeyMapItemFocus(char currRow);
void SEQ_VIS_writeKeyMapPattern(char line, char patternPos, char *name);
void SEQ_VIS_writeEmptyKeyMapPattern(char line);
void SEQ_VIS_showPagePatt();
void SEQ_VIS_showPageBars();
void SEQ_VIS_drawHorizontalSetupLine(unsigned short line);
void SEQ_VIS_writeStepBar(unsigned short step);
void SEQ_VIS_writeStepFlam(unsigned short step);
void SEQ_VIS_writeFlams();
void SEQ_VIS_writeVelocityBars();
void SEQ_VIS_writeVelocityBar(unsigned short mi);
void SEQ_VIS_writeSetupItems();
void SEQ_VIS_clearSetupArea();
void SEQ_VIS_clearItemFocus(char currRow);
void SEQ_VIS_writeItemFocus(char currRow);
void SEQ_VIS_writeSingleGrid(unsigned short line, unsigned short track);
void SEQ_VIS_writeSingleStep(unsigned short line, unsigned short track, unsigned short step);
void SEQ_VIS_writeFullGrid();
void SEQ_VIS_clearFullGrid();
void SEQ_VIS_writeStep(unsigned short track, unsigned short step, unsigned short inc);
void SEQ_VIS_writeTrackName(unsigned short trackID);
void SEQ_VIS_writeAllStates();
void SEQ_VIS_writeState(unsigned short track);
void SEQ_VIS_writeGridHeader();
void SEQ_VIS_writeTrackNames();
void SEQ_VIS_writePattDataRow();
void SEQ_VIS_writeExternalSynch(char synchOn);
void SEQ_VIS_writeLoop(char loopOn);
void SEQ_VIS_writePlayState();
void SEQ_VIS_writePlayMode();
void SEQ_VIS_writeBPM();
void SEQ_VIS_writeSwing();
void SEQ_VIS_writeTime(char forceUpdate);
void SEQ_VIS_writeMenu();
void SEQ_VIS_writeSetupGeneral();
void SEQ_VIS_showSysexDumpDialog();
void SEQ_VIS_writeSetupPattern();
void SEQ_VIS_writeSetupPatternLabels(char row, char col);
void SEQ_VIS_writePatternSteps(struct Pattern *patt, char row, char col);
void SEQ_VIS_clearPatternSteps(char row, char col);
void SEQ_VIS_writeSetupTrack(char track);
void SEQ_VIS_updateSetupTrack();
void SEQ_VIS_writeOnOff(unsigned short row, unsigned short col, unsigned short length, unsigned short toggle);
void SEQ_VIS_writeTrackArrow();
void SEQ_VIS_clearTrackArrow();
void SEQ_VIS_writeArrow(unsigned short row, unsigned short col);
void SEQ_VIS_clearArrow(unsigned short row, unsigned short col);
void SEQ_VIS_updateHighlitArea();
void SEQ_VIS_updateHighlitAreaAll();
void SEQ_VIS_updateHighlitAreaSingleTrack(unsigned short line, unsigned short track);
void SEQ_VIS_writeNum(unsigned short row, unsigned short col, unsigned short length, int number);
void SEQ_VIS_loadCharsFromStorage();
void SEQ_VIS_splashScreen();
void SEQ_VIS_writeFolderName(char folderNum, char* name);
void SEQ_VIS_writePatternName(char patternNum, char* name);
void SEQ_VIS_showPageFileFolders();
void SEQ_VIS_showPageFilePatterns(char folderNum);
void SEQ_VIS_showPageFileSongs(char folderNum);
void SEQ_VIS_showPageFilePerformances(char folderNum);
void SEQ_VIS_writeFileNames(char folderNum);
void SEQ_VIS_writeName(char* name);
void SEQ_VIS_writePaddedName(char* name);
void SEQ_VIS_drawLineOfChars(char character, char repetitions);
void SEQ_VIS_changeFolderDialog();
void SEQ_VIS_changeFileDialog();
void SEQ_VIS_saveFileDialog();
void SEQ_VIS_nameFileDialog();
void SEQ_VIS_nameFileDialogUpdateName(char *filename, short cursorPos);
void SEQ_VIS_drawDialogBox(char row, char col, char height, char width);
void SEQ_VIS_showCursor();
void SEQ_VIS_hideCursor();
void SEQ_VIS_writeCurrPerformancePattern(int patternNum);
void SEQ_VIS_writeCurrPatternName(char* name);
void SEQ_VIS_writeNextPatternName(char* name);
void SEQ_VIS_changePerfPatternDialog();
void SEQ_VIS_changePerfPatternDialogUpdateName(char *filename);
void SEQ_VIS_changePatternDialog();
void SEQ_VIS_changePatternDialogUpdateName(char *filename);
void SEQ_VIS_writePatternStatus(char patternLoaded);
#endif