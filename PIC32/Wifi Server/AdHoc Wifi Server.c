/*
 * Project Name:
     WiFi_AdHoc_Server
 * Copyright:
     (c) Mikroelektronika, 2012.

 * description  :
 *      this code shows how to use the Net_Wireless_MCW1001 library :
 *
 * Test configuration:
     Test configuration:
     MCU:             P32MX460F512L
                      http://ww1.microchip.com/downloads/en/DeviceDoc/61143G.pdf
     Dev.Board:       LV32MX v6
                      http://www.mikroe.com/eng/products/view/462/lv-32mx-v6-development-system/
     Oscillator:      XT-PLL, 80.000MHz
     ext. modules:    ac:WIFI_Plus_Click       on UART1 (RF2, RF8)
                      ac:Potentiometer_Board   on PORTB (close J7 jumper)
                              on portd (turn on sw.3)
     SW:              mikroC PRO for PIC32
                      http://www.mikroe.com/eng/categories/view/89/pic32-compilers/
 * NOTES:
     - Pull-down PORTC.B1, PORTC.B2, PORTC.B3, PORTC.B4 at SW5 and J6, and put jumper J15 in VCC position (board specific)
     - Turn ON PORTE ar SW12.5
 */
/**
Config for EasyPIC Fusion with click in left position:
- RST at RC1
- RT:  RF4 (UART2)
- RX:  RF5 (UART2)
- CTS: SCL/RA2 (SCL2)
- RTS: SDA/RA3 (SDA2)
- GP0: RB8/AN
- GP1: RD0/PWM
- GP2: RE8/INT

**/
sbit Net_Wireless_MCW1001_Chip_RST           at RC1_bit; // RF0_bit;
sbit Net_Wireless_MCW1001_Chip_RST_Direction at TRISC1_bit; //TRISF0_bit;

////////////////////////////////////////////////////////////////////////////////
// Global variables

// Const MAX_SIZE_OF_DATA refers to Net_Wireless_MCW1001_TCP_SendBytes function. There
// is possible only to transmit messages to the MCW1001A in packets of max 500 byte size.
// Size of: AP header + Trailer = 11bytes  (for Net_Wireless_MCW1001_TCP_SendBytes function), and because of that max size of TCP
// data is 500 - 11 = 489
const unsigned int MAX_SIZE_OF_DATA = 489;
char txBuffer[MAX_SIZE_OF_DATA];
unsigned int bufferIndex = 0;

char myIpAdd[4]    = {169, 254, 0, 10};
char myMacAdd[6]   = {0x22, 0x33, 0x44, 0x55, 0x66, 0x88};
char netMask[4]    = {255, 255, 0, 0};
char gatewayAdd[4] = {169, 254, 0, 1};
char remoteIpAdd[4];
unsigned int remotePort, localPort;
//char strSSID[13] = "WiFi Control";
char *strSSID = "Joakims kompetansedagger";
char channels[11]  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
char wifiStatus, bindResponse, backLog, listenResponse;
char socketHandle, socketChild;
char response;

char interstatus = 1;



unsigned char   getRequest[15] ;                                        // HTTP request buffer
unsigned char   dyna[29] ;                                              // buffer for dynamic response
unsigned long   httpCounter = 0 ;                                       // counter of HTTP requests

/************************************************************
 * ROM constant strings
 */
const code   char httpHeader[] = "HTTP/1.1 200 OK\nContent-type: " ;  // HTTP header
const code   char httpMimeTypeHTML[] = "text/html\n\n" ;              // HTML MIME type
const code   char httpMimeTypeScript[] = "text/plain\n\n" ;           // TEXT MIME type
unsigned char httpMethod[] = "GET /";

// web page
const code char *indexPage = "<HTML><HEAD><meta name=\"viewport\" content=\"width=device-width, initial-scale=2\"></HEAD><BODY style='font-size: 20px'>Hei Belle og Maiblomsten!</body></html>";

////////////////////////////////////////////////////////////////////////////////
// Interrupt routine for UART module

void interrupt() iv IVT_UART_2 ilevel 7 ics ICS_SRS {
  Net_Wireless_MCW1001_Receive();
//  LATB = interstatus++;
  U2RXIF_bit = 0;           // ensure interrupt not pending
}

////////////////////////////////////////////////////////////////////////////////
// Interrupt routine which increment Net_Wireless_MCW1001_Time variable every second.

unsigned int wifiTmr = 0;           // timer prescaler
void Timer1Interrupt() iv IVT_TIMER_1 ilevel 7 ics ICS_SRS {
  wifiTmr++;
  if(wifiTmr >= 10) {
    Net_Wireless_MCW1001_Time++ ;
    wifiTmr = 0;
//    LATD = 0xFF;
  }
  T1IF_bit = 0;        // clear TMR0IF
}


////////////////////////////////////////////////////////////////////////////////
// Initialization of MCU modules

//Place/Copy this part in declaration section
void InitTimer1(){
  T1CON         = 0x8030;
  T1IE_bit         = 1;
  T1IF_bit         = 0;
  T1IP0_bit         = 1;
  T1IP1_bit         = 1;
  T1IP2_bit         = 1;
  PR1                 = 31250;
  TMR1                 = 0;
}



void InitMcu() {

  AD1PCFG |= 0xFF7F;         // all digital but rb7(AN7)
  TRISB.B7 = 1;              // set PORTB as input for buttons and adc

  // Initialization of timer..............
  InitTimer1();
    /*
  TMR1 = 0;                 // reset timer value to zero
  PR1 = 65535;              // Load period register

  T1IP0_bit = 1;            // set interrupt
  T1IP1_bit = 1;            // priority
  T1IP2_bit = 1;            // to 7

  TCKPS0_bit = 1;           // Set Timer Input Clock
  TCKPS1_bit = 1;           // Prescale value to 1:256

// Enabling timer 1 hangs the EnableInterrupts for some reason!
//  T1IE_bit = 1;             // Enable Timer1 Interrupt
//  ON__T1CON_bit = 1;        // Enable Timer1
*/
  TRISB = 0;                // Initialize PORTB as output
  TRISD = 0;                // Initialize PORTD as output

  // Initialization of UART interrupt..............
  U2IP0_bit = 1;            // set interrupt
  U2IP1_bit = 1;            // priority
  U2IP2_bit = 1;            // to 7

  U2STA.B7 = 0;             // 0x = Interrupt flag bit is set when a character is received
  U2RXIF_bit = 0;           // ensure interrupt not pending
  U2RXIE_bit = 1;           // enable intterupt

  LATD = 1;

  EnableInterrupts();       // Enable all interrupts

  LATD = 2;

  // Set RE7 pin as digital output, so we can monitor TCP status on that pin
  TRISE.B7 = 0;
  PORTE.B7 = 0;

  Net_Wireless_MCW1001_HwReset();


  LATD = 3;              // Set PORTD to zero
}

////////////////////////////////////////////////////////////////////////////////
// Initialization of WiFi module

void InitWiFi() {
volatile char response;

  Net_Wireless_MCW1001_TimeToWait = 1;
  response = 1;
  while(response != 0){
    response = Net_Wireless_MCW1001_SetMode(_NET_WIRELESS_MCW1001_CP_1, _NET_WIRELESS_MCW1001_MODE_ADHOC); // Set "Connection Profile 1" mode (Ad-Hoc mode)
  }

  LATD=5;

  Net_Wireless_MCW1001_TimeToWait = 10;
  response =Net_Wireless_MCW1001_SetChannelList(1, channels);                     // Choose only channerl one for communication
  response =Net_Wireless_MCW1001_SetSecurity_Open(_NET_WIRELESS_MCW1001_CP_1);    // Set open security level
  response =Net_Wireless_MCW1001_SetSSID(_NET_WIRELESS_MCW1001_CP_1, strSSID);    // Set SSID of network

  LATD=6;

  // Set network parameters
  response =Net_Wireless_MCW1001_SetNetworkMask(netMask);
  Net_Wireless_MCW1001_SetGatewayIP(gatewayAdd);
  Net_Wireless_MCW1001_SetMAC(myMacAdd);
  Net_Wireless_MCW1001_SetIP(myIpAdd);

  Net_Wireless_MCW1001_SetArpTime(1);                                   // Set gratuitous ARP timing
  Net_Wireless_MCW1001_SetRetryCount(5, 0);                             // No retries for the connection if the first attempt fails,
                                                                        // in order to establish a new network

  LATD=7;
}

////////////////////////////////////////////////////////////////////////////////
// Fill local buffer with const data. When buffer overflow, send it to WiFi module

unsigned int    PutConstString(const code char *s) {
  unsigned int numOfSentBytes;

  while(*s) {
   txBuffer[bufferIndex++] = *s++;
    if(bufferIndex == MAX_SIZE_OF_DATA) {
      Net_Wireless_MCW1001_TCP_SendBytes(socketChild, txBuffer, MAX_SIZE_OF_DATA, &numOfSentBytes);
      bufferIndex = 0;
    }
  }
  return(bufferIndex) ;
}

////////////////////////////////////////////////////////////////////////////////
// Fill local buffer with data. When buffer overflow, send it to WiFi module

unsigned int    PutString(char *s) {
  unsigned int numOfSentBytes;

  while(*s) {
    txBuffer[bufferIndex++] = *s++;
    if(bufferIndex == MAX_SIZE_OF_DATA) {
      Net_Wireless_MCW1001_TCP_SendBytes(socketChild, txBuffer, MAX_SIZE_OF_DATA, &numOfSentBytes);
      bufferIndex = 0;
    }
  }
  return(bufferIndex) ;
}

////////////////////////////////////////////////////////////////////////////////

void SendHttpResponse() {

  char response, i;
  unsigned int numOfReceiveBytes = 0, numOfSentBytes;
  unsigned int    length;                     // my reply length

  length = 0;
  Net_Wireless_MCW1001_TCP_ReadBytes(&socketChild, 10, getRequest, &numOfReceiveBytes);
  getRequest[10] = 0;

  if(memcmp(getRequest, httpMethod, 5)) {     // only GET method is supported here
    return;
  }

  httpCounter++ ;                             // one more request done
  if(getRequest[5] == 's') {                  // if request path name starts with s, store dynamic data in transmit buffer

    // the text string replied by this request can be interpreted as javascript statements
    // by browsers
    bufferIndex = 0;
    PutConstString(httpHeader) ;              // HTTP header
    PutConstString(httpMimeTypeScript) ;      // with text MIME type

/*
    // add AN4 value to reply
    WordToStr(ADC_Read(4), dyna) ;
    PutConstString("var AN4=") ;
    PutString(dyna) ;
    PutConstString(";") ;

    // add PORTD value (buttons) to reply
    PutConstString("var PORTD=") ;
    WordToStr(PORTD, dyna) ;
    PutString(dyna) ;
    PutConstString(";") ;

    // add PORTA value (LEDs) to reply
    PutConstString("var PORTA=") ;
    WordToStr(PORTA, dyna) ;
    PutString(dyna) ;
    PutConstString(";") ;

    // add HTTP requests counter to reply
    WordToStr(httpCounter, dyna) ;
    PutConstString("var REQ=") ;
    PutString(dyna) ;
    length = PutConstString(";") ;
    */
    if(length != 0)                          // If there is some data (in txBuffer) which is not sent
      Net_Wireless_MCW1001_TCP_SendBytes(socketChild, txBuffer, length, &numOfSentBytes);

  }
  else if(getRequest[5] == 't') {            // if request path name starts with t, toggle PORTA (LED) bit number that comes after

    unsigned char   bitMask = 0 ;            // for bit mask
    Sound_Play(800, 40);                     // Play sound at 800Hz for 40ms
    if(isdigit(getRequest[6])) {             // if 0 <= bit number <= 9, bits 8 & 9 does not exist but does not matter
      bitMask = getRequest[6] - '0' ;        // convert ASCII to integer
      bitMask = 1 << bitMask ;               // create bit mask
      LATA ^= bitMask ;                      // toggle PORTA with xor operator
    }

  }

  if(length == 0) {                          // what do to by default

    bufferIndex = 0;
    PutConstString(httpHeader) ;             // HTTP header
    PutConstString(httpMimeTypeHTML) ;       // with HTML MIME type
    length = PutConstString(indexPage) ;              // HTML page first part
    //length = PutConstString(indexPage2) ;    // HTML page second part
    if(length != 0)                          // If there is some data (in txBuffer) which is not sent
      Net_Wireless_MCW1001_TCP_SendBytes(socketChild, txBuffer, length, &numOfSentBytes);

  }
}

////////////////////////////////////////////////////////////////////////////////

void RunHttpServer() {

  LATD=10;
  while(1) {
    socketHandle = 0;
    backLog = 1;
    localPort = 80;
    Net_Wireless_MCW1001_SocketCreate(&socketHandle, _NET_WIRELESS_MCW1001_SOCKET_TYPE_TCP);   // Create TCP socket
    Net_Wireless_MCW1001_SocketBind(socketHandle, &localPort, &bindResponse);                  // Bind socket to the listen port
    Net_Wireless_MCW1001_TCP_Listen(socketHandle, &backLog, &listenResponse);                  // Prepare the socket to listen for connection
              
    LATD=11;                                                                                               // with one children socket.
    while(1) {
      socketChild = socketHandle;
      Net_Wireless_MCW1001_TCP_Accept(&socketChild, &remotePort, remoteIpAdd);                 // Accept incoming conncetion
      if (socketChild != 254) {                                                                // Accept function set socketChild.
        break;                                                                                 // If socketChild == 254 there is no incoming connection
      }
      LATD=12;
    }

    LATD=13;
    SendHttpResponse();
    Net_Wireless_MCW1001_SocketClose(socketChild);                                             // Close sockets
    Net_Wireless_MCW1001_SocketClose(socketHandle);
    LATD=14;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Polling TCP receiving buffer to see is there some received packet


void CheckTcpReceive() {
  char response, dat[10], i;
  unsigned int numOfReceiveBytes = 0;

  LATD=13;
  response = Net_Wireless_MCW1001_TCP_ReadBytes(&socketChild, 10, dat, &numOfReceiveBytes);
  LATD=14;
  // If there is no error, and number of received bytes is greater than zero
  if( (response == _NET_WIRELESS_MCW1001_NO_ERROR) && (numOfReceiveBytes != 0) ) {
    for(i = 0; i < numOfReceiveBytes; i++) {
      LATD=15;
//      CommandDecode(dat[i]);
    }
  }
}


void main() {

  InitMcu();

  UART2_Init(115200);
  Delay_ms(2000);
  LATD=4;

  InitWiFi();
  LATD=8;
  // Open "ad-hoc server"
  Net_Wireless_MCW1001_Connect(_NET_WIRELESS_MCW1001_CP_1, &wifiStatus);
  localPort = 80;

  LATD=9;

  RunHttpServer();


  LATB=0;
  while(1) {
    socketHandle = 0;
    backLog = 1;
    Net_Wireless_MCW1001_SocketCreate(&socketHandle, _NET_WIRELESS_MCW1001_SOCKET_TYPE_TCP);   // Create TCP socket
    Net_Wireless_MCW1001_SocketBind(socketHandle, &localPort, &bindResponse);                  // Bind socket to the listen port
    Net_Wireless_MCW1001_TCP_Listen(socketHandle, &backLog, &listenResponse);                  // Prepare the socket to listen for connection
                                                                                               // with one children socket.
    LATD=10;
    while(1) {
      socketChild = socketHandle;
      Net_Wireless_MCW1001_TCP_Accept(&socketChild, &remotePort, remoteIpAdd);                 // Accept incoming conncetion
      if (socketChild != 254) {                                                                // Accept function set socketChild.
                                                                                               // If socketChild == 254 there is no incoming connection
        PORTE.B7 = 1;                                                                          // Connection established
        LATD=11;
        break;
      }
    }

    LATB=0xFF;

    // Connection established
    while(1) {

      CheckTcpReceive();

      if(Net_Wireless_MCW1001_TCP_Status(socketChild, 0xff) != 0) {             // If connection closed improperly from remote side
//        OutputLedPort = 0;                                                      // Clear low nibble of "LED port" and TCO connection status indicator
        Net_Wireless_MCW1001_SocketClose(socketChild);                          // Close sockets
        Net_Wireless_MCW1001_SocketClose(socketHandle);
        LATD=12;
        break;
      }

    }

  }

}