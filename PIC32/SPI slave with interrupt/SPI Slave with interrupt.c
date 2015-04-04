/**
 * PIC32MX SPI slave example using interrupt for detecting received data, 
 * tested on an EasyPIC Fusion v8 with a PIC32MX795F512L receiving data from an 
 * EasyPIC3 with a PIC18F458
 *
 * Compiler versions tested:
 *   - MikroC PRO for PIC v6.5.0
 *   - MikroC PRO for PIC32 v3.5.0
 *
 * Example uses SPI3A as this is the only SPI port on the PIC32MX795F512L that
 * has all the pins on the same output header.
 *
 * SPI pins:
 *   RF13 = SCK (pin 39)
 *   RF4 = SDI (pin 49)
 *   RF5 = SDO (pin 50)
 *
 * Note:
 *  - _SPI_ACTIVE_2_IDLE in MikroC PRO for PIC32 equals _SPI_HIGH_2_LOW in
 *    MikroC PRO for PIC
 *  - in MikroC, SPI3A is named SPI4
 *  - Data received through SPI is displayed on PORTB
 *  - PIC32 SPI is 5v tolerant so no voltage conversion is needed between a
 *    PIC18F and a PIC32.
 */
unsigned short rcdata;

void SPI4_interrupt() iv IVT_SPI_4 ilevel 6 ics ICS_SOFT{

  char rxByte;

  // read bytes until the enhanced circular buffer is empty
  // (in this demo only the last recevived byte will be visible
  // on PORTB. A real world case would of course do something useful to the 
  // bytes).
  //
  // This check is only necessary if you have turned on the enhanced circular
  // buffer, without it, only one byte will be available anyway.

  // TODO: THIS DOES NOT WORK, the SPIRBE does not become 1.

 // while (SPI4STAT.SPIRBE == 0){
 
 
    rxByte = SPI4BUF;
    PORTB = rxByte;
  //}

  //reset interrupt
  SPI4RXIF_bit = 0;
}

void SPI4_init_interrupts(){

  //Trigger interrupt when buffer is not empty
   SPI4CON.SRXISEL1 = 0;
   SPI4CON.SRXISEL0 = 1;
   
   //Enhanced circular buffer - turn on internal 128 bit FIFO RX and TX buffers
   SPI4CON.ENHBUF = 1;

   //Clear data received interrupt flag
   SPI4RXIF_bit = 0;

   //Set priority as 6
   IPC8.B4 = 1;  //SPI3AIP2
   IPC8.B3 = 1;  //SPI3AIP1
   IPC8.B2 = 0;  //SPI3AIP0

   //Enable interrupt on receive
   SPI4RXIE_bit = 1;
}

void main() {

  unsigned short i;

  SPI4_init_interrupts();
  EnableInterrupts();

  //NB: SPI4 = SPI3A on the chip as Mikroelektronika and Microchip use
  //    different naming schemes
  SPI4_Init_Advanced(
    _SPI_SLAVE,
    _SPI_8_BIT,
    1024, // not used in slave mode
    _SPI_SS_DISABLE,
    _SPI_DATA_SAMPLE_MIDDLE,
    _SPI_CLK_IDLE_LOW,
    _SPI_ACTIVE_2_IDLE);

  //Visualise input on port B
  TRISB = 0;
  PORTB = 0;

  //Show that I'm alive on port D
  TRISD = 0;
  PORTD = 0;

  while(1){
    if(PORTD == 0){
      PORTD = 0xFF;
    } else {
      PORTD = 0;
    }
    delay_ms(1000);
  }
}