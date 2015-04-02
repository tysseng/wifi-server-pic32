#include <built_in.h>
#include "controller.h"
#include "command_handler.h"
#include "GLCD/GLCD.h"
#include "GLCD/sequencer_visualisation.h"
#include "keyboard/keyboard.h"
#include "keyboard/dials.h"
#include "keyboard/leds.h"
#include "keyboard/multitap.h"
#include "sequencer/navigation.h"
#include "sequencer/sequencer.h"
#include "sequencer/page_performance.h"
#include "sequencer/page_file.h"
#include "sequencer/page_flam_vel.h"
#include "sequencer/page_setup.h"
#include "sequencer/page_song.h"
#include "sequencer/page_pattern.h"
#include "midi/midi.h"
#include "clipboard/clipboard.h"
#include "storage/external_storage.h"
#include "shared/global.h"
#include "shared/types.h"
#include "shared/definitions.h"
#include "../shared/bus_definitions.h"

/*
Known bugs:
- Performance mode: dialog box action funker av og til ikke???
- Flam er på en eller annen måte omvendt - en flam 2 - flam 0 ligger lenger
  fra hverandre enn en flam 0 - flam 0, mens de egentlig skal ligge tettere.
- Skjerm resettes av og til???

Todo:
- Progress bar for sysex dump
- Auto-lagre pattern når på pattern-side og ikke spiller.
- Bytte declare med const char e.l.?
- Mangler mulighet til å endre navn på track fra bestemt tromme til track xx
- dial speed - raskere når den vris raskere rundt.
- bytte ut folderNum fileNum med pattNum?
- test sysex import.
- test if external synch is working and fast enough

Missing features:
- Send midi sync
*/

// state variables;
char pulseCounter = 0;
char MCU_timer0Source;

void interrupt() {
/*
  char midiData;

  if(PIR1.TMR1IF){
    // TIMER 1 - internal tempo clock
    // TODO: To get a correct clock, we need to substract the time used
    // for the if clause and the setting of the interrupt.

    // 20 instruction cycles should be extracted from the pulse timer

    TMR1H = pulseTimerStartH;
    TMR1L = pulseTimerStartL;

    PIE1.TMR1IE = 1;
    PIR1.TMR1IF = 0;

    MCU_IRQ_playNotesAndStepNext(1);

  } else if(INTCON.T0IF){
    // TIMER 0 - tap tempo timer
    // if an interrupt has been triggered, it means that the user has stopped
    // tapping the tempo or is tapping too slow. Stop timer and indicate that
    // there are not enough correct samples to set a new bpm.
    
    switch(MCU_timer0Source){
    case TIMER0_SOURCE_TAP_TEMPO:
      G_SEQ_enoughTaps = 0;
      currentTap = -1;
      T0CON = 0x06;
      INTCON = 0xE0; // Set GIE, PEIE, T0IE, clear T0IF
      break;
    case TIMER0_SOURCE_KEY_REPEAT:
      G_CMD_repeatLastKey = 1;
      MCU_restartKeyRepeatTimer();
      break;
    case TIMER0_SOURCE_DIAL_SPEED:
      break;
    }
  } else if (PIR1.RCIF) {
    // MIDI input
    
    midiData = RCREG;
    if( midiData & MIDI_MASK_STATUS_BYTE ) {
      MCU_IRQ_handleMidiStatusByte(midiData);
    } else {
      MCU_IRQ_handleMidiDataByte(midiData);
    }
    
    //TODO: DO WE HAVE TO RESET INTERRUPT HERE?
  }*/
}

void MCU_IRQ_handleMidiStatusByte(char midiData){
  MIDI_lastStatus = midiData;
  MIDI_byteCounter = 0;

  switch(midiData){
  case MIDI_STATUS_MIDI_CLOCK:
    if(G_MIDI_isInPlayMode) MCU_IRQ_playNotesAndStepNext(2);
    break;
  }
}

void MCU_IRQ_handleMidiDataByte(char midiData){
  MIDI_byteCounter++;

  if(MIDI_lastStatus == MIDI_STATUS_SYSEX_START){
    MCU_IRQ_handleSysexData(midiData);
  }
}

//
// data import from PC using sysex
//
void MCU_IRQ_handleSysexData(char sysexData){
  switch(MIDI_byteCounter){
  case 1:
    G_MIDI_shouldFlushSysexBuffer = 0;
    if(sysexData == 0){
      G_MIDI_isSysexAddressCorrect = 1;
    } else {
      G_MIDI_isSysexAddressCorrect = 0;
    }
    break;
  case 2:
    if(sysexData != G_MIDI_sysexIdByte1){
      G_MIDI_isSysexAddressCorrect = 0;
    }
    break;
  case 3:
    if(sysexData != G_MIDI_sysexIdByte2){
      G_MIDI_isSysexAddressCorrect = 0;
    }
    if(G_MIDI_isSysexAddressCorrect){
      EEX_importMemoryFromMidiStart();
    }
    break;
  default:
    MIDI_sysexBuffer[MIDI_byteCounter-4] = sysexData;
    if(MIDI_byteCounter - 4 == 255){
      G_MIDI_shouldFlushSysexBuffer = 1;
      MIDI_byteCounter = 3;
    }
 }
}

/**
 * counterIncrease should be 1 when we're using a 48ppqn counter
 * (internal timer), and 2 when using 24ppqm (Midi clock)
 */
void MCU_IRQ_playNotesAndStepNext(char counterIncrease){
    if(pulseCounter == pulseInterval){
      pulseCounter = 0;
      G_MCU_incrStep = 1;

      // after playing last pulse of a 16th interval,
      // adjust the length of pulseTimer to accomodate swing
      if(G_MCU_firstSixteenth){
        pulseTimerStartH = Hi(pulseTimerStartSecondSwing);
        pulseTimerStartL = Lo(pulseTimerStartSecondSwing);
        G_MCU_firstSixteenth=0;
      } else {
        pulseTimerStartH = Hi(pulseTimerStartFirstSwing);
        pulseTimerStartL = Lo(pulseTimerStartFirstSwing);
        G_MCU_firstSixteenth=1;
      }
    }


    // play note. check every note for flam.  (flam is 24ppqn, we use 48 for the
    // counters
    if(pulseCounter % 2 == 0){
      SEQ_playNotes(pulseCounter / 2);
    }

    pulseCounter = pulseCounter + counterIncrease;

    // Check if we have reached the end of the current note:
    if(pulseCounter == pulseInterval){
      G_MCU_endOfNoteReached = 1;
    }
  }
  
/**
 * Set special registers necessary for proper operation, as well as permanent
 * port directions for address lines
 **/
void MCU_init(){
  //Necessary to access individual bits on ports
  ADCON0 = 0; // turn off A/D converter
  CMCON |= 0x07; //turn off comparators

  //ADCON1 = 0x07; //turn off analogue inputs (for P18F458)
  ADCON1 = 0x0F;   //turn off analogue inputs (for P18F4620)
  CCP1CON = 0 ;    //disable the enhanced capture/PWM module (for P18F4620)


  ADDRESS_BUS_DIRECTION.F0 = DATA_OUT;
  ADDRESS_BUS_DIRECTION.F1 = DATA_OUT;
  ADDRESS_BUS_DIRECTION.F2 = DATA_OUT;
  ADDRESS_BUS_DIRECTION.F3 = DATA_OUT;
  KBD_DISABLE_DIRECTION = DATA_OUT;
  LED_ENABLE_DIRECTION = DATA_OUT;
  DIAL_ENABLE_DIRECTION = DATA_OUT;
  TRISE.F1 = DATA_OUT; //debug led 1
  TRISE.F2 = DATA_OUT; //debug led 2
  PORTE.F1 = 0;
  PORTE.F2 = 0;
  
  G_MCU_incrStep = 0;
  G_MCU_firstSixteenth = 1;
  G_MCU_endOfNoteReached = 0;

}

void MCU_initTimers(){
  // Set timer0 to 16 bit and assign a prescaler of 128.
  T0CON = 0x06;
  TMR0H = 0;
  TMR0L = 0;

  // Enable GIE (all interrupt sources)
  // Enable PEIE (all peripheral interrupt sources - timer1, 2, 3 etc)
  // Enable T0IE (enable TMRO interrupt)
  // Disable T0IF (clear TMR0 interrupt)
  INTCON = 0xE0;

  // Timer1 - PPQN timer, main midi/system clock.
  // Set to 16 bit and assigned a prescaler of 4.
  T1CON = 0xA4; // Set to 16 bit, prescaler of 4 and timer stopped
  PIR1.TMR1IF = 0; // clear interrupt
  PIE1.TMR1IE = 1; // enable interrupt

  MCU_resetPulseTimer();
}

/**
 * Starts pulse timer if external synch is not on
 **/
void MCU_startPulseTimer(){
  if(G_MIDI_isExternalSynchOn == 0){
    T1CON.TMR1ON = 1;
  }
}

/**
 * Stops pulse timer
 **/
void MCU_stopPulseTimer(){
  T1CON.TMR1ON = 0;
}

/**
 * Set pulse timer counter to starting value
 **/
void MCU_resetPulseTimer(){
  pulseTimerStartH = Hi(pulseTimerStartFirstSwing);
  pulseTimerStartL = Lo(pulseTimerStartFirstSwing);
  G_MCU_firstSixteenth = 1;
  pulseCounter = 0;

  TMR1H = 0xFF;
  TMR1L = 0xFF;
}

void MCU_startTapTempoTimer(){
  T0CON = 0x86;
  TMR0H = 0;
  TMR0L = 0;
  MCU_timer0Source = TIMER0_SOURCE_TAP_TEMPO;
}

//
// Startsthe key repeat timer. Because consecutive delays should be smaller
// than the first delay, we need a separate restart method
//
void MCU_startKeyRepeatTimer(){

  // first delay is approx 0.4s
  T0CON = 0x86;
  TMR0H = 0xC0;
  TMR0L = 0x00;
  MCU_timer0Source = TIMER0_SOURCE_KEY_REPEAT;
}

//
// Restarts the key repeat timer after the first repeat. Because consecutive
// delays should be smaller than the first delay, we need a separate restart
// method
//
void MCU_restartKeyRepeatTimer(){

  // consecutive delays are approx 0.09s
  T0CON = 0x86;
  INTCON = 0xE0; // Set GIE, PEIE, T0IE, clear T0IF
  TMR0H = 0xF2;
  TMR0L = 0x00;
}

void MCU_stopKeyRepeatTimer(){
  T0CON = 0x06;
}

//
// These status bytes are handled outside of the interrupt routine because
// they rely heavily on other methods, leading to all sorts of reentrancy
// issues.
//
void MCU_handleExternalSynchControl(){
  if(MIDI_lastStatus != 0 && MIDI_lastStatus != MIDI_STATUS_MIDI_CLOCK){
    switch(MIDI_lastStatus){
    case MIDI_STATUS_MIDI_START:
      MCU_prepareForStart();
      break;
    case MIDI_STATUS_MIDI_CONTINUE:
      MCU_continuePlayback();
      break;
    case MIDI_STATUS_MIDI_STOP:
      MCU_pausePlayback();
      break;
    }
    MIDI_lastStatus = 0;
  }
}

//
// Externally triggered sequencer continue
//
void MCU_continuePlayback(){
  // don't enter play mode if external synch is off.
  G_MIDI_isInPlayMode = G_MIDI_isExternalSynchOn;

  // if a continue message is received, the sequencer is expected to be
  // paused by an earlier MIDI_STOP message. If this is not the case, we
  // might as well not play the song, as the song position has probably
  // been messed up.
  if(G_MIDI_isInPlayMode && sequencer.playState == PLAY_PAUSED){
    if(currPage == PAGE_SONG){
      PGSO_playOrLoad();
    } else if(currPage == PAGE_PERFORMANCE){
      PGPF_togglePlayPause();
    } else if(currPage == PAGE_PATT){
      PGP_togglePlayPause();
    }
  }
}

//
// Externally triggered sequencer stop
//
void MCU_pausePlayback(){
  if(G_MIDI_isInPlayMode){
    G_MIDI_isInPlayMode = 0;
    if(currPage == PAGE_SONG){
      PGSO_playOrLoad();
    } else if(currPage == PAGE_PERFORMANCE){
      PGPF_togglePlayPause();
    } else if(currPage == PAGE_PATT){
      PGP_togglePlayPause();
    }
  }
}

//
// When a midi start message is received, we need to reset the sequencer and
// disable the timer to prepare for external clock signals.
//
void MCU_prepareForStart(){
  // don't enter play mode if external synch is off.
  G_MIDI_isInPlayMode = G_MIDI_isExternalSynchOn;
  if(G_MIDI_isInPlayMode){
    if(currPage == PAGE_SONG){
      PGSO_stopAndReset();
      PGSO_playOrLoad(); // Load pattern
      PGSO_playOrLoad(); // Enter play mode
    } else if(currPage == PAGE_PERFORMANCE){
      PGPF_stop();
      PGPF_togglePlayPause();
    } else if(currPage == PAGE_PATT){
      PGP_stop();
      PGP_togglePlayPause();
    }
  }
}


/**
Må fikses i pro-kode:

Kommentert ut ASM_USART_Write

Byttet F1 med B1

Kommentert ut UART1_Write fra midi.c

Minneproblemer:

1 track tar 139 bytes.
dvs. det er plass til 8 tracks, ikke 12.
12 tracks tar 1668 bytes

Pga bakgrunnslasting av tracks er det i praksis 24 tracks, samt 1 for clipboard.
Dvs. 1 track tar 66,72 bytes (i praksis 2 bytes pr. step). Mindre kan det ikke bli.

Det er 1236 bytes igjen til tracks. Dvs. 1 track må maks ta 49 bytes!
 */

/**
 * Doowoop, read keys and write leds. Forever and ever.
 **/
void main(){

  unsigned short row=0;
  unsigned short timer = 0;
  unsigned short currLed = 0;

  MCU_init();
  MIDI_init();
  CB_init();
  GLCD_CTRL_initPorts();

  CMD_init();
  KBD_init();
  DIAL_init();
  LED_init();
  
  LED_off(4);

  EEX_init();
  SEQ_init();
  NAV_init();

  // prepare display
  GLCD_CTRL_init();

  SEQ_VIS_splashScreen();
  SEQ_VIS_loadCharsFromStorage();

  PGFV_initPage();
//  PGS_initPage();

  PGF_initPage();
//  PGPF_initPage();
//  PGSO_initPage();
  MCU_initTimers();

//  PGSO_showPage();
  PGP_showPage(RESET_POSITIONS);
//  PGF_showPage(1);


  while(1){

    for(row = 0; row < ROWCOUNT; row++){
      LED_writeLeds(row);
//      KBD_read(row);
    }

    if(G_CMD_repeatLastKey){
     // CMD_repeatLastKey();
    }

    if(G_MCU_endOfNoteReached){
      SEQ_performEndOfNoteActions();
      G_MCU_endOfNoteReached = 0;
    }

    if(G_MCU_incrStep){
      SEQ_incrementStep();
      if(currPage != PAGE_FILE && currPage != PAGE_SETUP){
        SEQ_VIS_writeTime(0);
      }
      G_MCU_incrStep = 0;
    }

    if(G_MIDI_shouldFlushSysexBuffer && G_MIDI_isSysexAddressCorrect){
      EEX_importMemoryFromMidi(MIDI_sysexBuffer);
      G_MIDI_shouldFlushSysexBuffer = 0;
      G_MIDI_isSysexAddressCorrect = 0;
    }
    
    // check if we need to perform any external synch
    // controlled playback changes
    MCU_handleExternalSynchControl();
    
    DIAL_read();
  }
}