#include "mpg200.h"

//PG200 TX type
#define TYPE_ADDR 1
#define TYPE_DATA 0
#define TYPE_IGNORED 0

//RX ring buffer length. Must not exceed 256 bytes...
#define RX_LEN 256

//midi modes
#define MIDI_NOTE_OFF 0x80
#define MIDI_NOTE_ON 0x90
#define MIDI_MODE_CC 0xB0

//midi routing modes
#define MIDI_ROUTE_TO_THRU 0
#define MIDI_ROUTE_TO_PG200 1

//Constants for all PG200 controllers
#define NUMBER_OF_SWITCHES 16
#define NUMBER_OF_CONTROLLERS 34
#define SW_A_RANGE 0
#define SW_A_WAVE 1
#define SW_A_LFO 2
#define SW_A_ENV 3
#define SW_B_RANGE 4
#define SW_B_WAVE 5
#define SW_B_LFO 6
#define SW_B_ENV 7
#define SW_SYNC 8
#define SW_VCF_POL 9
#define SW_VCA 10
#define SW_CHORUS 11
#define SW_DCO_POL 12
#define SW_LFO_WAVE 13
#define SW_MANUAL 14
#define SW_WRITE 15
#define POT_B_FINE 16
#define POT_B_TUNE 17
#define POT_DCO_ENV 18
#define POT_DCO_LFO 19
#define POT_MIX 20
#define POT_HPF 21
#define POT_RESONANCE 22
#define POT_CUTOFF 23
#define POT_VCF_ENV 24
#define POT_VCF_LFO 25
#define POT_PITCH_FOLLOW 26
#define POT_LEVEL 27
#define POT_LFO_RATE 28
#define POT_LFO_DELAY 29
#define POT_A 30
#define POT_D 31
#define POT_S 32
#define POT_R 33

// When starting up, the PG_200 needs to tell the JX-3P that it is connected.
// It does this by sending a special message with the following address and
// data bits
#define PG200_PING_ADDR 128
#define PG200_PING_DATA 0

//JX3P Busy data line
#define STATUS_TRIS TRISC5_bit
#define STATUS_PORT PORTC.F5 //Change this to real data line later. (PORTC.F5)
#define JX3P_IS_BUSY 0

//JX3P RXMODE line
#define TXMODE_TRIS TRISC4_bit
#define TXMODE_PORT PORTC.F4 //Change this to real data line later. (PORTC.F5)
#define TXMODE_MIDI 1
#define TXMODE_PG200 0

//Output modes - how to treat the mixing of midi and pg-200 signals
#define OUTPUTMODE_BLOCK_MIDI 0
#define OUTPUTMODE_REVERT_TO_MIDI 1
#define OUTPUTMODE_INSTANT_SWITCH 2

//COMMENTS:
//- Midi note messages and cc messages must not be mixed (e.g. they must originate from the same source) to prevent problems with running statuses. This should not really be a problem in real life though.
//- Programmer switch must be in MIDI BUS position for both midi and pg-200 to work at the same time.

//TODO: 
//- Change counters when changing Xtal!
//- Reset counter to 0 if an unknown CC number is received
//- Allow variable boundaries for switch midi input
//- Check if we need to send not 2*midi value but 2*midi value +1 or something, to get proper tuning from centerable pots like osc2 tune.
//- remove bytes not addressed for this device
//- change midi channel
//- update map on sysex (write to flash memory)
//- read map from flash on startup
//- check that switches are correctly implemented (0 vs. 1)
//- implement Manual and Write
//- Send initial button states if they haven't yet been sent (on startup - but only the first time parts of a button changes..
//  May be able to just send a button state whenever a button state message arrives, and use the same
//  mask every time instead of calculating it.
//- BUG: Any subsequent controller that is sent that does not match any known pg200 controller, will work on the previous controller recognized. Set currentController to unknown and check for this.

// maps all of the 127 possible midi cc addresses to their corresponding
// pg-200 address - if any.
char pg200midiToPg[127];

// address to send to the jx-3p for a particular pg200 controller (switch, button or pot)
char pg200address[NUMBER_OF_CONTROLLERS];

// current switch states, used for calculating the necessary mask to send.
// Only three state bytes are needed (representing the three scan lines in the
// physical PG-200 switch grid)
char pg200switchStates[3];
// indicates how many shift-left operations need to be carried out on the
// calculated switch byte for a particular switch
char pg200switchShifts[NUMBER_OF_SWITCHES];
// indicates how many states a particular switch may have
char pg200switchStatesPossible[NUMBER_OF_SWITCHES];
// counts midi bytes after a CC init has been received
char midiByteCounter;

// the last pg200 controller received
char lastPg200Controller;
// the JX3P address of the last pg200 controller received
char lastPg200Address;

//ring buffer for receiving midi data
char rxBuffer[RX_LEN];
//The last position in the rxBuffer that was read from
char rxReadPtr;
//The last position in the rxBuffer thas was written to
char rxWritePtr;

//If MIDI_ROUTE_TO_THRU, incoming midi bytes should be passed on, not treated as PG200 data
//IF MIDI_ROUTE_TO_PG200, incoming midi bytes are treated as PG200 data
char midiRouting;

// last change from PG200 to MIDI mode has settled if this is 1
char clearToSendMidi = 1;

// if this is 1, mode has been switched from pg200 to midi and we are ready to send midi - so
// loop through the missing notes and send them before continuing "normal" midi sends.
char clearToSendMissingNoteOffs = 0;

// first byte of a midi message has been sent, so clear to send the remaining bytes.
char hasSentFirstMidiByte = 0;

// counter to count times timer has timed out, necessary because timer0 is too fast.
char timer0TimeoutCount = 0;

// array that holds notes that should have been turned off but that weren't because
// the mpg200 was in pg200 mode when the off messages arrived. As soon as it reverts
// back to midi, it should transmit these messages.
char missingNoteOffs[127];
char hasMissingNoteOffs = 0;

// last received midi status and first parameter (e.g. key for note on/off)
char lastMidiStatus = 0;
char lastMidiParam1 = 0;

/**** SYSEX input ****/

//possible sysex operations:
#define SYSEX_OP_NONE 0
#define SYSEX_OP_WRITE_SETTINGS_TO_EE 1
#define SYSEX_OP_CLEAR_SETTINGS_FROM_EE 2
#define SYSEX_OP_CHANGE_SETTING 3

// status of current sysex reception, what was the last operation and parameters
// received.
char currentSysexOperation = 0;
char currentSysexParam1 = 0;

// are we currently receiving sysex data?
bit inSysexMode;

// is current sysex stream intended for this device?
bit sysexForThisDevice;

//index of sysex byte after sysex status
unsigned int sysexByteCounter = 0;

// cyclic counter counting bytes after sysex address, reset to 0 when one sysex
// operation has been completed (operation length in bytes may depend)
char sysexDataCounter = 0;
char sysexAddress[] = {0, 43, 102}; //randomly chosen but must start with 0

/**** SETTINGS  ****/

#define POS_SETTINGS_IN_EE 0
#define POS_MIDI_CH 1
#define POS_OUT_MODE 2
#define POS_CONTROLLERS 3
#define POS_SWITCH_TO_MIDI_TIMER_OVERFLOWS 37
#define SETTINGS_LENGTH 38

char settings[] = {
  0, // default: no settings in ee. If any settings are found in ee memory on startup, they are used instead.
  0, // default midi channel
  OUTPUTMODE_REVERT_TO_MIDI, //default output mode

  //Default midi mapping
  //switches
  72,  // SW_A_RANGE
  73,  // SW_A_WAVE
  82,  // SW_A_LFO
  81,  // SW_A_ENV
  74,  // SW_B_RANGE
  75,  // SW_B_WAVE
  80,  // SW_B_LFO
  79,  // SW_B_ENV
  76,  // SW_SYNC
  77,  // SW_VCF_POL
  78,  // SW_VCA
  85,  // SW_CHORUS
  84,  // SW_DCO_POL
  83,  // SW_LFO_WAVE
  127, // SW_MANUAL
  127, // SW_WRITE

  //Pots
  12, // POT_B_FINE
  13, // POT_B_TUNE
  14, // POT_DCO_ENV
  15, // POT_DCO_LFO
  16, // POT_MIX
  17, // POT_HPF
  18, // POT_RESONANCE
  19, // POT_CUTOFF
  20, // POT_VCF_ENV
  21, // POT_VCF_LFO
  22, // POT_PITCH_FOLLOW
  23, // POT_LEVEL
  24, // POT_LFO_RATE
  25, // POT_LFO_DELAY
  26, // POT_A 30
  27, // POT_D 31
  28, // POT_S 32
  29, // POT_R 33
  
  4, //DEFAULT_SWITCH_TO_MIDI_TIMER_OVERFLOWS - 20MHz: 4 timeouts approx 50ms delay after PG-200 before switching to midi (+ another 26ms before midi can be sent).
};

/**** PROGRAM START ****/

void interrupt(){
  char midiByte;
  char midiStatus;

  if (PIR1.RCIF){
    midiByte = RCREG;
    writeToRxBuffer(midiByte);
  } else if (TMR0IF_bit){
    //timer0 is too fast, we need to let it overflow several times before
    //we can get a long enough delay
    TMR0IF_bit = 0;
    if(timer0TimeoutCount < settings[POS_SWITCH_TO_MIDI_TIMER_OVERFLOWS] ){
      PORTC.F3 = 1;
      timer0TimeoutCount++;
      TMR0H = 0;
      TMR0L = 0;
    } else {
      timer0TimeoutCount = 0;
      PORTC.F3 = 0;
      switchToMidi();
      TMR0ON_bit = 0;
    }
  } else if (TMR1IF_bit){
    TMR1IF_bit = 0;
    TMR1ON_bit = 0;
    clearToSendMissingNoteOffs = 1; // ready to send midi, indicate that we should send any missing notes off (which in turn should set clearToSend to 1 to commence midi processing
  }
}

void startSetClearToSendTimer(){
  // timer1 has a max timeout of 105 ms at 20MHz.
  // With a 1:2 prescaler, the delay is 26ms.
  TMR1ON_bit = 1;
  TMR1H = 0;
  TMR1L = 0;
}

void switchToMidi(){
  clearToSendMidi = 0;
  TXMODE_PORT = TXMODE_MIDI;
  startSetClearToSendTimer();
}

void startSwitchToMidiTimer(){
  //if already running, do nothing.
  if(TMR0ON_bit == 0){
    // timer0 has a max timeout of 13 ms at 20MHz
    TMR0H = 0;
    TMR0L = 0;

    TMR0ON_bit = 1;
  }
}

void stopSwitchToMidiTimer(){
    TMR0ON_bit = 0;
}

// send any note off messages that were lost while the mpg200 was in pg200 mode.
void sendMissingNoteOffs(){
  char i=0;
  clearToSendMissingNoteOffs = 0;
  if(hasMissingNoteOffs == 1){
    delay_ms(75);
    hasMissingNoteOffs = 0;
    for(i=0; i<127; i++){ //loop over all possible note offs.
      if(missingNoteOffs[i] == 1){
        missingNoteOffs[i] = 0;
        transmit(MIDI_NOTE_OFF, TYPE_IGNORED, TXMODE_MIDI); // transmit status message note off on midi channel 0 (the only one JX-3P can receive)
        transmit(i, TYPE_IGNORED, TXMODE_MIDI);             // transmit key
        transmit(0, TYPE_IGNORED, TXMODE_MIDI);             // transmit velocity zero
      }
    }
  }
  clearToSendMidi = 1;
}
  
void switchTxMode(char newMode){
  if(settings[POS_OUT_MODE] == OUTPUTMODE_BLOCK_MIDI){
    newMode = TXMODE_PG200;
  } else if(settings[POS_OUT_MODE] == OUTPUTMODE_REVERT_TO_MIDI){
    if(TXMODE_PORT != newMode){
      if(newMode == TXMODE_PG200){
        TXMODE_PORT = newMode;
        delay_ms(20); //wait 20 seconds for JX3P to detect mode change before trying to send PG200 message
      } else { // new mode is midi. wait a little in case more PG200 messages arrive in quick succession.
        // start / reset switch-to-midi timer
        startSwitchToMidiTimer();
      }
    } else {
      if(newMode == TXMODE_PG200){
        // stop any running switch-to-midi timers. A new one will be set once the data has been transmitted.
        stopSwitchToMidiTimer();
      }
    }
  } else { //mode == instant switch
    TXMODE_PORT = newMode;
  }
}

// midi cc-to-pg200 address mapping that works with the moog little phatty (sort of)
void loadLittlePhattyMidiMap(){

  pg200midiToPg[11]  = SW_B_WAVE;    //LP osc 2 wave: 11
  pg200midiToPg[74]  = SW_B_RANGE;   //LP osc 2 octave: 74
  pg200midiToPg[9]   = SW_A_WAVE;    // LP osc 1 wave: 9
  pg200midiToPg[75]  = SW_A_RANGE;   //LP osc 1 octave: 75
  pg200midiToPg[127] = SW_A_LFO;
  pg200midiToPg[127] = SW_A_ENV;
  pg200midiToPg[127] = SW_B_LFO;
  pg200midiToPg[127] = SW_B_ENV;
  pg200midiToPg[127] = SW_VCA;
  pg200midiToPg[127] = SW_VCF_POL;
  pg200midiToPg[127] = SW_SYNC;
  pg200midiToPg[127] = SW_CHORUS;
  pg200midiToPg[127] = SW_DCO_POL;
  pg200midiToPg[127] = SW_LFO_WAVE;
  pg200midiToPg[127] = SW_MANUAL;
  pg200midiToPg[127] = SW_WRITE;
  pg200midiToPg[127] = POT_B_FINE;
  pg200midiToPg[10]  = POT_B_TUNE;   //LP Osc 2 Freq: 10
  pg200midiToPg[15]  = POT_DCO_ENV;  //LP Osc 1 level: 15
  pg200midiToPg[16]  = POT_DCO_LFO;  //LP Osc 2 level: 16
  pg200midiToPg[127] = POT_MIX;
  pg200midiToPg[127] = POT_HPF;
  pg200midiToPg[21]  = POT_RESONANCE;//LP Filter Res: 21
  pg200midiToPg[19]  = POT_CUTOFF;   //LP Filter Cutoff: 19
  pg200midiToPg[27]  = POT_VCF_ENV;  //LP Filter Env Amt: 27
  pg200midiToPg[127] = POT_VCF_LFO;
  pg200midiToPg[127] = POT_PITCH_FOLLOW;
  pg200midiToPg[7]   = POT_LEVEL;    //LP Volume: 7
  pg200midiToPg[3]   = POT_LFO_RATE; //LP LFO Rate: 3
  pg200midiToPg[127] = POT_LFO_DELAY;
  pg200midiToPg[28]  = POT_A;        //LP Vol A: 28
  pg200midiToPg[29]  = POT_D;        //LP Vol D: 29
  pg200midiToPg[30]  = POT_S;        //LP Vol S: 30
  pg200midiToPg[31]  = POT_R;        //LP Vol R: 31
}

// read default midi cc-to-pg200 address mapping from settings array.
void loadDefaultMidiMap(){
  char i;
  for(i=0; i < NUMBER_OF_CONTROLLERS; i++){
    pg200midiToPg[settings[POS_CONTROLLERS + i]] = i;
  }
}

void loadPg200maps(){
  char i;
  for(i=0; i<127; i++){
    pg200midiToPg[i] = 255;
  }
  
  // dco 2 waveform
  pg200address[SW_B_WAVE] = 0;
  pg200switchShifts[SW_B_WAVE] = 6;
  pg200switchStatesPossible[SW_B_WAVE] = 4; //saw, pulse, square, noise

  // dco 2 range
  pg200address[SW_B_RANGE] = 0;
  pg200switchShifts[SW_B_RANGE] = 4;
  pg200switchStatesPossible[SW_B_RANGE] = 3; //16', 8', 4'

  // dco 1 waveform
  pg200address[SW_A_WAVE] = 0;
  pg200switchShifts[SW_A_WAVE] = 2;
  pg200switchStatesPossible[SW_A_WAVE] = 3; //saw, pulse, square

  // dco 1 range
  pg200address[SW_A_RANGE] = 0;
  pg200switchShifts[SW_A_RANGE] = 0;
  pg200switchStatesPossible[SW_A_RANGE] = 3; //16', 8', 4'

  // dco 1 frequency modulation from LFO
  pg200address[SW_A_LFO] = 1;
  pg200switchShifts[SW_A_LFO] = 7;
  pg200switchStatesPossible[SW_A_LFO] = 2; //Off, On

  // dco 1 frequency modulation from envelope
  pg200address[SW_A_ENV] = 1;
  pg200switchShifts[SW_A_ENV] = 6;
  pg200switchStatesPossible[SW_A_ENV] = 2; //Off, On

  // dco 2 frequency modulation from LFO
  pg200address[SW_B_LFO] = 1;
  pg200switchShifts[SW_B_LFO] = 5;
  pg200switchStatesPossible[SW_B_LFO] = 2; //Off, On

  // dco 2 frequency modulation from envelope
  pg200address[SW_B_ENV] = 1;
  pg200switchShifts[SW_B_ENV] = 4;
  pg200switchStatesPossible[SW_B_ENV] = 2; //Off, On

  // vca mode
  pg200address[SW_VCA] = 1;
  pg200switchShifts[SW_VCA] = 3;
  pg200switchStatesPossible[SW_VCA] = 2; //Gate, Env

  // vcf envelope polarity
  pg200address[SW_VCF_POL] = 1;
  pg200switchShifts[SW_VCF_POL] = 2;
  pg200switchStatesPossible[SW_VCF_POL] = 2; //Inverted, Normal

  // Cross modulation (Called "Sync" in service notes)
  pg200address[SW_SYNC] = 1;
  pg200switchShifts[SW_SYNC] = 0;
  pg200switchStatesPossible[SW_SYNC] = 3; //Off, Sync, Metal

  // Chorus
  pg200address[SW_CHORUS] = 2;
  pg200switchShifts[SW_CHORUS] = 3;
  pg200switchStatesPossible[SW_CHORUS] = 2; //Off, On

  // dco envelope polarity
  pg200address[SW_DCO_POL] = 2;
  pg200switchShifts[SW_DCO_POL] = 2;
  pg200switchStatesPossible[SW_DCO_POL] = 2; //Inverted, Normal

  // lfo waveform
  pg200address[SW_LFO_WAVE] = 2;
  pg200switchShifts[SW_LFO_WAVE] = 0;
  pg200switchStatesPossible[SW_LFO_WAVE] = 3; //Sine, Square, Random

  pg200address[SW_MANUAL] = 130; //chk - Manual
  pg200address[SW_WRITE] = 129; //chk - Write
  pg200address[POT_B_FINE] = 16; //chk - dco 2 fine tine
  pg200address[POT_B_TUNE] = 17; //chk - dco 2 tune
  pg200address[POT_DCO_ENV] = 18; //chk - dco 2 envelope amount
  pg200address[POT_DCO_LFO] = 19; //chk - dco 2 lfo amount
  pg200address[POT_MIX] = 20; //chk - vcf source mix
  pg200address[POT_HPF] = 21; //chk - vcf hpf
  pg200address[POT_RESONANCE] = 22; //chk - vcf resonance
  pg200address[POT_CUTOFF] = 23; //chk - vcf cutoff freq
  pg200address[POT_VCF_ENV] = 24; //chk - env mod
  pg200address[POT_VCF_LFO] = 25; //chk - lfo mod
  pg200address[POT_PITCH_FOLLOW] = 26; //chk - vcf pitch follow
  pg200address[POT_LEVEL] = 27; //chk - vca level
  pg200address[POT_LFO_RATE] = 28; //chk  - lfa rate
  pg200address[POT_LFO_DELAY] = 29; //chk - lfo delay time
  pg200address[POT_A] = 30; //chk - envelope attach
  pg200address[POT_D] = 31; //chk - envelope decay
  pg200address[POT_S] = 32; //chk - envelope sustain
  pg200address[POT_R] = 33; //chk - envelope release
  
  // default midi cc-to-pg200 address mapping.
  loadDefaultMidiMap();
}

// ******* [start midi input ring buffer] *******

// Write a byte to the next position in the rx buffer.
// A "hole" in the buffer of 1 byte is necessary to detect when we have filled
// the buffer (to prevent overwrites)
// NB: If the buffer is full the byte will be dropped! This may happen in
// extreme cases such as the Moog Little Phatty's filter CC, which sends
// 4 midi bytes for each step (two cc's and two data bytes), meaning that
// a single turn of the dial generates 512 messages. With running status
// messages, it is impossible to detect this, so dropping a byte means that
// the control only returns to normal if a new CC (Bx) is received. In the
// case of the LP, one has to press a key before turning a new dial.
// - Need to rewrite this to reject the second message if the first is rejected
// (or the next two if it is a status message that is rejected).
void writeToRxBuffer(char input){
  char nextRxPtr;
  nextRxPtr = (rxWritePtr + 1) % RX_LEN;

  //check if the read pointer is ahead of the write pointer
  if(nextRxPtr != rxReadPtr){
    rxWritePtr = nextRxPtr;
    rxBuffer[rxWritePtr] = input;
  }
}

// Gets a midi byte from the input buffer and sends it to the treatMidiByte
// method. Increments read pointer if byte has been treated properly.
void readFromRxBuffer(){
  char nextRxPtr;
  char midiByte;

  if(rxReadPtr != rxWritePtr){
    nextRxPtr = (rxReadPtr + 1) % RX_LEN;
    midiByte = rxBuffer[nextRxPtr];
    treatMidiByte(midiByte);

    //update read pointer once we are sure the byte has been treated
    rxReadPtr = nextRxPtr;
  }
}
// ******* [end midi input ring buffer] *******


// ******** [start midi data handling and conversion] ********

// Gets a midi byte from the input buffer and performs the necessary actions.
// Returns 0 if everything went fine (If the pg200 write buffer is full, we
// will return 1 and try to read the byte again later).
void treatMidiByte(char midiByte){

  if(midiByte.F7 == 1){ //status byte
  
    lastMidiStatus = (midiByte & 0xF0);

    //cc message designated for this device, these are not passed on.
    if(lastMidiStatus == MIDI_MODE_CC && (midiByte & 0x0F) == settings[POS_MIDI_CH]){
      midiRouting = MIDI_ROUTE_TO_PG200;
      midiByteCounter = 1;
    } else {
      midiRouting = MIDI_ROUTE_TO_THRU;
      if(OUTPUTMODE_INSTANT_SWITCH || TXMODE_PORT == TXMODE_MIDI && clearToSendMidi == 1){
        // We have switched to midi mode and waited the necessary time for the signal to settle.
        // If message is destined for this device (and thus the jx-3p), we have to change the
        // midi channel to 0. NB: Non musical commands like sysex messages are left untouched (all these start with 0xFx).
        if(lastMidiStatus != 0xF0 && (midiByte & 0x0F == settings[POS_MIDI_CH])){
          midiByte = midiByte & 0xF0; //strip address bits.
        }
        transmit(midiByte, TYPE_IGNORED, TXMODE_MIDI);
        
        // when this is one, we know that the first midi byte has been sent and that we are ok to send more
        // midi bytes. If we did not check this, it would be possible that the first midi byte was blocked
        // while the second/third etc were not. This would lead to errors.
        hasSentFirstMidiByte = 1;
      } else {
        //do not send midi, tell the world that the first byte was not send in order to block subsequent bytes.
        hasSentFirstMidiByte = 0;
      }

      // Treat sysex status messages.
      if(midiByte == 0xF0){
        sysexByteCounter = 0;
        inSysexMode = 1;
        sysexForThisDevice = 1; //will be set to false if address check fails later.
        //sysex start
      } else if(midiByte == 0xF7){
        //sysex end
        inSysexMode = 0;
      } else {
        //sysex aborted if in sysex mode
        inSysexMode = 0;
      }
    }
  } else { // data byte
    if(midiRouting == MIDI_ROUTE_TO_PG200){
      if(midiByteCounter == 1){ //convert CC number to pg200controller and store address, prepares for data transfer
      
        //TODO: Reset counter to 0 if an unknown CC number is received
        convertAndStoreAddress(midiByte);
        midiByteCounter = 2;
      } else if(midiByteCounter == 2){ //only one data byte should be used for CC messages
        convertAndTransmitData(midiByte);
        midiByteCounter = 1; //reset to 1 to allow for running status messages.
      }
    } else {
      //normal midi message, pass through without altering
      if(TXMODE_PORT == TXMODE_MIDI && hasSentFirstMidiByte == 1){ //block message unless first midi message has been sent.
        transmit(midiByte, TYPE_IGNORED, TXMODE_MIDI);
      } else { // midi is temporarily blocked. Record any note off-messages for replay once midi is on again.
        if(midiByteCounter == 2){ // if this is the second parameter received
          if(lastMidiStatus == MIDI_NOTE_OFF || lastMidiStatus == MIDI_NOTE_ON && midiByte == 0) { //if we are in note off mode or in note on and velocity is zero (=note off)
            missingNoteOffs[lastMidiParam1] = 1;
            hasMissingNoteOffs = 1;
          }
        }
      }

      if(inSysexMode){
        // check if sysex is meant for this device
        
        if(sysexByteCounter < 3){
          if(!sysexAddress[sysexByteCounter]){
            sysexForThisDevice = 0;
          }
        } else {
          if(sysexForThisDevice){
            treatSysexByte(midiByte);
          }
        sysexByteCounter++;
        }
      }
      
      //Store last received first-parameter. switch to 1 after second param to allow for running status
      if(midiByteCounter == 1){ // first parameter received
        lastMidiParam1 = midiByte;
        midiByteCounter = 2;
      } else if(midiByteCounter == 2){ // second parameter received
        midiByteCounter = 1;
      }
    }
  }
}

void treatSysexByte(char midiByte){
  if(sysexDataCounter == 0){
    //first data byte, signals type of operation
    currentSysexOperation = midiByte;
    if(midiByte == SYSEX_OP_WRITE_SETTINGS_TO_EE){
      writeSettingsToEE();
    } else if(midiByte == SYSEX_OP_CLEAR_SETTINGS_FROM_EE){
      clearSettingsFromEE();
    } else if(midiByte == SYSEX_OP_CHANGE_SETTING){
      //do nothing, need more data before we can change settings.
      sysexDataCounter++;
    }
  } else if(sysexDataCounter == 1){
    if(currentSysexOperation == SYSEX_OP_CHANGE_SETTING){
      // read position in settings array to change
      currentSysexParam1 = midiByte;
    }
    sysexDataCounter++;
  } else if(sysexDataCounter == 2){
    if(currentSysexOperation == SYSEX_OP_CHANGE_SETTING){
      // change settings value at position indicated by previous sysex parameter.
      settings[currentSysexParam1] = midiByte;
    }
    sysexDataCounter++;
  }
}

char hasSettingsInEE(){
  return EEPROM_Read(0); //if this is 0, no settings data should be read from EE.
}

void writeSettingsToEE(){
  char i;
  settings[0] = 1; //indicate that ee has settings data.
  for(i=0; i<SETTINGS_LENGTH; i++){
    EEPROM_Write(i, settings[i]);
  }
}

void readSettingsFromEE(){
  char i;
  for(i=0; i<SETTINGS_LENGTH; i++){
    settings[i] = EEPROM_Read(i);
  }
}

void clearSettingsFromEE(){
  char i;
  for(i=0; i<SETTINGS_LENGTH; i++){
    EEPROM_Write(i, 0);
  }
}

char getNewStateOne(char shift, char oldState){
  char mask;
  // Set option bits to 00
  mask = 0b0000011 << shift;
  mask = ~mask; //all bits except the two to set to 0 is now 1
  return oldState & mask;
}

char getNewStateTwo(char shift, char oldState){
  char mask;
  char newState;
  
  // Set option bits to 01
  // reset msb of the two holding the state to 0
  mask = 0b00000001 << (shift + 1);
  mask = ~mask; //all bits except the one to set to 0 is now 1
  newState = oldState & mask;

  // set lsb of the two holding the state to 1
  mask = 0b00000001 << shift;
  newState = newState | mask; //all bits except the two to set to 0 is now 1
  return newState;
}

char getNewStateThree(char shift, char oldState){
  char mask;
  char newState;

  // Set option bits to 10:
  // set msb of the two holding the state to 1
  mask = 0b00000001 << (shift + 1);
  newState = oldState | mask;

  //reset lsb of the two holding the state to 0
  mask = 0b00000001 << shift;
  mask = ~mask; //all bits except the one to set to 0 is now 1
  newState = newState & mask; //all bits except the two to set to 0 is now 1
  return newState;
}

char getNewStateFour(char shift, char oldState){
  char mask;

  // Set option bits to 11:
  mask = 0b00000011 << shift;
  return oldState | mask;
}

// Convert a 0-127 midi value into a binary switch
char getNewStateTwoOptions(char shift, char oldState, char midiValue){
  char mask;

  if(midiValue < 64){
    // Set option bits to 0
    mask = 0b00000001 << shift;
    mask = ~mask; //all bits except the one to set to 0 is now 1
    return oldState & mask;
  } else {
    // Set option bits to 1:
    mask = 0b00000001 << shift;
    return oldState | mask;
  }
}

// Convert a 0-127 midi value into a three state switch value
char getNewStateThreeOptions(char shift, char oldState, char midiValue){
  if(midiValue < 42){
    return getNewStateOne(shift, oldState);
  } else if(midiValue < 84){
    return getNewStateTwo(shift, oldState);
  } else{
    return getNewStateThree(shift, oldState);
  }
}

// Convert a 0-127 midi value into a four state switch value
char getNewStateFourOptions(char shift, char oldState, char midiValue){
  if(midiValue < 32){
    return getNewStateOne(shift, oldState);
  } else if(midiValue < 64){
    return getNewStateTwo(shift, oldState);
  } else if(midiValue < 96){
    return getNewStateThree(shift, oldState);
  } else {
    return getNewStateFour(shift, oldState);
  }
}

// Generates a switch state byte. Input parameters tells how far to the left
// to shift the result (as switch states are shared between several switches)
// and how many possible states the switch can have (to automatically convert
// the 0-127 midi range into equally sized spans for each state).
char getNewButtonState(char shift, char oldState, char midiValue, char optionCount){
  char result;
  switch(optionCount){
    case 2:
      return getNewStateTwoOptions(shift, oldState, midiValue);
    case 3:
      return getNewStateThreeOptions(shift, oldState, midiValue);
    case 4:
      return getNewStateFourOptions(shift, oldState, midiValue);
  }
  return 0;
}

// Converts a midi cc number to a pg-200 address and stores it untill a
// data byte is received.
void convertAndStoreAddress(char ccNumber){
  char pg200controller;

  // store the pg200 controller, needed when sending data, to select
  // between switch and pot output.
  lastPg200controller = pg200midiToPg[ccNumber];

  // get and store the the pg200 address for the controller
  lastPg200Address = pg200address[lastPg200controller];
}

// Convert a midi data byte to a pg-200 data byte and send it to the jx-3p
void convertAndTransmitData(char midiValue){

  char newCombinedSwitchesState;
  char oldCombinedSwitchesState;
  char mask;

  // write address before sending data. As we allow running statuses from
  // the midi controller, but the JX-3P does not support running status,
  // we have to send the address for every new data byte encountered.
  if(lastPg200Controller < NUMBER_OF_SWITCHES){
    oldCombinedSwitchesState = pg200switchStates[lastPg200Address];
    newCombinedSwitchesState = getNewButtonState(
      pg200switchShifts[lastPg200Controller],
      oldCombinedSwitchesState,
      midiValue,
      pg200switchStatesPossible[lastPg200Controller]
    );

    //calculate mask (figure out what bits have changed)
    mask = newCombinedSwitchesState ^ oldCombinedSwitchesState;
    if(mask != 0){
      //last controller was a switch, a mask and bitmask is needed
      transmit(lastPg200Address, TYPE_ADDR, TXMODE_PG200);
      transmit(mask, TYPE_DATA, TXMODE_PG200);
      transmit(newCombinedSwitchesState, TYPE_DATA, TXMODE_PG200);
      
      //Store the new switch state
      pg200switchStates[lastPg200Address] = newCombinedSwitchesState;
    }
  } else if(lastPg200Controller < NUMBER_OF_CONTROLLERS){
    //last controller was a potmeter, which means we only have to double
    //value (as pg-200 controllers have 8 bit resolution whereas midi only
    //has 7 bit)
    transmit(lastPg200Address, TYPE_ADDR, TXMODE_PG200);
    transmit(midiValue*2, TYPE_DATA, TXMODE_PG200);
  } else {
    //do nothing, unknown controller received.
  }
}
// ******** [end midi data handling and conversion] ********


void transmit(char input, char pg200type, char txmode){

  // switch between sending midi and pg200 messages
  switchTxMode(txmode);

  // enable/disable 9 bit transfer (PG-200 mode = 1, midi mode = 0)
  TXSTA.TX9 = ~txmode;

  // set 9th bit of transfer buffer to type
  TXSTA.TX9D = pg200type.F0;

  // write data to JX3P
  UART1_Write(input);

  // if output mode is revert to midi, switch to midi after sending pg200 message.
  if(settings[POS_OUT_MODE] == OUTPUTMODE_REVERT_TO_MIDI){
    switchTxMode(TXMODE_MIDI);
  }
}

void resetSwitchStates(){
  char i;
  for(i=0; i<3; i++){
    pg200switchStates[i] = 0;
  }
}

void sendPing(){
    // enable 9 bit transfer (PG-200 mode)
    TXSTA.TX9 = 1;

    // switch JX3p to receive PG200 signals
    switchTxMode(TXMODE_PG200);
    delay_ms(500);
    
    TXSTA.TX9D = TYPE_ADDR;
    UART1_Write(PG200_PING_ADDR);
    delay_us(70);
    
    TXSTA.TX9D = TYPE_DATA;
    UART1_Write(PG200_PING_DATA);
    delay_ms(17);
    
    // prepare for receiving midi
    if(settings[POS_OUT_MODE] != OUTPUTMODE_BLOCK_MIDI){
      switchTxMode(TXMODE_MIDI);
    }
}

void setupRxBuffer(){
  rxReadPtr = 0;
  rxWritePtr = 0;
}

void setupMidi(){
  char i;
  
  midiByteCounter = 0;
  lastPg200Controller = NUMBER_OF_CONTROLLERS+1;
  midiRouting = MIDI_ROUTE_TO_THRU;
  
  for(i=0; i<127; i++){
    missingNoteOffs[i]=0;
  }
  
  inSysexMode = 0;
  sysexForThisDevice = 0;
}

void setupInterrupts(){
  //enable interrupts. NB: PEIE/GIE also affects timer0/1
  PEIE_bit = 1;
  GIE_bit = 1;
  RCIE_bit = 1;            // enable USART RX interrupt
}

void setupUsart(){
  UART1_Init(31250);        // initialize USART module
  Delay_ms(100);            // Wait for UART module to stabilize
}

void setupTxPort(){
  TXMODE_TRIS = 0;
}

void flashStatus(char times){
  char i;
  for(i=0; i< times; i++){
    PORTC.F5 = 1;
    delay_ms(100);
    STATUS_PORT = 0;
    delay_ms(100);
  }
}

// read switch and change output mode.
void readSwitch(){

  char line1;
  char line2;
  char line3;

  if(PORTC.F0 == 1){
    if(settings[POS_OUT_MODE] != 0){
      PORTC.F5 = 0;
      delay_ms(500);
      flashStatus(1);
      settings[POS_OUT_MODE] = OUTPUTMODE_BLOCK_MIDI;
      TXMODE_PORT = TXMODE_PG200;
      delay_ms(500);
      PORTC.F5 = 1;
    }
  } else if(PORTC.F1 == 1){
    if(settings[POS_OUT_MODE] != 1){
      PORTC.F5 = 0;
      delay_ms(500);
      flashStatus(2);
      settings[POS_OUT_MODE] = OUTPUTMODE_REVERT_TO_MIDI;
      TXMODE_PORT = TXMODE_MIDI;
      clearToSendMidi = 1;
      delay_ms(500);
      PORTC.F5 = 1;
    }
  } else if(PORTC.F2 == 1){
    if(settings[POS_OUT_MODE] != 2){
      PORTC.F5 = 0;
      delay_ms(500);
      flashStatus(3);
      settings[POS_OUT_MODE] = OUTPUTMODE_INSTANT_SWITCH;
      TXMODE_PORT = TXMODE_MIDI;
      delay_ms(500);
      PORTC.F5 = 1;
    }
  }
}


void setupTimers(){
  PSA_bit = 0;
  T0PS0_bit = 1;
  T0PS1_bit = 1;
  T0PS2_bit = 1;
  T0CS_bit = 0;
  TMR0ON_bit = 0;
  TMR0IF_bit = 0;
  TMR0IE_bit = 1;

  T1CKPS0_bit = 1;
  T1CKPS1_bit = 0;
  TMR1CS_bit = 0;
  TMR1ON_bit = 0;
  TMR1IF_bit = 0;
  TMR1IE_bit = 1;
}

void main() {
  ADCON0 = 0; // turn off A/D converter
  ADCON1 = 0x07; //turn off analogue inputs (for P18F458)

  TRISA = 0;
  TRISB = 0;
  TRISC = 0;

  PORTA = 0;
  PORTB = 0;
  PORTC = 0;

  //Set read direction on switch lines
  TRISC0_bit=1;
  TRISC1_bit=1;
  TRISC2_bit=1;

  STATUS_TRIS = 0;
  flashStatus(3);

  setupInterrupts();
  resetSwitchStates();

  // overwrite default settings with settings from EE.
  if(hasSettingsInEE()){
    readSettingsFromEE();
  }
  
  loadPg200maps();
  setupRxBuffer();
  setupMidi();
  setupUsart();
  setupTimers();
  setupTxPort();

  //Send 'ping' to JX-3P, telling it that the PG-200 is connected.
  sendPing();

  flashStatus(3);
  PORTC.F5=1;

  while(1){
    if(clearToSendMissingNoteOffs == 1){
      sendMissingNoteOffs();
    }
  
    //read from the rx data and convert and transmit midi and pg-200 messages
    //to the JX-3P - for ever and ever!
    readFromRxBuffer();
    readSwitch();
  }
}