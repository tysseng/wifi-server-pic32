#ifndef _KEYBOARD_PRIVATE_H
#define _KEYBOARD_PRIVATE_H
void KBD_generateEvents(unsigned short row, unsigned short states, unsigned short press);
void KEYMAP_init(unsigned short keymap[]);
#endif