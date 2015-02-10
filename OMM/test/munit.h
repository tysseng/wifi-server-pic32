#ifndef _MUNIT_H
#define _MUNIT_H

// pointer to test function
typedef void (*testFunc)();

extern short failedtests;
extern void msg(char* messsage);
extern void add(testFunc aTest);
extern void run();
extern void reset();
extern void error();

#endif
