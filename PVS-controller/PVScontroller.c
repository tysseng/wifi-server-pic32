//TODO:
// korrekte output pins
// hindre sending av note hvis velocity timer > 70 (passe på at timer ikke
// overflow'er.
// reset CD4022 on init?
// lytte på carry fra CD4022? Er dette nødvendig?
// sjekke om klokkepulsbredde til CD4022 er bred nok.
// vurdere om KYBD bør være koblet til interrupt for å sikre at vi ikke mister data.

// turn on/off tests. remove for production code.
//#define RUNTESTS

#ifdef RUNTESTS
  // debug output buffers, used in asserts in tests
  unsigned short lastNoteSent;
  unsigned short lastVelocitySent;

  // Mock ports when in test mode
  #define OUTPUT_BUS mockedOutputBus
  #define DATA_BUS_DISABLED_PIN mockedDataBusDisabledPin.b0
  #define OUTPUT_READY_PIN mockedOutputReadyPin.b0
  #define COL_SCAN_TIMER_INTERRUPT mockedColScanTimerInterrupt.b0
  #define MCU_RECEIVING_DATA_INTERRUPT mockedMcuReceivingDataInterrupt.b0
  unsigned short mockedOutputBus;
  unsigned short mockedDataBusDisabledPin;
  unsigned short mockedOutputReadyPin;
  unsigned short mockedColScanTimerInterrupt;
  unsigned short mockedMcuReceivingDataInterrupt;
#endif

// Real ports when not in test mode
#ifndef RUNTESTS
  #define OUTPUT_BUS latd
  #define DATA_BUS_DISABLED_PIN portb.f7
  #define OUTPUT_READY_PIN late.f2
  #define COL_SCAN_TIMER_INTERRUPT TMR0IF_bit
  #define MCU_RECEIVING_DATA_INTERRUPT RBIF_bit
#endif

#define OUTPUT_BUS_TRIS trisd
#define DATA_BUS_DISABLED_PIN_TRIS trisb.f7
#define OUTPUT_READY_PIN_TRIS trise.f2
#define COLUMN_CLOCK_PIN late.f1
#define COLUMN_CLOCK_PIN_TRIS trise.f1
#define KEYS_START_ROW portc
#define KEYS_START_ROW_TRIS trisc
#define KEYS_END_ROW porta
#define KEYS_END_ROW_TRIS trisa

#include "PVSvelocityTable.h"
#include "PVScontroller.h"
#include "PVScontroller.test.h"

// Lowest possible note, C0 = 0100100
#define BASE_NOTE 36

// Commands to send to the master MCU. note off is 0 so no need to use a command
#define COMMAND_NOTE_ON 0b10000000

volatile unsigned short noteTimers[KEYCOUNT];
volatile unsigned short noteVelocity[KEYCOUNT];

volatile unsigned short noteStartSwitchStates[COLUMNS];
volatile unsigned short noteEndSwitchStates[COLUMNS];

volatile unsigned short readyToSendOff[COLUMNS];
volatile unsigned short readyToSendOn[COLUMNS];

volatile unsigned short currentColumn;
volatile unsigned short cycleCounter;

volatile unsigned short mainMcuReceivingData;

// use different name for interrupt function in test mode to be able to
// call it from a test.
#ifndef RUNTESTS
void interrupt() {
  interruptBody();
}
#endif

void interruptBody(){

  // colScanTimer must run 8 times faster than the desired cycle counter speed
  // as we only update the cycleCounter once every key has been scanned.
  if(COL_SCAN_TIMER_INTERRUPT){
  
    // clear interrupt and restart timer
    COL_SCAN_TIMER_INTERRUPT = 0;
    TMR0L = 0x28;

    checkKeyStartSwitches(KEYS_START_ROW, currentColumn);
    checkKeyBottomSwitches(KEYS_END_ROW, currentColumn);

    // Switch to next row - lets values settle untill next timeout.
    currentColumn = ++currentColumn % COLUMNS;

    // Send clock pulse to increment CD4022 counter (column selector)
    COLUMN_CLOCK_PIN = 1;
    delay_us(2);
    COLUMN_CLOCK_PIN = 0;

    // Increment cycle counter before starting a new round.
    // Counter is only incremented once per round, not once per column
    if(currentColumn == 0){
      cycleCounter++;
    }
  } else if(MCU_RECEIVING_DATA_INTERRUPT){

    // clear interrupt
    MCU_RECEIVING_DATA_INTERRUPT = 0;
    if(DATA_BUS_DISABLED_PIN == 0){
      mainMcuReceivingData = 1;
    } else {
      mainMcuReceivingData = 0;
    }
  }
}

void checkKeyStartSwitches(unsigned short newState, unsigned short column){

  unsigned short row = 0;
  unsigned short mask = 0b00000001;
  unsigned short noteIndex = column;
  unsigned short changes = noteStartSwitchStates[column] ^ newState;

  // most times nothing has changed, so return without further checking.
  if(changes == 0){
    return;
  }

  for(row=0; row < ROWS; row++){
    if(changes & mask){ // row has changed
      if(newState & mask){ // row has been turned on, store start time
        noteTimers[noteIndex] = cycleCounter;
      } else { // row has been turned off
        readyToSendOff[column] = readyToSendOff[column] | mask; // set bit at row position
      }
    }
    
    //step to next note which is COLUMNS higher than the previous one.
    noteIndex += COLUMNS;
    
    mask = mask << 1;
    
    // last row does not contain a full set of keys.
    if(noteIndex >= KEYCOUNT){
      break;
    }
  }
  
  noteStartSwitchStates[column] = newState;
}

void checkKeyBottomSwitches(unsigned short newState, unsigned short column){

  unsigned short row = 0;
  unsigned short mask = 0b00000001;
  unsigned short noteIndex = column;
  unsigned short changes = noteEndSwitchStates[column] ^ newState;

  // most times nothing has changed, so return without further checking.
  if(changes == 0){
    return;
  }
  
  for(row=0; row < ROWS; row++){
    if((changes & mask) && (newState & mask)){ // row has changed and is now turned on.
      // since we're using unsigned shorts, we will get a mod 256 effect,
      // so the calculation will be correct even if cycleCounter is less
      // than noteTimer
      noteVelocity[noteIndex] = cycleCounter - noteTimers[noteIndex];
      readyToSendOn[column] = readyToSendOn[column] | mask; // set bit at row position
    }

    //step to next note which is COLUMNS higher than the previous one.
    noteIndex += COLUMNS;

    mask = mask << 1;

    // last row does not contain a full set of keys.
    if(noteIndex >= KEYCOUNT){
      break;
    }
  }
  
  noteEndSwitchStates[column] = newState;
}

// blocking send, sets a data value on the output and waits until the receiver
// indicates that the value has been read.
void send(unsigned short value){

  OUTPUT_BUS = value;
  OUTPUT_READY_PIN = 1; //indicate to the main mcu that data is ready

  // remove waiting code when in test mode to allow tests to pass
  #ifndef RUNTESTS
    // wait until main mcu indicates that data bus is ready. Output is blocked
    // by the 74HC367 transparent latches until the data bus disabled line goes 
    // low, so data may be put on the output bus even before this flag is read.
    while(DATA_BUS_DISABLED_PIN){
      delay_us(1);
    }
    // keep output until main mcu disables the data bus.
    while(DATA_BUS_DISABLED_PIN == 0){
      delay_us(1);
    }
    OUTPUT_BUS = 0;
  #endif
  OUTPUT_READY_PIN = 0;
}

unsigned short calculateVelocity(unsigned short velocityTime){
  if(velocityTime < 70){
    return velocities[velocityTime];
  }
  return 0;
}

void sendNoteOn(unsigned short noteIndex, unsigned short velocityTime){
  unsigned short velocity = calculateVelocity(velocityTime);
  unsigned short noteToSend = COMMAND_NOTE_ON | (BASE_NOTE + noteIndex);
  
  // store calculated results when in test mode to allow asserts on them
  #ifdef RUNTESTS
    lastNoteSent = noteToSend;
    lastVelocitySent = velocity;
  #endif
  send(noteToSend);
  send(velocity);
}

void sendNoteOns(){
  unsigned short column, row;
  unsigned short mask;
  unsigned short noteIndex = 0;

  for(row=0; row < ROWS; row++){
    mask = 1;
    for(column=0; column < COLUMNS; column++){
      if(readyToSendOn[column] & mask){
        sendNoteOn(noteIndex, noteVelocity[noteIndex]);
        
        // clear readyToSend. Only the current bit may be cleared, as new
        // ones may have been set through interrupts while we are in this loop
        readyToSendOn[column] &= ~mask;
      }
      noteIndex++;
    }
    mask = mask << 1;
  }
}

void sendNoteOff(unsigned short noteIndex){
  // note off-command is 0 so no need for any special treatment.
  unsigned short noteToSend = (BASE_NOTE + noteIndex);
  send(noteToSend);
  #ifdef RUNTESTS
    lastNoteSent = noteToSend;
  #endif
}

void sendNoteOffs(){
  unsigned short column, row;
  unsigned short mask;
  unsigned short noteIndex = 0;

  for(row=0; row < ROWS; row++){
    mask = 1;
    for(column=0; column < COLUMNS; column++){
      if(readyToSendOff[column] & mask){
        sendNoteOff(noteIndex);
        
        // clear readyToSend. Only the current bit may be cleared, as new
        // ones may have been set through interrupts while we are in this loop
        readyToSendOff[column] &= ~mask;
      }
      noteIndex++;
    }
    mask = mask << 1;
  }
}

void initKeyScanner(){
  unsigned short i;
  for(i=0; i<COLUMNS; i++){
    noteStartSwitchStates[i]=0;
    noteEndSwitchStates[i]=0;
    readyToSendOff[i]=0;
    readyToSendOn[i]=0;
  }
  currentColumn = 0;
  // TODO: Make sure CD4022 counter is at 0 when we start, may read input
  // from counter.
}

void initVelocityTiming(){
  unsigned short i;
  for(i=0; i<KEYCOUNT; i++){
    noteTimers[i]=0;
    noteVelocity[i]=0;
  }
  cycleCounter = 0;
}


void disableAnalogPins(){

  //turn off analog inputs (set ports to digital)
  ANCON1  = 0;
  ANCON0  = 0;

  // turn off A/D converter
  ADCON0 = 0;
}

void setupIOPorts(){
  // 0 = output
  // 1 = input
  OUTPUT_BUS_TRIS = 0;
  DATA_BUS_DISABLED_PIN_TRIS = 1;
  OUTPUT_READY_PIN_TRIS = 0;
  COLUMN_CLOCK_PIN_TRIS = 0;
  KEYS_START_ROW_TRIS = 0xFF;
  KEYS_END_ROW_TRIS = 0xFF;
}

void setupTimers(){
  // timer should trigger interrupt every 54uS for a total cycle length of
  // 432uS.
  T0CON             = 0xC8; // prescaler 1:1 if frequency is 16MHz
  //T0CON         = 0xC1; // prescaler 1:4 if frequency is 64MHz
  TMR0L             = 0x28;
  GIE_bit         = 1;
  TMR0IE_bit = 1;
}

void setupPortBInterrupt(){
  RBIE_bit = 1; // turn on interrupt on portb changes.
}

void setupOscillator(){
  //Additional settings in project -> edit project
  // LF-INTOSC Low-power = LF-INTOSC in High-Power mode during Sleep
  // SOSC Power Selection and mode = Digital (SCLKI) mode (necessary for RC0 and RC1 to work properly)
  // Oscillator = Internal RC oscillator
  // PLL x4 = Disabled
  // Frequency: 16MHz
  INTSRC_bit = 1;
  IRCF2_bit = 1;
  IRCF1_bit = 1;
  IRCF0_bit = 1;

  PLLEN_bit = 0; // PLL disabled, frequency 16MHz
  //PLLEN_bit = 1; // 4 x PLL enabled, frequency 64MHz

}

void init(){
  mainMcuReceivingData = 0;
  initKeyScanner();
  initVelocityTiming();
  setupPortBInterrupt();
}

#ifdef RUNTESTS
void main() {
  init();
  lastNoteSent = 0;
  lastVelocitySent = 0;
  runTests();
}
#endif

#ifndef RUNTESTS
void main() {
  setupOscillator();
  
  init();
  disableAnalogPins();
  setupIOPorts();
  setupTimers();

  // back and forth, forever and ever.
  while(1){
    sendNoteOns();
    sendNoteOffs();
  }
}
#endif