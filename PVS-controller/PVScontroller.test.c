#include "test/munit.h"
#include "test/asserts.h"
#include "PVScontroller.h"

void testThatNoteOnStartSetsTimer(){
    // given
    cycleCounter = 123;
    
    // when
    // row 6, column 0 should change note 0 + 6 * 8  =
    calcNoteOnOff(0b01000000, 0);
    
    // note on should store the current cycleCounter to calculate velocity later
    assertEquals(123, noteTimers[48], "wrong timer value");
}

void testThatNoteOnDoesNotTriggerTwice(){
    // given
    cycleCounter = 123;
    calcNoteOnOff(0b01000000, 0); // should store initial value in noteTimers
    cycleCounter = 128; // pretend time has moved forward

    // when note is still pressed (same state)
    calcNoteOnOff(0b01000000, 0);

    // noteTimers should contain the first value
    assertEquals(123, noteTimers[48], "wrong timer value");
}

void testThatNoteOffUpdatesReadyToSendOffBit(){
    // given
    calcNoteOnOff(0b01000000, 0); // turns note on
    readyToSendOff[0] = 0b00000001; // one note off is ready to send, must not be changed by new note off.
    
    // when note is turned off again
    calcNoteOnOff(0b00000000, 0);

    // both the new note off and the old one should be present.
    assertEquals(0b01000001, readyToSendOff[0], "wrong note off bits set");
    
    // when note off is cleared it should not be set again unless the state changes
    readyToSendOff[0] = 0b00000000; // all note offs cleared
    calcNoteOnOff(0b00000000, 0); // same state

    // no new note off should be set
    assertEquals(0b00000000, readyToSendOff[0], "Note off should not have been set");
}

void testThatCorrectTimerBytesAreSet(){
    cycleCounter=2;
    calcNoteOnOff(0b10000000, 0); // note 7 * 8 + 0 = 56
    assertEquals(2, noteTimers[56], "Note 56 not set");
    
    calcNoteOnOff(0b01000000, 1); // note 6 * 8 + 1 = 49
    assertEquals(2, noteTimers[49], "Note 49 not set");
    
    calcNoteOnOff(0b00100000, 2); // note 5 * 8 + 2 = 42
    assertEquals(2, noteTimers[42], "Note 42 not set");
    
    calcNoteOnOff(0b00010000, 3); // note 4 * 8 + 3 = 35
    assertEquals(2, noteTimers[35], "Note 35 not set");
    
    calcNoteOnOff(0b00001000, 4); // note 3 * 8 + 4 = 28
    assertEquals(2, noteTimers[28], "Note 28 not set");
    
    calcNoteOnOff(0b10000100, 5); // note 2 * 8 + 5 = 21
    assertEquals(2, noteTimers[21], "Note 21 not set");
    
    calcNoteOnOff(0b10000010, 6); // note 1 * 8 + 6 = 14
    assertEquals(2, noteTimers[14], "Note 14 not set");
    
    calcNoteOnOff(0b10000001, 7); // note 0 * 8 + 7 = 7
    assertEquals(2, noteTimers[7], "Note 7 not set");
    //TO DO: Sjekk note 61, 62 og 63, og sjekk notes off i separat test
}

void runTests(){
    reset();
    add(&testThatNoteOnStartSetsTimer);
    add(&testThatNoteOnDoesNotTriggerTwice);
    add(&testThatNoteOffUpdatesReadyToSendOffBit);
    add(&testThatCorrectTimerBytesAreSet);
    run(&init);
}