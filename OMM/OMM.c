#define MAX_OPERATIONS 20
//function pointer to a matrix node function
typedef void (*nodeFunction)(struct matrixNode *);

//node in matrix
typedef struct matrixNode{
    short result;
    short params[8];
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

void addNode(Node *aNode){
    nodes[nodesInUse] = aNode;
    nodesInUse++;
}

void nodeFuncSum(Node *aNode){
    aNode->result = aNode->params[0] + aNode->params[1];
}

void runMatrix(){
  unsigned short i;
  Node *aNode;
  for(i = 0; i<nodesInUse; i++){
    aNode = nodes[i];
    aNode->func(aNode);
  }
}

void main() {
    Node aNode;

    Lcd_Init();                        // Initialize Lcd

    Lcd_Cmd(_LCD_CLEAR);               // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
    Lcd_Out(1,1,txt1);                 // Write text in first row

    aNode.func = &nodeFuncSum;
    aNode.params[0] = 4;
    aNode.params[1] = 8;
    addNode(&aNode);

    runMatrix();

    Lcd_Chr(2,1,48+aNode.result);
}