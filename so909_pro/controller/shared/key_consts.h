#ifndef _KEY_CONST_H
#define _KEY_CONST_H

// key directions
#define KEYDIR_DOWN 1
#define KEYDIR_UP 0

// steps
#define KEY_SEQ01 0
#define KEY_SEQ02 1
#define KEY_SEQ03 2
#define KEY_SEQ04 3
#define KEY_SEQ05 4
#define KEY_SEQ06 5
#define KEY_SEQ07 6
#define KEY_SEQ08 7

#define KEY_SEQ09 8
#define KEY_SEQ10 9
#define KEY_SEQ11 10
#define KEY_SEQ12 11
#define KEY_SEQ13 12
#define KEY_SEQ14 13
#define KEY_SEQ15 14
#define KEY_SEQ16 15

#define KEY_SEQ17 16
#define KEY_SEQ18 17
#define KEY_SEQ19 18
#define KEY_SEQ20 19
#define KEY_SEQ21 20
#define KEY_SEQ22 21
#define KEY_SEQ23 22
#define KEY_SEQ24 23

#define KEY_SEQ25 24
#define KEY_SEQ26 25
#define KEY_SEQ27 26
#define KEY_SEQ28 27
#define KEY_SEQ29 28
#define KEY_SEQ30 29
#define KEY_SEQ31 30
#define KEY_SEQ32 31

// play control keys
#define KEY_TAP_TEMPO 32
#define KEY_REC 33
#define KEY_STOP 34
#define KEY_PLAY 35
#define KEY_SOLO 36
#define KEY_MUTE 37

// instrument select
#define KEY_BASS 38
#define KEY_SNARE 39
#define KEY_LOW 40
#define KEY_MID 41
#define KEY_HI 42
#define KEY_RIMSHOT 43
#define KEY_HANDCLAP 44
#define KEY_HH_CLOSED 45
#define KEY_HH_OPEN 46
#define KEY_CYMBAL_CRASH 47
#define KEY_CYMBAL_RIDE 48
#define KEY_TRACK_12 49

// edit
#define KEY_CUT 50
#define KEY_COPY 51
#define KEY_PASTE 52
#define KEY_SWAP 53

// numpad
#define KEY_0 54
#define KEY_1 55
#define KEY_2 56
#define KEY_3 57
#define KEY_4 58
#define KEY_5 59
#define KEY_6 60
#define KEY_7 61
#define KEY_8 62
#define KEY_9 63
#define KEY_SHIFT 64

// note modifiers
#define KEY_FLAM 65
#define KEY_VELOCITY 66
#define KEY_ACCENT 67
#define KEY_ENTER 68

//arrow keys
#define KEY_LEFT 69
#define KEY_UP 70
#define KEY_DOWN 71
#define KEY_RIGHT 72

//added to keys upon release, making it possible to send key release events.
#define KEY_RELEASE_OFFSET 128

#endif