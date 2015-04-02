#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#define ROWCOUNT 11 //how many of the rows are used for keys/leds.

extern unsigned short keymap[88];

void KBD_init();
void KBD_read(unsigned short row);
#endif