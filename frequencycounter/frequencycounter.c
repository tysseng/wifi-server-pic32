/*
 A program for measuring musical frequencies. Adapted from johnt's CCP test
 program found at http://www.mikroe.com/forum/viewtopic.php?f=88&t=26489
 
 The program measures a pulse train on the CCP1 input pin (PORTC.B2) and
 calculates the input frequency through an average of 16 samples.
 
 The result is accurate down to +/- 0.1 Hz, which is good enough for audio
 frequencies as the human ear cannot distinguish between changes of 
 less than 3 Hz (in the 1000 to 2000Hz range).

 HARDWARE:
       The code is written for PIC18F458 at 20MHz.
       T1 is a 16 bit counter in free running mode is used to capture pulse widths
       PORTC Pin 2 is used for input
       PORTB is used for an LCD display, connected to the default pins of the
       Mikroelektronika EasyPIC3 development board.
 
  Written using MicroC PRO for PIC v 4.60
  
  If changing the operating speed, you must take care so that T1 does not over-
  flow.

  The current implementation seems to be accurate for frequencies in the range
  150Hz to 6.4kHz. (saw waves. clean square waves will be measureable all the
  way down to 77Hz without a prescaler. With a 1:4 prescaler, frequencies down
  to 20Hz seem measureable. Similarly, a 1:8 prescaler will let you measure
  down to 10Hz.

  To interface the circuit with an audio signal, an external circuit is
  necessary. Ex: To convert a +/- 15V sine or similar signal into a 0-5v input
  pulse, the following circuit can be used (though it requires a triple-
  value power supply):

                   LM311 comparator        /--------------> output to PORTC.B2
                       ------              |
  GND   <-------------|1    8|---> +15V    |
  Input <-------------|2    7|-------------o-[ 1kOhm pull up resistor ]--> +5V
  GND   <-------------|3    6|---\ (connect pin 5 to pin 6)
  -15V  <-------------|4    5|---/
                       ------
  Joakim Tysseng, March 2011
*/
#include "frequencycounter.h"
#include "frequencyToNoteConverter.h"
#include "built_in.h"              // to get useful built in commands highest() delay_ms() etc

// LCD module connections
sbit LCD_RS at RB2_bit;
sbit LCD_EN at RB3_bit;
sbit LCD_D4 at RB4_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D7 at RB7_bit;

sbit LCD_RS_Direction at TRISB2_bit;
sbit LCD_EN_Direction at TRISB3_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;
// End LCD module connections

//GLOBAL CONSTANTS
const unsigned short pulsesToCapture = 16;

//PREDEFINED TEXTS
char txt1[] = "Note:           ";
char txt2[] = "       /        ";

//GLOBAL VARIABLES
unsigned int capturedData = 0 ;              // value captured by the CCP module
unsigned short captures[2*pulsesToCapture] ; // cyclic storage for 16 captures (ints)
unsigned short pulsesCaptured = 0 ;

unsigned long  clockCyclesPerSecondTimesFactor = 0;
unsigned short clockCyclesPerTimerClick = 0;
unsigned short interruptDelayClockCycles = 0;

//GLOBAL POINTERS
unsigned short *pCaptures ;                  // pointer to the captures array


void main(){
    unsigned int frequency;
    unsigned int average;

    hardwareInit() ;
    initPitchArray();
    frequencyCalculatorInit(10);
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,1,txt1);
    Lcd_Out(2,1,txt2);

    startCapture();                    // first capture
    do {
       //perform another capture when we are done treating the previous one.
       if(pulsesCaptured > pulsesToCapture){
         average = getAverageCapture();
         frequency = calculateFrequency(average);
         printNote(frequency, 10);
         startCapture();
       }
    } while (1);
}

// Start capture process, enabling the CCP module and resetting necessary
// variables.
void startCapture(){
    pCaptures = &captures ;              // resetting overwrites previous captures
    pulsesCaptured = 0;
    CCP1CON = 0b00000101 ;               // Start capture, capture on every rising edge
}

//returns the frequency, depending on the selected scale factor the last digits
//may be decimals
unsigned int calculateFrequency(unsigned int timer){
  unsigned long clockCycles;
  
  // Convert to long to get correct calculation later
  unsigned long timerAsLong = timer;

  // We have to add extra clock cycles to the supplied timer value.
  // This is because we lose some time between when the interrupt is triggered
  // and the timer is restarted. If we do not compensate for this, the measured
  // frequency will be too high. The error will be larger for higher
  // frequencies, as the lost time accounts for a larger proportion of the total
  // period.

  clockCycles = timerAsLong * clockCyclesPerTimerClick + interruptDelayClockCycles;
  return clockCyclesPerSecondTimesFactor / clockCycles;
}

unsigned int getAverageCapture(){
  unsigned short i;
  unsigned long sum = 0;
  for(i=0; i<2*pulsesToCapture; i+=2){
    sum += (captures[i] << 8) + captures[i+1];  //combine two shorts to an int.
  }
  return sum >> 4;
}

void hardwareInit(void) {          // basic hardware control
  ADCON1 = 0x07;
  PORTA = 0x00 ;
  TRISA = 0X00 ;
  PORTC = 0b00000100 ;             // normally high - depending on hardware
  TRISC = 0b00000100 ;             // portc.b2 as input

  T1CON.TMR1ON = 0 ;               // ensure it's off
  T1CON.T1OSCEN = 0 ;              // ensure OSC is OFF
  T1CON.TMR1CS = 0;                // Timer1 counts pulses from internal oscillator
  T1CON.T1CKPS1 = 1;               // prescale the input pulses
  T1CON.T1CKPS0 = 0;               // Assigned T1 prescaler rate is 1:4

  INTCON.PEIE = 1 ;                // enable peripheral interrupts
  PIE1.CCP1IE = 1 ;                // enable CCP1 capture interrupt
  INTCON.GIE = 1 ;                 // enable global interrupts
}

void frequencyCalculatorInit(unsigned short factor){
    // Using a 1:4 prescaler for the timer seems to be the best tradeoff between
    // lowest possible measureable frequency and reasonable accuracy in the
    // high frequency region. Effective work area is between 20 and
    clockCyclesPerSecondTimesFactor = Clock_kHz() * 1000 * factor;

    // prescaler = 1:4
    clockCyclesPerTimerClick = 16;

    // 16 instructions plus some strange offset. 10 seems to work when prescaler
    // is 1:1, 15 when prescaler is 1:4
    interruptDelayClockCycles = 16 * 4 + 15;
}

// How to accurately measure the delay between when an interrupt occurs and 
// the time T1 is restarted (PIC18):
// - Every interrupt has a latency of three instruction cycles.
// - Each instruction takes two cycles, but the instruction pipeline handles
//   two instructions at the time, meaning instructions only take one cycle in
//   practice.
// - If a branch/goto is triggered, the pipeline is reset and the first
//   instruction takes two cycles.

// In this code:
// 3 instructions interrupt latency
// 2 instructions in C, none of which triggers a branch.
// 11 instructions in assembly
// = 16 cycles. 

// NB: For some reason the correct number to add to the timer when
// calculating the correct frequency is always two more than this.
// I am not sure where the last cycles go, could it be interrupt jitter?

//INTERRUPT HANDLING
void interrupt(void) {

    // Capture CPP interrupt ie. rising edge on PORTC pin 2 has occured
    if (PIR1.CCP1IF){

        // The following asm code is compiled from the C code below, and is
        // kept in ASM to keep the instruction count the same for all versions
        // of the compiler (necessary to achieve correct timing).
        asm{
            MOVF        CCPR1H+0, 0
            MOVWF       _capturedData+1
            MOVF        CCPR1L+0, 0
            MOVWF       _capturedData+0
            BCF         T1CON+0, 0
            CLRF        TMR1H+0
            CLRF        TMR1L+0
            BSF         T1CON+0, 5
            BCF         T1CON+0, 4
            BCF         PIR1+0, 0
            BSF         T1CON+0, 0
        }

        // C code for the ASM above - if you want to recompile the code.
        /*
        hi(capturedData) = CCPR1H ;  // capture data
        lo(capturedData) = CCPR1L ;

        // Restart Timer1
        T1CON.TMR1ON = 0 ;               //++ Turning off the timer also resets T1 prescaler
        TMR1H = 0x00 ;                   // Set initial value for the timer TMR1 for maximum counts available
        TMR1L = 0x00 ;
        T1CON.T1CKPS1 = 1 ;              // Assigned T1 prescaler rate is 1:4
        T1CON.T1CKPS0 = 0 ;
        PIR1.TMR1IF = 0 ;                // clear T1 interrupt flag
        T1CON.TMR1ON = 1 ;               // turn T1 back on
        */
        
        storeInput() ;                   // copy the captured data to the array of samples.
        PIR1.CCP1IF = 0 ;                // clear capture flag after grabbing data
    }
}

void storeInput(void) {
    if (pulsesCaptured == 0) {           // ignore startup delay - start storing between first + second edge
        pulsesCaptured = 1 ;
        return ;
    }
    pulsesCaptured++;
    if(pulsesCaptured > pulsesToCapture){
        CCP1CON = 0 ;                    // Stop capture when the desired number of pulses have been captured.
    }
   *(pCaptures++) = hi(capturedData) ;   // store captured hibyte:lobyte in array
   *(pCaptures++) = lo(capturedData) ;
}