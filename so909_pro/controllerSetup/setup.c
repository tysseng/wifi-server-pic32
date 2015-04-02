#include <built_in.h>
#include "chars.h"
#include "file.h"
#include "../shared/bus_definitions.h"
#include "../shared/sequencer_setup.h"
#include "../controller/GLCD/GLCD.h"
#include "../controller/storage/external_storage.h"

char currLine;

void MCU_init(){

  ADCON0 = 0; // turn off A/D converter
  CMCON |= 0x07; //turn off comparators

  //ADCON1 = 0x07; //turn off analogue inputs (for P18F458)
  ADCON1 = 0x0F;   //turn off analogue inputs (for P18F4620)
  CCP1CON = 0 ;    //disable the enhanced capture/PWM module (for P18F4620)
  
  ADDRESS_BUS_DIRECTION.B0 = DATA_OUT;
  ADDRESS_BUS_DIRECTION.B1 = DATA_OUT;
  ADDRESS_BUS_DIRECTION.B2 = DATA_OUT;
  ADDRESS_BUS_DIRECTION.B3 = DATA_OUT;
  KBD_DISABLE_DIRECTION = DATA_OUT;
  LED_ENABLE_DIRECTION = DATA_OUT;
  DIAL_ENABLE_DIRECTION = DATA_OUT;
  DATA_BUS_DIRECTION = DATA_IN;
  TRISE.B1 = DATA_OUT; //debug led 1
  TRISE.B2 = DATA_OUT; //debug led 2
  KBD_DISABLE = 1;
  LED_ENABLE = 0;
  
  UART1_Init(31250);
}

void MCU_setupFat(){

  struct Pattern pattern;

  GLCD_CTRL_PrtAdr(currLine++, 2);
  GLCD_CTRL_PrtTxt("Writing default setup and FAT");
  FILE_resetFileAndPatternNames();
  SEQ_SETUP_setDefaultSetup(&pattern);
  EEX_writeSetup(&pattern);

}

void MCU_initChars(){
  GLCD_CTRL_PrtAdr(currLine++, 2);
  GLCD_CTRL_PrtTxt("Initializing characters");
  SETUP_CHAR_init();
}

void MCU_loadGrid(){
  GLCD_CTRL_PrtAdr(currLine++, 2);
  GLCD_CTRL_PrtTxt("Storing grid characters");
  SETUP_CHAR_loadGridChars();
}

void MCU_loadInstrument(){
  GLCD_CTRL_PrtAdr(currLine++, 2);
  GLCD_CTRL_PrtTxt("Storing instrument characters");
  SETUP_CHAR_loadInstrumentChars();
}

void MCU_loadSpecial(){
  GLCD_CTRL_PrtAdr(currLine++, 2);
  GLCD_CTRL_PrtTxt("Storing special characters");
  SETUP_CHAR_loadSpecialChars();
}

void MCU_loadFlam(){
  GLCD_CTRL_PrtAdr(currLine++, 2);
  GLCD_CTRL_PrtTxt("Storing flam characters");
  SETUP_CHAR_loadFlamChars();
}

void MCU_loadBorder(){
  GLCD_CTRL_PrtAdr(currLine++, 2);
  GLCD_CTRL_PrtTxt("Storing border characters");
  SETUP_CHAR_loadBorderChars();
}

void main(){

  MCU_init();

  PORTE.B1 = 0;
  GLCD_CTRL_initPorts();
  GLCD_CTRL_init();

  GLCD_CTRL_PrtAdr(1, 0);
  GLCD_CTRL_PrtTxt("  Machinebeats initialization program  ");
  GLCD_CTRL_PrtAdr(2, 0);
  GLCD_CTRL_PrtTxt("---------------[v 0.9b]----------------");

  currLine=4;

  MCU_setupFat();
  MCU_initChars();
  MCU_loadGrid();
  MCU_loadInstrument();
  MCU_loadSpecial();
  MCU_loadFlam();
  MCU_loadBorder();
  delay_ms(100);

  GLCD_CTRL_PrtAdr(13, 2);
  GLCD_CTRL_PrtTxt("Program finished  ");
}