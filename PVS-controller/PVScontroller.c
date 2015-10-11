//TODO: legg inn beskyttelse mot hanging notes (send off hvis start er off og hasSentOn er 1).

// Concepts:
// start key/start switch: The first switch that is closed when a key is pressed
// end key/end switch: The second switch that is closed when a key is pressed
//
// Velocity time is calculated as the time (in counter cycles of 432uS) between
// start and end switches are closed. The velocity is  mapped to a faux
// logarithmic output using a constant map. Only 70 cycles are allowed, if a
// key is still pressed but has not reached bottom after 70 cycles, a note
// on with velocity 1 is sent.
//
// Maximum velocity is 127, minimum is 1.
//
// Note on is sent as two bytes, the first has MSB=1 and is the note, the second
// is the velocity.
//
// Note off is sent as a single byte with MSB=0. The rest of the bits are the
// same as for the corresponding note on.
//
// C0 (the lowest C on the keyboard) is sent as 36 + 0b10000000 (on) or 36 (off)
// other notes follows sequentially.
//
// Once data is ready to be sent, the controller buts the data on the output bus
// and pulls its output ready pin _low_ (it is normally high). The main MCU
// detects this and sends a very short _low_ pulse on the data_bus_disabled_pin.
// This pulse simultaneously turns on the output buffers (separate chip) and
// puts the output data on the prophet VS common data bus. Once the pulse goes
// high again it is safe to remove the data.
//
// If the main MCU sees a 0 on the data bus, it interprets this as no more data
// and stops reading. This is the reason why minimum velocity is 1, not 0.


// turn on/off tests. remove for production code.
//#define RUNTESTS

#ifdef RUNTESTS
  // debug output buffers, used in asserts in tests
  unsigned short lastNoteSent;
  unsigned short lastVelocitySent;

  // Mock ports when in test mode
  #define OUTPUT_BUS mockedOutputBus
  #define DATA_BUS_DISABLED_PORT mockedDataBusDisabledPort
  #define OUTPUT_READY_PIN mockedOutputReadyPin.b0
  #define COL_SCAN_TIMER_INTERRUPT mockedColScanTimerInterrupt.b0
  #define MCU_RECEIVING_DATA_INTERRUPT mockedMcuReceivingDataInterrupt.b0
  unsigned short mockedOutputBus;
  unsigned short mockedDataBusDisabledPort;
  unsigned short mockedOutputReadyPin;
  unsigned short mockedColScanTimerInterrupt;
  unsigned short mockedMcuReceivingDataInterrupt;
  unsigned short bytesSent;
#endif

// Real ports when not in test mode
#ifndef RUNTESTS
  #define OUTPUT_BUS latd
  #define DATA_BUS_DISABLED_PORT portb
  #define OUTPUT_READY_PIN late.f2 //NB: OUTPUT_READY is (correctly) inverted, 1 means no data is ready
  #define COL_SCAN_TIMER_INTERRUPT TMR0IF_bit
  #define MCU_RECEIVING_DATA_INTERRUPT INT3IF_bit
#endif

#define OUTPUT_BUS_TRIS trisd

//NB: Remember to change IOCBx_bit when changing input pin on PORTB!
#define DATA_BUS_DISABLED_PIN_TRIS_1 trisb.f4
#define DATA_BUS_DISABLED_PIN_TRIS_2 trisb.f3
#define OUTPUT_READY_PIN_TRIS trise.f2
#define COLUMN_CLOCK_PIN late.f1
#define COLUMN_CLOCK_PIN_TRIS trise.f1
#define COLUMN_CARRY_PIN PORTE.f0
#define COLUMN_CARRY_PIN_TRIS trise.f0
#define KEYS_START_ROW portc
#define KEYS_START_ROW_TRIS trisc
#define KEYS_END_ROW_MISSING_BIT portb.f0 // bugfix for missing pin on port a
#define KEYS_END_ROW_MISSING_BIT_TRIS trisb.f0
#define KEYS_END_ROW porta
#define KEYS_END_ROW_TRIS trisa

#include "PVSvelocityTable.h"
#include "PVScontroller.h"
#include "PVScontroller.test.h"


// One byte per key, stores the timer value at the time the start switch of a
// key is closed and is used when calculating the velocity
unsigned short noteTimers[KEYCOUNT];

// Stores the last state of the start/end switches to be able to detect changes.
// They map to pin 13-20 (start) and 37-30 (end and in that order) of the
// circuit (not the MCU, mind you).
unsigned short noteStartSwitchStates[COLUMNS];
unsigned short noteEndSwitchStates[COLUMNS];

// This variable is doing some magic. It is a bitwise variable, one bit maps
// to one key. If the bit is 0, a new on-command can be sent if the key reaches
// bottom or the velocity timer for that key times out. Once the on-command is
// sent it is set to 1, indicating that a note-off must be sent when the key
// reaches top again. The variable is also set to 1 if the key reaches top and
// its value is 0 (meaning that no on-command has been sent). This prevents
// timeouts if the key has been released before timeout occurs.
unsigned short hasSentOn[COLUMNS];

// The column in the key matrix that is currently being read.
unsigned short currentColumn;

// A 256 bit counter used for calculating velocity. Incremented by a timer 
// interrupt every 432uS.
volatile unsigned short cycleCounter;

// Set to true by the interrupt that detects a transition from 0 to 1 on the
// DATA_BUS_DISABLED_PIN and reset when a new byte is put on the output bus.
volatile unsigned short mainMcuHasReadData;

void interrupt() {
  unsigned short DATA_BUS_DISABLED_VALUE;
  
  if(MCU_RECEIVING_DATA_INTERRUPT){
    // The main MCU reads data when the ~kybd line is low, so when this
    // line goes high the data transfer has finished.
    mainMcuHasReadData = 1;

    // clear interrupt
    MCU_RECEIVING_DATA_INTERRUPT = 0;
  }
}

void interrupt_low() {
  if(COL_SCAN_TIMER_INTERRUPT){

    // clear interrupt and restart timer
    COL_SCAN_TIMER_INTERRUPT = 0;
    TMR0L = 0x28;

    cycleCounter++;
  }
}

void checkKeyStartSwitches(unsigned short newState, unsigned short column, unsigned short savedCycleCounter){

  unsigned short row = 0;
  unsigned short mask = 0b00000001;
  unsigned short noteIndex = column;
  unsigned short changes = noteStartSwitchStates[column] ^ newState;

  // most times nothing has changed so return without further checking.
  if(changes == 0){
    return;
  }

  for(row=0; row < ROWS; row++){
    if(changes & mask){ // row has changed
      if(newState & mask){ // row has been turned on, store start time
        noteTimers[noteIndex] = savedCycleCounter;
        hasSentOn[column] &= (~mask); // on has not been sent.
      } else { // row has been turned off
        //TODO: prevent key bouncing? holder det å sette note on not sent?
        if(hasSentOn[column] & mask){
          sendNoteOff(noteIndex);
        } else {
          // switch was turned off without having sent any on. Revert to
          // initial state by setting hasSentOn to true. This prevents
          // timeout from sending a note on if the start key is no longer
          // pressed
          hasSentOn[column] |= mask;
        }
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


void checkKeyEndSwitches(unsigned short newState, unsigned short column, unsigned short savedCycleCounter){

  unsigned short row = 0;
  unsigned short mask = 0b00000001;
  unsigned short noteIndex = column;
  unsigned short changes = noteEndSwitchStates[column] ^ newState;
  unsigned short velocity;
  
  // most times nothing has changed, so return without further checking.
  if(hasSentOn[column] == 0b11111111){
    return;
  }

  for(row=0; row < ROWS; row++){
    // Check to see if note on has already been sent. This prevents double sends
    // if key has only been released slightly or max time between start and end 
    // has been reached. It also prevents unecessary checking of keys that have
    // not been pressed as hasSentOn is only 0 when a key press start is
    // detected (and is reset to 1 if the key is released before reaching
    // bottom).
    if(!(hasSentOn[column] & mask)){
      if((changes & mask) && (newState & mask)){
        // Row has changed and is now turned on.

        // Since we're using unsigned shorts, we will get a mod 256 effect,
        // so the calculation will be correct even if cycleCounter is less
        // than noteTimer
        velocity = savedCycleCounter - noteTimers[noteIndex];
        sendNoteOn(noteIndex, velocity);
        hasSentOn[column] |= mask;
      } else {
        // Check if the maximum allowed delay between key press start and end
        // has been reached.
        if(savedCycleCounter - noteTimers[noteIndex] >= MAX_VELOCITY_TIME){
          sendNoteOn(noteIndex, MAX_VELOCITY_TIME);
          hasSentOn[column] |= mask;
        }
      }
    }

    // step to next note which is COLUMNS higher than the previous one.
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
void send(unsigned short value, unsigned short resetAfter){

  OUTPUT_BUS = value;
  mainMcuHasReadData = 0;
  OUTPUT_READY_PIN = 0; //indicate to the main mcu that data is ready

  #ifdef RUNTESTS
    // for testing, measures the number of bytes sent in total.
    bytesSent++;
  #endif
  
  // remove waiting code when in test mode to allow tests to pass
  #ifndef RUNTESTS
    // wait until main mcu indicates that it has read the data. The
    // data is read when the ~kybd line is low, so when this line goes
    // high the data transfer has finished.
    while(!mainMcuHasReadData){
    }

    if(resetAfter){
      OUTPUT_BUS = 0;
    }
  #endif
  // NB: OUTPUT_READY_PIN must be set to 1 by the calling
  // function when all bytes have been sent.
}

unsigned short calculateVelocity(unsigned short velocityTime){
  if(velocityTime < MAX_VELOCITY_TIME){
    return velocities[velocityTime];
  }
  return 1;
}

void sendNoteOn(unsigned short noteIndex, unsigned short velocityTime){
  unsigned short velocity = calculateVelocity(velocityTime);
  unsigned short noteToSend = (COMMAND_NOTE_ON | (BASE_NOTE + noteIndex));
  
  // store calculated results when in test mode to allow asserts on them
  #ifdef RUNTESTS
    lastNoteSent = noteToSend;
    lastVelocitySent = velocity;
  #endif
  send(noteToSend, 0);
  send(velocity, 1);

  // reset output ready since we have finished sending data
  OUTPUT_READY_PIN = 1;
}

void sendNoteOff(unsigned short noteIndex){
  // note off-command is 0 so no need for any special treatment.
  unsigned short noteToSend = (BASE_NOTE + noteIndex);
  
  send(noteToSend, 1);
  #ifdef RUNTESTS
    lastNoteSent = noteToSend;
  #endif
  
  // reset output ready since we have finished sending data
  OUTPUT_READY_PIN = 1;
}

void scanColumn(){
  unsigned short startKeyState;
  unsigned short endKeyState;
  unsigned short columnToCheck;
  unsigned short savedCycleCounter;

  // temporary store values, lets the rest of the system continue its job
  // while we process the data.
  startKeyState = KEYS_START_ROW;
  endKeyState = KEYS_END_ROW;
  // workaround for missing pin on real port.
  endKeyState.B4 = KEYS_END_ROW_MISSING_BIT;
  
  columnToCheck = currentColumn;
  savedCycleCounter = cycleCounter;

  // Switch to next row - lets values settle while we work on the current values
  currentColumn = ++currentColumn % COLUMNS;
  
  // Send clock pulse to increment CD4022 counter (column selector)
  COLUMN_CLOCK_PIN = 1;
  delay_us(2);
  COLUMN_CLOCK_PIN = 0;
  
  // Now go process the data!
  checkKeyStartSwitches(startKeyState, columnToCheck, savedCycleCounter);
  checkKeyEndSwitches(endKeyState, columnToCheck, savedCycleCounter);
}

void initKeyScanner(){
  unsigned short i;
  for(i=0; i<COLUMNS; i++){
    noteStartSwitchStates[i]=0;
    noteEndSwitchStates[i]=0;
    hasSentOn[i]=0xFF;
  }
  
  // column counter is calibrated later, just before scanning starts.
}

void initVelocityTiming(){
  unsigned short i;
  for(i=0; i<KEYCOUNT; i++){
    noteTimers[i]=0;
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
  
  // outputs
  OUTPUT_BUS_TRIS = 0;
  OUTPUT_BUS = 0;
  
  OUTPUT_READY_PIN_TRIS = 0;
  OUTPUT_READY_PIN = 1;
  
  COLUMN_CLOCK_PIN_TRIS = 0;
  COLUMN_CLOCK_PIN = 0;

  // inputs
  COLUMN_CARRY_PIN_TRIS = 1;
  DATA_BUS_DISABLED_PIN_TRIS_1 = 1;
  DATA_BUS_DISABLED_PIN_TRIS_2 = 1;
  KEYS_START_ROW_TRIS = 0xFF;
  KEYS_END_ROW_TRIS = 0xFF;
  KEYS_END_ROW_MISSING_BIT_TRIS = 1; // bugfix pin, replaces missing pin on port a
}

void setupTimers(){
  // timer should trigger interrupt every 432uS (the scan clock period of the
  // original 68b01

  // enable interrupt priorities
  IPEN_bit = 1;
  
  // clock is initially stopped.
  T0CON    = 0x42; // 8bit timer, prescaler 1:8 if frequency is 16MHz
  T0CON    = 0x44; // 8bit timer, prescaler 1:32 if frequency is 64MHz
  TMR0L    = 0x28;

  // clear any initial interrupt
  COL_SCAN_TIMER_INTERRUPT = 0;

  GIEL_bit   = 1; // enable low priority interrupts
  TMR0IP_bit = 0; // timer 0 interrupt has low priority
  TMR0IE_bit = 1; // turn on interrupt for timer 0
}

void startColumnClock(){
  TMR0ON_bit = 1;
}

void setupPortBInterrupt(){

  INT3IE_bit = 1; // enable external interrupts on portb.f3
  INTEDG3_bit = 1; // trigger on rising edge (i.e. read is complete)
  INT3IP_bit = 1; // external interrupt 3 has high priority
  
  // clear any initial interrupt
  MCU_RECEIVING_DATA_INTERRUPT = 0;

  GIEH_bit  = 1; // enable high priority interrupts
  RBIE_bit  = 0; // turn off interrupt on portb changes.
}

void setupOscillator(){
  //Additional settings in project -> edit project:
  // LF-INTOSC Low-power = LF-INTOSC in High-Power mode during Sleep
  // SOSC Power Selection and mode = Digital (SCLKI) mode (necessary for RC0 and RC1 to work properly)
  // Oscillator = Internal RC oscillator
  
  //TODO: Untested, these were left unchanged (disabled/16MHz) when changing
  // PLL in code and freq in other settings.
  // PLL x4 = Enabled
  // Frequency: 64MHz
  INTSRC_bit = 1;
  IRCF2_bit = 1;
  IRCF1_bit = 1;
  IRCF0_bit = 1;

  PLLEN_bit = 1; // 4 x PLL enabled, frequency 64MHz
}

// makes sure that the currently selected column is 0
void calibrateColumnClock(){

  while(COLUMN_CARRY_PIN == 1){
    COLUMN_CLOCK_PIN = 1;
    delay_us(2);
    COLUMN_CLOCK_PIN = 0;
    delay_us(20);
  }

  while(COLUMN_CARRY_PIN == 0){
    COLUMN_CLOCK_PIN = 1;
    delay_us(2);
    COLUMN_CLOCK_PIN = 0;
    delay_us(20);
  }
  
  // when the carry line goes high after having been low, we know that
  // the 0 output line of the CD4022 is high (See timing diagram on page
  // two of http://www.ti.com/lit/ds/symlink/cd4017b.pdf)
  currentColumn = 0;
}

void init(){
  mainMcuHasReadData = 0;
  initKeyScanner();
  initVelocityTiming();
  setupPortBInterrupt();
}

void initTests(){
  init();
  lastNoteSent = 0;
  lastVelocitySent = 0;
  bytesSent = 0;
}

#ifdef RUNTESTS
void main() {
  initTests();
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

  //Wait for the main mcu to get ready
  delay_ms(2000);
  
  calibrateColumnClock();

  // let the fun begin!
  startColumnClock();
  
  // back and forth, forever and ever.
  while(1){
    scanColumn();
  }
}

#endif