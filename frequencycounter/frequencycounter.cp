#line 1 "C:/git/mikroc/frequencycounter/frequencycounter.c"
#line 1 "c:/git/mikroc/frequencycounter/frequencycounter.h"
void interrupt(void);
void main(void);
void hardwareInit(void);
void frequencyCalculatorInit(unsigned short factor);
void storeInput(void);
void startCapture();
unsigned int getAverageCapture();
unsigned int calculateFrequency(unsigned int timerValue);
#line 1 "c:/git/mikroc/frequencycounter/frequencytonoteconverter.h"
void initPitchArray();
void printNote(unsigned int frequency, int factor);
unsigned short getOctave(unsigned short semitone);
unsigned short getSemitoneInOctave(unsigned short semitone);
void lookupAndPrintTone(unsigned short octave, unsigned short semitone, int mismatch);
void printTone(char* note, unsigned short octave);
void printMismatch(int mismatch);
unsigned short getNumberOfSymbols(int mismatch);
void Lcd_Out_ULong(unsigned short row, unsigned short col, unsigned long number);
void Lcd_Out_UInt(unsigned short row, unsigned short col, unsigned int number);
void Lcd_Out_UShort(unsigned short row, unsigned short col, unsigned short number);
#line 1 "c:/users/public/documents/mikroelektronika/mikroc pro for pic/include/built_in.h"
#line 48 "C:/git/mikroc/frequencycounter/frequencycounter.c"
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



const unsigned short pulsesToCapture = 16;


char txt1[] = "Note:           ";
char txt2[] = "       /        ";


unsigned int capturedData = 0 ;
unsigned short captures[2*pulsesToCapture] ;
unsigned short pulsesCaptured = 0 ;

unsigned long clockCyclesPerSecondTimesFactor = 0;
unsigned short clockCyclesPerTimerClick = 0;
unsigned short interruptDelayClockCycles = 0;


unsigned short *pCaptures ;


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

 startCapture();
 do {

 if(pulsesCaptured > pulsesToCapture){
 average = getAverageCapture();
 frequency = calculateFrequency(average);
 printNote(frequency, 10);
 startCapture();
 }
 } while (1);
}



void startCapture(){
 pCaptures = &captures ;
 pulsesCaptured = 0;
 CCP1CON = 0b00000101 ;
}



unsigned int calculateFrequency(unsigned int timer){
 unsigned long clockCycles;


 unsigned long timerAsLong = timer;








 clockCycles = timerAsLong * clockCyclesPerTimerClick + interruptDelayClockCycles;
 return clockCyclesPerSecondTimesFactor / clockCycles;
}

unsigned int getAverageCapture(){
 unsigned short i;
 unsigned long sum = 0;
 for(i=0; i<2*pulsesToCapture; i+=2){
 sum += (captures[i] << 8) + captures[i+1];
 }
 return sum >> 4;
}

void hardwareInit(void) {
 ADCON1 = 0x07;
 PORTA = 0x00 ;
 TRISA = 0X00 ;
 PORTC = 0b00000100 ;
 TRISC = 0b00000100 ;

 T1CON.TMR1ON = 0 ;
 T1CON.T1OSCEN = 0 ;
 T1CON.TMR1CS = 0;
 T1CON.T1CKPS1 = 0;
 T1CON.T1CKPS0 = 0;

 INTCON.PEIE = 1 ;
 PIE1.CCP1IE = 1 ;
 INTCON.GIE = 1 ;
}

void frequencyCalculatorInit(unsigned short factor){



 clockCyclesPerSecondTimesFactor = Clock_kHz() * 1000 * factor;


 clockCyclesPerTimerClick = 16;



 interruptDelayClockCycles = 16 * 4 + 15;
}
#line 196 "C:/git/mikroc/frequencycounter/frequencycounter.c"
void interrupt(void) {


 if (PIR1.CCP1IF){




 asm{
 MOVF CCPR1H+0, 0
 MOVWF _capturedData+1
 MOVF CCPR1L+0, 0
 MOVWF _capturedData+0
 BCF T1CON+0, 0
 CLRF TMR1H+0
 CLRF TMR1L+0
 BSF T1CON+0, 5
 BCF T1CON+0, 4
 BCF PIR1+0, 0
 BSF T1CON+0, 0
 }
#line 233 "C:/git/mikroc/frequencycounter/frequencycounter.c"
 storeInput() ;
 PIR1.CCP1IF = 0 ;
 }
}

void storeInput(void) {
 if (pulsesCaptured == 0) {
 pulsesCaptured = 1 ;
 return ;
 }
 pulsesCaptured++;
 if(pulsesCaptured > pulsesToCapture){
 CCP1CON = 0 ;
 }
 *(pCaptures++) =  ((char *)&capturedData)[1]  ;
 *(pCaptures++) =  ((char *)&capturedData)[0]  ;
}
