#include <built_in.h>
#include "file.h"
#include "../controller/shared/types.h"
#include "../controller/storage/external_storage.h"
#include "../controller/storage/external_storage_private.h"

void FILE_resetFileAndPatternNames(){
  EEX_init();
  FILE_writePatternFileAddresses();
  FILE_writeSongFileAddresses();
  FILE_writePerformanceFileAddresses();
}

void FILE_writePerformanceFileAddresses(){
  char perfNum;
  int address = MEMMAP_POS_PERFORMANCES;

  for(perfNum = 0; perfNum < MEMMAP_FILES_PER_FOLDER; perfNum++){
    FILE_writeFileNameAndAddress(MEMMAP_CHIP_PERFORMANCES, MEMMAP_FOLDER_PERFORMANCE, perfNum, address);
    address = address + MEMMAP_SIZE_PERFORMANCE;
  }
}

void FILE_writeSongFileAddresses(){
  char songNum;
  int address = MEMMAP_POS_SONGS;

  for(songNum = 0; songNum < MEMMAP_FILES_PER_FOLDER; songNum++){
    FILE_writeFileNameAndAddress(MEMMAP_CHIP_SONGS, MEMMAP_FOLDER_SONG, songNum, address);
    address = address + MEMMAP_SIZE_SONG;
  }
}

void FILE_writePatternFileAddresses(){
  char folderNum;
  char pattNum;
  char name[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  char chip = MEMMAP_CHIP1;
  int address = MEMMAP_POS_PATTERNS;

  for(folderNum = 0; folderNum < MEMMAP_PATTERN_FOLDERS; folderNum++){
    EEX_writeFolderName(folderNum, name);
    delay_ms(5);

    // Patterns in folders 5-8 are stored on chip number 2
    // Addresses are re-used on the second chip.
    if(folderNum==4){
      chip = MEMMAP_CHIP2;
      address = MEMMAP_POS_PATTERNS;
    }

    for(pattNum = 0; pattNum < MEMMAP_FILES_PER_FOLDER; pattNum++){
      FILE_writeFileNameAndAddress(chip, folderNum, pattNum, address);
      address = address + MEMMAP_SIZE_PATT;
    }
  }
}

void FILE_writeFileNameAndAddress(char chip, char folderNum, char fileNum, int address){
  char name[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  char fileAddress[4] = {0, 0, 0, 0};
  fileAddress[0] = chip;

  EEX_writeFileName(folderNum, fileNum, name);
  delay_ms(5);

  fileAddress[1] = Hi(address);
  fileAddress[2] = Lo(address);

  EEX_writeFileAddress(folderNum, fileNum, fileAddress);
  delay_ms(5);
}
