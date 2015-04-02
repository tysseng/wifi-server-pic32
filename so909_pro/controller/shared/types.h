#ifndef _TYPES_H
#define _TYPES_H

#include "definitions.h"

struct MenuItem{
  unsigned short col;        // x-position on display
  unsigned short row;        // y-position on display

  // pointer to action function that is executed when a key is pressed
  void (*keyListener)(char);
};

struct InputField{
  unsigned short labelLength; // length in characters for label text, including : and space after text
  unsigned short length;     // length in characters when displayed
  int maxValue;   // maximum value
  int currValue;  // current entered value
  unsigned short col;        // x-position on display
  unsigned short row;        // y-position on display

  // pointer to action function that is executed on every change.
  // function signature is: functionName(short value, short enterpressed);
  void (*action)(struct InputField*, short);
};

struct Track{
  unsigned short name;            // name index (see char_addrs.h)
  unsigned short basenote;        // note output for each step.
  unsigned short velocity;        // track default velocity
  unsigned short accent;          // track default accent
  unsigned short channel;         // midi output channel. (0-15)
  unsigned short steps[MAX_STEPS]; // all steps in step patt.
  short flams[MAX_STEPS]; // All flams (sub note divisions)
};

struct Pattern{
  unsigned short folderNum;    // if pattern is saved, this is the folder it's in (1-indexed)
  unsigned short fileNum;      // if pattern is saved, this is the file it's in (1-indexed)
  unsigned short res;          // sub-beats pr. beat.
  unsigned short bpm;          // beats per minute
  unsigned short bpb;          // beats per bar
  unsigned short trackcount;   // number of used tracks
  unsigned short steps;        // steps in current pattern
  unsigned short swingBalance; // 0 to 100, giving the relationship between the first and second 16th.
  struct Track *tracks;
};

struct Sequencer{
  unsigned short playState;    // play/stop/pause/rec
  unsigned short playMode;     // pattern/performance/song
  unsigned short currTrack;    // track assigned to sequencer buttons etc
  unsigned short currStep;
  unsigned short currSub;      // subdivided beat
  unsigned short currBeat;     // current beat at the current resolution
  unsigned int currBar;        // current bar, from 1 to 999
  unsigned short trackStates[MAX_TRACKS]; // solo/mute for each track
};

struct Flam{
  unsigned short vel;    // what pulses of the flam should be played
  unsigned short acc;    // what pulses of the flam should be accented
  unsigned short symbol; // symbol that visualizes this flam
};

// double linked list of patterns to play.
// up to 128 patterns are allowed.
struct SongElement{
  char pattern; //4 bit folder + 4 bit file
  char repeat;
};

//highlite CB_area/buffer for copy/cut/swap
struct Area{
  unsigned short x1,x2,y1,y2;         //start/end coordinates
};
#endif