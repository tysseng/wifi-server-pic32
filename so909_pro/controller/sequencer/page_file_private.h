#include "../storage/external_storage.h"

extern unsigned short currFolder;
extern short currFile;

void PGF_initDialogMenuItems();
void PGF_setCurrentDialogItem(struct MenuItem *item);
void PGF_resetCurrName();

void PGF_toggleDialogItem(unsigned short item);
void PGF_toggleFolder(unsigned short folderNum);
void PGF_toggleFile(short fileNum);

// key listeners
void PGF_listenerLoadFile(char key);
void PGF_listenerSaveFile(char key);
void PGF_listenerDeleteFile(char key);
void PGF_listenerNameFileOrFolder(char key);
void PGF_listenerEnterCharacter(char key);
void PGF_listenerCancel(char key);

void PGF_listenerShowChangeFolderDialog(char key);
void PGF_listenerShowRenameDialog(char key);
void PGF_listenerShowNameNewFileDialog(char key);
void PGF_listenerShowFileDialog(char key);

void PGF_showChangeFileDialog();
void PGF_showSaveFileDialog();

void PGF_closeDialog();
