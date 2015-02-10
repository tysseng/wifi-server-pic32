#include "munit.h"

void assertEquals(short expected, short actual, char* message){
    if(expected != actual){
        msg(message);
        error();
    }
}

void fail(char* message){
    msg(message);
    error();
}