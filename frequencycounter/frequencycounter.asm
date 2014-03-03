
_main:

;frequencycounter.c,83 :: 		void main(){
;frequencycounter.c,87 :: 		hardwareInit() ;
	CALL        _hardwareInit+0, 0
;frequencycounter.c,88 :: 		initPitchArray();
	CALL        _initPitchArray+0, 0
;frequencycounter.c,89 :: 		frequencyCalculatorInit(10);
	MOVLW       10
	MOVWF       FARG_frequencyCalculatorInit_factor+0 
	CALL        _frequencyCalculatorInit+0, 0
;frequencycounter.c,90 :: 		Lcd_Init();
	CALL        _Lcd_Init+0, 0
;frequencycounter.c,91 :: 		Lcd_Cmd(_LCD_CLEAR);
	MOVLW       1
	MOVWF       FARG_Lcd_Cmd_out_char+0 
	CALL        _Lcd_Cmd+0, 0
;frequencycounter.c,92 :: 		Lcd_Cmd(_LCD_CURSOR_OFF);
	MOVLW       12
	MOVWF       FARG_Lcd_Cmd_out_char+0 
	CALL        _Lcd_Cmd+0, 0
;frequencycounter.c,93 :: 		Lcd_Out(1,1,txt1);
	MOVLW       1
	MOVWF       FARG_Lcd_Out_row+0 
	MOVLW       1
	MOVWF       FARG_Lcd_Out_column+0 
	MOVLW       _txt1+0
	MOVWF       FARG_Lcd_Out_text+0 
	MOVLW       hi_addr(_txt1+0)
	MOVWF       FARG_Lcd_Out_text+1 
	CALL        _Lcd_Out+0, 0
;frequencycounter.c,94 :: 		Lcd_Out(2,1,txt2);
	MOVLW       2
	MOVWF       FARG_Lcd_Out_row+0 
	MOVLW       1
	MOVWF       FARG_Lcd_Out_column+0 
	MOVLW       _txt2+0
	MOVWF       FARG_Lcd_Out_text+0 
	MOVLW       hi_addr(_txt2+0)
	MOVWF       FARG_Lcd_Out_text+1 
	CALL        _Lcd_Out+0, 0
;frequencycounter.c,96 :: 		startCapture();                    // first capture
	CALL        _startCapture+0, 0
;frequencycounter.c,97 :: 		do {
L_main0:
;frequencycounter.c,99 :: 		if(pulsesCaptured > pulsesToCapture){
	MOVF        _pulsesCaptured+0, 0 
	SUBLW       16
	BTFSC       STATUS+0, 0 
	GOTO        L_main3
;frequencycounter.c,100 :: 		average = getAverageCapture();
	CALL        _getAverageCapture+0, 0
;frequencycounter.c,101 :: 		frequency = calculateFrequency(average);
	MOVF        R0, 0 
	MOVWF       FARG_calculateFrequency_timerValue+0 
	MOVF        R1, 0 
	MOVWF       FARG_calculateFrequency_timerValue+1 
	CALL        _calculateFrequency+0, 0
;frequencycounter.c,102 :: 		printNote(frequency, 10);
	MOVF        R0, 0 
	MOVWF       FARG_printNote_frequency+0 
	MOVF        R1, 0 
	MOVWF       FARG_printNote_frequency+1 
	MOVLW       10
	MOVWF       FARG_printNote_factor+0 
	MOVLW       0
	MOVWF       FARG_printNote_factor+1 
	CALL        _printNote+0, 0
;frequencycounter.c,103 :: 		startCapture();
	CALL        _startCapture+0, 0
;frequencycounter.c,104 :: 		}
L_main3:
;frequencycounter.c,105 :: 		} while (1);
	GOTO        L_main0
;frequencycounter.c,106 :: 		}
L_end_main:
	GOTO        $+0
; end of _main

_startCapture:

;frequencycounter.c,110 :: 		void startCapture(){
;frequencycounter.c,111 :: 		pCaptures = &captures ;              // resetting overwrites previous captures
	MOVLW       _captures+0
	MOVWF       _pCaptures+0 
	MOVLW       hi_addr(_captures+0)
	MOVWF       _pCaptures+1 
;frequencycounter.c,112 :: 		pulsesCaptured = 0;
	CLRF        _pulsesCaptured+0 
;frequencycounter.c,113 :: 		CCP1CON = 0b00000101 ;               // Start capture, capture on every rising edge
	MOVLW       5
	MOVWF       CCP1CON+0 
;frequencycounter.c,114 :: 		}
L_end_startCapture:
	RETURN      0
; end of _startCapture

_calculateFrequency:

;frequencycounter.c,118 :: 		unsigned int calculateFrequency(unsigned int timer){
;frequencycounter.c,122 :: 		unsigned long timerAsLong = timer;
	MOVF        FARG_calculateFrequency_timer+0, 0 
	MOVWF       calculateFrequency_timerAsLong_L0+0 
	MOVF        FARG_calculateFrequency_timer+1, 0 
	MOVWF       calculateFrequency_timerAsLong_L0+1 
	MOVLW       0
	MOVWF       calculateFrequency_timerAsLong_L0+2 
	MOVWF       calculateFrequency_timerAsLong_L0+3 
;frequencycounter.c,131 :: 		clockCycles = timerAsLong * clockCyclesPerTimerClick + interruptDelayClockCycles;
	MOVF        calculateFrequency_timerAsLong_L0+0, 0 
	MOVWF       R0 
	MOVF        calculateFrequency_timerAsLong_L0+1, 0 
	MOVWF       R1 
	MOVF        calculateFrequency_timerAsLong_L0+2, 0 
	MOVWF       R2 
	MOVF        calculateFrequency_timerAsLong_L0+3, 0 
	MOVWF       R3 
	MOVF        _clockCyclesPerTimerClick+0, 0 
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Mul_32x32_U+0, 0
	MOVF        _interruptDelayClockCycles+0, 0 
	ADDWF       R0, 0 
	MOVWF       R4 
	MOVLW       0
	ADDWFC      R1, 0 
	MOVWF       R5 
	MOVLW       0
	ADDWFC      R2, 0 
	MOVWF       R6 
	MOVLW       0
	ADDWFC      R3, 0 
	MOVWF       R7 
;frequencycounter.c,132 :: 		return clockCyclesPerSecondTimesFactor / clockCycles;
	MOVF        _clockCyclesPerSecondTimesFactor+0, 0 
	MOVWF       R0 
	MOVF        _clockCyclesPerSecondTimesFactor+1, 0 
	MOVWF       R1 
	MOVF        _clockCyclesPerSecondTimesFactor+2, 0 
	MOVWF       R2 
	MOVF        _clockCyclesPerSecondTimesFactor+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
;frequencycounter.c,133 :: 		}
L_end_calculateFrequency:
	RETURN      0
; end of _calculateFrequency

_getAverageCapture:

;frequencycounter.c,135 :: 		unsigned int getAverageCapture(){
;frequencycounter.c,137 :: 		unsigned long sum = 0;
	CLRF        getAverageCapture_sum_L0+0 
	CLRF        getAverageCapture_sum_L0+1 
	CLRF        getAverageCapture_sum_L0+2 
	CLRF        getAverageCapture_sum_L0+3 
;frequencycounter.c,138 :: 		for(i=0; i<2*pulsesToCapture; i+=2){
	CLRF        R5 
L_getAverageCapture4:
	MOVLW       32
	SUBWF       R5, 0 
	BTFSC       STATUS+0, 0 
	GOTO        L_getAverageCapture5
;frequencycounter.c,139 :: 		sum += (captures[i] << 8) + captures[i+1];  //combine two shorts to an int.
	MOVLW       _captures+0
	MOVWF       FSR0 
	MOVLW       hi_addr(_captures+0)
	MOVWF       FSR0H 
	MOVF        R5, 0 
	ADDWF       FSR0, 1 
	BTFSC       STATUS+0, 0 
	INCF        FSR0H, 1 
	MOVF        POSTINC0+0, 0 
	MOVWF       R1 
	MOVF        R1, 0 
	MOVWF       R3 
	CLRF        R2 
	MOVF        R5, 0 
	ADDLW       1
	MOVWF       R0 
	CLRF        R1 
	MOVLW       0
	ADDWFC      R1, 1 
	MOVLW       _captures+0
	ADDWF       R0, 0 
	MOVWF       FSR2 
	MOVLW       hi_addr(_captures+0)
	ADDWFC      R1, 0 
	MOVWF       FSR2H 
	MOVF        POSTINC2+0, 0 
	ADDWF       R2, 0 
	MOVWF       R0 
	MOVLW       0
	ADDWFC      R3, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	ADDWF       getAverageCapture_sum_L0+0, 1 
	MOVF        R1, 0 
	ADDWFC      getAverageCapture_sum_L0+1, 1 
	MOVLW       0
	ADDWFC      getAverageCapture_sum_L0+2, 1 
	ADDWFC      getAverageCapture_sum_L0+3, 1 
;frequencycounter.c,138 :: 		for(i=0; i<2*pulsesToCapture; i+=2){
	MOVLW       2
	ADDWF       R5, 1 
;frequencycounter.c,140 :: 		}
	GOTO        L_getAverageCapture4
L_getAverageCapture5:
;frequencycounter.c,141 :: 		return sum >> 4;
	MOVLW       4
	MOVWF       R4 
	MOVF        getAverageCapture_sum_L0+0, 0 
	MOVWF       R0 
	MOVF        getAverageCapture_sum_L0+1, 0 
	MOVWF       R1 
	MOVF        getAverageCapture_sum_L0+2, 0 
	MOVWF       R2 
	MOVF        getAverageCapture_sum_L0+3, 0 
	MOVWF       R3 
	MOVF        R4, 0 
L__getAverageCapture14:
	BZ          L__getAverageCapture15
	RRCF        R3, 1 
	RRCF        R2, 1 
	RRCF        R1, 1 
	RRCF        R0, 1 
	BCF         R3, 7 
	ADDLW       255
	GOTO        L__getAverageCapture14
L__getAverageCapture15:
;frequencycounter.c,142 :: 		}
L_end_getAverageCapture:
	RETURN      0
; end of _getAverageCapture

_hardwareInit:

;frequencycounter.c,144 :: 		void hardwareInit(void) {          // basic hardware control
;frequencycounter.c,145 :: 		ADCON1 = 0x07;
	MOVLW       7
	MOVWF       ADCON1+0 
;frequencycounter.c,146 :: 		PORTA = 0x00 ;
	CLRF        PORTA+0 
;frequencycounter.c,147 :: 		TRISA = 0X00 ;
	CLRF        TRISA+0 
;frequencycounter.c,148 :: 		PORTC = 0b00000100 ;             // normally high - depending on hardware
	MOVLW       4
	MOVWF       PORTC+0 
;frequencycounter.c,149 :: 		TRISC = 0b00000100 ;             // portc.b2 as input
	MOVLW       4
	MOVWF       TRISC+0 
;frequencycounter.c,151 :: 		T1CON.TMR1ON = 0 ;               // ensure it's off
	BCF         T1CON+0, 0 
;frequencycounter.c,152 :: 		T1CON.T1OSCEN = 0 ;              // ensure OSC is OFF
	BCF         T1CON+0, 3 
;frequencycounter.c,153 :: 		T1CON.TMR1CS = 0;                // Timer1 counts pulses from internal oscillator
	BCF         T1CON+0, 1 
;frequencycounter.c,154 :: 		T1CON.T1CKPS1 = 0;               // prescale the input pulses
	BCF         T1CON+0, 5 
;frequencycounter.c,155 :: 		T1CON.T1CKPS0 = 0;               // Assigned T1 prescaler rate is 1:1 so T1 is Fosc/4 with 20Mhz clock = 0.2uS per tick
	BCF         T1CON+0, 4 
;frequencycounter.c,157 :: 		INTCON.PEIE = 1 ;                // enable peripheral interrupts
	BSF         INTCON+0, 6 
;frequencycounter.c,158 :: 		PIE1.CCP1IE = 1 ;                // enable CCP1 capture interrupt
	BSF         PIE1+0, 2 
;frequencycounter.c,159 :: 		INTCON.GIE = 1 ;                 // enable global interrupts
	BSF         INTCON+0, 7 
;frequencycounter.c,160 :: 		}
L_end_hardwareInit:
	RETURN      0
; end of _hardwareInit

_frequencyCalculatorInit:

;frequencycounter.c,162 :: 		void frequencyCalculatorInit(unsigned short factor){
;frequencycounter.c,166 :: 		clockCyclesPerSecondTimesFactor = Clock_kHz() * 1000 * factor;
	MOVLW       0
	MOVWF       R0 
	MOVLW       45
	MOVWF       R1 
	MOVLW       49
	MOVWF       R2 
	MOVLW       1
	MOVWF       R3 
	MOVF        FARG_frequencyCalculatorInit_factor+0, 0 
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Mul_32x32_U+0, 0
	MOVF        R0, 0 
	MOVWF       _clockCyclesPerSecondTimesFactor+0 
	MOVF        R1, 0 
	MOVWF       _clockCyclesPerSecondTimesFactor+1 
	MOVF        R2, 0 
	MOVWF       _clockCyclesPerSecondTimesFactor+2 
	MOVF        R3, 0 
	MOVWF       _clockCyclesPerSecondTimesFactor+3 
;frequencycounter.c,169 :: 		clockCyclesPerTimerClick = 16;
	MOVLW       16
	MOVWF       _clockCyclesPerTimerClick+0 
;frequencycounter.c,173 :: 		interruptDelayClockCycles = 16 * 4 + 15;
	MOVLW       79
	MOVWF       _interruptDelayClockCycles+0 
;frequencycounter.c,174 :: 		}
L_end_frequencyCalculatorInit:
	RETURN      0
; end of _frequencyCalculatorInit

_interrupt:

;frequencycounter.c,196 :: 		void interrupt(void) {
;frequencycounter.c,199 :: 		if (PIR1.CCP1IF){
	BTFSS       PIR1+0, 2 
	GOTO        L_interrupt7
;frequencycounter.c,205 :: 		MOVF        CCPR1H+0, 0
	MOVF        CCPR1H+0, 0, 1
;frequencycounter.c,206 :: 		MOVWF       _capturedData+1
	MOVWF       _capturedData+1, 1
;frequencycounter.c,207 :: 		MOVF        CCPR1L+0, 0
	MOVF        CCPR1L+0, 0, 1
;frequencycounter.c,208 :: 		MOVWF       _capturedData+0
	MOVWF       _capturedData+0, 1
;frequencycounter.c,209 :: 		BCF         T1CON+0, 0
	BCF         T1CON+0, 0, 1
;frequencycounter.c,210 :: 		CLRF        TMR1H+0
	CLRF        TMR1H+0, 1
;frequencycounter.c,211 :: 		CLRF        TMR1L+0
	CLRF        TMR1L+0, 1
;frequencycounter.c,212 :: 		BSF         T1CON+0, 5
	BSF         T1CON+0, 5, 1
;frequencycounter.c,213 :: 		BCF         T1CON+0, 4
	BCF         T1CON+0, 4, 1
;frequencycounter.c,214 :: 		BCF         PIR1+0, 0
	BCF         PIR1+0, 0, 1
;frequencycounter.c,215 :: 		BSF         T1CON+0, 0
	BSF         T1CON+0, 0, 1
;frequencycounter.c,233 :: 		storeInput() ;                   // copy the captured data to the array of samples.
	CALL        _storeInput+0, 0
;frequencycounter.c,234 :: 		PIR1.CCP1IF = 0 ;                // clear capture flag after grabbing data
	BCF         PIR1+0, 2 
;frequencycounter.c,235 :: 		}
L_interrupt7:
;frequencycounter.c,236 :: 		}
L_end_interrupt:
L__interrupt19:
	RETFIE      1
; end of _interrupt

_storeInput:

;frequencycounter.c,238 :: 		void storeInput(void) {
;frequencycounter.c,239 :: 		if (pulsesCaptured == 0) {           // ignore startup delay - start storing between first + second edge
	MOVF        _pulsesCaptured+0, 0 
	XORLW       0
	BTFSS       STATUS+0, 2 
	GOTO        L_storeInput8
;frequencycounter.c,240 :: 		pulsesCaptured = 1 ;
	MOVLW       1
	MOVWF       _pulsesCaptured+0 
;frequencycounter.c,241 :: 		return ;
	GOTO        L_end_storeInput
;frequencycounter.c,242 :: 		}
L_storeInput8:
;frequencycounter.c,243 :: 		pulsesCaptured++;
	INCF        _pulsesCaptured+0, 1 
;frequencycounter.c,244 :: 		if(pulsesCaptured > pulsesToCapture){
	MOVF        _pulsesCaptured+0, 0 
	SUBLW       16
	BTFSC       STATUS+0, 0 
	GOTO        L_storeInput9
;frequencycounter.c,245 :: 		CCP1CON = 0 ;                    // Stop capture when the desired number of pulses have been captured.
	CLRF        CCP1CON+0 
;frequencycounter.c,246 :: 		}
L_storeInput9:
;frequencycounter.c,247 :: 		*(pCaptures++) = hi(capturedData) ;   // store captured hibyte:lobyte in array
	MOVFF       _pCaptures+0, FSR1
	MOVFF       _pCaptures+1, FSR1H
	MOVF        _capturedData+1, 0 
	MOVWF       POSTINC1+0 
	INFSNZ      _pCaptures+0, 1 
	INCF        _pCaptures+1, 1 
;frequencycounter.c,248 :: 		*(pCaptures++) = lo(capturedData) ;
	MOVFF       _pCaptures+0, FSR1
	MOVFF       _pCaptures+1, FSR1H
	MOVF        _capturedData+0, 0 
	MOVWF       POSTINC1+0 
	INFSNZ      _pCaptures+0, 1 
	INCF        _pCaptures+1, 1 
;frequencycounter.c,249 :: 		}
L_end_storeInput:
	RETURN      0
; end of _storeInput
