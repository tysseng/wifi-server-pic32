#ifndef _GLCD_H
#define _GLCD_H

/****************************************************************
Test drivers voor het aansturen van een Optrex DMF-50773NF-FW
display met de Toshiba T6963C controller
A.Koopman november 2005

Adapted to microC by Joakim Tysseng
****************************************************************/

/**
Pin configuration, 18F452 / Powertip PG-240128A
-----------------------------------------------------------------------------
    PIC    |symbol|     explanation       | Pin number, display
-----------------------------------------------------------------------------
  PORTB.F0 |  WR  | Data write            |  6 (0 = enable)
  PORTB.F1 |  RD  | Data read             |  5 (0 = enable)
  PORTB.F2 |  CD  | Command / data select |  4 (0 = data select, 1 = command)
  PORTB.F3 |  RST | Reset                 | 16 (0 = reset)
  PORTB.F4 |  CE  | Chip enable           | 15 (0 = enable)
     ?     |  MD2 | Select number of cols | 18
     ?     |  FS1 | Font selection        | 19
  PORTD    | DB0-7| Data bus line         | 7-14
-----------------------------------------------------------------------------

Other pins on display:
---------------------------
 1 - Vss - Power supply GND
 2 - Vdd - Power supply (+), 5v (+/- 0.5v max)
 3 - Vo  - Contrast Adjust
17 - Vee - Negative output
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

 16 ----- 5V ---- switch ---- GND (use pull-up resistor!)

 18 - GND
 19 - 5v ???
 20 - 5v

 A - +5v
 K -----/\/\/\/--- GND
       Resistor
**/

/**** FUNCTIONS ****/
void GLCD_CTRL_initPorts();
void GLCD_CTRL_init();
void GLCD_CTRL_DReset();
void GLCD_CTRL_StCheck();
void GLCD_CTRL_AMCheck();
void GLCD_CTRL_Outdata(char Cdata);
void GLCD_CTRL_SendCmd(char SCommand);
void GLCD_CTRL_CmdAdr(char ADRLSB, char ADRMSB);
void GLCD_CTRL_TextHome();
void GLCD_CTRL_TextArea();
void GLCD_CTRL_GraphHome();
void GLCD_CTRL_GraphArea();
void GLCD_CTRL_SetMode();
void GLCD_CTRL_SetDisplay();
void GLCD_CTRL_CursorOn();
void GLCD_CTRL_CursorOff();
void GLCD_CTRL_SetOffset();
void GLCD_CTRL_CursorPtr(char row, char col);
void GLCD_CTRL_ScreenAdr(char MSB, char LSB);
void GLCD_CTRL_CleanTxtScreen();
void GLCD_CTRL_AutoWrite(char value);
void GLCD_CTRL_ClrGraph();
void GLCD_CTRL_PrtAdr(char row, char col);
void GLCD_CTRL_LL_pushAdr();
void GLCD_CTRL_LL_popAdr();
void GLCD_CTRL_PrtTxt(char* Text);
void GLCD_CTRL_PrtCharInc(char c);
void GLCD_CTRL_PaintO(int x, int y, int p);
void GLCD_CTRL_Horizontal_Line(int x1, int x2, char y, char p);
void GLCD_CTRL_Vertical_Line(int start, int end, int col, int paint);
void GLCD_CTRL_RectangleOn(int xA, int yA, int xB, int yB, int paint);
void GLCD_CTRL_H_dotted(int start, int end, int rij, int paint);
void GLCD_CTRL_V_dotted(int start, int end, int col, int paint);
void GLCD_CTRL_LoadChar(char keycode, char* charData );
void GLCD_CTRL_LoadBarGraphChars(char keycode);
void GLCD_CTRL_PrtByte(unsigned short byte);
void GLCD_CTRL_LL_writeNum3(unsigned int number);
void GLCD_CTRL_LL_writeNum2(unsigned int number);
#endif