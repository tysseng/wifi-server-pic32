/**
 * PIC18F Minimal SPI slave example, tested on an EasyPIC3 with a PIC18F458
 * receiving data from a PIC32MX795F512L on an EasyPIC Fusion v7
 *
 * Compiler versions tested:
 *   - MikroC PRO for PIC v6.5.0
 *   - MikroC PRO for PIC32 v3.5.0
 *
 * SPI pins:
 *   RC3 = SCK (pin 18)
 *   RC4 = SDI (pin 24)
 *   RC5 = SDO (pin 23)
 *
 * Note:
 *  - _SPI_HIGH_2_LOW in MikroC PRO for PIC equals_SPI_ACTIVE_2_IDLE in
 *    MikroC PRO for PIC32
 *  - Data received through SPI is displayed on PORTD
 *  - No voltage conversion is needed even if connected to a 3.3v master as long
 *    as the master is 5V tolerant (the PIC32 is).
 */
unsigned short rcdata;

void main() {
  SPI1_Init_Advanced(
    _SPI_SLAVE_SS_DIS,
    _SPI_DATA_SAMPLE_MIDDLE,
    _SPI_CLK_IDLE_LOW,
    _SPI_HIGH_2_LOW
  );

  TRISD=0;
  PORTD=0;
  LATC.B3 = 1; //weird bug! This is required????

  while(1){
     rcdata=SSPBUF;
     PORTD = rcdata;
  }
}