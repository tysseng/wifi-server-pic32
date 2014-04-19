#ifndef _UTIL_H
#define _UTIL_H
char UTIL_getFolderNum(char patternNum);
char UTIL_getFileNum(char patternNum);
char UTIL_getPatternNum(char folderNum, char fileNum);
int UTIL_getPrintablePatternNum(char patternNum);
int UTIL_getPrintablePatternNumFromFolderFile(char folderNum, char fileNum);
char UTIL_getPatternNumFromPrintable(int printablePatternNum);
#endif