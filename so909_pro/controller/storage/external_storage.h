#ifndef _EXTERNAL_STORAGE_H
#define _EXTERNAL_STORAGE_H

/**
Memory map:

byte range:
   0 -  127: 8 folder names * 16 bytes (128 bytes)
 128 - 1663: 12 file names * 16 bytes * 8 folders (1536 bytes)
1664 - 2047: 12 files * 8 folders * 4 byte address (288 bytes) (only 3 bytes used, but 4 bytes used to get correct boundaries for continous reads)
2048 -

FAT is only on the first memory module and takes up 2560 bytes.
On the second module, this space is used for display characters.

8 folders
12 patterns per folder
16 chars per folder name
16 chars per track name
2 byte address
tracks: 64 data, 5 setup
12 tracks per pattern
5 setup bytes per pattern
= 69*12+5 = 833 bytes per pattern --> 7 blocks of 128 bytes = 896
*/

// Setup: indexes for separate parameters
// Global:
#define MEMMAP_INDEX_GEXTSYNCHON 0
#define MEMMAP_INDEX_GSYSEXID1 1
#define MEMMAP_INDEX_GSYSEXID2 2
// Pattern:
#define MEMMAP_INDEX_PSTEPS 0
#define MEMMAP_INDEX_PRES 1
#define MEMMAP_INDEX_PBPB 2
#define MEMMAP_INDEX_PTRACKCOUNT 3
#define MEMMAP_INDEX_PSWINGBALANCE 4
// Track
#define MEMMAP_INDEX_TNAME 0
#define MEMMAP_INDEX_TBASENOTE 1
#define MEMMAP_INDEX_TVELOCITY 2
#define MEMMAP_INDEX_TACCENT 3
#define MEMMAP_INDEX_TCHANNEL 4

#define MEMMAP_CHIP1 0xA0
#define MEMMAP_CHIP2 0xA2

#define MEMMAP_CHIP_SONGS 0xA0
#define MEMMAP_CHIP_PERFORMANCES 0xA2
#define MEMMAP_CHIP_CURRENT_SETUP 0xA2

// File allocation table
#define MEMMAP_SIZE_FOLDER_CONTENT_NAMES 192    // 16 chars * 12 patterns
#define MEMMAP_SIZE_CONTENT_ADDRESS 4           // length of address pointers [chip, hi byte, low byte]
#define MEMMAP_SIZE_FOLDER_CONTENT_ADDRESSES 48 // length of all addresses for a single folder

// Pattern sizes
#define MEMMAP_SIZE_PATT 896
#define MEMMAP_SIZE_TRACK 64                    // velocity + flam * 32
#define MEMMAP_SIZE_TRACK_SETUP 5
#define MEMMAP_SIZE_PATT_SETUP 5
#define MEMMAP_SIZE_12_TRACKS 768

// Performance sizes
#define MEMMAP_SIZE_PERFORMANCE 32               // 32 key/pattern mappings
#define MEMMAP_FOLDER_PERFORMANCE 9              // performances are found in the tenth folder

// Song sizes
#define MEMMAP_FOLDER_SONG 8                     // songs are found in the ninth folder
#define MEMMAP_SIZE_SONG 256                     // 128 pattern/repeat combinations, 2 bytes each
#define MEMMAP_SONG_ELEMENTS 128                 // 128 elements

#define MEMMAP_PATTERN_FOLDERS 8                 // first four folders are on chip one, next 4 on chip two
#define MEMMAP_OTHER_FOLDERS 2                   // song and performance
#define MEMMAP_FILES_PER_FOLDER 12

#define MEMMAP_POS_FOLDERS 0
#define MEMMAP_POS_FOLDER_CONTENT 128            // 16 chars * 8 folders
#define MEMMAP_POS_FOLDER_CONTENT_ADDRESSES 2048 // Start pos for address pointers = pattern folder name size (128) + pattern/song/perf name size (192 chars * 10 folders)
#define MEMMAP_POS_PATTERNS 2560                 // Position of first pattern = startpos for address pointers + pattern addresses
                                                 // (480bytes, rounded up to 512 bytes)
#define MEMMAP_POS_SONGS 60416                   // First byte after patterns, POS PATTERNS + PATT SIZE * 64
#define MEMMAP_POS_PERFORMANCES 60416            // First byte after patterns, POS PATTERNS + PATT SIZE * 64

#define MEMMAP_POS_CURR_PATT 60800               // Area where the current pattern is stored, makes sure patterns are still there if the machine is switched
                                                 // off and on. First byte after performances, POS_PERFORMANCES + SIZE_PERFORMANCE * 12
#define MEMMAP_POS_CURR_SETUP 61696              // CURR_PATT + PATTERN SIZE, area for storing sequencer setup


void EEX_init();

void EEX_readFileName(char folderNum, char pattNum, char* name);
void EEX_writeFileName(char folderNum, char pattNum, char* name);

void EEX_readFolderName(char folderNum, char* name);
void EEX_writeFolderName(char folderNum, char* name);

void EEX_readPattern(char folderNum, char pattNum, struct Pattern* targetPatt);
void EEX_writePattern(char folderNum, char pattNum, struct Pattern* sourcePatt);

void EEX_readSetupGeneral();
void EEX_writeSetupGeneral();

void EEX_readSetup(struct Pattern *pattern);
void EEX_writeSetup(struct Pattern *pattern);

void EEX_writeSetupParameter(char item, char index, char value);

void EEX_readSong(char songNum, struct SongElement* songElements);
void EEX_writeSong(char songNum, struct SongElement* songElements);

void EEX_readPerformance(char perfNum, char* patternKeyMap);
void EEX_writePerformance(char perfNum, char* patternKeyMap);

void EEX_readDisplayCharacter(char keycode, char* charData);

void EEX_exportMemoryAsMidi(char chip);
void EEX_importMemoryFromMidiStart();
void EEX_importMemoryFromMidi(char *dataFromMidi);
#endif