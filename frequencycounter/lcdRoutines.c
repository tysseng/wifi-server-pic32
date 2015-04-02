#include "lcdRoutines.h"

void Lcd_Out_ULong(unsigned short row, unsigned short col, unsigned long number){
  unsigned char digits[9];
  digits[0] = number % 10;
  digits[1] = (number / 10) % 10;
  digits[2] = (number / 100) % 10;
  digits[3] = (number / 1000) % 10;
  digits[4] = (number / 10000) % 10;
  digits[5] = (number / 100000) % 10;
  digits[6] = (number / 1000000) % 10;
  digits[7] = (number / 10000000) % 10;
  digits[8] = (number / 100000000) % 10;
  digits[9] = (number / 1000000000) % 10;

  Lcd_Chr(row, col+9, 48 + digits[0]);
  Lcd_Chr(row, col+8, 48 + digits[1]);
  Lcd_Chr(row, col+7, 48 + digits[2]);
  Lcd_Chr(row, col+6, 48 + digits[3]);
  Lcd_Chr(row, col+5, 48 + digits[4]);
  Lcd_Chr(row, col+4, 48 + digits[5]);
  Lcd_Chr(row, col+3, 48 + digits[6]);
  Lcd_Chr(row, col+2, 48 + digits[7]);
  Lcd_Chr(row, col+1, 48 + digits[8]);
  Lcd_Chr(row, col  , 48 + digits[9]);
}

void Lcd_Out_UInt(unsigned short row, unsigned short col, unsigned int number){
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
  Lcd_Chr(row, col  , 48 + digits[4]);
}


void Lcd_Out_UShort(unsigned short row, unsigned short col, unsigned short number){
  unsigned char digits[3];
  digits[0] = number % 10;
  digits[1] = (number / 10) % 10;
  digits[2] = (number / 100) % 10;

  Lcd_Chr(row, col+2, 48 + digits[0]);
  Lcd_Chr(row, col+1, 48 + digits[1]);
  Lcd_Chr(row, col  , 48 + digits[2]);
}


// Utility function that writes the five last digits of an integer to the LCD
// display
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

void Lcd_Out_Freq(unsigned short row, unsigned short col, unsigned long number){
  unsigned char digits[5];
  digits[0] = number % 10;
  digits[1] = (number / 10) % 10;
  digits[2] = (number / 100) % 10;
  digits[3] = (number / 1000) % 10;
  digits[4] = (number / 10000) % 10;
  digits[5] = (number / 100000) % 10;

  Lcd_Chr(row, col+6, 48 + digits[0]);
  Lcd_Chr(row, col+5, 48 + digits[1]);
  Lcd_Chr(row, col+4, 46);
  Lcd_Chr(row, col+3, 48 + digits[2]);
  Lcd_Chr(row, col+2, 48 + digits[3]);
  Lcd_Chr(row, col+1, 48 + digits[4]);
  if(digits[5] == 0){
    Lcd_Chr(row, col, 32);
  } else {
    Lcd_Chr(row, col, 48 + digits[5]);
  }
}