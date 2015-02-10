#include "munit.h"
#include "asserts.h"
#include "matrix.private.h"
#include "matrix.h"

void testSum(){

}

// setup and run test suite
void runMatrixTests(){
    resetTests();
    addTest(&testSum);
    runTests();
}
