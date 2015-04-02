void MCU_IRQ_handleMidiStatusByte(char midiData);
void MCU_IRQ_handleMidiDataByte(char midiData);
void MCU_IRQ_handleSysexData(char sysexData);

void MCU_handleExternalSynchControl();
void MCU_continuePlayback();
void MCU_pausePlayback();
void MCU_prepareForStart();

void MCU_IRQ_playNotesAndStepNext(char counterIncrease);

void MCU_startPulseTimer();
void MCU_stopPulseTimer();
void MCU_resetPulseTimer();
void MCU_startTapTempoTimer();
void MCU_startKeyRepeatTimer();
void MCU_restartKeyRepeatTimer();
void MCU_stopKeyRepeatTimer();