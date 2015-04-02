#ifndef _PAGE_PREFORMANCE_PRIVATE_H
#define _PAGE_PERFORMANCE_PRIVATE_H
void PGPF_initDialogMenuItems();
void PGPF_redrawPage();
void PGPF_togglePrevPage(char cursorOnTop);
void PGPF_togglePrevPatt();
void PGPF_toggleNextPage();
void PGPF_toggleNextPatt();
void PGPF_resetPatternKeyMap();
void PGPF_writeKeyMapPatterns();
void PGPF_getFileName(char index);
void PGPF_loadNextPatt(char folderNum, char fileNum);
void PGPF_deletePattern();
void PGPF_swapPatterns();
void PGPF_quickNextPatternInput(char key);
void PGPF_inputActionNextPatternChanged(struct InputField *field, short enter);
void PGPF_inputActionPatternChanged(struct InputField *field, short enter);
void PGPF_listenerShowChangePatternDialog(char key);
void PGPF_listenerEnterNextPatternNum(char key);
void PGPF_listenerEnterPatternNum(char key);
void PGPF_listenerUpdatePattern(char key);
void PGPF_listenerCancel(char key);
void PGPF_closeDialog();
void PGPF_toggleDialogItem(unsigned short item);
#endif