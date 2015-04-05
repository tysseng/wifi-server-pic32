/**
 * PIC32MX SPI slave example using interrupt for detecting received data and
 * responding with a byte that the master may read and show.
 *
 * Tested on an EasyPIC Fusion v8 with a PIC32MX795F512L receiving data from an
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
 *  - Data received from the master is displayed on PORTD
 *  - Number of received get data-commands (and incidently, data returned to
 *    the master) is displayed on PORTB
 *  - PIC32 SPI is 5v tolerant so no voltage conversion is needed between a
 *    PIC18F and a PIC32.
 */
 
/**
 PIC SPI theory
 ==============
 SPI data transfer works like this: Whenever the SPI clock runs, anything that
 is  in the SPI4BUF will be sent through the SDO pin on both the master and
 slave.

 As the master is the one to control the clock, no data is sent from the slave
 without the clock running, and the easiest way to get the clock running is
 to send data from the master. An easier way to think of it may be this:

 Whenever data is received from the master, whatever is in the slave's SPI4BUF
 at the time the data is received is returned and SPI4BUF is filled with the
 received data. This happens automatically.

 If no data is written to the SPI4BUF by the slave between two received bytes
 from the master, the previously received byte will be returned to the master.

 Returning data from the slave to the master
 ===========================================
 As the slave most often will not know what data it should send and when to
 send it before getting instructions from a master, a normal way to make the
 slave send data back to the master is to fill the SPI4BUF only after a
 command is received from the master. The master will then have to write a new
 byte to get the data (but the slave may choose to ignore exactly WHAT it
 receives this time around).

 In MikroC, the dialogue between the master and slave would act out like this:
 1) The MASTER sends a command using SPI_write(command);

 2) The SLAVE sends whatever is in its SPI4BUF while at the same time receiving
    the data

 3) The SLAVE reads the contents of its SPI4BUF (which also clears it) and
    interprets the command.

 4) If the received command is a 'return data please' command, it writes its
    data to the SPI4BUF, but the data is not sent yet as the SPIclock is not
    running - remember, the clock is only running while the master sends data.

 5) The MASTER now issues an SPI_read(). This actually sends data to the slave
    but the value may be ignored.

 6) Since the SLAVE receives data, it also sends back the data that is in the
    SPI4BUF, which the master receives and returns as the result of the
    SPI_read() command.
*/

// A "get data command" that the master may send to get data from the slave.
// The value of the command is just chosen arbitrary for this example, the
// main point is that it has to be part of a protocol, something the master and
// the slave has agreed upon in advance.
// Whenever the slave receives this command, it knows that it should fill the
// SSPBUF with the data that the master wants.
unsigned short GET_DATA_COMMAND=0xFF;

// Counter that just counts the number of get data commands received. It is
// sent back to the master the next time data is written.
char txByte = 0;

void SPI4_interrupt() iv IVT_SPI_4 ilevel 6 ics ICS_SOFT{

  char rxByte;

  rxByte = SPI4BUF;
  PORTD = rxByte;

  if(rxByte == GET_DATA_COMMAND){
    // Aha, the master wants our data! Whatever was in the SSPBUF when
    // we received this command has already been sent back to the master
    // (and ignored). But by placing our data in the output buffer we will
    // make sure that the next time we hear back from the master (i.e. when
    // the master issues an SPI_read() command, it will get our data.
    PORTB = txByte;
    SPI4BUF = txByte++;
  }
  // there is no else here, but in the 'else' case, data from the SSPBUF
  // that was written during the previous call is returned
    
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

  //Visualise output on portB
  TRISB=0;
  PORTB=0;

  //Visualise input on port D
  TRISD=0;
  PORTD=0;

  while(1){
    // do whatever you want to do here, play some music, measure something or
    // just do nothing.
  }
}