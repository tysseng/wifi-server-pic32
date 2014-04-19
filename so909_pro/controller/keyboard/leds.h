#ifndef _LEDS_H
#define _LEDS_H
void LED_init();
void LED_on(unsigned short led);
void LED_off(unsigned short led);
char LED_state(unsigned short led);
void LED_stepOn(unsigned short step);
void LED_stepOff(unsigned short step);
void LED_stepOnFromInterrupt(unsigned short step);
void LED_stepOffFromInterrupt(unsigned short step);
void LED_recordOn();
void LED_recordOff();
void LED_tempoBarOn();
void LED_tempoQuarterOn();
void LED_tempoAllOff();
void LED_tempoAllOffFromInterrupt();
void LED_updateModeLeds(char mode);
void LED_writeLeds(unsigned short row);
#endif