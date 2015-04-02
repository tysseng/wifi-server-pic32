#ifndef _MULTITAP_H
#define _MULTITAP_H
short MT_initMultiTap(char *currName);
short MT_addChar(char key, char *currName);
short MT_removeChar(char *currName);
short MT_moveCursorLeft();
short MT_moveCursorRight(char *currName);
#endif