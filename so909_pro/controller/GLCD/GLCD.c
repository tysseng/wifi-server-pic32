#include <built_in.h> //needed for Hi() and Lo()
#include "GLCD.h"
#include "../../shared/bus_definitions.h"
#include "../../shared/char_addrs.h"

#define COMMAND_DELAY 5
/****************************************************************
Low level Graphical LCD functions

Test drivers voor het aansturen van een Optrex DMF-50773NF-FW
display met de Toshiba T6963C controller
A.Koopman november 2005

Adapted to microC by Joakim Tysseng
****************************************************************/


/**

Pin configuration, 18F258 / Powertip PG-240128A
-----------------------------------------------------------------------------
    PIC    |symbol|     explanation       | Pin number, display
-----------------------------------------------------------------------------
  LCD_CTRL_PORT.F0 |  CD  | Command / data select |  4 (0 = data select, 1 = command)
  LCD_CTRL_PORT.F1 |  RD  | Data read             |  5 (0 = enable)
  LCD_CTRL_PORT.F2 |  WR  | Data write            |  6 (0 = enable)
  LCD_CTRL_PORT.F5 |  RST | Reset                 | 16 (0 = reset)
  LCD_DATA_PORT    | DB0-7| Data bus line         | 7-14
-----------------------------------------------------------------------------

Other pins on display:
---------------------------
 1 - Vss - Power supply GND
 2 - Vdd - Power supply (+), 5v (+/- 0.5v max)
 3 - Vo  - Contrast Adjust
17 - Vee - Negative output
18 - MD2 - Number of columns 1 = 30, 0 = 40
19 - FS1 - Font selection (font width). 1=6px, 0=8px
20 - N/C - Not connected

Backlight:
A - anode,  positive (+)
K - catode, negative (-)

---------------------------

Display pin connections:
 1 - gnd
 2 - 5v

          10k pot
    GND __/  |  \__ 17, Vee
 3 _________/

 16 ----- 5V ---- switch ---- GND (use pull-up resistor!

 18 - GND
 19 - 5v, = 6px wide fonts
 20 - 5v

 A - +5v
 K -----/\/\/\/--- GND
       Resistor
**/

const char CheckStart = 0b00100101; //WR=1,RD=0,CD=1,RST=1 - disable data write, enable data read, command mode
const char CheckStop  = 0b00100111; //WR=1,RD=1,CD=1,RST=1 - disable data write, disable data read, command mode
const char WRData1    = 0b00100110; //WR=1,RD=1,CD=0,RST=1 - disable data write, disable data read, data mode
const char WRData2    = 0b00100010; //WR=0,RD=1,CD=0,RST=1 - enable data write, disable data read, data mode
const char WRData3    = 0b00100110; //WR=1,RD=1,CD=0,RST=1 - disable data write, disable data read, data mode
const char WRData4    = 0b00100111; //WR=1,RD=1,CD=1,RST=1 - disable data write, disable data read, command mode
const char SC1        = 0b00100011; //WR=0,RD=1,CD=1,RST=1 - send command 1 (enable data write, disable data read, command mode)
const char SC2        = 0b00100111; //WR=1,RD=1,CD=1,RST=1 - send command 2 (disable data write, disable data read, command mode)

/*************************[ Command definitions ]******************************/

//Register settings
const char CursorPtr   = 0x21; //0010 0001 -- Set Cursor Pointer  (X adr, Y adr)
const char OffsetReg   = 0x22; //0010 0010 -- Set Offset Pointer  (data, 0h)
const char ADPSet      = 0x24; //0010 0100 -- Set Address Pointer (High, Low)

//Set control word
const char TXHome      = 0x40; //0100 0000 -- Set Text Home Address
const char TXarea      = 0x41; //0100 0001 -- Set Text Area (number of columns per line)
const char GRHome      = 0x42; //0100 0010 -- Set Graphic Home Address
const char GRArea      = 0x43; //0100 0011 -- Set Graphic Area (number of bytes per line)

//mode settings - Two must be combined to make up a complete mode
const char modeOR     = 0x80; //1000 x000
const char modeXOR    = 0x81; //1000 x001
const char modeAND    = 0x81; //1000 x011 -- should this be 1000 0010?
const char modeTA     = 0x84; //1000 x100 -- Text Attribute mode
const char modeCGROM  = 0x80; //1000 0xxx -- Internal CG ROM mode
const char modeCGRAM  = 0x88; //1000 1xxx -- External CG RAM mode

//display modes - two can be combined for both graphics and text
const char dispOff    = 0x90; //1001 0000
const char dispCursNB = 0x92; //1001 xx10 -- cursor on, blink off
const char dispCursB  = 0x93; //1001 xx11 -- cursor on, blink on
const char dispText   = 0x94; //1001 x1xx -- Text on
const char dispGfx    = 0x98; //1001 1xxx -- Graphics on

//Cursor pattern select
const char curs1line  = 0xA0; //1010 0000 -- 1-line cursor
const char curs2line  = 0xA1; //1010 0001 -- 2-line cursor
const char curs3line  = 0xA2; //1010 0010 -- 3-line cursor
const char curs4line  = 0xA3; //1010 0011 -- 4-line cursor
const char curs5line  = 0xA4; //1010 0100 -- 5-line cursor
const char curs6line  = 0xA5; //1010 0101 -- 6-line cursor
const char curs7line  = 0xA6; //1010 0110 -- 7-line cursor
const char curs8line  = 0xA7; //1010 0111 -- 8-line cursor

//Data auto read/write - continous write/read without command
const char AWrite     = 0xB0; //1011 0000 -- Set Data Auto Write
const char ARead      = 0xB1; //1011 0001 -- Set Data Auto Read
const char AReset     = 0xB2; //1011 0010 -- Auto reset

//Data read/write
const char DWI        = 0xC0; //1100 0000 -- write char, inc pointer (ADP)
const char DRI        = 0xC1; //1100 0001 -- read  char, inc pointer (ADP)
const char DWD        = 0xC2; //1100 0010 -- write char, dec pointer (ADP)
const char DRD        = 0xC3; //1100 0011 -- read  char, dec pointer (ADP)
const char DWN        = 0xC4; //1100 0100 -- write char, leave pointer (ADP)
const char DRN        = 0xC5; //1100 0101 -- read  char, leave pointer (ADP)

//Screen Peek/copy
const char ScreenPeek = 0xE0; //1110 0000 -- Screen peek
const char ScreenCopy = 0xE8; //1110 1000 -- Screen copy

//Bit set/reset
const char BitSet     = 0xF8; //1111 1000 -- Bit set, must be or'ed with address
const char BitReset   = 0xF0; //1111 0000 -- Bit reset, must be or'ed with addr.

/***********************[ End, Command definitions ]***************************/

//Address offset to start of character generator ram.

//FIX: WORKS WITH 3 ONLY!
const char Offset   = 0x03;

//text RAM start address
const char Adrmsb   = 0x00;
const char Adrlsb   = 0x00;

//Length of each text row (or rather, distance to start of next row)
const char Cols   = 0x28; //40 columns 240/6 pixels

//The text uses 40 cols * 16 rows = 640 bytes of ram, starting from 0x0000.
//We then start with graphics from 0x0280 (address 640)
//Graphics RAM start address
const char Graphmsb = 0x02;
const char Graphlsb = 0x80;




//JT: why are these global? does it save space?
char StatusCheck;
int Teller;
int Dpos;
unsigned DposH;
char DposLsb;
char DposMsb;
int Wi;
int Pos;
char Letter;
int Teller1;
                //dim Displ as string[40]
char* Displ; //not sure about this, see above.

//text pointer address storage.
unsigned short oldAdrRow = 0;
unsigned short oldAdrCol = 0;
unsigned short currRow;
unsigned short currCol;

//
// Setup port directions and initial values for LCD control
//
void GLCD_CTRL_initPorts(){
  LCD_CTRL_DIRECTION = DATA_OUT;
  LCD_DATA_DIRECTION = DATA_OUT;
  LCD_CTRL_PORT = 0x27; //00100111;
  LCD_DATA_PORT = 0x00;
}

//
// Prepare display for sequencer output
//
void GLCD_CTRL_init(){
  GLCD_CTRL_DReset();
  GLCD_CTRL_TextHome();
  GLCD_CTRL_TextArea();
  GLCD_CTRL_GraphHome();
  GLCD_CTRL_GraphArea();
  GLCD_CTRL_SetOffset();
  GLCD_CTRL_ClrGraph();
  GLCD_CTRL_CleanTxtScreen();
  GLCD_CTRL_SetMode();
  GLCD_CTRL_SetDisplay();
  GLCD_CTRL_SendCmd(curs1line);}

/**
 * Reset display, brings pin 0 low for 2ms?
 **/
void GLCD_CTRL_DReset(){
  LCD_CTRL_PORT.F5 = 1;
  Delay_us(1);
  LCD_CTRL_PORT.F5 = 0;
  Delay_us(1);
  LCD_CTRL_PORT.F5 = 1; // maak RESET pin 2 uSec laag !
}

/**
 * Status check, waits untill STA0 and STA1 are high, i.e. the display is
 * ready for new commands/data
 *
 * STA0 = command execution capability
 * STA1 = data read / write capability.
 */
void GLCD_CTRL_StCheck(){
  LCD_DATA_DIRECTION = DATA_IN;
  StatusCheck = 0;                  //reset StatusCheck
  LCD_CTRL_PORT = CheckStart;       //WR=1,RD=0,CD=1,RST=1,CE=0

  while( StatusCheck != 3 ) {       //wait until STA0 and STA1 is high
    StatusCheck = LCD_DATA_PORT & 0b00000011;
  }
  LCD_CTRL_PORT = CheckStop;        //WR=1,RD=1,CD=1,RST=1,CE=1
  LCD_DATA_DIRECTION = DATA_OUT;
}

/**
 * Auto mode check, waits untill display is ready for more data when in
 * auto write mode.
 *
 * STA3 = auto mode data write capability.
 */
void GLCD_CTRL_AMCheck(){
  LCD_DATA_DIRECTION = DATA_IN;       //LCD_DATA_PORT as inputs
  StatusCheck = 0;                  //reset StatusCheck
  LCD_CTRL_PORT = CheckStart;       //WR=1,RD=0,CD=1,RST=1,CE=0

  while( StatusCheck != 8 ) {       //wait until STA3 is high
    StatusCheck = LCD_DATA_PORT & 0b00001000;
  }

  LCD_CTRL_PORT = CheckStop;        //WR=1,RD=1,CD=1,RST=1,CE=1
  LCD_DATA_DIRECTION = DATA_OUT;      //LCD_DATA_PORT as output
}

/**
 * Send a single byte of data
 */
void GLCD_CTRL_Outdata(char Cdata){
  LCD_CTRL_PORT = WRData1;
  delay_us(COMMAND_DELAY);
  LCD_DATA_PORT = Cdata;
  delay_us(COMMAND_DELAY);
  LCD_CTRL_PORT = WRData2;
  delay_us(COMMAND_DELAY);
  LCD_CTRL_PORT = WRData3;
  delay_us(COMMAND_DELAY);
  LCD_CTRL_PORT = WRData4;
}

/**
 * Send a command
 */
void GLCD_CTRL_SendCmd(char SCommand){
  GLCD_CTRL_StCheck();
  LCD_DATA_PORT = SCommand;
  delay_us(COMMAND_DELAY);
  LCD_CTRL_PORT = SC1;
  delay_us(COMMAND_DELAY);
  LCD_CTRL_PORT = SC2;
}


/**
 * TODO: Currently not used!
 */
void GLCD_CTRL_CmdAdr(char ADRLSB, char ADRMSB){
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(ADRLSB);
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(ADRMSB);
}

/**
 * Set the location in the display memory where the text area starts
 */
void GLCD_CTRL_TextHome(){
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(Adrlsb);
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(Adrmsb);
  GLCD_CTRL_SendCmd(TXHome);
}

/**
 * Sets the number of columns in each display line. The number of columns
 * must be contained within the global Cols variable. (Cols is shared between
 * text area and graphics area because characters are set to 8bit width)
 */
void GLCD_CTRL_TextArea(){
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(Cols);
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(0x00);
  GLCD_CTRL_SendCmd(TXArea);
}

/**
 * Set the location in the display memory where the graphics area starts
 */
void GLCD_CTRL_GraphHome(){
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(Graphlsb);
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(Graphmsb);
  GLCD_CTRL_SendCmd(GRHome);
}

/**
 * Sets the number of bytes in each display line. The number of columns
 * must be contained within the global Cols variable. (Cols is shared between
 * text area and graphics area because characters are set to 8bit width).
 */
void GLCD_CTRL_GraphArea(){
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(Cols);
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(0x00);
  GLCD_CTRL_SendCmd(GRArea);
}

/**
 * set character mode to internal ROM, i.e. use built in characters.
 */
void GLCD_CTRL_SetMode(){
  GLCD_CTRL_SendCmd(modeCGROM);
}

/**
 * Turn on graphics and display, turn off cursor and blink.
 **/
void GLCD_CTRL_SetDisplay(){
  GLCD_CTRL_SendCmd(dispText|dispGfx);
}

void GLCD_CTRL_CursorOn(){
  GLCD_CTRL_SendCmd(dispText|dispGfx|dispCursB);
}

void GLCD_CTRL_CursorOff(){
  GLCD_CTRL_SendCmd(dispText|dispGfx);
}

void GLCD_CTRL_SetOffset(){
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(Offset);
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(0x00);
  GLCD_CTRL_SendCmd(OffsetReg);
}

/**
 * Set cursor pointer, I.E position of cursor
 */
void GLCD_CTRL_CursorPtr(char Row, char Col){
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(Col);
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(Row);
  GLCD_CTRL_SendCmd(CursorPtr);
}

/**
 * Set address pointer, used before reading/writing text, graphics, attributes
 * or user-character bytes.
 */
void GLCD_CTRL_ScreenAdr(char MSB, char LSB){
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(LSB);
  GLCD_CTRL_StCheck();
  GLCD_CTRL_Outdata(MSB);
  GLCD_CTRL_SendCmd(ADPSet);
}

/**
 * Write 0 to all of text memory
 */
void GLCD_CTRL_CleanTxtScreen(){
  GLCD_CTRL_ScreenAdr(0x00,0x00);
  GLCD_CTRL_SendCmd(AWrite);
  teller = 0;

  while(teller < 640){
    Teller++;
    GLCD_CTRL_AMCheck();
    GLCD_CTRL_Outdata(0x00);
  }
  GLCD_CTRL_SendCmd(AReset);
}

/**
 * Write a single character/1 byte when in auto write mode.
 */
void GLCD_CTRL_AutoWrite(char value){
  GLCD_CTRL_AMCheck();
  GLCD_CTRL_Outdata(value);
}

/**
 * Write empty data to all of graphics memory;
 */
void GLCD_CTRL_ClrGraph(){
  GLCD_CTRL_ScreenAdr(0x02,0x80);
  GLCD_CTRL_SendCmd(AWrite);
  teller1 = 0;              //count lines
  while( teller1 < 5500 ) { //was <> 5500
    GLCD_CTRL_Outdata(0x00);
    teller1++;              //i assume inc means ++
  }
  GLCD_CTRL_SendCmd(AReset);
}

/**
 * Set pointer to the correct address, given a row and column.
 * Row starts on 1, Col starts on 0????
 */
void GLCD_CTRL_PrtAdr(char row, char col){
  currRow = row;
  currCol = col;
  Dpos  = (row * 40) + col; //assumed 40 columns per row.
  DposH = (unsigned)Dpos;
  DposLsb = Lo(DposH);
  DposMsb = Hi(DposH);
  GLCD_CTRL_ScreenAdr(DposMsb,DposLsb);
}

/**
 * Store current cursor position
 */
void GLCD_CTRL_LL_pushAdr(){
  oldAdrRow = currRow;
  oldAdrCol = currCol;
}

/**
 * Reload old cursor position
 */
void GLCD_CTRL_LL_popAdr(){
  GLCD_CTRL_PrtAdr(oldAdrRow,oldAdrCol);
}

/**
 * Print a text string at the current position
 **/
void GLCD_CTRL_PrtTxt(char* Text){
  while( *Text != 0 ){
    GLCD_CTRL_Outdata(*Text - 0x20);
    GLCD_CTRL_SendCmd(DWI);
    Text++;
  }
}


/**
 * Print a character at the current position and increment write pointer
 **/
void GLCD_CTRL_PrtCharInc(char c){
  GLCD_CTRL_Outdata(c);
  GLCD_CTRL_SendCmd(DWI);
}


void GLCD_CTRL_PaintO(int x, int y, int p){
  volatile unsigned Paddress;
  char Pmsb;
  char Plsb;
  char Offset;

  if(p == 0){
    Offset = BitReset;
  } else {
    Offset = BitSet;
  }

  Paddress = 0x0280 + y*Cols + x/6;
  Pmsb = Hi(Paddress);
  Plsb = Lo(Paddress);

  GLCD_CTRL_ScreenAdr(Pmsb,Plsb);
  GLCD_CTRL_SendCmd(Offset | (5-(x % 6)));
}

//for some reason, x1 and x2 has to be ints. It seems that the
//variables undeflow or something
// x1 = starting x
// x2 = ending x
// y = start and end y
void GLCD_CTRL_Horizontal_Line(int x1, int x2, char y, char p){

  volatile unsigned Paddress;
  char Pmsb;
  char Plsb;
  char Offset;
  char currX;
  currX = x1;

  if(p == 0){
    Offset = 0xF0;
  } else {
    Offset = 0xF8;
  }

  Paddress = 0x0280 + (unsigned)y*Cols + x1/6;
  Pmsb = Hi(Paddress);
  Plsb = Lo(Paddress);
  GLCD_CTRL_ScreenAdr(Pmsb,Plsb);
  GLCD_CTRL_SendCmd(AWrite);

  //special case if line starts and ends within the same column. Remember
  //that this is not floating point division so the remainder is discarded.
  if( x1/6 == x2/6 ) {
    GLCD_CTRL_Outdata((0x3f >> (x1%6)) & (0xff << (5 - (x2%6))));
  } else {
    GLCD_CTRL_Outdata(0x3f >> (x1%6));           //partially filled column, start
    currX += (x1%6);
    while( x2 - currX > 7 ) {
      GLCD_CTRL_Outdata(0xFF);                   //fully filled columns
      currX += 6;
    }
    GLCD_CTRL_Outdata(0xff << (5 - (x2%6)));     //partially filled column, end
  }
  GLCD_CTRL_SendCmd(AReset);
}


/**
 * Draw vertical line
 **/
void GLCD_CTRL_Vertical_Line(int start, int end, int col, int paint){
  int i;
  i = start;
  while(i <= end){
    GLCD_CTRL_PaintO(col,i,paint);
    i++;
  }
}


void GLCD_CTRL_RectangleOn(int xA, int yA, int xB, int yB, int paint){
  int i;
  i = yA;
  while(i <= yB){
    GLCD_CTRL_Horizontal_Line(xA,xB,i,paint);
    i++;
  }
}


void GLCD_CTRL_H_dotted(int start, int end, int rij, int paint){
  int i;
  i = start;
  while(i <= end){
    GLCD_CTRL_PaintO(i,rij,paint);
    i += 2;
  }
}


void GLCD_CTRL_V_dotted(int start, int end, int col, int paint){
  int i;
  i = start;
  while(i <= end){
    GLCD_CTRL_PaintO(col,i,paint);
    i += 2;
  }
}

/**
 Load a user-defined character at the keycode address of memory area starting
 at offset. data is a 8 char long array where each char represents a line
 of the character.
 **/
void GLCD_CTRL_LoadChar(char keycode, char* charData ){

  char i;
  //Each block/charset of CG ram is 2048 bytes (=256 characters) long.
  //To find the start address, multiply offset by 0x0800 (=2048).
  //NB: we need to cast keycode, if not the higher order byte will be chopped
  //of. Address is volatile because of a bug in MikroC 5.0.0.0 (apparently
  //fixed in 5.0.0.1)
  volatile unsigned address;
  address = Offset * 0x0800 + (unsigned)keycode * 0x0008;

  GLCD_CTRL_ScreenAdr( Hi(address), Lo(address) );
  GLCD_CTRL_SendCmd(AWrite);
  for( i=0; i<8; i++){
    GLCD_CTRL_Outdata(charData[i]);
  }
  GLCD_CTRL_SendCmd(AReset);

}

/**
 * Load bar graph chars in a more efficient way than other chars
 **/
void GLCD_CTRL_LoadBarGraphChars(char keycode){
  char i,j;
  volatile unsigned address;
  address = Offset * 0x0800 + (unsigned)keycode * 0x0008;

  GLCD_CTRL_ScreenAdr( Hi(address), Lo(address) );
  GLCD_CTRL_SendCmd(AWrite);
  for( i=1; i<9; i++){
    for( j=8; j>0; j--){
      if(j<=i) GLCD_CTRL_Outdata(0x3E);
      else GLCD_CTRL_Outdata(0x00);
    }
  }
  GLCD_CTRL_SendCmd(AReset);

}

/**
 * Debug routine, print bit values of a short at the current position
 **/
void GLCD_CTRL_PrtByte(unsigned short byte){
  GLCD_CTRL_PrtCharInc(0x10+byte.F0);
  GLCD_CTRL_PrtCharInc(0x10+byte.F1);
  GLCD_CTRL_PrtCharInc(0x10+byte.F2);
  GLCD_CTRL_PrtCharInc(0x10+byte.F3);
  GLCD_CTRL_PrtCharInc(0x10+byte.F4);
  GLCD_CTRL_PrtCharInc(0x10+byte.F5);
  GLCD_CTRL_PrtCharInc(0x10+byte.F6);
  GLCD_CTRL_PrtCharInc(0x10+byte.F7);
}

/**
 Write a three digit int (with blank padding)
 **/
void GLCD_CTRL_LL_writeNum3(unsigned int number){
  if( number > 99 ) {
    GLCD_CTRL_PrtCharInc(0x10 + number / 100);
  } else {
    GLCD_CTRL_PrtCharInc(0x00);
  }
  if( number > 9 ) {
    GLCD_CTRL_PrtCharInc(0x10 + (number %100) / 10);
  } else {
    GLCD_CTRL_PrtCharInc(0x00);
  }
  GLCD_CTRL_PrtCharInc(0x10 + number % 10);
}

/**
 Write a two digit int (with blank padding)
 **/
void GLCD_CTRL_LL_writeNum2(unsigned int number){
  if( number > 9 ) {
    GLCD_CTRL_PrtCharInc(0x10 + (number %100) / 10);
  } else {
    GLCD_CTRL_PrtCharInc(0x00);
  }
  GLCD_CTRL_PrtCharInc(0x10 + number % 10);
}