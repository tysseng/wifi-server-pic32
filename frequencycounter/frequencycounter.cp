#line 1 "C:/git/mikroc/frequencycounter/frequencycounter.c"
#line 1 "c:/users/public/documents/mikroelektronika/mikroc pro for pic/include/built_in.h"
#line 44 "C:/git/mikroc/frequencycounter/frequencycounter.c"
void interrupt(void);
void main(void);
void hardwareInit(void);
void storeInput(void);
void startCapture();

unsigned int getAverageCapture();
unsigned int calculateFrequency(unsigned int timerValue);
void Lcd_Out_Timer(unsigned short row, unsigned short col, unsigned int number);
void Lcd_Out_Freq(unsigned short row, unsigned short col, unsigned int number);


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


char txt1[] = "Timer....: ";
char txt2[] = "Frequency: ";


unsigned int capturedData = 0 ;
unsigned short captures[2*pulsesToCapture] ;
unsigned short pulsesCaptured = 0 ;


unsigned short *pCaptures ;


void main(){
 unsigned int frequency;
 unsigned int average;

 hardwareInit() ;
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
 Lcd_Out_Timer(1, 12, average);
 Lcd_Out_Freq(2, 11, frequency);
 startCapture();
 }
 } while (1);
}



void startCapture(){
 pCaptures = &captures ;
 pulsesCaptured = 0;
 CCP1CON = 0b00000101 ;
}


unsigned int calculateFrequency(unsigned int timerValue){
 unsigned int clockcycles;






 clockcycles = (timerValue + 17) * 4;
 return (Clock_kHz() * 10000 ) / clockcycles;
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
#line 179 "C:/git/mikroc/frequencycounter/frequencycounter.c"
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
 BCF T1CON+0, 5
 BCF T1CON+0, 4
 BCF PIR1+0, 0
 BSF T1CON+0, 0
 }
#line 216 "C:/git/mikroc/frequencycounter/frequencycounter.c"
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



void Lcd_Out_Timer(unsigned short row, unsigned short col, unsigned int number){
 unsigned char digits[5];
 digits[0] = number % 10;
 digits[1] = (number / 10) % 10;
 digits[2] = (number / 100) % 10;
 digits[3] = (number / 1000) % 10;
 digits[4] = (number / 10000) % 10;

 Lcd_Chr(row, col+4, 48 + digits[0]);
 Lcd_Chr(row, col+3, 48 + digits[1]);
 Lcd_Chr(row, col+2, 48 + digits[2]);
 Lcd_Chr(row, col+1, 48 + digits[3]);
 Lcd_Chr(row, col, 48 + digits[4]);
}

void Lcd_Out_Freq(unsigned short row, unsigned short col, unsigned int number){
 unsigned char digits[5];
 digits[0] = number % 10;
 digits[1] = (number / 10) % 10;
 digits[2] = (number / 100) % 10;
 digits[3] = (number / 1000) % 10;
 digits[4] = (number / 10000) % 10;

 Lcd_Chr(row, col+5, 48 + digits[0]);
 Lcd_Chr(row, col+4, 46);
 Lcd_Chr(row, col+3, 48 + digits[1]);
 Lcd_Chr(row, col+2, 48 + digits[2]);
 Lcd_Chr(row, col+1, 48 + digits[3]);
 if(digits[4] == 0){
 Lcd_Chr(row, col, 32);
 } else {
 Lcd_Chr(row, col, 48 + digits[4]);
 }
}
