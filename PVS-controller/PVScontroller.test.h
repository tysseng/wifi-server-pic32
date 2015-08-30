#ifndef _PVSCONTROLLER_TEST_H
#define _PVSCONTROLLER_TEST_H

void runTests();
void initTests();

extern unsigned short mockedOutputBus;
extern unsigned short mockedDataBusDisabledPin;
extern unsigned short mockedOutputReadyPin;
extern unsigned short mockedColScanTimerInterrupt;
extern unsigned short mockedMcuReceivingDataInterrupt;
extern unsigned short lastNoteSent;
extern unsigned short lastVelocitySent;
extern unsigned short bytesSent;

#endif