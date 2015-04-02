#ifndef _MIDI_H
#define _MIDI_H

#define MIDI_MASK_STATUS_BYTE 0x80

#define MIDI_STATUS_NOTE_OFF 0x80
#define MIDI_STATUS_NOTE_ON 0x90
#define MIDI_STATUS_SYSEX_START 0xF0
#define MIDI_STATUS_SONG_POSITION 0xF2
#define MIDI_STATUS_SONG_SELECT 0xF3
#define MIDI_STATUS_SYSEX_END 0xF7

// Playback status
#define MIDI_STATUS_MIDI_CLOCK 0xF8
#define MIDI_STATUS_MIDI_START 0xFA
#define MIDI_STATUS_MIDI_CONTINUE 0xFB
#define MIDI_STATUS_MIDI_STOP 0xFC


extern int MIDI_byteCounter;
extern char MIDI_lastStatus;
extern char MIDI_sysexBuffer[256];

void MIDI_init();
void MIDI_sendNote( unsigned short channel, unsigned short note, unsigned short velocity );
void MIDI_sendNoteOff( unsigned short channel, unsigned short note, unsigned short velocity );
void MIDI_sendAllOff();
void MIDI_sendSysexStart();
void MIDI_sendSysexEnd();
#endif