//8 octaves times 12 semi tones
//format: two decimals written as integers.
unsigned long pitches[9][12];

void initPitchArray(){
  short octave;
  short semitone;
  short divideBy = 2;
  
  pitches[8][0]  = 418601;
  pitches[8][0]  = 443492;
  pitches[8][0]  = 469864;
  pitches[8][0]  = 497803;
  pitches[8][0]  = 527404;
  pitches[8][0]  = 558765;
  pitches[8][0]  = 591991;
  pitches[8][0]  = 627193;
  pitches[8][0]  = 664488;
  pitches[8][0]  = 704000;
  pitches[8][0]  = 745862;
  pitches[8][0]  = 790213;
  
  for(octave=7; octave>-1; octave--){
    for(semitone=0; semitone<12; semitone++){
      pitches[octave][semitone] = pitches[octave+1][semitone] / divideBy;
    }
    divideBy << 2;
  }
}

unsigned int printNote(unsigned int frequency, int factor){



}