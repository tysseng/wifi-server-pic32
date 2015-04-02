//bus ports
#define ADDRESS_BUS PORTA
#define ADDRESS_BUS_DIRECTION TRISA

#define KBD_DISABLE PORTA.B4
#define KBD_DISABLE_DIRECTION TRISA.B4

#define LED_ENABLE PORTA.B5
#define LED_ENABLE_DIRECTION TRISA.B5

#define DIAL_ENABLE PORTE.B0
#define DIAL_ENABLE_DIRECTION TRISE.B0

#define DATA_BUS PORTB
#define DATA_BUS_DIRECTION TRISB

#define LCD_DATA_PORT PORTD
#define LCD_DATA_DIRECTION TRISD

#define LCD_CTRL_PORT PORTC
#define LCD_CTRL_DIRECTION TRISC

//bus control
#define SEL_DATA_IN 0     //enable tristate buffers and disable LED output
#define SEL_DATA_OUT 0x30 //disable tristate buffers and enable LED output
#define DATA_IN 0xFF      //set all lines to read
#define DATA_OUT 0        //set all lines to write