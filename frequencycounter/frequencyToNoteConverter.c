//8 octaves times 12 semi tones
//format: two decimals written as integers.
unsigned long pitches[108];

void initPitchArray(){
  short octave;
  short semitone;
  short divideBy = 2;
  
  pitches[96]  = 418601;
  pitches[97]  = 443492;
  pitches[98]  = 469864;
  pitches[99]  = 497803;
  pitches[100]  = 527404;
  pitches[101]  = 558765;
  pitches[102]  = 591991;
  pitches[103]  = 627193;
  pitches[104]  = 664488;
  pitches[105]  = 704000;
  pitches[106]  = 745862;
  pitches[107]  = 790213;
  
  for(octave=7; octave>-1; octave--){
    for(semitone=0; semitone<12; semitone++){
      pitches[octave * 12 + semitone] = pitches[(octave+1) * 12 + semitone] / divideBy;
    }
    divideBy << 2;
  }
}

unsigned int printNote(unsigned int frequency, int factor){

  long frequency100 = factor == 10 ? frequency * 10 : frequency;

  unsigned short semitone;
  unsigned short lowerBound;
  unsigned short upperBound;
  unsigned short closestMatch;
  unsigned short octave;
  unsigned short semitoneInOctave;
  int mismatch;

  //implement as binary search later
  lowerBound = 0;
  upperBound = 0;

  // NB: Does not handle edge cases
  for(semitone = 0; semitone < 108; semitone++){
    if(frequency100 > pitches[semitone]{
      lowerBound = upperBound;
      upperBound = semitone;
    } else {
      break;
    }
  }
  
  //find closest match
  if(frequency100 - pitches[lowerBound] > pitches[higherBound] - frequency100){
    closestMatch = upperBound;
  } else {
    closestMatch = lowerBound;
  }

  //find percentage mismatch
  mismatch = 10000L * (frequency100 - pitches[closestMatch]) / pitches[closestMatch];
  
  octave = getOctave(semitone);
  semitoneInOctave = getSemitoneInOctave(semitone);

  printTone(octave, semitone, mismatch);
}

unsigned short getOctave(unsigned short semitone){
  return semitone / 12;
}

unsigned short getSemitoneInOctave(unsigned short semitone){
  return semitone % 12;
}

void lookupAndPrintTone(unsigned short octave, unsigned short semitone, int mismatch){
  switch (semitone){
    case 0:
      printTone("C  ", octave);
      break;
    case 1:
      printTone("C# ", octave);
      break;
    case 2:
      printTone("D  ", octave);
      break;
    case 3:
      printTone("D# ", octave);
      break;
    case 4:
      printTone("E  ", octave);
      break;
    case 5:
      printTone("F  ", octave);
      break;
    case 6:
      printTone("F# ", octave);
      break;
    case 7:
      printTone("G  ", octave);
      break;
    case 8:
      printTone("G#  ", octave);
      break;
    case 9:
      printTone("A  ", octave);
      break;
    case 10:
      printTone("A#  ", octave);
      break;
    case 11:
      printTone("B  ", octave);
      break;
  }
  printMismatch(mismatch);
}

void printTone(char* note, unsigned short octave){
    //"1234567890123456"
    //"note: 0-C# +++++"
    Lcd_Chr(1, 6, 48 + octave);
    Lcd_Out(1,8,octave);
}

void printMismatch(int mismatch){
     char symbol;
     unsigned short numberOfSymbols;

     if(mismatch > 0){
       symbol = '+';
       numberOfSymbols = getNumberOfSymbols(mismatch);
     } else {
       symbol = '-';
       numberOfSymbols = getNumberOfSymbols(-1 * mismatch);
     }
}

unsigned short getNumberOfSymbols(int mismatch){
  //max mismatch is roughly 300, which equals 3%.
  //TODO: Finn grenser for hva som er ustemt.
}