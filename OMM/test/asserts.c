#include "munit.h"

void assertEquals(int expected, int actual, char* message){
    if(expected != actual){
        msg(message);
        error();
    }
}

void fail(char* message){
    msg(message);
    error();
}