#include "input.h"

#include "../GLCD/sequencer_visualisation.h"

struct InputField *currentInputField;
char inputPatternPos=0;


//
// Set current input field and move arrows indicating current field.
//
// To turn off input field focus, send a 0 pointer.
//
void IN_setCurrentInputField(struct InputField *field){
  if(currentInputField){
    SEQ_VIS_clearArrow(currentInputField->row, currentInputField->col - currentInputField->labelLength - 1);
  }
  if(field){
    currentInputField = field;
    SEQ_VIS_writeArrow(field->row, field->col - field->labelLength - 1);
  }
}

//
// Removes current input field focus without redrawing/clearing arrow
//
void IN_clearCurrentInputField(){
  currentInputField = NO_INPUT_FOCUS;
}

//
// Wrapper that ensures that a patter number adheres to the rules for folder/
// file combinations:
// The first entered number (hundreds/folder number) must be between 1 and 8
// inclusive, the second number (tens, file tens) must be 0 or 1, and the third
// number (file ones) must be between 1 and 9 inclusive if tens are 0, otherwise
// it must be between 0 and 2 inclusive.
//
void IN_addNumberToPatternInputField(unsigned short number){

  if(currentInputField == 0){
    return;
  }
  
  if(inputPatternPos == 0){
    currentInputField->currValue = 0;
  }
  
  switch(inputPatternPos){
  case 0:
    if(inputPatternPos == 0 && number > 0 && number < 9){
      IN_addNumberToInputField(number);
      inputPatternPos = (inputPatternPos+1) % 3;
    }
    break;
  case 1:
    if(inputPatternPos == 1 && number < 2){
      IN_addNumberToInputField(number);
      inputPatternPos = (inputPatternPos+1) % 3;
    }
    break;
  case 2:
    if(inputPatternPos == 2 && (
        ((currentInputField->currValue % 10) == 0 && number > 0 ) ||
        ((currentInputField->currValue % 10) == 1 && number < 3 ))
    ){
      IN_addNumberToInputField(number);
      inputPatternPos = (inputPatternPos+1) % 3;
    }
    break;
  }
}

//
// Add a number to an input field, shifting the current digits left. If a shift
// would make the value larger than the allowed maximum, that digit is dropped.
//
void IN_addNumberToInputField(unsigned short number){

  unsigned short ones;
  unsigned short tens;
  unsigned short hundreds;

  if(currentInputField == 0){
    return;
  }

  ones = number;
  tens = currentInputField->currValue;
  tens = tens % 10;
  hundreds = (currentInputField->currValue - tens);
  hundreds = (hundreds % 100) / 10;

  if(ones > currentInputField->maxValue){
    return;
  } else if(tens * 10 + ones > currentInputField->maxValue){
    currentInputField->currValue = ones;
  } else if(hundreds * 100 + tens * 10 + ones > currentInputField->maxValue){
    currentInputField->currValue = tens * 10 + ones;
  } else {
    currentInputField->currValue = hundreds * 100 + tens * 10 + ones;
  }

  if(currentInputField){
    currentInputField->action(currentInputField, 0);
  }
  IN_refreshInputField(currentInputField);
}

//
// Write the current value of a field to its position on the display
//
void IN_refreshInputField(struct InputField *field){
  if(field->maxValue > 1){
    SEQ_VIS_writeNum(
      field->row,
      field->col,
      field->length,
      field->currValue
    );
  } else {
    SEQ_VIS_writeOnOff(
      field->row,
      field->col,
      field->length,
      field->currValue
    );
  }
}

//
// Signal to the action that enter has been pressed on the current input field
//
void IN_enterInputField(){
  if(currentInputField){
    currentInputField->action(currentInputField, 1);
  }
}