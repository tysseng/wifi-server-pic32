#include "sequencer_setup.h"
#include "char_addrs.h"
#include "../controller/shared/global.h"
#include "../controller/shared/definitions.h"

void SEQ_SETUP_setDefaultSetup(struct Pattern *pattToSetup){

  unsigned short i;

  pattToSetup->res = 4;
  pattToSetup->bpm = 120;
  pattToSetup->bpb = 4;
  pattToSetup->trackcount=11;
  pattToSetup->steps=32;
  pattToSetup->swingBalance = 50;

  //init tracks and states
  for( i=0; i<MAX_TRACKS; i++) {
    pattToSetup->tracks[i].channel=0x09; //channel 10
    pattToSetup->tracks[i].velocity=64;
    pattToSetup->tracks[i].accent=32;
    pattToSetup->tracks[i].name=i+3;
  }

  //set last track name separately, as this always should be 'track 12'
  pattToSetup->tracks[MAX_TRACKS-1].name = TIDtrack12;

  //default track base notes
  pattToSetup->tracks[0].basenote  = 0x23; //bass
  pattToSetup->tracks[1].basenote  = 0x26; //snare
  pattToSetup->tracks[2].basenote  = 0x29; //lo
  pattToSetup->tracks[3].basenote  = 0x2D; //mid
  pattToSetup->tracks[4].basenote  = 0x30; //hi
  pattToSetup->tracks[5].basenote  = 0x25; //rim shot
  pattToSetup->tracks[6].basenote  = 0x27; //hand clap
  pattToSetup->tracks[7].basenote  = 0x2A; //closed hihat
  pattToSetup->tracks[8].basenote  = 0x2E; //open hihat
  pattToSetup->tracks[9].basenote  = 0x31; //crash cymbal
  pattToSetup->tracks[10].basenote = 0x33; //ride cymbal
  pattToSetup->tracks[11].basenote = 0x00; //not set
  
  G_MIDI_sysexIdByte1 = 0x22;
  G_MIDI_sysexIdByte2 = 0x23;
  G_MIDI_isExternalSynchOn = 0;
}