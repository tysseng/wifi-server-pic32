#ifndef _NAVIGATION_H
#define _NAVIGATION_H

extern unsigned short pageFocus;
extern unsigned short currPage;
extern unsigned short currDialog;
extern unsigned short currDialogItem;
extern struct MenuItem *currMenuItem;
extern void (*currKeyListener)(char);

void NAV_init();
void NAV_shiftOn();
void NAV_shiftOff();
void NAV_previousMode();
void NAV_nextMode();
void NAV_toggleFocus();
void NAV_setCurrentDialogItem(struct MenuItem *item);
void NAV_listenerNOOP(char key);

#endif