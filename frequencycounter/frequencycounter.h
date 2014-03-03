void interrupt(void);
void main(void);
void hardwareInit(void);          // setup hardware start conditions
void frequencyCalculatorInit(unsigned short factor);
void storeInput(void);            // copies a captured value into the array of periods.
void startCapture();              // performs a capture of the selected number of pulses.
unsigned int getAverageCapture();
unsigned int calculateFrequency(unsigned int timerValue);
