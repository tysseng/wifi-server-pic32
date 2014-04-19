#ifndef _LED_CONSTS_H
#define _LED_CONSTS_H

//Green leds
#define LED_GREEN_SEQ01 0
#define LED_GREEN_SEQ02 1
#define LED_GREEN_SEQ03 2
#define LED_GREEN_SEQ04 3
#define LED_GREEN_SEQ05 4
#define LED_GREEN_SEQ06 5
#define LED_GREEN_SEQ07 6
#define LED_GREEN_SEQ08 7
#define LED_GREEN_SEQ09 8
#define LED_GREEN_SEQ10 9
#define LED_GREEN_SEQ11 10
#define LED_GREEN_SEQ12 11
#define LED_GREEN_SEQ13 12
#define LED_GREEN_SEQ14 13
#define LED_GREEN_SEQ15 14
#define LED_GREEN_SEQ16 15
#define LED_GREEN_SEQ17 16
#define LED_GREEN_SEQ18 17
#define LED_GREEN_SEQ19 18
#define LED_GREEN_SEQ20 19
#define LED_GREEN_SEQ21 20
#define LED_GREEN_SEQ22 21
#define LED_GREEN_SEQ23 22
#define LED_GREEN_SEQ24 23
#define LED_GREEN_SEQ25 24
#define LED_GREEN_SEQ26 25
#define LED_GREEN_SEQ27 26
#define LED_GREEN_SEQ28 27
#define LED_GREEN_SEQ29 28
#define LED_GREEN_SEQ30 29
#define LED_GREEN_SEQ31 30
#define LED_GREEN_SEQ32 31

#define LED_GREEN_TEMPO 32
#define LED_GREEN_REC 33

#define LED_GREEN_MODE_1 34
#define LED_GREEN_MODE_2 35
#define LED_GREEN_MODE_3 36
#define LED_GREEN_MODE_4 37
#define LED_GREEN_MODE_5 38

//Red leds
#define LED_RED_SEQ01 39
#define LED_RED_SEQ02 40
#define LED_RED_SEQ03 41
#define LED_RED_SEQ04 42
#define LED_RED_SEQ05 43
#define LED_RED_SEQ06 44
#define LED_RED_SEQ07 45
#define LED_RED_SEQ08 46
#define LED_RED_SEQ09 47
#define LED_RED_SEQ10 48
#define LED_RED_SEQ11 49
#define LED_RED_SEQ12 50
#define LED_RED_SEQ13 51
#define LED_RED_SEQ14 52
#define LED_RED_SEQ15 53
#define LED_RED_SEQ16 54
#define LED_RED_SEQ17 55
#define LED_RED_SEQ18 56
#define LED_RED_SEQ19 57
#define LED_RED_SEQ20 58
#define LED_RED_SEQ21 59
#define LED_RED_SEQ22 60
#define LED_RED_SEQ23 61
#define LED_RED_SEQ24 62
#define LED_RED_SEQ25 63
#define LED_RED_SEQ26 64
#define LED_RED_SEQ27 65
#define LED_RED_SEQ28 66
#define LED_RED_SEQ29 67
#define LED_RED_SEQ30 68
#define LED_RED_SEQ31 69
#define LED_RED_SEQ32 70

#define LED_RED_TEMPO 71
#define LED_RED_REC 72

#define LED_RED_MODE_1 73
#define LED_RED_MODE_2 74
#define LED_RED_MODE_3 75
#define LED_RED_MODE_4 76
#define LED_RED_MODE_5 77

#define LED_RED_OFFSET 39 //offset between green and red color on the same led.

/**
 * map column/row to led number. indexes are [row*8+column] ([ledselX*8+ledsX])
 **/
void LEDMAP_init(unsigned short ledmap[]){
  ledmap[0*8+0] = LED_GREEN_TEMPO;
  ledmap[0*8+1] = LED_RED_TEMPO;
  ledmap[0*8+2] = LED_GREEN_REC;
  ledmap[0*8+3] = LED_RED_REC;
  ledmap[0*8+4] = LED_GREEN_MODE_1;
  ledmap[0*8+5] = LED_RED_MODE_1;
  ledmap[0*8+6] = 0; //not used so index doesn't matter.
  ledmap[0*8+7] = 0; //not used so index doesn't matter.

  ledmap[1*8+0] = LED_GREEN_MODE_2;
  ledmap[1*8+1] = LED_RED_MODE_2;
  ledmap[1*8+2] = LED_GREEN_MODE_3;
  ledmap[1*8+3] = LED_RED_MODE_3;
  ledmap[1*8+4] = LED_GREEN_MODE_4;
  ledmap[1*8+5] = LED_RED_MODE_4;
  ledmap[1*8+6] = LED_GREEN_MODE_5;
  ledmap[1*8+7] = LED_RED_MODE_5;

  ledmap[2*8+0] = LED_GREEN_SEQ01;
  ledmap[2*8+1] = LED_RED_SEQ01;
  ledmap[2*8+2] = LED_GREEN_SEQ02;
  ledmap[2*8+3] = LED_RED_SEQ02;
  ledmap[2*8+4] = LED_GREEN_SEQ03;
  ledmap[2*8+5] = LED_RED_SEQ03;
  ledmap[2*8+6] = LED_GREEN_SEQ04;
  ledmap[2*8+7] = LED_RED_SEQ04;

  ledmap[3*8+0] = LED_GREEN_SEQ17;
  ledmap[3*8+1] = LED_RED_SEQ17;
  ledmap[3*8+2] = LED_GREEN_SEQ18;
  ledmap[3*8+3] = LED_RED_SEQ18;
  ledmap[3*8+4] = LED_GREEN_SEQ19;
  ledmap[3*8+5] = LED_RED_SEQ19;
  ledmap[3*8+6] = LED_GREEN_SEQ20;
  ledmap[3*8+7] = LED_RED_SEQ20;

  ledmap[4*8+0] = LED_GREEN_SEQ05;
  ledmap[4*8+1] = LED_RED_SEQ05;
  ledmap[4*8+2] = LED_GREEN_SEQ06;
  ledmap[4*8+3] = LED_RED_SEQ06;
  ledmap[4*8+4] = LED_GREEN_SEQ07;
  ledmap[4*8+5] = LED_RED_SEQ07;
  ledmap[4*8+6] = LED_GREEN_SEQ08;
  ledmap[4*8+7] = LED_RED_SEQ08;

  ledmap[5*8+0] = LED_GREEN_SEQ21;
  ledmap[5*8+1] = LED_RED_SEQ21;
  ledmap[5*8+2] = LED_GREEN_SEQ22;
  ledmap[5*8+3] = LED_RED_SEQ22;
  ledmap[5*8+4] = LED_GREEN_SEQ23;
  ledmap[5*8+5] = LED_RED_SEQ23;
  ledmap[5*8+6] = LED_GREEN_SEQ24;
  ledmap[5*8+7] = LED_RED_SEQ24;

  ledmap[6*8+0] = LED_GREEN_SEQ09;
  ledmap[6*8+1] = LED_RED_SEQ09;
  ledmap[6*8+2] = LED_GREEN_SEQ10;
  ledmap[6*8+3] = LED_RED_SEQ10;
  ledmap[6*8+4] = LED_GREEN_SEQ11;
  ledmap[6*8+5] = LED_RED_SEQ11;
  ledmap[6*8+6] = LED_GREEN_SEQ12;
  ledmap[6*8+7] = LED_RED_SEQ12;
  
  ledmap[7*8+0] = LED_GREEN_SEQ25;
  ledmap[7*8+1] = LED_RED_SEQ25;
  ledmap[7*8+2] = LED_GREEN_SEQ26;
  ledmap[7*8+3] = LED_RED_SEQ26;
  ledmap[7*8+4] = LED_GREEN_SEQ27;
  ledmap[7*8+5] = LED_RED_SEQ27;
  ledmap[7*8+6] = LED_GREEN_SEQ28;
  ledmap[7*8+7] = LED_RED_SEQ28;

  ledmap[8*8+0] = LED_GREEN_SEQ13;
  ledmap[8*8+1] = LED_RED_SEQ13;
  ledmap[8*8+2] = LED_GREEN_SEQ14;
  ledmap[8*8+3] = LED_RED_SEQ14;
  ledmap[8*8+4] = LED_GREEN_SEQ15;
  ledmap[8*8+5] = LED_RED_SEQ15;
  ledmap[8*8+6] = LED_GREEN_SEQ16;
  ledmap[8*8+7] = LED_RED_SEQ16;
  
  ledmap[9*8+0] = LED_GREEN_SEQ29;
  ledmap[9*8+1] = LED_RED_SEQ29;
  ledmap[9*8+2] = LED_GREEN_SEQ30;
  ledmap[9*8+3] = LED_RED_SEQ30;
  ledmap[9*8+4] = LED_GREEN_SEQ31;
  ledmap[9*8+5] = LED_RED_SEQ31;
  ledmap[9*8+6] = LED_GREEN_SEQ32;
  ledmap[9*8+7] = LED_RED_SEQ32;

}
#endif