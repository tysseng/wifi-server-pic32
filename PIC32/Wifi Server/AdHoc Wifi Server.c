/**
Tiny adhoc wifi http server

Config for EasyPIC Fusion v7 with "wifi plus" click in left position:

- RST at RC1
- RT:  RF4 (UART2)
- RX:  RF5 (UART2)
- CTS: SCL/RA2 (SCL2)
- RTS: SDA/RA3 (SDA2)
- GP0: RB8/AN
- GP1: RD0/PWM
- GP2: RE8/INT

Use UART2 for io.
**/

sbit Net_Wireless_MCW1001_Chip_RST           at RC1_bit;
sbit Net_Wireless_MCW1001_Chip_RST_Direction at TRISC1_bit;

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
char *strSSID = "Joakims kompetansedag";
char channels[11]  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
char wifiStatus, bindResponse, backLog, listenResponse;
char socketHandle, socketChild;
char response;

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
const code char *indexPage = 
      "<HTML><HEAD><meta name=\"viewport\" content=\"width=device-width, initial-scale=2\"></HEAD>\
      <BODY style='font-size: 20px'>\
      <p><a href=\"/paa\">Skru p&aring;</a></p>\
      <p><a href=\"/av\">Skru av</a></p>";

const code char *indexPage2 =
      "</body></html>";

////////////////////////////////////////////////////////////////////////////////
// Interrupt routine for UART module

void interrupt() iv IVT_UART_2 ilevel 7 ics ICS_SRS {
  Net_Wireless_MCW1001_Receive();
  U2RXIF_bit = 0;           // ensure interrupt not pending
}

////////////////////////////////////////////////////////////////////////////////
// Interrupt routine which increment Net_Wireless_MCW1001_Time variable every second.

// timer prescaler. each timer interval is 100ms @ 80MHz
unsigned int wifiTmr = 0;
void Timer1Interrupt() iv IVT_TIMER_1 ilevel 7 ics ICS_SRS {
  wifiTmr++;
  if(wifiTmr >= 10) {
    Net_Wireless_MCW1001_Time++ ;
    wifiTmr = 0;
  }
  T1IF_bit = 0;
}


////////////////////////////////////////////////////////////////////////////////
// Initialization of MCU modules

void InitTimer1(){
  T1CON     = 0x8030;
  T1IE_bit  = 1;
  T1IF_bit  = 0;
  T1IP0_bit = 1;
  T1IP1_bit = 1;
  T1IP2_bit = 1;
  PR1       = 31250;
  TMR1      = 0;
}


void InitMcu() {

  AD1PCFG |= 0xFF7F;         // all digital but rb7(AN7)
  TRISB.B7 = 1;              // set PORTB as input for buttons and adc

  InitTimer1();

  // Debug leds etc
  TRISB = 0;                // Initialize PORTB as output
  TRISD = 0;                // Initialize PORTD as output

  // Initialization of UART interrupt
  U2IP0_bit = 1;            // set interrupt
  U2IP1_bit = 1;            // priority
  U2IP2_bit = 1;            // to 7

  U2STA.B7 = 0;             // 0x = Interrupt flag bit is set when a character is received
  U2RXIF_bit = 0;           // ensure interrupt not pending
  U2RXIE_bit = 1;           // enable intterupt

  EnableInterrupts();       // Enable all interrupts

  // Set RE7 pin as digital output, so we can monitor TCP status on that pin
  TRISE.B7 = 0;
  PORTE.B7 = 0;

  Net_Wireless_MCW1001_HwReset();
  
  UART2_Init(115200);
  Delay_ms(2000);
}

////////////////////////////////////////////////////////////////////////////////
// Initialization of WiFi module

void InitWiFi() {
  volatile char response;

  Net_Wireless_MCW1001_TimeToWait = 1;
  response = 1;
  while(response != 0){
    // Set "Connection Profile 1" mode to Ad-Hoc
    response = Net_Wireless_MCW1001_SetMode(_NET_WIRELESS_MCW1001_CP_1, _NET_WIRELESS_MCW1001_MODE_ADHOC);
  }

  Net_Wireless_MCW1001_TimeToWait = 10;
  response =Net_Wireless_MCW1001_SetChannelList(1, channels);                     // Choose only channel l one for communication
  response =Net_Wireless_MCW1001_SetSecurity_Open(_NET_WIRELESS_MCW1001_CP_1);    // Set open security level
  response =Net_Wireless_MCW1001_SetSSID(_NET_WIRELESS_MCW1001_CP_1, strSSID);    // Set SSID of network

  // Set network parameters
  response =Net_Wireless_MCW1001_SetNetworkMask(netMask);
  Net_Wireless_MCW1001_SetGatewayIP(gatewayAdd);
  Net_Wireless_MCW1001_SetMAC(myMacAdd);
  Net_Wireless_MCW1001_SetIP(myIpAdd);

  Net_Wireless_MCW1001_SetArpTime(1);                                   // Set gratuitous ARP timing
  Net_Wireless_MCW1001_SetRetryCount(5, 0);                             // No retries for the connection if the first attempt fails,
}

// Fill local buffer with const data. When buffer overflows, send it to WiFi module
unsigned int PutConstString(const code char *s) {
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

// Fill local buffer with data. When buffer overflows, send it to WiFi module
unsigned int PutString(char *s) {
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

void SendHttpResponse() {

  char response, i;
  unsigned int numOfReceiveBytes = 0, numOfSentBytes;
  unsigned int length;                     // my reply length

  length = 0;
  Net_Wireless_MCW1001_TCP_ReadBytes(&socketChild, 10, getRequest, &numOfReceiveBytes);
  getRequest[10] = 0;

  if(memcmp(getRequest, httpMethod, 5)) {     // only GET method is supported here
    return;
  }

  // Send header and page start
  bufferIndex = 0;
  PutConstString(httpHeader) ;             // HTTP header
  PutConstString(httpMimeTypeHTML) ;       // with HTML MIME type
  length = PutConstString(indexPage) ;     // HTML page first part

  httpCounter++ ;                          // one more request done
  
  // if request path name starts with a, turn off leds
  if(getRequest[5] == 'a') {
    LATB = 0;
    PutConstString("<p>Status: AV</p>");
  }
  
  // if request path name starts with p, turn on leds
  else if(getRequest[5] == 'p') {
    LATB = 0xff;
    PutConstString("<p>Status: P&Aring;</p>");
  }

  // output page end
  length = PutConstString(indexPage2) ;

  // If there is some data (in txBuffer) which is not sent
  if(length != 0){
    Net_Wireless_MCW1001_TCP_SendBytes(socketChild, txBuffer, length, &numOfSentBytes);
  }
}

// Eternally looping service method
void RunHttpServer() {

  LATD=10;
  while(1) {
    socketHandle = 0;
    backLog = 1;
    localPort = 80;
    Net_Wireless_MCW1001_SocketCreate(&socketHandle, _NET_WIRELESS_MCW1001_SOCKET_TYPE_TCP);   // Create TCP socket
    Net_Wireless_MCW1001_SocketBind(socketHandle, &localPort, &bindResponse);                  // Bind socket to the listen port
    Net_Wireless_MCW1001_TCP_Listen(socketHandle, &backLog, &listenResponse);                  // Prepare the socket to listen for connection
                                                                                               // with one children socket.
    while(1) {
      socketChild = socketHandle;
      Net_Wireless_MCW1001_TCP_Accept(&socketChild, &remotePort, remoteIpAdd);                 // Accept incoming conncetion
      if (socketChild != 254) {                                                                // Accept function set socketChild.
        break;                                                                                 // If socketChild == 254 there is no incoming connection
      }
      // Server waiting for connection
      LATD.B1 = 1;
      LATD.B2 = 0;
    }

    // Server is handling request
    LATD.B1 = 0;
    LATD.B2 = 1;

    SendHttpResponse();
    Net_Wireless_MCW1001_SocketClose(socketChild);                                             // Close sockets
    Net_Wireless_MCW1001_SocketClose(socketHandle);
  }
}

void main() {

  InitMcu();
  InitWiFi();
  // Open "ad-hoc server"
  Net_Wireless_MCW1001_Connect(_NET_WIRELESS_MCW1001_CP_1, &wifiStatus);
  localPort = 80;

  // Signal "wifi ready"
  LATD.B0=1;

  RunHttpServer();

}