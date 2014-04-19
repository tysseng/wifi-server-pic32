#ifndef _PAGE_SONG_PRIVATE_H
#define _PAGE_SONG_PRIVATE_H
void PGSO_initDialogMenuItems();
void PGSO_insertPattern();
void PGSO_deletePattern();
char PGSO_getArrayIndex(char elementNum);
void PGSO_writeSongPatterns();
void PGSO_getFileName(char index);
void PGSO_togglePrevPage(char cursorOnTop);
void PGSO_togglePrevPatt();
void PGSO_toggleNextPage();
void PGSO_toggleNextPatt();
void PGSO_listenerShowChangePatternDialog(char key);
void PGSO_listenerEnterPatternNum(char key);
void PGSO_listenerEnterPatternRepeat(char key);
void PGSO_listenerUpdatePattern(char key);
void PGSO_listenerCancel(char key);
void PGSO_inputActionNoop(struct InputField *field, short enter);
void PGSO_inputActionPatternChanged(struct InputField *field, short enter);
void PGSO_stopOrReset();
void PGSO_closeDialog();
void PGSO_toggleDialogItem(unsigned short item);
void PGSO_updatePatternStatus();
#endif