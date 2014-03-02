
_main:

;frequencycounter.c,95 :: 		void main(){
;frequencycounter.c,99 :: 		hardwareInit() ;
	CALL        _hardwareInit+0, 0
;frequencycounter.c,100 :: 		frequencyCalculatorInit(100);
	MOVLW       100
	MOVWF       FARG_frequencyCalculatorInit_factor+0 
	CALL        _frequencyCalculatorInit+0, 0
;frequencycounter.c,101 :: 		Lcd_Init();
	CALL        _Lcd_Init+0, 0
;frequencycounter.c,102 :: 		Lcd_Cmd(_LCD_CLEAR);
	MOVLW       1
	MOVWF       FARG_Lcd_Cmd_out_char+0 
	CALL        _Lcd_Cmd+0, 0
;frequencycounter.c,103 :: 		Lcd_Cmd(_LCD_CURSOR_OFF);
	MOVLW       12
	MOVWF       FARG_Lcd_Cmd_out_char+0 
	CALL        _Lcd_Cmd+0, 0
;frequencycounter.c,104 :: 		Lcd_Out(1,1,txt1);
	MOVLW       1
	MOVWF       FARG_Lcd_Out_row+0 
	MOVLW       1
	MOVWF       FARG_Lcd_Out_column+0 
	MOVLW       _txt1+0
	MOVWF       FARG_Lcd_Out_text+0 
	MOVLW       hi_addr(_txt1+0)
	MOVWF       FARG_Lcd_Out_text+1 
	CALL        _Lcd_Out+0, 0
;frequencycounter.c,105 :: 		Lcd_Out(2,1,txt2);
	MOVLW       2
	MOVWF       FARG_Lcd_Out_row+0 
	MOVLW       1
	MOVWF       FARG_Lcd_Out_column+0 
	MOVLW       _txt2+0
	MOVWF       FARG_Lcd_Out_text+0 
	MOVLW       hi_addr(_txt2+0)
	MOVWF       FARG_Lcd_Out_text+1 
	CALL        _Lcd_Out+0, 0
;frequencycounter.c,107 :: 		startCapture();                    // first capture
	CALL        _startCapture+0, 0
;frequencycounter.c,108 :: 		do {
L_main0:
;frequencycounter.c,110 :: 		if(pulsesCaptured > pulsesToCapture){
	MOVF        _pulsesCaptured+0, 0 
	SUBLW       16
	BTFSC       STATUS+0, 0 
	GOTO        L_main3
;frequencycounter.c,111 :: 		average = getAverageCapture();
	CALL        _getAverageCapture+0, 0
	MOVF        R0, 0 
	MOVWF       main_average_L0+0 
	MOVF        R1, 0 
	MOVWF       main_average_L0+1 
;frequencycounter.c,112 :: 		frequency = calculateFrequency(average);
	MOVF        R0, 0 
	MOVWF       FARG_calculateFrequency_timerValue+0 
	MOVF        R1, 0 
	MOVWF       FARG_calculateFrequency_timerValue+1 
	CALL        _calculateFrequency+0, 0
	MOVF        R0, 0 
	MOVWF       main_frequency_L0+0 
	MOVF        R1, 0 
	MOVWF       main_frequency_L0+1 
;frequencycounter.c,113 :: 		Lcd_Out_Timer(1, 12, average);
	MOVLW       1
	MOVWF       FARG_Lcd_Out_Timer_row+0 
	MOVLW       12
	MOVWF       FARG_Lcd_Out_Timer_col+0 
	MOVF        main_average_L0+0, 0 
	MOVWF       FARG_Lcd_Out_Timer_number+0 
	MOVF        main_average_L0+1, 0 
	MOVWF       FARG_Lcd_Out_Timer_number+1 
	CALL        _Lcd_Out_Timer+0, 0
;frequencycounter.c,114 :: 		Lcd_Out_Freq(2, 11, frequency);
	MOVLW       2
	MOVWF       FARG_Lcd_Out_Freq_row+0 
	MOVLW       11
	MOVWF       FARG_Lcd_Out_Freq_col+0 
	MOVF        main_frequency_L0+0, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+0 
	MOVF        main_frequency_L0+1, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+1 
	CALL        _Lcd_Out_Freq+0, 0
;frequencycounter.c,115 :: 		startCapture();
	CALL        _startCapture+0, 0
;frequencycounter.c,116 :: 		}
L_main3:
;frequencycounter.c,117 :: 		} while (1);
	GOTO        L_main0
;frequencycounter.c,118 :: 		}
L_end_main:
	GOTO        $+0
; end of _main

_startCapture:

;frequencycounter.c,122 :: 		void startCapture(){
;frequencycounter.c,123 :: 		pCaptures = &captures ;              // resetting overwrites previous captures
	MOVLW       _captures+0
	MOVWF       _pCaptures+0 
	MOVLW       hi_addr(_captures+0)
	MOVWF       _pCaptures+1 
;frequencycounter.c,124 :: 		pulsesCaptured = 0;
	CLRF        _pulsesCaptured+0 
;frequencycounter.c,125 :: 		CCP1CON = 0b00000101 ;               // Start capture, capture on every rising edge
	MOVLW       5
	MOVWF       CCP1CON+0 
;frequencycounter.c,126 :: 		}
L_end_startCapture:
	RETURN      0
; end of _startCapture

_calculateFrequency:

;frequencycounter.c,130 :: 		unsigned int calculateFrequency(unsigned int timer){
;frequencycounter.c,134 :: 		unsigned long timerAsLong = timer;
	MOVF        FARG_calculateFrequency_timer+0, 0 
	MOVWF       calculateFrequency_timerAsLong_L0+0 
	MOVF        FARG_calculateFrequency_timer+1, 0 
	MOVWF       calculateFrequency_timerAsLong_L0+1 
	MOVLW       0
	MOVWF       calculateFrequency_timerAsLong_L0+2 
	MOVWF       calculateFrequency_timerAsLong_L0+3 
;frequencycounter.c,143 :: 		clockCycles = timerAsLong * clockCyclesPerTimerClick + interruptDelayClockCycles;
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
;frequencycounter.c,144 :: 		return clockCyclesPerSecondTimesFactor / clockCycles;
	MOVF        _clockCyclesPerSecondTimesFactor+0, 0 
	MOVWF       R0 
	MOVF        _clockCyclesPerSecondTimesFactor+1, 0 
	MOVWF       R1 
	MOVF        _clockCyclesPerSecondTimesFactor+2, 0 
	MOVWF       R2 
	MOVF        _clockCyclesPerSecondTimesFactor+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
;frequencycounter.c,145 :: 		}
L_end_calculateFrequency:
	RETURN      0
; end of _calculateFrequency

_getAverageCapture:

;frequencycounter.c,147 :: 		unsigned int getAverageCapture(){
;frequencycounter.c,149 :: 		unsigned long sum = 0;
	CLRF        getAverageCapture_sum_L0+0 
	CLRF        getAverageCapture_sum_L0+1 
	CLRF        getAverageCapture_sum_L0+2 
	CLRF        getAverageCapture_sum_L0+3 
;frequencycounter.c,150 :: 		for(i=0; i<2*pulsesToCapture; i+=2){
	CLRF        R5 
L_getAverageCapture4:
	MOVLW       32
	SUBWF       R5, 0 
	BTFSC       STATUS+0, 0 
	GOTO        L_getAverageCapture5
;frequencycounter.c,151 :: 		sum += (captures[i] << 8) + captures[i+1];  //combine two shorts to an int.
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
;frequencycounter.c,150 :: 		for(i=0; i<2*pulsesToCapture; i+=2){
	MOVLW       2
	ADDWF       R5, 1 
;frequencycounter.c,152 :: 		}
	GOTO        L_getAverageCapture4
L_getAverageCapture5:
;frequencycounter.c,153 :: 		return sum >> 4;
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
L__getAverageCapture16:
	BZ          L__getAverageCapture17
	RRCF        R3, 1 
	RRCF        R2, 1 
	RRCF        R1, 1 
	RRCF        R0, 1 
	BCF         R3, 7 
	ADDLW       255
	GOTO        L__getAverageCapture16
L__getAverageCapture17:
;frequencycounter.c,154 :: 		}
L_end_getAverageCapture:
	RETURN      0
; end of _getAverageCapture

_hardwareInit:

;frequencycounter.c,156 :: 		void hardwareInit(void) {          // basic hardware control
;frequencycounter.c,157 :: 		ADCON1 = 0x07;
	MOVLW       7
	MOVWF       ADCON1+0 
;frequencycounter.c,158 :: 		PORTA = 0x00 ;
	CLRF        PORTA+0 
;frequencycounter.c,159 :: 		TRISA = 0X00 ;
	CLRF        TRISA+0 
;frequencycounter.c,160 :: 		PORTC = 0b00000100 ;             // normally high - depending on hardware
	MOVLW       4
	MOVWF       PORTC+0 
;frequencycounter.c,161 :: 		TRISC = 0b00000100 ;             // portc.b2 as input
	MOVLW       4
	MOVWF       TRISC+0 
;frequencycounter.c,163 :: 		T1CON.TMR1ON = 0 ;               // ensure it's off
	BCF         T1CON+0, 0 
;frequencycounter.c,164 :: 		T1CON.T1OSCEN = 0 ;              // ensure OSC is OFF
	BCF         T1CON+0, 3 
;frequencycounter.c,165 :: 		T1CON.TMR1CS = 0;                // Timer1 counts pulses from internal oscillator
	BCF         T1CON+0, 1 
;frequencycounter.c,166 :: 		T1CON.T1CKPS1 = 0;               // prescale the input pulses
	BCF         T1CON+0, 5 
;frequencycounter.c,167 :: 		T1CON.T1CKPS0 = 0;               // Assigned T1 prescaler rate is 1:1 so T1 is Fosc/4 with 20Mhz clock = 0.2uS per tick
	BCF         T1CON+0, 4 
;frequencycounter.c,169 :: 		INTCON.PEIE = 1 ;                // enable peripheral interrupts
	BSF         INTCON+0, 6 
;frequencycounter.c,170 :: 		PIE1.CCP1IE = 1 ;                // enable CCP1 capture interrupt
	BSF         PIE1+0, 2 
;frequencycounter.c,171 :: 		INTCON.GIE = 1 ;                 // enable global interrupts
	BSF         INTCON+0, 7 
;frequencycounter.c,172 :: 		}
L_end_hardwareInit:
	RETURN      0
; end of _hardwareInit

_frequencyCalculatorInit:

;frequencycounter.c,174 :: 		void frequencyCalculatorInit(unsigned short factor){
;frequencycounter.c,178 :: 		clockCyclesPerSecondTimesFactor = Clock_kHz() * 1000 * factor;
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
;frequencycounter.c,181 :: 		clockCyclesPerTimerClick = 16;
	MOVLW       16
	MOVWF       _clockCyclesPerTimerClick+0 
;frequencycounter.c,185 :: 		interruptDelayClockCycles = 16 * 4 + 15;
	MOVLW       79
	MOVWF       _interruptDelayClockCycles+0 
;frequencycounter.c,186 :: 		}
L_end_frequencyCalculatorInit:
	RETURN      0
; end of _frequencyCalculatorInit

_interrupt:

;frequencycounter.c,208 :: 		void interrupt(void) {
;frequencycounter.c,211 :: 		if (PIR1.CCP1IF){
	BTFSS       PIR1+0, 2 
	GOTO        L_interrupt7
;frequencycounter.c,217 :: 		MOVF        CCPR1H+0, 0
	MOVF        CCPR1H+0, 0, 1
;frequencycounter.c,218 :: 		MOVWF       _capturedData+1
	MOVWF       _capturedData+1, 1
;frequencycounter.c,219 :: 		MOVF        CCPR1L+0, 0
	MOVF        CCPR1L+0, 0, 1
;frequencycounter.c,220 :: 		MOVWF       _capturedData+0
	MOVWF       _capturedData+0, 1
;frequencycounter.c,221 :: 		BCF         T1CON+0, 0
	BCF         T1CON+0, 0, 1
;frequencycounter.c,222 :: 		CLRF        TMR1H+0
	CLRF        TMR1H+0, 1
;frequencycounter.c,223 :: 		CLRF        TMR1L+0
	CLRF        TMR1L+0, 1
;frequencycounter.c,224 :: 		BSF         T1CON+0, 5
	BSF         T1CON+0, 5, 1
;frequencycounter.c,225 :: 		BCF         T1CON+0, 4
	BCF         T1CON+0, 4, 1
;frequencycounter.c,226 :: 		BCF         PIR1+0, 0
	BCF         PIR1+0, 0, 1
;frequencycounter.c,227 :: 		BSF         T1CON+0, 0
	BSF         T1CON+0, 0, 1
;frequencycounter.c,245 :: 		storeInput() ;                   // copy the captured data to the array of samples.
	CALL        _storeInput+0, 0
;frequencycounter.c,246 :: 		PIR1.CCP1IF = 0 ;                // clear capture flag after grabbing data
	BCF         PIR1+0, 2 
;frequencycounter.c,247 :: 		}
L_interrupt7:
;frequencycounter.c,248 :: 		}
L_end_interrupt:
L__interrupt21:
	RETFIE      1
; end of _interrupt

_storeInput:

;frequencycounter.c,250 :: 		void storeInput(void) {
;frequencycounter.c,251 :: 		if (pulsesCaptured == 0) {           // ignore startup delay - start storing between first + second edge
	MOVF        _pulsesCaptured+0, 0 
	XORLW       0
	BTFSS       STATUS+0, 2 
	GOTO        L_storeInput8
;frequencycounter.c,252 :: 		pulsesCaptured = 1 ;
	MOVLW       1
	MOVWF       _pulsesCaptured+0 
;frequencycounter.c,253 :: 		return ;
	GOTO        L_end_storeInput
;frequencycounter.c,254 :: 		}
L_storeInput8:
;frequencycounter.c,255 :: 		pulsesCaptured++;
	INCF        _pulsesCaptured+0, 1 
;frequencycounter.c,256 :: 		if(pulsesCaptured > pulsesToCapture){
	MOVF        _pulsesCaptured+0, 0 
	SUBLW       16
	BTFSC       STATUS+0, 0 
	GOTO        L_storeInput9
;frequencycounter.c,257 :: 		CCP1CON = 0 ;                    // Stop capture when the desired number of pulses have been captured.
	CLRF        CCP1CON+0 
;frequencycounter.c,258 :: 		}
L_storeInput9:
;frequencycounter.c,259 :: 		*(pCaptures++) = hi(capturedData) ;   // store captured hibyte:lobyte in array
	MOVFF       _pCaptures+0, FSR1
	MOVFF       _pCaptures+1, FSR1H
	MOVF        _capturedData+1, 0 
	MOVWF       POSTINC1+0 
	INFSNZ      _pCaptures+0, 1 
	INCF        _pCaptures+1, 1 
;frequencycounter.c,260 :: 		*(pCaptures++) = lo(capturedData) ;
	MOVFF       _pCaptures+0, FSR1
	MOVFF       _pCaptures+1, FSR1H
	MOVF        _capturedData+0, 0 
	MOVWF       POSTINC1+0 
	INFSNZ      _pCaptures+0, 1 
	INCF        _pCaptures+1, 1 
;frequencycounter.c,261 :: 		}
L_end_storeInput:
	RETURN      0
; end of _storeInput

_Lcd_Out_Timer:

;frequencycounter.c,265 :: 		void Lcd_Out_Timer(unsigned short row, unsigned short col, unsigned int number){
;frequencycounter.c,267 :: 		digits[0] = number % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Timer_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Timer_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Timer_digits_L0+0 
;frequencycounter.c,268 :: 		digits[1] = (number / 10) % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Timer_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Timer_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Timer_digits_L0+1 
;frequencycounter.c,269 :: 		digits[2] = (number / 100) % 10;
	MOVLW       100
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Timer_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Timer_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Timer_digits_L0+2 
;frequencycounter.c,270 :: 		digits[3] = (number / 1000) % 10;
	MOVLW       232
	MOVWF       R4 
	MOVLW       3
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Timer_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Timer_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Timer_digits_L0+3 
;frequencycounter.c,271 :: 		digits[4] = (number / 10000) % 10;
	MOVLW       16
	MOVWF       R4 
	MOVLW       39
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Timer_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Timer_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Timer_digits_L0+4 
;frequencycounter.c,273 :: 		Lcd_Chr(row, col+4, 48 + digits[0]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       4
	ADDWF       FARG_Lcd_Out_Timer_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+0, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,274 :: 		Lcd_Chr(row, col+3, 48 + digits[1]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       3
	ADDWF       FARG_Lcd_Out_Timer_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+1, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,275 :: 		Lcd_Chr(row, col+2, 48 + digits[2]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       2
	ADDWF       FARG_Lcd_Out_Timer_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+2, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,276 :: 		Lcd_Chr(row, col+1, 48 + digits[3]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Timer_col+0, 0 
	ADDLW       1
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+3, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,277 :: 		Lcd_Chr(row, col, 48 + digits[4]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Timer_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+4, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,278 :: 		}
L_end_Lcd_Out_Timer:
	RETURN      0
; end of _Lcd_Out_Timer

_Lcd_Out_Freq:

;frequencycounter.c,280 :: 		void Lcd_Out_Freq(unsigned short row, unsigned short col, unsigned int number){
;frequencycounter.c,282 :: 		digits[0] = number % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+0 
;frequencycounter.c,283 :: 		digits[1] = (number / 10) % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+1 
;frequencycounter.c,284 :: 		digits[2] = (number / 100) % 10;
	MOVLW       100
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+2 
;frequencycounter.c,285 :: 		digits[3] = (number / 1000) % 10;
	MOVLW       232
	MOVWF       R4 
	MOVLW       3
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+3 
;frequencycounter.c,286 :: 		digits[4] = (number / 10000) % 10;
	MOVLW       16
	MOVWF       R4 
	MOVLW       39
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+4 
;frequencycounter.c,288 :: 		Lcd_Chr(row, col+5, 48 + digits[0]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       5
	ADDWF       FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+0, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,289 :: 		Lcd_Chr(row, col+4, 46);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       4
	ADDWF       FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVLW       46
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,290 :: 		Lcd_Chr(row, col+3, 48 + digits[1]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       3
	ADDWF       FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+1, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,291 :: 		Lcd_Chr(row, col+2, 48 + digits[2]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       2
	ADDWF       FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+2, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,292 :: 		Lcd_Chr(row, col+1, 48 + digits[3]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Freq_col+0, 0 
	ADDLW       1
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+3, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,293 :: 		if(digits[4] == 0){
	MOVF        Lcd_Out_Freq_digits_L0+4, 0 
	XORLW       0
	BTFSS       STATUS+0, 2 
	GOTO        L_Lcd_Out_Freq10
;frequencycounter.c,294 :: 		Lcd_Chr(row, col, 32);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVLW       32
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,295 :: 		} else {
	GOTO        L_Lcd_Out_Freq11
L_Lcd_Out_Freq10:
;frequencycounter.c,296 :: 		Lcd_Chr(row, col, 48 + digits[4]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+4, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencycounter.c,297 :: 		}
L_Lcd_Out_Freq11:
;frequencycounter.c,298 :: 		}
L_end_Lcd_Out_Freq:
	RETURN      0
; end of _Lcd_Out_Freq
