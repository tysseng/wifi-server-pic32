#ifndef _TYPES_H
#define _TYPES_H

// datatype to use for matrix, makes it easier to switch between 8 and 16 bit
// operations later.
#define matrixint short
#define matrixlongint int

//function pointer to a matrix node function
typedef void (*nodeFunction)(struct matrixNode *);

//node in matrix
typedef struct matrixNode{
    // function to run when this Node is accessed
    nodeFunction func;

    // parameters passed to this node
    matrixint params[8];

    // Bitwise variable that indicates usage of params:
    // 1 signifies that param is a constant
    // 0 that it is the index of the Node to get result from
    unsigned short paramIsConstant;

    // How many of the input params are used
    unsigned short paramsInUse;

    // high res status variable for ramps etc, to reduce effects of rounding
    // errors.
    int highResState;

    // state, holds flags for
    short state;

    // placeholder for result from Node function
    matrixint result;

} Node;

#endif