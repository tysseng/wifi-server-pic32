#include "../shared/types.h"

extern struct InputField *currentInputField;

void IN_clearCurrentInputField();
void IN_setCurrentInputField(struct InputField *field);
void IN_refreshInputField(struct InputField *field);
void IN_addNumberToPatternInputField(unsigned short number);
void IN_addNumberToInputField(unsigned short number);
void IN_enterInputField();