#include <built_in.h>
#include "external_storage.h"
#include "external_storage_private.h"
#include "../GLCD/GLCD.h"
#include "../sequencer/sequencer.h"
#include "../../shared/char_addrs.h"
#include "../shared/types.h"
#include "../shared/global.h"
#include "../shared/definitions.h"
#include "../shared/asm_usart.h"

int currImportBlock;
char currImportChip;

void EEX_init(){
  I2C1_Init(200000);
}

//
// Read a single file name
//
void EEX_readFileName(char folderNum, char fileNum, char* name){
  int address;
  address = MEMMAP_POS_FOLDER_CONTENT +
            folderNum * MEMMAP_SIZE_FOLDER_CONTENT_NAMES +
            fileNum * NAME_LENGTH;
  EEX_readBytes(MEMMAP_CHIP1, address, name, NAME_LENGTH);
}

//
// Write a single file name
//
void EEX_writeFileName(char folderNum, char fileNum, char* name){
  int address;
  address = MEMMAP_POS_FOLDER_CONTENT +
            folderNum * MEMMAP_SIZE_FOLDER_CONTENT_NAMES +
            fileNum * NAME_LENGTH;
  EEX_writeBytes(MEMMAP_CHIP1, address, name, NAME_LENGTH);
}

//
// Read a single folder name
//
void EEX_readFolderName(char folderNum, char* name){
  int address;
  address = MEMMAP_POS_FOLDERS + folderNum * NAME_LENGTH;
  EEX_readBytes(MEMMAP_CHIP1, address, name, NAME_LENGTH);
}

//
// Write a single folder name
//
void EEX_writeFolderName(char folderNum, char* name){
  int address;
  address = MEMMAP_POS_FOLDERS + folderNum * NAME_LENGTH;
  EEX_writeBytes(MEMMAP_CHIP1, address, name, NAME_LENGTH);
}


//
// Read a file address pointer. Format: {chip, hi, lo}
//
void EEX_readFileAddress(char folderNum, char fileNum, char* fileAddress){
  int address;
  address = MEMMAP_POS_FOLDER_CONTENT_ADDRESSES +
            folderNum * MEMMAP_SIZE_FOLDER_CONTENT_ADDRESSES +
            fileNum * MEMMAP_SIZE_CONTENT_ADDRESS;
  EEX_readBytes(MEMMAP_CHIP1, address, fileAddress, MEMMAP_SIZE_CONTENT_ADDRESS);
}

//
// Write a file address pointer. Format: {chip, hi, lo}
//
void EEX_writeFileAddress(char folderNum, char fileNum, char* fileAddress){
  int address;
  address = MEMMAP_POS_FOLDER_CONTENT_ADDRESSES +
            folderNum * MEMMAP_SIZE_FOLDER_CONTENT_ADDRESSES +
            fileNum * MEMMAP_SIZE_CONTENT_ADDRESS;
  EEX_writeBytes(MEMMAP_CHIP1, address, fileAddress, MEMMAP_SIZE_CONTENT_ADDRESS);
}

//
// Read a full pattern
//
void EEX_readPattern(char folderNum, char pattNum, struct Pattern* targetPatt){

  char i;
  char addressBytes[4];
  int address;

  targetPatt->folderNum = folderNum+1;
  targetPatt->fileNum = pattNum+1;

  EEX_readFileAddress(folderNum, pattNum, addressBytes);
  address = (addressBytes[1] << 8) + addressBytes[2];

  for(i = 0; i< MAX_TRACKS; i++){
    EEX_readSteps(addressBytes[0], address, (targetPatt->tracks[i]).steps, (targetPatt->tracks[i]).flams);
    address = address + MEMMAP_SIZE_TRACK;
  }

  EEX_readTrackAndPatternSetup(addressBytes[0], address, targetPatt);

}

//
// Write a full pattern
//
void EEX_writePattern(char folderNum, char pattNum, struct Pattern* sourcePatt){

  char i;
  char addressBytes[18];
  int address;
  
  EEX_readFileAddress(folderNum, pattNum, addressBytes);
  address = (addressBytes[1] << 8) + addressBytes[2];

  for(i = 0; i< MAX_TRACKS; i++){
    EEX_writeSteps(addressBytes[0], address, (sourcePatt->tracks[i]).steps, (sourcePatt->tracks[i]).flams);
    address = address + MEMMAP_SIZE_TRACK;
    delay_ms(5);
  }
  EEX_writeTrackAndPatternSetup(addressBytes[0], address, sourcePatt);
}

//
// Read all steps in a single track from a specified address.
//
void EEX_readSteps(char chip, int address, char* velocity, char* flam){

  char i;

  EEX_startRead(chip, address);
  for( i=0; i<MAX_STEPS; i++){
    if(i!=(MAX_STEPS-1)){
      velocity[i] = I2C1_Rd(SEND_ACK);
      flam[i] = I2C1_Rd(SEND_ACK);
    } else {
      velocity[i] = I2C1_Rd(SEND_ACK);
      flam[i] = I2C1_Rd(DONT_SEND_ACK);
    }
  }

  I2C1_Stop();
}

//
// Write all steps in a single track to a specified address
//
void EEX_writeSteps(char chip, int address, char* velocity, char* flam){

  char i;

  EEX_startWrite(chip, address);
  for( i=0; i<MAX_STEPS; i++){
    I2C1_Wr(velocity[i]); // Write data
    I2C1_Wr(flam[i]);     // Write data
  }
  I2C1_Stop();
}

//
// Read global setup for pattern and setup details for all tracks
//
void EEX_readTrackAndPatternSetup(char chip, int address, struct Pattern* targetPatt){
  char i;

  EEX_startRead(chip, address);
  
  for(i = 0; i< MAX_TRACKS; i++){
    (targetPatt->tracks[i]).name     = I2C1_Rd(SEND_ACK);
    (targetPatt->tracks[i]).basenote = I2C1_Rd(SEND_ACK);
    (targetPatt->tracks[i]).velocity = I2C1_Rd(SEND_ACK);
    (targetPatt->tracks[i]).accent   = I2C1_Rd(SEND_ACK);
    (targetPatt->tracks[i]).channel  = I2C1_Rd(SEND_ACK);
  }

  targetPatt->steps = I2C1_Rd(SEND_ACK);
  targetPatt->res = I2C1_Rd(SEND_ACK);
  targetPatt->bpb = I2C1_Rd(SEND_ACK);
  targetPatt->trackcount = I2C1_Rd(SEND_ACK);
  if(G_SEQ_resetBpmOnLoad){
    targetPatt->swingBalance = I2C1_Rd(SEND_ACK);
    targetPatt->bpm = I2C1_Rd(DONT_SEND_ACK);
  } else {
    targetPatt->swingBalance = I2C1_Rd(DONT_SEND_ACK);
    targetPatt->bpm = patt->bpm;
  }
  I2C1_Stop();
}

//
// Read global setup for pattern and setup details for all tracks
//
void EEX_writeTrackAndPatternSetup(char chip, int address, struct Pattern* sourcePatt){
  char i;

  EEX_startWrite(chip, address);

  for(i = 0; i< MAX_TRACKS; i++){
    I2C1_Wr((sourcePatt->tracks[i]).name);
    I2C1_Wr((sourcePatt->tracks[i]).basenote);
    I2C1_Wr((sourcePatt->tracks[i]).velocity);
    I2C1_Wr((sourcePatt->tracks[i]).accent);
    I2C1_Wr((sourcePatt->tracks[i]).channel);
  }

  I2C1_Wr(sourcePatt->steps);
  I2C1_Wr(sourcePatt->res);
  I2C1_Wr(sourcePatt->bpb);
  I2C1_Wr(sourcePatt->trackcount);
  I2C1_Wr(sourcePatt->swingBalance);
  I2C1_Wr(sourcePatt->bpm);
  
  I2C1_Stop();
}

void EEX_readSetupGeneral(){

  EEX_startRead(MEMMAP_CHIP_CURRENT_SETUP, MEMMAP_POS_CURR_SETUP);
  G_MIDI_isExternalSynchOn = I2C1_Rd(SEND_ACK);
  G_MIDI_sysexIdByte1 = I2C1_Rd(SEND_ACK);
  G_MIDI_sysexIdByte2 = I2C1_Rd(DONT_SEND_ACK);
  I2C1_Stop();
}

void EEX_writeSetupGeneral(){
  EEX_startWrite(MEMMAP_CHIP_CURRENT_SETUP, MEMMAP_POS_CURR_SETUP);
  I2C1_Wr(G_MIDI_isExternalSynchOn);
  I2C1_Wr(G_MIDI_sysexIdByte1);
  I2C1_Wr(G_MIDI_sysexIdByte2);
  I2C1_Stop();
  delay_ms(5);
}

void EEX_readSetup(struct Pattern *pattern){
  EEX_readSetupGeneral();
  EEX_readTrackAndPatternSetup(MEMMAP_CHIP_CURRENT_SETUP, MEMMAP_POS_CURR_PATT, pattern);
}

void EEX_writeSetup(struct Pattern *pattern){
  EEX_writeSetupGeneral();
  EEX_writeTrackAndPatternSetup(MEMMAP_CHIP_CURRENT_SETUP, MEMMAP_POS_CURR_PATT, pattern);
}

//
// Maps a setup parameter item and index to a position in memory
//
void EEX_writeSetupParameter(char item, char index, char value){
  if(item == SETUP_ITEM_GENERAL){
    switch(index){
      case 0: EEX_writeSetupGeneralParameter(MEMMAP_INDEX_GEXTSYNCHON, value); break;
      case 1: EEX_writeSetupGeneralParameter(MEMMAP_INDEX_GSYSEXID1, value); break;
      case 2: EEX_writeSetupGeneralParameter(MEMMAP_INDEX_GSYSEXID2, value); break;
    }
  } else if(item == SETUP_ITEM_PATTERN){
    switch(index){
      case 0: EEX_writeSetupPatternParameter(MEMMAP_INDEX_PTRACKCOUNT, value); break;
      case 1: EEX_writeSetupPatternParameter(MEMMAP_INDEX_PSTEPS, value); break;
      case 2: EEX_writeSetupPatternParameter(MEMMAP_INDEX_PRES, value); break;
      case 3: EEX_writeSetupPatternParameter(MEMMAP_INDEX_PBPB, value); break;
      case 4: EEX_writeSetupPatternParameter(MEMMAP_INDEX_PSWINGBALANCE, value); break;
    }
  } else {
    switch(index){
//      case 0: EEX_writeSetupTrackParameter(item-2, MEMMAP_INDEX_TNAME, value); break;
      case 0: EEX_writeSetupTrackParameter(item-2, MEMMAP_INDEX_TBASENOTE, value); break;
      case 1: EEX_writeSetupTrackParameter(item-2, MEMMAP_INDEX_TVELOCITY, value); break;
      case 2: EEX_writeSetupTrackParameter(item-2, MEMMAP_INDEX_TACCENT, value); break;
      case 3: EEX_writeSetupTrackParameter(item-2, MEMMAP_INDEX_TCHANNEL, value - 1); break;
    }
  }
}

//
// Write single general setup parameter
//
void EEX_writeSetupGeneralParameter(char index, char value){
  int address;
  address = MEMMAP_POS_CURR_SETUP + index;
  EEX_writeByte(MEMMAP_CHIP_CURRENT_SETUP, address, value);
}

//
// Write single pattern setup parameter
//
void EEX_writeSetupPatternParameter(char index, char value){
  int address;
  address =
    MEMMAP_POS_CURR_PATT +
    MEMMAP_SIZE_TRACK_SETUP * MAX_TRACKS +
    index;

  EEX_writeByte(MEMMAP_CHIP_CURRENT_SETUP, address, value);
}

//
// Write single track setup parameter
//
void EEX_writeSetupTrackParameter(char track, char index, char value){
  int address;
  address =
    MEMMAP_POS_CURR_PATT +
    MEMMAP_SIZE_TRACK_SETUP * track +
    index;

  EEX_writeByte(MEMMAP_CHIP_CURRENT_SETUP, address, value);
}

//
// Read global setup for pattern and setup details for all tracks
//
void EEX_readSong(char songNum, struct SongElement* songElements){

  char addressBytes[4];
  int address;

  EEX_readFileAddress(MEMMAP_FOLDER_SONG, songNum, addressBytes);
  address = (addressBytes[1] << 8) + addressBytes[2];

  //NB: change this if song length != 128, as we have to align reads to
  //128byte section borders.
  GLCD_CTRL_PrtAdr(5,12);
  EEX_readSongPart(addressBytes[0], address, songElements, 0, (MAX_SONG_PATTERNS/2));

  address = address + 128;
  EEX_readSongPart(addressBytes[0], address, songElements, MAX_SONG_PATTERNS/2, MAX_SONG_PATTERNS);
}

//
// A song is more than 128 bytes and thus have to be read in two separate ops
//
void EEX_readSongPart(char chip, int address, struct SongElement* songElements, char start, char end){
  char i;

  EEX_startRead(chip, address);
  for(i = start; i < end-1; i++){
    songElements[i].pattern = I2C1_Rd(SEND_ACK);
    songElements[i].repeat = I2C1_Rd(SEND_ACK);
  }
  songElements[end-1].pattern = I2C1_Rd(SEND_ACK);
  songElements[end-1].repeat = I2C1_Rd(DONT_SEND_ACK);

  I2C1_Stop();
}

//
// Write all song elements of a song.
//
void EEX_writeSong(char songNum, struct SongElement* songElements){
  char addressBytes[4];
  int address;

  EEX_readFileAddress(MEMMAP_FOLDER_SONG, songNum, addressBytes);
  address = (addressBytes[1] << 8) + addressBytes[2];

  //NB: change this if song length != 128, as we have to align writes to
  //128byte section borders.
  EEX_writeSongPart(addressBytes[0], address, songElements, 0, MEMMAP_SONG_ELEMENTS/2);
  delay_ms(5);
  
  address = address + 128;
  EEX_writeSongPart(addressBytes[0], address, songElements, MEMMAP_SONG_ELEMENTS/2, MEMMAP_SONG_ELEMENTS);
}

void EEX_writeSongPart(char chip, int address, struct SongElement* songElements, char start, char end){
  char i;

  EEX_startWrite(chip, address);

  for(i = start; i < end; i++){
    I2C1_Wr(songElements[i].pattern);
    I2C1_Wr(songElements[i].repeat);
  }

  I2C1_Stop();
}

//
// Read all key-pattern mappings for a performance
//
void EEX_readPerformance(char perfNum, char* patternKeyMap){
  char addressBytes[4];
  int address;
  char i;

  EEX_readFileAddress(MEMMAP_FOLDER_PERFORMANCE, perfNum, addressBytes);
  address = (addressBytes[1] << 8) + addressBytes[2];

  EEX_startRead(addressBytes[0], address);
  for(i = 0; i < MEMMAP_SIZE_PERFORMANCE-1; i++){
    patternKeyMap[i] = I2C1_Rd(SEND_ACK);
  }
  patternKeyMap[MEMMAP_SIZE_PERFORMANCE-1] = I2C1_Rd(DONT_SEND_ACK);

  I2C1_Stop();

}

//
// Write all key-pattern mappings for a performance
//
void EEX_writePerformance(char perfNum, char* patternKeyMap){
  char addressBytes[4];
  int address;
  char i;

  EEX_readFileAddress(MEMMAP_FOLDER_PERFORMANCE, perfNum, addressBytes);
  address = (addressBytes[1] << 8) + addressBytes[2];

  EEX_startWrite(addressBytes[0], address);
  for(i = 0; i < MEMMAP_SIZE_PERFORMANCE; i++){
    I2C1_Wr(patternKeyMap[i]);
  }
  I2C1_Stop();

}

//
// Read a single display character from the external storage
//
void EEX_readDisplayCharacter(char keycode, char* charData){
  int address;
  address = MEMMAP_POS_CHAR + (keycode - CHAR_FIRST_ADDRESS) * 8;
  EEX_readBytes(MEMMAP_ADDR_CHARACTER_CHIP, address, charData, 8);
}

void EEX_startRead(char chip, int address){
  I2C1_Start();
  I2C1_Wr(chip);         // Device address + Write
  I2C1_Wr(Hi(address));  // High order byte of address
  I2C1_Wr(Lo(address));  // Low order byte of address
  I2C1_Repeated_Start();
  I2C1_Wr(chip+1);       // Device address + Read
}

void EEX_startWrite(char chip, int address){
  I2C1_Start();
  I2C1_Wr(chip);          // Device address + Write
  I2C1_Wr(Hi(address));   // High order byte of address
  I2C1_Wr(Lo(address));   // Low order byte of address
}

//
// Read a single byte from a specified address
//
char EEX_readByte(char chip, int address){
  char byteReadFromMem;
  EEX_startRead(chip, address);
  byteReadFromMem = I2C1_Rd(DONT_SEND_ACK);
  I2C1_Stop();
  return byteReadFromMem;
}

//
// Write a single byte to a specified address
//
void EEX_writeByte(char chip, int address, char byteToWrite){
  EEX_startWrite(chip, address);
  I2C1_Wr(byteToWrite);        // Write data
  I2C1_Stop();
}

//
// Read an arbitrary number of bytes from the specified address
//
void EEX_readBytes(char chip, int address, char* targetByteArray, char length){
  char i;

  EEX_startRead(chip, address);
  for(i=0; i<length; i++){
    if(i!=length-1){
      targetByteArray[i] = I2C1_Rd(SEND_ACK);
    } else {
      targetByteArray[i] = I2C1_Rd(DONT_SEND_ACK);
    }
  }

  I2C1_Stop();
}

//
// Write an arbitrary number of bytes to the specified address
//
void EEX_writeBytes(char chip, int address, char* bytesToWrite, char length){
  char i;

  EEX_startWrite(chip, address);
  for( i=0; i<length; i++){
    I2C1_Wr(bytesToWrite[i]);
  }
  I2C1_Stop();
}

void EEX_exportMemoryAsMidi(char chip){

  int i;
  char byteFromMemory;
  char hi;
  char lo;

  EEX_startRead(chip, 0);
  for(i=0; i<65535; i++){
    byteFromMemory = I2C1_Rd(SEND_ACK);
    hi = byteFromMemory >> 4;
    lo = byteFromMemory & 0x0F;
    ASM_USART_Write(hi);
    ASM_USART_Write(lo);
  }
  
  byteFromMemory = I2C1_Rd(DONT_SEND_ACK);
  hi = byteFromMemory >> 4;
  lo = byteFromMemory & 0x0F;
  ASM_USART_Write(hi);
  ASM_USART_Write(lo);

  I2C1_Stop();
}

void EEX_importMemoryFromMidiStart(){
  currImportChip = MEMMAP_CHIP1;
  currImportBlock = 0;
}

//
// Writes 128 bytes of data to memory
//
void EEX_importMemoryFromMidi(char *dataFromMidi){

  char i;
  char combinedData;
  
  if(currImportBlock == 512){
    currImportChip = MEMMAP_CHIP2;
    currImportBlock = 0;
  }

  EEX_startWrite(currImportChip, currImportBlock*8);
  for(i=0; i<128;i++){
    combinedData = (dataFromMidi[2*i] << 4) | dataFromMidi[2*i+1];
    I2C1_Wr(combinedData);
  }
  I2C1_Stop();
  delay_ms(5);
  currImportBlock++;
}