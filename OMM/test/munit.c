#include "munit.h"

short failedtests;
testFunc tests[255];
short currTest = 0;

void msg(char* messsage){
    //do nothing
}

void add(testFunc aTest){
    tests[currTest++] = aTest;
}

void run(){
    short i;
    for(i=0; i<currTest; i++){
        tests[i]();
        if(failedtests){
            break;
        }
    }
}

void reset(){
    currTest = 0;
}

void error(){
    failedtests = 1;
}
