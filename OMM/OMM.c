#define MAX_OPERATIONS 20
//function pointer to a matrix node function
typedef void (*nodeFunction)(struct matrixNode *);

//node in matrix
typedef struct matrixNode{
    // placeholder for result from Node function
    short result;
    
    // parameters passed to this node
    short params[8];
    
    // Bitwise variable that indicates usage of params:
    // 1 signifies that param is a constant
    // 0 that it is the index of the Node to get result from
    unsigned short paramIsConstant;

    // How many of the input params are used
    unsigned short paramsInUse;

    // function to run when this Node is accessed
    nodeFunction func;
} Node;

// matrix nodes
Node *nodes[MAX_OPERATIONS];
short nodesInUse = 0;

// Lcd module connections
sbit LCD_RS at LATB2_bit;
sbit LCD_EN at LATB3_bit;
sbit LCD_D4 at LATB4_bit;
sbit LCD_D5 at LATB5_bit;
sbit LCD_D6 at LATB6_bit;
sbit LCD_D7 at LATB7_bit;

sbit LCD_RS_Direction at TRISB2_bit;
sbit LCD_EN_Direction at TRISB3_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;

char txt1[] = "OMM illustrator";
char txt2[] = "EasyPIC7";




/*
void interrupt(){
    //reset timer etc
} */

// params can be pointers to the result of the previous Node in the matrix or 
// they can be constants. This function figures out which one and returns its 
// value.
unsigned short getParam(Node *aNode, unsigned short paramId){
  unsigned short type;

  type = (aNode->paramIsConstant >> paramId) & 0b00000001;
  if(type == 1){
    return aNode->params[paramId];
  } else {
    return nodes[aNode->params[paramId]]->result;
  }
}

// sum an arbitrary number of inputs.
void nodeFuncSum(Node *aNode){
  unsigned short i;
  aNode->result = 0;
  for(i=0; i<aNode->paramsInUse; i++){
    aNode->result += getParam(aNode, i);
  }
}

// accepts a single input and inverts it.
void nodeFuncInvert(Node *aNode){
  unsigned short param;
  param = getParam(aNode, 0);
  
  //hack to allow whole range to be inverted. chops off top as 128 is not
  //possible with an 8 bit signed variable
  if(param == -128){
    aNode->result = 127;
  } else {
    aNode->result = -param;
  }
}

// accepts a single input and iverts it around the "center" (64 or -63) of the 
// side of 0 that the input is: 1 becomes 127 but -1 becomes -126.
// usefull for inverting envelopes without turning them negative.
void nodeFuncInvertEachSide(Node *aNode){
  unsigned short param;
  param = getParam(aNode, 0);

  if(param > 0){
    aNode->result = 127-param;
  } else {
    aNode->result = -128-param;
  }
}

// add Node to the matrix.
void addNode(Node *aNode){
    nodes[nodesInUse] = aNode;
    nodesInUse++;
}

// loop over the matrix array once and calculate all results
void runMatrix(){
  unsigned short i;
  Node *aNode;
  for(i = 0; i<nodesInUse; i++){
    aNode = nodes[i];
    aNode->func(aNode);
  }
}

//TODO: Does not work with -128 for some reason
void printSignedShort(unsigned short row, unsigned short col, short in){
  short rest;
  if(in < 0){
    Lcd_Chr(row, col, '-');
    in = -in;
  } else {
    Lcd_Chr(row, col, ' ');
  }

  rest = in % 10;
  Lcd_Chr(row, col+3, 48 + rest);
  
  rest = (in / 10) % 10;
  Lcd_Chr(row, col+2, 48 + rest);
  
  rest = (in / 100) % 10;
  Lcd_Chr(row, col+1, 48 + rest);
}

void main() {
    Node aNode, aNode2, aNode3;

    // DEBUG STUFF
    Lcd_Init();                        // Initialize Lcd
    Lcd_Cmd(_LCD_CLEAR);               // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
    Lcd_Out(1,1,txt1);                 // Write text in first row

    aNode.func = &nodeFuncSum;
    aNode.params[0] = 120;
    aNode.params[1] = 2;
    aNode.params[2] = 1;
    aNode.paramsInUse = 3;
    aNode.paramIsConstant = 0b00000111;
    addNode(&aNode);

    aNode2.func = &nodeFuncSum;
    aNode2.params[0] = 1;
    aNode2.params[1] = 1;
    aNode2.paramsInUse = 2;
    aNode2.paramIsConstant = 0b00000011;
    addNode(&aNode2);

    aNode3.func = &nodeFuncInvert;
    aNode3.params[0] = 0;
    aNode3.paramsInUse = 1;
    aNode3.paramIsConstant = 0b00000000;
    addNode(&aNode3);

    runMatrix();

    // DEBUG STUFF
    printSignedShort(2,7,aNode.result);
    Lcd_Chr(2,3,48+aNode2.result);
    Lcd_Chr(2,5,48+aNode3.result);
    
    printSignedShort(2,7,-128);
}