#include "test/munit.h"
#include "test/asserts.h"
#include "PVScontroller.h"
#include "PVScontroller.test.h"

void testThatKeyStartSetsTimer(){
    // when
    // row 6, column 0 should change note 0 + 6 * 8  = 48
    checkKeyStartSwitches(0b01000000, 0, 123);
    
    // note on should store the current cycleCounter to calculate velocity later
    assertEquals(123, noteTimers[48], "wrong timer value");
}

void testThatKeyStartSetsHasSentOnToFalse(){
    checkKeyStartSwitches(0b01000000, 0, 123);
    assertEquals(0b10111111, hasSentOn[0], "has sent on not set correctly");
}

void testThatHasSentOnsAreMergedOnKeyStart(){
    checkKeyStartSwitches(0b01000000, 0, 123);
    checkKeyStartSwitches(0b00000010, 0, 123);
    assertEquals(0b10111101, hasSentOn[0], "has sent on not merged correctly");
}

void testThatKeyStartDoesNotSetTimerTwice(){
    // given
    checkKeyStartSwitches(0b01000000, 0, 123); // should store initial value in noteTimers

    // when note is still pressed (same state, pretend time has moved forward)
    checkKeyStartSwitches(0b01000000, 0, 128);

    // noteTimers should contain the first value
    assertEquals(123, noteTimers[48], "wrong timer value");
}

void testThatCorrectTimerBytesAreSet(){
    checkKeyStartSwitches(0b10000000, 0, 2); // note 7 * 8 + 0 = 56
    assertEquals(2, noteTimers[56], "Note 56 not set");
    
    checkKeyStartSwitches(0b01000000, 1, 2); // note 6 * 8 + 1 = 49
    assertEquals(2, noteTimers[49], "Note 49 not set");
    
    checkKeyStartSwitches(0b00100000, 2, 2); // note 5 * 8 + 2 = 42
    assertEquals(2, noteTimers[42], "Note 42 not set");
    
    checkKeyStartSwitches(0b00010000, 3, 2); // note 4 * 8 + 3 = 35
    assertEquals(2, noteTimers[35], "Note 35 not set");
    
    checkKeyStartSwitches(0b00001000, 4, 2); // note 3 * 8 + 4 = 28
    assertEquals(2, noteTimers[28], "Note 28 not set");
    
    checkKeyStartSwitches(0b10000100, 5, 2); // note 2 * 8 + 5 = 21
    assertEquals(2, noteTimers[21], "Note 21 not set");
    
    checkKeyStartSwitches(0b10000010, 6, 2); // note 1 * 8 + 6 = 14
    assertEquals(2, noteTimers[14], "Note 14 not set");
    
    checkKeyStartSwitches(0b10000001, 7, 2); // note 0 * 8 + 7 = 7
    assertEquals(2, noteTimers[7], "Note 7 not set");
}

void testThatNoteOffIsSentOnKeyStartOff(){
    // simulate that key on has been sent
    hasSentOn[0] = 0b00000001;
    
    // simulate that key is still pressed
    noteStartSwitchStates[0] = 0b00000001;
    
    // release key
    checkKeyStartSwitches(0b00000000, 0, 0);
    
    // check that note off has been sent.
    assertEquals(bytesSent, 1, "Wrong number of notes sent");
}

void testThatNoteOffIsNotSentIfNoteOnHadNotBeenSent(){
    // simulate that key on has NOT been sent
    hasSentOn[0] = 0b00000000;

    // simulate that key is still pressed
    noteStartSwitchStates[0] = 0b00000001;

    // release key
    checkKeyStartSwitches(0b00000000, 0, 0);

    // make sure no note off has been sent
    assertEquals(bytesSent, 0, "Wrong number of notes sent");
}

void testThatNotesAbove60AreIgnoredByKeyStart(){
    hasSentOn[5] = 0xFF;
    hasSentOn[6] = 0xFF;
    hasSentOn[7] = 0xFF;

    checkKeyStartSwitches(0b11111111, 5, 0); //simulate all keys pressed
    assertEquals(0b10000000, hasSentOn[5], "Note 61 should not read");

    checkKeyStartSwitches(0b11111111, 6, 0); //simulate all keys pressed
    assertEquals(0b10000000, hasSentOn[6], "Note 62 should not read");

    checkKeyStartSwitches(0b11111111, 7, 0); //simulate all keys pressed
    assertEquals(0b10000000, hasSentOn[7], "Note 63 should not read");
}

void testVelocityCalculationNormal(){
    unsigned short storedCycleCounter;

    // simulate key strike at cycle 0.
    noteTimers[0] = 0;
    hasSentOn[0] = 0b11111110;
    
    // simulate that key hits bottom after 2 ticks
    storedCycleCounter = 2;
    checkKeyEndSwitches(0b00000001, 0, storedCycleCounter);

    assertEquals(2, noteVelocity[0], "Normal velocity is wrong");
}

void testVelocityCalculationInverse(){
    unsigned short storedCycleCounter;

    // simulate key strike start at cycle 4.
    noteTimers[0] = 4;
    hasSentOn[0] = 0b11111110;
    
    // simulate that key hits bottom after 254 ticks
    // (counter has overflowed and restarted).
    storedCycleCounter = 2;
    checkKeyEndSwitches(0b00000001, 0, storedCycleCounter);

    assertEquals(254, noteVelocity[0], "Inverse velocity is wrong");
}

void testThatKeyEndSetsHasSentOnToTrue(){
    hasSentOn[2] = 0;
    checkKeyEndSwitches(0b00000001, 2, 0); // simulate key strike end
    
    assertEquals(0b00000001, hasSentOn[2], "has sent on set");
}

void testThatHasSentOnsAreMergedOnKeyEnd(){
    hasSentOn[2] = 0;
    checkKeyEndSwitches(0b00000001, 2, 0); // simulate key strike end 1
    checkKeyEndSwitches(0b10000000, 2, 0); // simulate key strike end 2

    assertEquals(0b10000001, hasSentOn[2], "ready to sends shoud have been merged");
}

void testThatNoteOnIsSentOnKeyEndOn(){
}

void testThatNoteOnIsOnlySentOnceOnKeyEndOn(){
    // Key has reached bottom
    hasSentOn[0] = 0b11111110;
    checkKeyEndSwitches(0b0000001, 0, 0);

    // Key is released slightly
    checkKeyEndSwitches(0b0000000, 0, 0);

    // Key is fully pressed again witout reaching top first
    checkKeyEndSwitches(0b0000001, 0, 0);
    
    assertEquals(2, bytesSent, "Wrong number of notes sent");
}


void testThatNotesAbove60AreIgnoredByKeyEnd(){
    hasSentOn[5] = 0;
    hasSentOn[6] = 0;
    hasSentOn[7] = 0;
    
    checkKeyEndSwitches(0b11111111, 5, 0);
    assertEquals(0b01111111, hasSentOn[5], "Note 61 should not be read");

    checkKeyEndSwitches(0b11111111, 6, 0);
    assertEquals(0b01111111, hasSentOn[6], "Note 62 should not be read");

    checkKeyEndSwitches(0b11111111, 7, 0);
    assertEquals(0b01111111, hasSentOn[7], "Note 63 should not be read");
}

void testThatKeyEndTimesOutAndSendsNoteOn(){
    unsigned short storedCycleCounter;
    
    // simulate key strike at cycle 0.
    hasSentOn[0] = 0b11111110;
    noteTimers[0] = 0;

    // simulate that more than max velocity time has passed
    storedCycleCounter = MAX_VELOCITY_TIME;

    // no key ends reached but time is more that max
    checkKeyEndSwitches(0, 0, storedCycleCounter);

    assertEquals(bytesSent, 2, "Wrong number of notes sent");
}

void testThatNoteOnIsOnlySentOnceOnKeyEndTimeOut(){
    unsigned short storedCycleCounter;

    // simulate key strike at cycle 0.
    hasSentOn[0] = 0b11111110;
    noteTimers[0] = 0;

    // simulate that more than max velocity time has passed
    storedCycleCounter = MAX_VELOCITY_TIME;

    // no key ends reached but time is more that max
    checkKeyEndSwitches(0, 0, storedCycleCounter);
    checkKeyEndSwitches(0, 0, storedCycleCounter);

    assertEquals(bytesSent, 2, "Wrong number of notes sent");
}


void testThatKeyEndTimeoutSetsHasSentOnsToTrue(){
    unsigned short storedCycleCounter;
    
    // key start detected for row 0 col 0
    hasSentOn[0] = 0b11111100;
    
    // simulate key strike at cycle 0 - timers are 0 by default

    // simulate that more than max velocity time has passed
    storedCycleCounter = MAX_VELOCITY_TIME;

    // no key ends reached but time is more that max
    checkKeyEndSwitches(0, 0, storedCycleCounter);
    
    assertEquals(0b11111111, hasSentOn[0], "has sent on set");
}

//TODO: Check that no on is sent if start key is not still pressed.

void testThatSendCopiesValueToOutputBus(){
    mockedOutputBus = 0;
    
    send(8, 1);
    assertEquals(8, mockedOutputBus, "wrong value on output bus");
}

void testThatSendNoteOnSendsCorrectNote(){
    sendNoteOn(23, 10);
    assertEquals(0b10111011, lastNoteSent, "wrong note on sent"); // 24 + 23 + MSB set
}

void testThatSendNoteOnSendsCorrectVelocity(){
    sendNoteOn(23, 10);
// TODO
//    assertEquals(0b10101111, lastVelocitySent, "wrong velocity sent");
}

void testThatSendNoteOnSetsOutputReadyAfterSend(){
    sendNoteOn(23, 10);
    
    //NB: OUTPUT_READY is (correctly) inverted, 1 means no data is ready
    assertEquals(1, mockedOutputReadyPin, "output ready pin not reset");
}

void testThatSendNoteOffSendsCorrectNote(){
    sendNoteOff(23);
    assertEquals(0b00111011, lastNoteSent, "wrong note off sent"); // 24 + 23 + MSB not set
}

void testThatSendNoteOffSetsOutputReadyAfterSend(){
    sendNoteOff(23);
    
    //NB: OUTPUT_READY is (correctly) inverted, 1 means no data is ready
    assertEquals(1, mockedOutputReadyPin, "output ready pin not reset");
}

void testThatScanColumnIncrementsCurrentColumn(){
    currentColumn = 0;
    scanColumn();
    assertEquals(1, currentColumn, "current column not incremented");
}

void testThatScanColumnIncrementRollsOver(){
    currentColumn = COLUMNS-1;
    scanColumn();
    assertEquals(0, currentColumn, "current column should have rolled over");
}


void runTests(){
    reset();    /*
    add(&testThatKeyStartSetsTimer);
    add(&testThatKeyStartSetsHasSentOnToFalse);
    add(&testThatHasSentOnsAreMergedOnKeyStart);
    add(&testThatKeyStartDoesNotSetTimerTwice);
    add(&testThatCorrectTimerBytesAreSet);
    add(&testThatNoteOffIsSentOnKeyStartOff);
    add(&testThatNoteOffIsNotSentIfNoteOnHadNotBeenSent);
    add(&testThatNotesAbove60AreIgnoredByKeyStart);
    add(&testVelocityCalculationNormal);
    add(&testVelocityCalculationInverse);
    add(&testThatKeyEndSetsHasSentOnToTrue);
    add(&testThatHasSentOnsAreMergedOnKeyEnd);
    add(&testThatNoteOnIsSentOnKeyEndOn);
    add(&testThatNoteOnIsOnlySentOnceOnKeyEndOn);
    add(&testThatNotesAbove60AreIgnoredByKeyEnd);
    add(&testThatKeyEndTimesOutAndSendsNoteOn);
    add(&testThatNoteOnIsOnlySentOnceOnKeyEndTimeOut);
    add(&testThatKeyEndTimeoutSetsHasSentOnsToTrue);
    add(&testThatSendCopiesValueToOutputBus);
    add(&testThatSendNoteOnSendsCorrectNote);
    add(&testThatSendNoteOnSendsCorrectVelocity); */
    add(&testThatSendNoteOnSetsOutputReadyAfterSend);
    add(&testThatSendNoteOffSendsCorrectNote);
    add(&testThatSendNoteOffSetsOutputReadyAfterSend);
    add(&testThatScanColumnIncrementsCurrentColumn);
    add(&testThatScanColumnIncrementRollsOver);
    run(&initTests);
}