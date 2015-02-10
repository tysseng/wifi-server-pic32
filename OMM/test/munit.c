#include "munit.h"

unsigned short failedtests;
testFunc tests[255];
unsigned short currTest = 0;

void msg(char* messsage){
    //do nothing
}

void add(testFunc aTest){
    tests[currTest++] = aTest;
}

void run(callback runBetweenTests){
    unsigned short i;
    for(i=0; i<currTest; i++){
        tests[i]();
        if(failedtests){
//            break;
        }
        runBetweenTests();
    }
}

void reset(){
    currTest = 0;
    failedtests = 0;
}

void error(){
    failedtests = 1;
}
