#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

//External storage
#define DONT_SEND_ACK 0
#define SEND_ACK 1

//sequencer setup
#define TICKS_USED_BY_INTERRUPT 5

//track volume states
#define STATE_NONE 0
#define STATE_MUTE 1
#define STATE_SOLO 2
#define NO_SOLO -1

//play state
#define PLAY_STOPPED   0
#define PLAY_PLAYING   1
#define PLAY_PAUSED    2

//play mode
#define PLAY_MODE_PATTERN 0
#define PLAY_MODE_PERFORMANCE 1
#define PLAY_MODE_SONG 2

//flam modes
#define NO_FLAM -1

//Controller timers
#define TIMER0_SOURCE_TAP_TEMPO 0
#define TIMER0_SOURCE_KEY_REPEAT 1
#define TIMER0_SOURCE_DIAL_SPEED 2

//File page: different items in the file browser
#define FILE_ITEM_SONG 8
#define FILE_ITEM_PERFORMANCE 9
#define NO_FILE_FOCUS -1

//Setup page
#define RESET_POSITIONS 1
#define DONT_RESET_POSITIONS 0
#define SETUP_ITEM_GENERAL 0
#define SETUP_ITEM_PATTERN 1

// what part of the screen has the focus
#define FOCUS_LEFT 0
#define FOCUS_RIGHT 1

// input field focus
#define NO_INPUT_FOCUS 0

// navigation
#define NUMBER_OF_MODES 5

// pages
#define PAGE_NONE 100
#define PAGE_PATT 0
#define PAGE_SONG 1
#define PAGE_PERFORMANCE 2
#define PAGE_FILE 3
#define PAGE_SETUP 4
#define PAGE_BARS 5

// dialog boxes
#define DIALOG_NO_MENU_ITEM 0
#define DIALOG_NO_DIALOG 0
#define DIALOG_CHANGE_FOLDER 1
#define DIALOG_CHANGE_FILE 2
#define DIALOG_SAVE_FILE 3
#define DIALOG_RENAME 4
#define DIALOG_NAME_NEW_FILE 5
#define DIALOG_CHANGE_PATTERN 6

// currently selected file/pattern in sequencer
#define NOT_SELECTED 0

// song page
#define SONG_ELEMENTS_PER_PAGE 12

// performance page
#define PERF_ELEMENTS_PER_PAGE 10

// interrupt control
#define DISABLE_ALL_INTERRUPTS INTCON.GIE=0
#define ENABLE_ALL_INTERRUPTS INTCON.GIE=1

// file name length
#define NAME_LENGTH 16

// sequencer setup
#define MAX_TRACKS 1
#define MAX_STEPS 32
#define MAX_SONG_PATTERNS 128
#define MAX_KEY_MAP_SIZE 32

// Speed/rhythm control
#define BPM_MAX 240
#define BPM_MIN 60
#define SWING_MAX 90
#define SWING_MIN 10

// step modes, is step active or disabled
#define STEP_OFF 0
#define STEP_ON 1

// number of taps used when calculating bpm based on tap tempo button
#define TAPS_REQUIRED 4
#endif