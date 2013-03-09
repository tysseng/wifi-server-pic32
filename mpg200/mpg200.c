#include "mpg200.h"

//PG200 TX type
#define TYPE_ADDR 1
#define TYPE_DATA 0
#define TYPE_IGNORED 0

//RX ring buffer length. Must not exceed 256 bytes...
#define RX_LEN 256

//midi modes
#define MIDI_MODE_CC 0xB0
#define DEFAULT_MIDI_CHANNEL 0

//midi routing modes
#define MIDI_ROUTE_TO_THRU 0
#define MIDI_ROUTE_TO_PG200 1

//Constants for all PG200 controllers
#define NUMBER_OF_SWITCHES 16
#define NUMBER_OF_CONTROLLERS 34
#define SW_B_WAVE 0
#define SW_B_RANGE 1
#define SW_A_WAVE 2
#define SW_A_RANGE 3
#define SW_A_LFO 4
#define SW_A_ENV 5
#define SW_B_LFO 6
#define SW_B_ENV 7
#define SW_VCA 8
#define SW_VCF_POL 9
#define SW_SYNC 10
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
//- Programmer switch must be in MIDI BUS positior for both midi and pg-200 to work at the same time.

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
//- BUG: Any subsequent controller that is sent that does not match any known pg200 controller, will work on the previous controller recognized. Set currentController to unknown and check for this.

/** Parameters that should be user configurable **/
char midiChannel;

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

// default mode.
char outputMode = OUTPUTMODE_BLOCK_MIDI;

// last change from PG200 to MIDI mode has settled if this is 1
char clearToSendMidi = 1;

// first byte of a midi message has been sent, so clear to send the remaining bytes.
char hasSentFirstMidiByte = 0;

// counter to count times timer has timed out, necessary because timer0 is too fast.
char timer0TimeoutCount = 0;

// these should be configurable via midi and have defaults that can be reverted to
#define defaultSwitchToMidiTimerOverflows 4
#define defaultOutputMode OUTPUTMODE_BLOCK_MIDI
char configSwitchToMidiTimerOverflows = defaultSwitchToMidiTimerOverflows; //20MHz: 4 timeouts approx 50ms delay after PG-200 before switching to midi (+ another 26ms before midi can be sent).
char configOutputMode = defaultOutputMode; // read from config/midi instead of from switch

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
    if(timer0TimeoutCount < configSwitchToMidiTimerOverflows){
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
    clearToSendMidi = 1;
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

void switchTxMode(char newMode){
  if(outputMode == OUTPUTMODE_BLOCK_MIDI){
    newMode = TXMODE_PG200;
  } else if(outputMode == OUTPUTMODE_REVERT_TO_MIDI){
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

// default midi cc-to-pg200 address mapping.
void loadDefaultMidiMap(){

  //pots
  pg200midiToPg[12] = POT_B_FINE;
  pg200midiToPg[13] = POT_B_TUNE;
  pg200midiToPg[14] = POT_DCO_ENV;
  pg200midiToPg[15] = POT_DCO_LFO;
  pg200midiToPg[16] = POT_MIX;
  pg200midiToPg[17] = POT_HPF;
  pg200midiToPg[18] = POT_RESONANCE;
  pg200midiToPg[19] = POT_CUTOFF;
  pg200midiToPg[20] = POT_VCF_ENV;
  pg200midiToPg[21] = POT_VCF_LFO;
  pg200midiToPg[22] = POT_PITCH_FOLLOW;
  pg200midiToPg[23] = POT_LEVEL;
  pg200midiToPg[24] = POT_LFO_RATE;
  pg200midiToPg[25] = POT_LFO_DELAY;
  pg200midiToPg[26] = POT_A;
  pg200midiToPg[27] = POT_D;
  pg200midiToPg[28] = POT_S;
  pg200midiToPg[29] = POT_R;
  
  //switches
  pg200midiToPg[72]  = SW_A_RANGE;
  pg200midiToPg[73]  = SW_A_WAVE;
  pg200midiToPg[74]  = SW_B_RANGE;
  pg200midiToPg[75]  = SW_B_WAVE;
  pg200midiToPg[76]  = SW_SYNC;
  pg200midiToPg[77]  = SW_VCF_POL;
  pg200midiToPg[78]  = SW_VCA;
  pg200midiToPg[79]  = SW_B_ENV;
  pg200midiToPg[80]  = SW_B_LFO;
  pg200midiToPg[81]  = SW_A_ENV;
  pg200midiToPg[82]  = SW_A_LFO;
  pg200midiToPg[83]  = SW_LFO_WAVE;
  pg200midiToPg[84]  = SW_DCO_POL;
  pg200midiToPg[85]  = SW_CHORUS;

  // not used
  pg200midiToPg[127] = SW_MANUAL;
  pg200midiToPg[127] = SW_WRITE;

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
  loadLittlePhattyMidiMap();
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
    if((midiByte & 0xF0) == MIDI_MODE_CC && (midiByte & 0x0F) == midiChannel){ //cc message designated for this device
      midiRouting = MIDI_ROUTE_TO_PG200;
      midiByteCounter = 1;
    } else {
      midiRouting = MIDI_ROUTE_TO_THRU;
      
      if(OUTPUTMODE_INSTANT_SWITCH || TXMODE_PORT == TXMODE_MIDI && clearToSendMidi == 1){
        //we have switched to midi mode and waited the necessary time for the signal to settle
        
        transmit(midiByte, TYPE_IGNORED, TXMODE_MIDI);
        
        // when this is one, we know that the first midi byte has been sent and that we are ok to send more
        // midi bytes. If we did not check this, it would be possible that the first midi byte was blocked
        // while the second/third etc were not. This would lead to errors.
        hasSentFirstMidiByte = 1;
      } else {
        //do not send midi, tell the world that the first byte was not send in order to block subsequent bytes.
        hasSentFirstMidiByte = 0;
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
      }
    }
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
  if(outputMode == OUTPUTMODE_REVERT_TO_MIDI){
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
    if(outputMode != OUTPUTMODE_BLOCK_MIDI){
      switchTxMode(TXMODE_MIDI);
    }
}

void setupRxBuffer(){
  rxReadPtr = 0;
  rxWritePtr = 0;
}

void setupMidi(){
  midiChannel = DEFAULT_MIDI_CHANNEL;
  midiByteCounter = 0;
  lastPg200Controller = NUMBER_OF_CONTROLLERS+1;
  midiRouting = MIDI_ROUTE_TO_THRU;
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
    if(outputMode != 0){
      PORTC.F5 = 0;
      delay_ms(500);
      flashStatus(1);
      outputMode = OUTPUTMODE_BLOCK_MIDI;
      TXMODE_PORT = TXMODE_PG200;
      delay_ms(500);
      PORTC.F5 = 1;
    }
  } else if(PORTC.F1 == 1){
    if(outputMode != 1){
      PORTC.F5 = 0;
      delay_ms(500);
      flashStatus(2);
      outputMode = OUTPUTMODE_REVERT_TO_MIDI;
      TXMODE_PORT = TXMODE_MIDI;
      clearToSendMidi = 1;
      delay_ms(500);
      PORTC.F5 = 1;
    }
  } else if(PORTC.F2 == 1){
    if(outputMode != 2){
      PORTC.F5 = 0;
      delay_ms(500);
      flashStatus(3);
      outputMode = OUTPUTMODE_INSTANT_SWITCH;
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
    //read from the rx data and convert and transmit midi and pg-200 messages
    //to the JX-3P - for ever and ever!
    readFromRxBuffer();
    readSwitch();
  }
}