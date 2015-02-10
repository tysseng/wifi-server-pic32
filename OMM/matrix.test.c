#include "munit.h"
#include "asserts.h"
#include "../matrix.private.h"
#include "../matrix.h"

void testSum(){
    assertEquals(1,2,"sum is not correct");
}

// setup and run test suite
void runMatrixTests(){
    reset();
    add(&testSum);
    run();
}
