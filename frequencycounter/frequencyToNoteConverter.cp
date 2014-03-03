#line 1 "C:/git/mikroc/frequencycounter/frequencyToNoteConverter.c"
#line 1 "c:/git/mikroc/frequencycounter/frequencytonoteconverter.h"
void initPitchArray();
void printNote(unsigned int frequency, int factor);
unsigned short getOctave(unsigned short semitone);
unsigned short getSemitoneInOctave(unsigned short semitone);
void lookupAndPrintTone(unsigned short octave, unsigned short semitone, int mismatch);
void printTone(char* note, unsigned short octave);
void printMismatch(int mismatch);
unsigned short getNumberOfSymbols(int mismatch);
void Lcd_Out_ULong(unsigned short row, unsigned short col, unsigned long number);
void Lcd_Out_UInt(unsigned short row, unsigned short col, unsigned int number);
void Lcd_Out_UShort(unsigned short row, unsigned short col, unsigned short number);
#line 1 "c:/git/mikroc/frequencycounter/lcdroutines.h"
void Lcd_Out_Freq(unsigned short row, unsigned short col, unsigned long number);
void Lcd_Out_ULong(unsigned short row, unsigned short col, unsigned long number);
void Lcd_Out_UInt(unsigned short row, unsigned short col, unsigned int number);
void Lcd_Out_UShort(unsigned short row, unsigned short col, unsigned short number);
void Lcd_Out_Timer(unsigned short row, unsigned short col, unsigned int number);
#line 6 "C:/git/mikroc/frequencycounter/frequencyToNoteConverter.c"
unsigned long pitches[108];

void initPitchArray(){
 short octave;
 short semitone;
 short divideBy = 2;

 pitches[96] = 418601;
 pitches[97] = 443492;
 pitches[98] = 469864;
 pitches[99] = 497803;
 pitches[100] = 527404;
 pitches[101] = 558765;
 pitches[102] = 591991;
 pitches[103] = 627193;
 pitches[104] = 664488;
 pitches[105] = 704000;
 pitches[106] = 745862;
 pitches[107] = 790213;

 for(octave=7; octave>-1; octave--){
 for(semitone=0; semitone<12; semitone++){
 pitches[octave * 12 + semitone] = pitches[(octave+1) * 12 + semitone] / divideBy;
 }
 divideBy << 2;
 }
}

void printNote(unsigned int frequency, int factor){

 unsigned long frequency100 = (factor == 10 ? 10L * frequency : 1L * frequency);

 unsigned long tmp;
 unsigned long tmp2;

 unsigned short semitone;
 unsigned short lowerBound = 0;
 unsigned short upperBound = 0;
 unsigned short closestMatch;
 unsigned short octave;
 unsigned short semitoneInOctave;
 int mismatch;



 for(semitone = 0; semitone < 108; semitone++){
 if(frequency100 > pitches[semitone]){
 lowerBound = semitone;
 upperBound = semitone+1;
 } else {
 break;
 }
 }

 if(frequency100 - pitches[lowerBound] > pitches[upperBound] - frequency100){
 closestMatch = upperBound;
 } else {
 closestMatch = lowerBound;
 }

 Delay_5ms();
 Lcd_Out_Freq(2, 1, frequency100);

 Delay_5ms();
 Lcd_Out_Freq(2, 9, pitches[closestMatch]);



 mismatch = 10000L * (frequency100 - pitches[closestMatch]) / pitches[closestMatch];

 octave = getOctave(closestMatch);
 semitoneInOctave = getSemitoneInOctave(closestMatch);
 lookupAndPrintTone(octave, semitoneInOctave, mismatch);
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
 printTone("-C  ", octave);
 break;
 case 1:
 printTone("-C# ", octave);
 break;
 case 2:
 printTone("-D  ", octave);
 break;
 case 3:
 printTone("-D# ", octave);
 break;
 case 4:
 printTone("-E  ", octave);
 break;
 case 5:
 printTone("-F  ", octave);
 break;
 case 6:
 printTone("-F# ", octave);
 break;
 case 7:
 printTone("-G  ", octave);
 break;
 case 8:
 printTone("-G# ", octave);
 break;
 case 9:
 printTone("-A  ", octave);
 break;
 case 10:
 printTone("-A# ", octave);
 break;
 case 11:
 printTone("-B ", octave);
 break;
 }

}

void printTone(char* note, unsigned short octave){


 Lcd_Out(1,8,note);
 Lcd_Chr(1, 7, 48 + octave);
}

void printMismatch(int mismatch){
 char symbol;
 unsigned short numberOfSymbols;
 unsigned short i;

 if(mismatch > 0){
 symbol = '+';
 numberOfSymbols = getNumberOfSymbols(mismatch);
 } else {
 symbol = '-';
 numberOfSymbols = getNumberOfSymbols(-1 * mismatch);
 }

 for(i = 0; i< numberOfSymbols; i++){
 Lcd_Chr(1, 12 + i, symbol);
 }
 for(i; i<3; i++){
 Lcd_Chr(1, 12 + i, ' ');
 }
}

unsigned short getNumberOfSymbols(int mismatch){

 if(mismatch >200){
 return 3;
 } else if(mismatch > 100) {
 return 2;
 } else if(mismatch > 50) {
 return 1;
 }
 return 0;

}
