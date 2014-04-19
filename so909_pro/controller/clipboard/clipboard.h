#ifndef _CLIPBOARD_H
#define _CLIPBOARD_H
extern struct Area CB_area;
extern struct Area CB_drawArea;
extern struct Area CB_currDrawArea;

extern short CB_cursorx;
extern short CB_cursory;

void CB_init();
void CB_resetCursor();
void CB_cursorSet(unsigned short x, unsigned short y);
void CB_cursorLeft();
void CB_cursorRight();
void CB_cursorUp();
void CB_cursorDown();
void CB_moveSelectionToTrack(char track);
void CB_resetArea();
void CB_resetCurrDrawArea();
void CB_areaStart(unsigned short x, unsigned short y);
void CB_areaEnd(unsigned short x, unsigned short y);
void CB_copy(unsigned short clear);
void CB_paste(unsigned short swap);
#endif