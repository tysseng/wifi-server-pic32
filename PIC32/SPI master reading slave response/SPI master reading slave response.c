/**
 * PIC32MX SPI master example that issues a 'get data' command to the slave and
 * reads back the result, tested on an EasyPIC Fusion v8 with a
 * PIC32MX795F512L sending data to an EasyPIC3 with a PIC18F458
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
 *  - PORTB is incremented every time data is sent
 *  - PORTD shows data received from the slave
 *  - PIC32 SPI is 5v tolerant so no voltage conversion is needed between a
 *    PIC18F and a PIC32.
 *  - Data sent has been confirmed with a Saleae logic probe.
 */
unsigned short state=0;

// A "get data command" that may be sent to the slave to retrieve data. The 
// value of the command is just chosen arbitrary for this example, the main 
// point is that it has to be part of a protocol, something the master and the 
// slave has agreed upon in advance. 
// Whenever the slave receives this command, it knows that it should fill the
// SSPBUF with the data that the master wants.
unsigned short GET_DATA_COMMAND=0xFF;

void main() {

  //Show that we are actually running.
  TRISB = 0;
  PORTB = 0;

  //Visualise slave response on port D
  TRISD = 0;
  PORTD = 0;

  //NB: SPI4 = SPI3A on the chip as Mikroelektronika and Microchip use
  //    different naming schemes
  SPI4_Init_Advanced(
    _SPI_MASTER,
    _SPI_8_BIT,
    1024,
    _SPI_SS_DISABLE,
    _SPI_DATA_SAMPLE_MIDDLE,
    _SPI_CLK_IDLE_LOW,
    _SPI_ACTIVE_2_IDLE);

  while(1){
    // Write command to slave. The slave will return whatever is in its SPI buffer,
    // but as this may only be garbage it is ignored - it is not returned to 
    // us by the SPI_write command.
    SPI4_write(GET_DATA_COMMAND);
    PORTB = state;
    state++;
    
    // Wait a little for the slave to get its data ready. The delay may not
    // be necessary in this example but in a real world case the slave may
    // spend some time measuring something etc. before it is ready to return
    // data.
    delay_ms(10);
    
    // A SPI_read command actually sends a byte to the slave and then
    // reads back whatever the slave returned. The value of the input parameter
    // is in fact sent to the slave where it may or may not be ignored. In
    // this example it is ignored by the slave.
    PORTD = SPI4_read(state);
    delay_ms(1000);
  }
}