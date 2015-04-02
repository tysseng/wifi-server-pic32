#include "midi.h"
#include "../shared/global.h"
#include "../shared/asm_usart.h"

int MIDI_byteCounter;
char MIDI_lastStatus;
char MIDI_sysexBuffer[256];


/**
 * Init usart and midi settings
 **/
void MIDI_init(){
//  UART1_Init(31250);
  G_MIDI_isSysexAddressCorrect = 0;
  G_MIDI_isInPlayMode = 0;
  G_MIDI_shouldFlushSysexBuffer = 0;
}

/**
 * Send a single midi 'note on'
 **/
void MIDI_sendNote( unsigned short channel, unsigned short note, unsigned short velocity ) {
//  UART1_Write(0x90 + channel);     // send 'note on'
//  UART1_Write(note);
//  UART1_Write(velocity);
}

/**
 * Send a single midi 'note off'
 **/
void MIDI_sendNoteOff( unsigned short channel, unsigned short note, unsigned short velocity ) {

//  UART1_Write(0x80 + channel);     // send 'note off'
//  UART1_Write(note);
//  UART1_Write(velocity);*/
}

void MIDI_sendAllOff(){
//  UART1_Write(0x7B);     // send 'all off'
}

void MIDI_sendSysexStart(){
  ASM_USART_Write(MIDI_STATUS_SYSEX_START);
  ASM_USART_Write(0x00);
  ASM_USART_Write(G_MIDI_sysexIdByte1);
  ASM_USART_Write(G_MIDI_sysexIdByte2);
}

void MIDI_sendSysexEnd(){
  ASM_USART_Write(MIDI_STATUS_SYSEX_END);
}