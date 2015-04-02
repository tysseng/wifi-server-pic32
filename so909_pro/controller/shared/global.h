#ifndef _GLOBAL_H
#define _GLOBAL_H

#define G_MIDI_isSysexAddressCorrect GLOBAL_statusBits1.F0
#define G_MIDI_shouldFlushSysexBuffer GLOBAL_statusBits1.F1
#define G_MIDI_isInPlayMode GLOBAL_statusBits1.F2
#define G_MIDI_isExternalSynchOn GLOBAL_statusBits1.F3
#define G_CMD_repeatLastKey GLOBAL_statusBits1.F4
#define G_KBD_rescanKeys GLOBAL_statusBits1.F5
#define G_KBD_keyscanComplete GLOBAL_statusBits1.F6
#define G_MCU_firstSixteenth GLOBAL_statusBits1.F7

#define G_MCU_incrStep GLOBAL_statusBits2.F0
#define G_MCU_endOfNoteReached GLOBAL_statusBits2.F1
#define G_CB_cursorOn GLOBAL_statusBits2.F2
#define G_CB_areaOn GLOBAL_statusBits2.F3 // 1 if area is larger than a single square
#define G_NAV_shiftOn GLOBAL_statusBits2.F4
#define G_SEQ_isRecording GLOBAL_statusBits2.F5
#define G_PGSO_songLoop GLOBAL_statusBits2.F6
#define G_SEQ_enoughTaps GLOBAL_statusBits2.F7

#define G_PGPF_patternLoadEnabled GLOBAL_statusBits3.F0
#define G_SEQ_resetBpmOnLoad GLOBAL_statusBits3.F1

#include "definitions.h"

extern char GLOBAL_statusBits1;
extern char GLOBAL_statusBits2;
extern char GLOBAL_statusBits3;
extern char name[NAME_LENGTH+1];

extern char G_MIDI_sysexIdByte1;
extern char G_MIDI_sysexIdByte2;

extern void GLOBAL_resetName();
extern void GLOBAL_nop();
#endif
[ACTIVE_TAB]
Value=global.c
