#include "test/munit.h"
#include "test/asserts.h"
#include "PVScontroller.h"
#include "PVScontroller.test.h"

void testThatNoteOnStartSetsTimer(){
    // given
    cycleCounter = 123;
    
    // when
    // row 6, column 0 should change note 0 + 6 * 8  =
    checkKeyStartSwitches(0b01000000, 0);
    
    // note on should store the current cycleCounter to calculate velocity later
    assertEquals(123, noteTimers[48], "wrong timer value");
}

void testThatNoteOnDoesNotTriggerTwice(){
    // given
    cycleCounter = 123;
    checkKeyStartSwitches(0b01000000, 0); // should store initial value in noteTimers
    cycleCounter = 128; // pretend time has moved forward

    // when note is still pressed (same state)
    checkKeyStartSwitches(0b01000000, 0);

    // noteTimers should contain the first value
    assertEquals(123, noteTimers[48], "wrong timer value");
}

void testThatNoteOffUpdatesReadyToSendOffBit(){
    // given
    checkKeyStartSwitches(0b01000000, 0); // turns note on
    readyToSendOff[0] = 0b00000001; // one note off is ready to send, must not be changed by new note off.
    
    // when note is turned off again
    checkKeyStartSwitches(0b00000000, 0);

    // both the new note off and the old one should be present.
    assertEquals(0b01000001, readyToSendOff[0], "wrong note off bits set");
    
    // when note off is cleared it should not be set again unless the state changes
    readyToSendOff[0] = 0b00000000; // all note offs cleared
    checkKeyStartSwitches(0b00000000, 0); // same state

    // no new note off should be set
    assertEquals(0b00000000, readyToSendOff[0], "Note off should not have been set");
}

void testThatCorrectTimerBytesAreSet(){
    cycleCounter=2;
    checkKeyStartSwitches(0b10000000, 0); // note 7 * 8 + 0 = 56
    assertEquals(2, noteTimers[56], "Note 56 not set");
    
    checkKeyStartSwitches(0b01000000, 1); // note 6 * 8 + 1 = 49
    assertEquals(2, noteTimers[49], "Note 49 not set");
    
    checkKeyStartSwitches(0b00100000, 2); // note 5 * 8 + 2 = 42
    assertEquals(2, noteTimers[42], "Note 42 not set");
    
    checkKeyStartSwitches(0b00010000, 3); // note 4 * 8 + 3 = 35
    assertEquals(2, noteTimers[35], "Note 35 not set");
    
    checkKeyStartSwitches(0b00001000, 4); // note 3 * 8 + 4 = 28
    assertEquals(2, noteTimers[28], "Note 28 not set");
    
    checkKeyStartSwitches(0b10000100, 5); // note 2 * 8 + 5 = 21
    assertEquals(2, noteTimers[21], "Note 21 not set");
    
    checkKeyStartSwitches(0b10000010, 6); // note 1 * 8 + 6 = 14
    assertEquals(2, noteTimers[14], "Note 14 not set");
    
    checkKeyStartSwitches(0b10000001, 7); // note 0 * 8 + 7 = 7
    assertEquals(2, noteTimers[7], "Note 7 not set");

}

void testThatNotesAbove60AreIgnoredInCalcNoteOnOff(){
    noteStartSwitchStates[5] = 0b11111111; // simulate that all keys are pressed
    checkKeyStartSwitches(0b00000000, 5); //simulate all keys released
    assertEquals(0b01111111, readyToSendOff[5], "Note 61 should not be sent");

    noteStartSwitchStates[6] = 0b11111111; // simulate that all keys are pressed
    checkKeyStartSwitches(0b00000000, 6); //simulate all keys released
    assertEquals(0b01111111, readyToSendOff[6], "Note 62 should not be sent");

    noteStartSwitchStates[7] = 0b11111111; // simulate that all keys are pressed
    checkKeyStartSwitches(0b00000000, 7); //simulate all keys released
    assertEquals(0b01111111, readyToSendOff[7], "Note 63 should not be sent");
}

void testVelocityCalculationNormal(){
    noteTimers[0] = 0; // simulate key strike at cycle 0.

    cycleCounter=2; // simulate that key hits bottom after 2 ticks
    checkKeyBottomSwitches(0b00000001, 0); // simulate key strike end
    
    assertEquals(2, noteVelocity[0], "Normal velocity is wrong");
}

void testVelocityCalculationInverse(){
    noteTimers[0] = 4; // simulate key strike at cycle 2.

    cycleCounter=2; // simulate that key hits bottom after 254 ticks (counter has overflowed and restarted.
    checkKeyBottomSwitches(0b00000001, 0); // simulate key strike end

    assertEquals(254, noteVelocity[0], "Inverse velocity is wrong");
}

void testThatReadyToSendIsSetOnKeyBottom(){
    checkKeyBottomSwitches(0b00000001, 2); // simulate key strike end
    
    assertEquals(0b00000001, readyToSendOn[2], "ready to send not set");
}

void testThatReadyToSendIsNotRetriggered(){
    checkKeyBottomSwitches(0b00000001, 2); // simulate key strike end

    readyToSendOn[2] = 0; // simulate that key has been sent

    checkKeyBottomSwitches(0b00000001, 2); // same state (key has not been released

    assertEquals(0b00000000, readyToSendOn[2], "ready to send should not be retriggered");
}

void testThatReadyToSendsAreMerged(){
    checkKeyBottomSwitches(0b00000001, 2); // simulate key strike end 1
    checkKeyBottomSwitches(0b10000000, 2); // simulate key strike end 2

    assertEquals(0b10000001, readyToSendOn[2], "ready to sends shoud have been merged");
}

void testThatNotesAbove60AreIgnoredInCalcVelocity(){
    checkKeyBottomSwitches(0b11111111, 5);
    assertEquals(0b01111111, readyToSendOn[5], "Note 61 should not be sent");

    checkKeyBottomSwitches(0b11111111, 6);
    assertEquals(0b01111111, readyToSendOn[6], "Note 62 should not be sent");

    checkKeyBottomSwitches(0b11111111, 7);
    assertEquals(0b01111111, readyToSendOn[7], "Note 63 should not be sent");

}

void testThatSendCopiesValueToOutputBus(){
    mockedOutputBus = 0;
    mockedReadyToSendPin.b0 = 1; // must be ready to send to pass waiting loop
    
    send(8);
    assertEquals(8, mockedOutputBus, "wrong value on output bus");
}

void testThatSendNoteOnSendsCorrectNote(){
    mockedReadyToSendPin.b0 = 1; // must be ready to send to pass waiting loop
    sendNoteOn(23, 10);
    
    assertEquals(0b10101111, lastNoteSent, "wrong note sent"); // 24 + 23 + MSB set
}

void testThatSendNoteOnSendsCorrectVelocity(){
    mockedReadyToSendPin.b0 = 1; // must be ready to send to pass waiting loop
    sendNoteOn(23, 10);
// TODO
//    assertEquals(0b10101111, lastVelocitySent, "wrong velocity sent");
}

void testThatSendNoteOnsClearsReadyToSend(){
    mockedReadyToSendPin.b0 = 1; // must be ready to send to pass waiting loop
    readyToSendOn[1] = 0b00000001; //note 8 is ready to send
   
    sendNoteOns();
    assertEquals(0, readyToSendOn[1], "ready to send should have been cleared");
}

void runTests(){
    reset();
    add(&testThatNoteOnStartSetsTimer);
    add(&testThatNoteOnDoesNotTriggerTwice);
    add(&testThatNoteOffUpdatesReadyToSendOffBit);
    add(&testThatCorrectTimerBytesAreSet);
    add(&testThatNotesAbove60AreIgnoredInCalcNoteOnOff);
    add(&testVelocityCalculationNormal);
    add(&testVelocityCalculationInverse);
    add(&testThatReadyToSendIsSetOnKeyBottom);
    add(&testThatReadyToSendIsNotRetriggered);
    add(&testThatReadyToSendsAreMerged);
    add(&testThatNotesAbove60AreIgnoredInCalcVelocity);
    add(&testThatSendCopiesValueToOutputBus);
    add(&testThatSendNoteOnSendsCorrectNote);
    add(&testThatSendNoteOnSendsCorrectVelocity);
    add(&testThatSendNoteOnsClearsReadyToSend);
    run(&init);
}