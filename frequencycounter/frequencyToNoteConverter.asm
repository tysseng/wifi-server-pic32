
_initPitchArray:

;frequencyToNoteConverter.c,8 :: 		void initPitchArray(){
;frequencyToNoteConverter.c,11 :: 		short divideBy = 2;
	MOVLW       2
	MOVWF       initPitchArray_divideBy_L0+0 
;frequencyToNoteConverter.c,13 :: 		pitches[96]  = 418601; //C8
	MOVLW       41
	MOVWF       _pitches+384 
	MOVLW       99
	MOVWF       _pitches+385 
	MOVLW       6
	MOVWF       _pitches+386 
	MOVLW       0
	MOVWF       _pitches+387 
;frequencyToNoteConverter.c,14 :: 		pitches[97]  = 443492;
	MOVLW       100
	MOVWF       _pitches+388 
	MOVLW       196
	MOVWF       _pitches+389 
	MOVLW       6
	MOVWF       _pitches+390 
	MOVLW       0
	MOVWF       _pitches+391 
;frequencyToNoteConverter.c,15 :: 		pitches[98]  = 469864;
	MOVLW       104
	MOVWF       _pitches+392 
	MOVLW       43
	MOVWF       _pitches+393 
	MOVLW       7
	MOVWF       _pitches+394 
	MOVLW       0
	MOVWF       _pitches+395 
;frequencyToNoteConverter.c,16 :: 		pitches[99]  = 497803;
	MOVLW       139
	MOVWF       _pitches+396 
	MOVLW       152
	MOVWF       _pitches+397 
	MOVLW       7
	MOVWF       _pitches+398 
	MOVLW       0
	MOVWF       _pitches+399 
;frequencyToNoteConverter.c,17 :: 		pitches[100]  = 527404;
	MOVLW       44
	MOVWF       _pitches+400 
	MOVLW       12
	MOVWF       _pitches+401 
	MOVLW       8
	MOVWF       _pitches+402 
	MOVLW       0
	MOVWF       _pitches+403 
;frequencyToNoteConverter.c,18 :: 		pitches[101]  = 558765;
	MOVLW       173
	MOVWF       _pitches+404 
	MOVLW       134
	MOVWF       _pitches+405 
	MOVLW       8
	MOVWF       _pitches+406 
	MOVLW       0
	MOVWF       _pitches+407 
;frequencyToNoteConverter.c,19 :: 		pitches[102]  = 591991;
	MOVLW       119
	MOVWF       _pitches+408 
	MOVLW       8
	MOVWF       _pitches+409 
	MOVLW       9
	MOVWF       _pitches+410 
	MOVLW       0
	MOVWF       _pitches+411 
;frequencyToNoteConverter.c,20 :: 		pitches[103]  = 627193;
	MOVLW       249
	MOVWF       _pitches+412 
	MOVLW       145
	MOVWF       _pitches+413 
	MOVLW       9
	MOVWF       _pitches+414 
	MOVLW       0
	MOVWF       _pitches+415 
;frequencyToNoteConverter.c,21 :: 		pitches[104]  = 664488;
	MOVLW       168
	MOVWF       _pitches+416 
	MOVLW       35
	MOVWF       _pitches+417 
	MOVLW       10
	MOVWF       _pitches+418 
	MOVLW       0
	MOVWF       _pitches+419 
;frequencyToNoteConverter.c,22 :: 		pitches[105]  = 704000;
	MOVLW       0
	MOVWF       _pitches+420 
	MOVLW       190
	MOVWF       _pitches+421 
	MOVLW       10
	MOVWF       _pitches+422 
	MOVLW       0
	MOVWF       _pitches+423 
;frequencyToNoteConverter.c,23 :: 		pitches[106]  = 745862;
	MOVLW       134
	MOVWF       _pitches+424 
	MOVLW       97
	MOVWF       _pitches+425 
	MOVLW       11
	MOVWF       _pitches+426 
	MOVLW       0
	MOVWF       _pitches+427 
;frequencyToNoteConverter.c,24 :: 		pitches[107]  = 790213; //B8
	MOVLW       197
	MOVWF       _pitches+428 
	MOVLW       14
	MOVWF       _pitches+429 
	MOVLW       12
	MOVWF       _pitches+430 
	MOVLW       0
	MOVWF       _pitches+431 
;frequencyToNoteConverter.c,26 :: 		for(octave=7; octave>-1; octave--){
	MOVLW       7
	MOVWF       initPitchArray_octave_L0+0 
L_initPitchArray0:
	MOVLW       128
	XORLW       255
	MOVWF       R0 
	MOVLW       128
	XORWF       initPitchArray_octave_L0+0, 0 
	SUBWF       R0, 0 
	BTFSC       STATUS+0, 0 
	GOTO        L_initPitchArray1
;frequencyToNoteConverter.c,27 :: 		for(semitone=0; semitone<12; semitone++){
	CLRF        initPitchArray_semitone_L0+0 
L_initPitchArray3:
	MOVLW       128
	XORWF       initPitchArray_semitone_L0+0, 0 
	MOVWF       R0 
	MOVLW       128
	XORLW       12
	SUBWF       R0, 0 
	BTFSC       STATUS+0, 0 
	GOTO        L_initPitchArray4
;frequencyToNoteConverter.c,28 :: 		pitches[octave * 12 + semitone] = pitches[(octave+1) * 12 + semitone] / divideBy;
	MOVF        initPitchArray_octave_L0+0, 0 
	MOVWF       R0 
	MOVLW       12
	MOVWF       R4 
	CALL        _Mul_8x8_S+0, 0
	MOVF        initPitchArray_semitone_L0+0, 0 
	ADDWF       R0, 0 
	MOVWF       R3 
	MOVLW       0
	BTFSC       initPitchArray_semitone_L0+0, 7 
	MOVLW       255
	ADDWFC      R1, 0 
	MOVWF       R4 
	MOVF        R3, 0 
	MOVWF       R0 
	MOVF        R4, 0 
	MOVWF       R1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	MOVLW       _pitches+0
	ADDWF       R0, 0 
	MOVWF       FLOC__initPitchArray+0 
	MOVLW       hi_addr(_pitches+0)
	ADDWFC      R1, 0 
	MOVWF       FLOC__initPitchArray+1 
	MOVF        initPitchArray_octave_L0+0, 0 
	ADDLW       1
	MOVWF       R0 
	CLRF        R1 
	MOVLW       0
	BTFSC       initPitchArray_octave_L0+0, 7 
	MOVLW       255
	ADDWFC      R1, 1 
	MOVLW       12
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	CALL        _Mul_16x16_U+0, 0
	MOVF        initPitchArray_semitone_L0+0, 0 
	ADDWF       R0, 0 
	MOVWF       R3 
	MOVLW       0
	BTFSC       initPitchArray_semitone_L0+0, 7 
	MOVLW       255
	ADDWFC      R1, 0 
	MOVWF       R4 
	MOVF        R3, 0 
	MOVWF       R0 
	MOVF        R4, 0 
	MOVWF       R1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	MOVLW       _pitches+0
	ADDWF       R0, 0 
	MOVWF       FSR0 
	MOVLW       hi_addr(_pitches+0)
	ADDWFC      R1, 0 
	MOVWF       FSR0H 
	MOVF        POSTINC0+0, 0 
	MOVWF       R0 
	MOVF        POSTINC0+0, 0 
	MOVWF       R1 
	MOVF        POSTINC0+0, 0 
	MOVWF       R2 
	MOVF        POSTINC0+0, 0 
	MOVWF       R3 
	MOVF        initPitchArray_divideBy_L0+0, 0 
	MOVWF       R4 
	MOVLW       0
	BTFSC       initPitchArray_divideBy_L0+0, 7 
	MOVLW       255
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVFF       FLOC__initPitchArray+0, FSR1
	MOVFF       FLOC__initPitchArray+1, FSR1H
	MOVF        R0, 0 
	MOVWF       POSTINC1+0 
	MOVF        R1, 0 
	MOVWF       POSTINC1+0 
	MOVF        R2, 0 
	MOVWF       POSTINC1+0 
	MOVF        R3, 0 
	MOVWF       POSTINC1+0 
;frequencyToNoteConverter.c,27 :: 		for(semitone=0; semitone<12; semitone++){
	INCF        initPitchArray_semitone_L0+0, 1 
;frequencyToNoteConverter.c,29 :: 		}
	GOTO        L_initPitchArray3
L_initPitchArray4:
;frequencyToNoteConverter.c,26 :: 		for(octave=7; octave>-1; octave--){
	DECF        initPitchArray_octave_L0+0, 1 
;frequencyToNoteConverter.c,31 :: 		}
	GOTO        L_initPitchArray0
L_initPitchArray1:
;frequencyToNoteConverter.c,32 :: 		}
L_end_initPitchArray:
	RETURN      0
; end of _initPitchArray

_printNote:

;frequencyToNoteConverter.c,34 :: 		void printNote(unsigned int frequency, int factor){
;frequencyToNoteConverter.c,36 :: 		unsigned long frequency100 = (factor == 10 ? 10L * frequency  : 1L * frequency);
	MOVLW       0
	XORWF       FARG_printNote_factor+1, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__printNote44
	MOVLW       10
	XORWF       FARG_printNote_factor+0, 0 
L__printNote44:
	BTFSS       STATUS+0, 2 
	GOTO        L_printNote6
	MOVLW       10
	MOVWF       R0 
	MOVLW       0
	MOVWF       R1 
	MOVLW       0
	MOVWF       R2 
	MOVLW       0
	MOVWF       R3 
	MOVF        FARG_printNote_frequency+0, 0 
	MOVWF       R4 
	MOVF        FARG_printNote_frequency+1, 0 
	MOVWF       R5 
	MOVLW       0
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Mul_32x32_U+0, 0
	MOVF        R0, 0 
	MOVWF       R4 
	MOVF        R1, 0 
	MOVWF       R5 
	MOVF        R2, 0 
	MOVWF       R6 
	MOVF        R3, 0 
	MOVWF       R7 
	GOTO        L_printNote7
L_printNote6:
	MOVF        FARG_printNote_frequency+0, 0 
	MOVWF       R0 
	MOVF        FARG_printNote_frequency+1, 0 
	MOVWF       R1 
	MOVLW       0
	MOVWF       R2 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       R4 
	MOVF        R1, 0 
	MOVWF       R5 
	MOVF        R2, 0 
	MOVWF       R6 
	MOVF        R3, 0 
	MOVWF       R7 
L_printNote7:
	MOVF        R4, 0 
	MOVWF       printNote_frequency100_L0+0 
	MOVF        R5, 0 
	MOVWF       printNote_frequency100_L0+1 
	MOVF        R6, 0 
	MOVWF       printNote_frequency100_L0+2 
	MOVF        R7, 0 
	MOVWF       printNote_frequency100_L0+3 
;frequencyToNoteConverter.c,42 :: 		unsigned short lowerBound = 0;
	CLRF        printNote_lowerBound_L0+0 
	CLRF        printNote_upperBound_L0+0 
;frequencyToNoteConverter.c,51 :: 		for(semitone = 0; semitone < 108; semitone++){
	CLRF        printNote_semitone_L0+0 
L_printNote8:
	MOVLW       108
	SUBWF       printNote_semitone_L0+0, 0 
	BTFSC       STATUS+0, 0 
	GOTO        L_printNote9
;frequencyToNoteConverter.c,52 :: 		if(frequency100 > pitches[semitone]){
	MOVF        printNote_semitone_L0+0, 0 
	MOVWF       R0 
	MOVLW       0
	MOVWF       R1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	MOVLW       _pitches+0
	ADDWF       R0, 0 
	MOVWF       FSR2 
	MOVLW       hi_addr(_pitches+0)
	ADDWFC      R1, 0 
	MOVWF       FSR2H 
	MOVF        POSTINC2+0, 0 
	MOVWF       R1 
	MOVF        POSTINC2+0, 0 
	MOVWF       R2 
	MOVF        POSTINC2+0, 0 
	MOVWF       R3 
	MOVF        POSTINC2+0, 0 
	MOVWF       R4 
	MOVF        printNote_frequency100_L0+3, 0 
	SUBWF       R4, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__printNote45
	MOVF        printNote_frequency100_L0+2, 0 
	SUBWF       R3, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__printNote45
	MOVF        printNote_frequency100_L0+1, 0 
	SUBWF       R2, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__printNote45
	MOVF        printNote_frequency100_L0+0, 0 
	SUBWF       R1, 0 
L__printNote45:
	BTFSC       STATUS+0, 0 
	GOTO        L_printNote11
;frequencyToNoteConverter.c,53 :: 		lowerBound = semitone;
	MOVF        printNote_semitone_L0+0, 0 
	MOVWF       printNote_lowerBound_L0+0 
;frequencyToNoteConverter.c,54 :: 		upperBound = semitone+1;
	MOVF        printNote_semitone_L0+0, 0 
	ADDLW       1
	MOVWF       printNote_upperBound_L0+0 
;frequencyToNoteConverter.c,55 :: 		} else {
	GOTO        L_printNote12
L_printNote11:
;frequencyToNoteConverter.c,56 :: 		break;
	GOTO        L_printNote9
;frequencyToNoteConverter.c,57 :: 		}
L_printNote12:
;frequencyToNoteConverter.c,51 :: 		for(semitone = 0; semitone < 108; semitone++){
	INCF        printNote_semitone_L0+0, 1 
;frequencyToNoteConverter.c,58 :: 		}
	GOTO        L_printNote8
L_printNote9:
;frequencyToNoteConverter.c,60 :: 		if(frequency100 - pitches[lowerBound] > pitches[upperBound] - frequency100){
	MOVF        printNote_lowerBound_L0+0, 0 
	MOVWF       R0 
	MOVLW       0
	MOVWF       R1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	MOVLW       _pitches+0
	ADDWF       R0, 0 
	MOVWF       FSR2 
	MOVLW       hi_addr(_pitches+0)
	ADDWFC      R1, 0 
	MOVWF       FSR2H 
	MOVF        printNote_frequency100_L0+0, 0 
	MOVWF       R5 
	MOVF        printNote_frequency100_L0+1, 0 
	MOVWF       R6 
	MOVF        printNote_frequency100_L0+2, 0 
	MOVWF       R7 
	MOVF        printNote_frequency100_L0+3, 0 
	MOVWF       R8 
	MOVF        POSTINC2+0, 0 
	SUBWF       R5, 1 
	MOVF        POSTINC2+0, 0 
	SUBWFB      R6, 1 
	MOVF        POSTINC2+0, 0 
	SUBWFB      R7, 1 
	MOVF        POSTINC2+0, 0 
	SUBWFB      R8, 1 
	MOVF        printNote_upperBound_L0+0, 0 
	MOVWF       R0 
	MOVLW       0
	MOVWF       R1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	MOVLW       _pitches+0
	ADDWF       R0, 0 
	MOVWF       FSR0 
	MOVLW       hi_addr(_pitches+0)
	ADDWFC      R1, 0 
	MOVWF       FSR0H 
	MOVF        POSTINC0+0, 0 
	MOVWF       R1 
	MOVF        POSTINC0+0, 0 
	MOVWF       R2 
	MOVF        POSTINC0+0, 0 
	MOVWF       R3 
	MOVF        POSTINC0+0, 0 
	MOVWF       R4 
	MOVF        printNote_frequency100_L0+0, 0 
	SUBWF       R1, 1 
	MOVF        printNote_frequency100_L0+1, 0 
	SUBWFB      R2, 1 
	MOVF        printNote_frequency100_L0+2, 0 
	SUBWFB      R3, 1 
	MOVF        printNote_frequency100_L0+3, 0 
	SUBWFB      R4, 1 
	MOVF        R8, 0 
	SUBWF       R4, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__printNote46
	MOVF        R7, 0 
	SUBWF       R3, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__printNote46
	MOVF        R6, 0 
	SUBWF       R2, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__printNote46
	MOVF        R5, 0 
	SUBWF       R1, 0 
L__printNote46:
	BTFSC       STATUS+0, 0 
	GOTO        L_printNote13
;frequencyToNoteConverter.c,61 :: 		closestMatch = upperBound;
	MOVF        printNote_upperBound_L0+0, 0 
	MOVWF       printNote_closestMatch_L0+0 
;frequencyToNoteConverter.c,62 :: 		} else {
	GOTO        L_printNote14
L_printNote13:
;frequencyToNoteConverter.c,63 :: 		closestMatch = lowerBound;
	MOVF        printNote_lowerBound_L0+0, 0 
	MOVWF       printNote_closestMatch_L0+0 
;frequencyToNoteConverter.c,64 :: 		}
L_printNote14:
;frequencyToNoteConverter.c,66 :: 		Delay_5ms();
	CALL        _Delay_5ms+0, 0
;frequencyToNoteConverter.c,67 :: 		Lcd_Out_Freq(2, 1, frequency100);
	MOVLW       2
	MOVWF       FARG_Lcd_Out_Freq_row+0 
	MOVLW       1
	MOVWF       FARG_Lcd_Out_Freq_col+0 
	MOVF        printNote_frequency100_L0+0, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+0 
	MOVF        printNote_frequency100_L0+1, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+1 
	MOVF        printNote_frequency100_L0+2, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+2 
	MOVF        printNote_frequency100_L0+3, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+3 
	CALL        _Lcd_Out_Freq+0, 0
;frequencyToNoteConverter.c,69 :: 		Delay_5ms();
	CALL        _Delay_5ms+0, 0
;frequencyToNoteConverter.c,70 :: 		Lcd_Out_Freq(2, 9, pitches[closestMatch]);
	MOVLW       2
	MOVWF       FARG_Lcd_Out_Freq_row+0 
	MOVLW       9
	MOVWF       FARG_Lcd_Out_Freq_col+0 
	MOVF        printNote_closestMatch_L0+0, 0 
	MOVWF       R0 
	MOVLW       0
	MOVWF       R1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	MOVLW       _pitches+0
	ADDWF       R0, 0 
	MOVWF       FSR0 
	MOVLW       hi_addr(_pitches+0)
	ADDWFC      R1, 0 
	MOVWF       FSR0H 
	MOVF        POSTINC0+0, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+0 
	MOVF        POSTINC0+0, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+1 
	MOVF        POSTINC0+0, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+2 
	MOVF        POSTINC0+0, 0 
	MOVWF       FARG_Lcd_Out_Freq_number+3 
	CALL        _Lcd_Out_Freq+0, 0
;frequencyToNoteConverter.c,74 :: 		mismatch = 10000L * (frequency100 - pitches[closestMatch]) / pitches[closestMatch];
	MOVF        printNote_closestMatch_L0+0, 0 
	MOVWF       R0 
	MOVLW       0
	MOVWF       R1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	RLCF        R0, 1 
	BCF         R0, 0 
	RLCF        R1, 1 
	MOVLW       _pitches+0
	ADDWF       R0, 0 
	MOVWF       FSR2 
	MOVLW       hi_addr(_pitches+0)
	ADDWFC      R1, 0 
	MOVWF       FSR2H 
	MOVF        POSTINC2+0, 0 
	MOVWF       FLOC__printNote+0 
	MOVF        POSTINC2+0, 0 
	MOVWF       FLOC__printNote+1 
	MOVF        POSTINC2+0, 0 
	MOVWF       FLOC__printNote+2 
	MOVF        POSTINC2+0, 0 
	MOVWF       FLOC__printNote+3 
	MOVF        printNote_frequency100_L0+0, 0 
	MOVWF       R0 
	MOVF        printNote_frequency100_L0+1, 0 
	MOVWF       R1 
	MOVF        printNote_frequency100_L0+2, 0 
	MOVWF       R2 
	MOVF        printNote_frequency100_L0+3, 0 
	MOVWF       R3 
	MOVF        FLOC__printNote+0, 0 
	SUBWF       R0, 1 
	MOVF        FLOC__printNote+1, 0 
	SUBWFB      R1, 1 
	MOVF        FLOC__printNote+2, 0 
	SUBWFB      R2, 1 
	MOVF        FLOC__printNote+3, 0 
	SUBWFB      R3, 1 
	MOVLW       16
	MOVWF       R4 
	MOVLW       39
	MOVWF       R5 
	MOVLW       0
	MOVWF       R6 
	MOVLW       0
	MOVWF       R7 
	CALL        _Mul_32x32_U+0, 0
	MOVF        FLOC__printNote+0, 0 
	MOVWF       R4 
	MOVF        FLOC__printNote+1, 0 
	MOVWF       R5 
	MOVF        FLOC__printNote+2, 0 
	MOVWF       R6 
	MOVF        FLOC__printNote+3, 0 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R0, 0 
	MOVWF       printNote_mismatch_L0+0 
	MOVF        R1, 0 
	MOVWF       printNote_mismatch_L0+1 
;frequencyToNoteConverter.c,76 :: 		octave = getOctave(closestMatch);
	MOVF        printNote_closestMatch_L0+0, 0 
	MOVWF       FARG_getOctave_semitone+0 
	CALL        _getOctave+0, 0
	MOVF        R0, 0 
	MOVWF       printNote_octave_L0+0 
;frequencyToNoteConverter.c,77 :: 		semitoneInOctave = getSemitoneInOctave(closestMatch);
	MOVF        printNote_closestMatch_L0+0, 0 
	MOVWF       FARG_getSemitoneInOctave_semitone+0 
	CALL        _getSemitoneInOctave+0, 0
;frequencyToNoteConverter.c,78 :: 		lookupAndPrintTone(octave, semitoneInOctave, mismatch);
	MOVF        printNote_octave_L0+0, 0 
	MOVWF       FARG_lookupAndPrintTone_octave+0 
	MOVF        R0, 0 
	MOVWF       FARG_lookupAndPrintTone_semitone+0 
	MOVF        printNote_mismatch_L0+0, 0 
	MOVWF       FARG_lookupAndPrintTone_mismatch+0 
	MOVF        printNote_mismatch_L0+1, 0 
	MOVWF       FARG_lookupAndPrintTone_mismatch+1 
	CALL        _lookupAndPrintTone+0, 0
;frequencyToNoteConverter.c,79 :: 		}
L_end_printNote:
	RETURN      0
; end of _printNote

_getOctave:

;frequencyToNoteConverter.c,81 :: 		unsigned short getOctave(unsigned short semitone){
;frequencyToNoteConverter.c,82 :: 		return semitone / 12;
	MOVLW       12
	MOVWF       R4 
	MOVF        FARG_getOctave_semitone+0, 0 
	MOVWF       R0 
	CALL        _Div_8x8_U+0, 0
;frequencyToNoteConverter.c,83 :: 		}
L_end_getOctave:
	RETURN      0
; end of _getOctave

_getSemitoneInOctave:

;frequencyToNoteConverter.c,85 :: 		unsigned short getSemitoneInOctave(unsigned short semitone){
;frequencyToNoteConverter.c,86 :: 		return semitone % 12;
	MOVLW       12
	MOVWF       R4 
	MOVF        FARG_getSemitoneInOctave_semitone+0, 0 
	MOVWF       R0 
	CALL        _Div_8x8_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
;frequencyToNoteConverter.c,87 :: 		}
L_end_getSemitoneInOctave:
	RETURN      0
; end of _getSemitoneInOctave

_lookupAndPrintTone:

;frequencyToNoteConverter.c,89 :: 		void lookupAndPrintTone(unsigned short octave, unsigned short semitone, int mismatch){
;frequencyToNoteConverter.c,90 :: 		switch (semitone){
	GOTO        L_lookupAndPrintTone15
;frequencyToNoteConverter.c,91 :: 		case 0:
L_lookupAndPrintTone17:
;frequencyToNoteConverter.c,92 :: 		printTone("-C  ", octave);
	MOVLW       ?lstr1_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr1_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,93 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,94 :: 		case 1:
L_lookupAndPrintTone18:
;frequencyToNoteConverter.c,95 :: 		printTone("-C# ", octave);
	MOVLW       ?lstr2_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr2_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,96 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,97 :: 		case 2:
L_lookupAndPrintTone19:
;frequencyToNoteConverter.c,98 :: 		printTone("-D  ", octave);
	MOVLW       ?lstr3_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr3_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,99 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,100 :: 		case 3:
L_lookupAndPrintTone20:
;frequencyToNoteConverter.c,101 :: 		printTone("-D# ", octave);
	MOVLW       ?lstr4_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr4_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,102 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,103 :: 		case 4:
L_lookupAndPrintTone21:
;frequencyToNoteConverter.c,104 :: 		printTone("-E  ", octave);
	MOVLW       ?lstr5_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr5_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,105 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,106 :: 		case 5:
L_lookupAndPrintTone22:
;frequencyToNoteConverter.c,107 :: 		printTone("-F  ", octave);
	MOVLW       ?lstr6_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr6_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,108 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,109 :: 		case 6:
L_lookupAndPrintTone23:
;frequencyToNoteConverter.c,110 :: 		printTone("-F# ", octave);
	MOVLW       ?lstr7_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr7_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,111 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,112 :: 		case 7:
L_lookupAndPrintTone24:
;frequencyToNoteConverter.c,113 :: 		printTone("-G  ", octave);
	MOVLW       ?lstr8_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr8_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,114 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,115 :: 		case 8:
L_lookupAndPrintTone25:
;frequencyToNoteConverter.c,116 :: 		printTone("-G# ", octave);
	MOVLW       ?lstr9_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr9_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,117 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,118 :: 		case 9:
L_lookupAndPrintTone26:
;frequencyToNoteConverter.c,119 :: 		printTone("-A  ", octave);
	MOVLW       ?lstr10_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr10_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,120 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,121 :: 		case 10:
L_lookupAndPrintTone27:
;frequencyToNoteConverter.c,122 :: 		printTone("-A# ", octave);
	MOVLW       ?lstr11_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr11_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,123 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,124 :: 		case 11:
L_lookupAndPrintTone28:
;frequencyToNoteConverter.c,125 :: 		printTone("-B ", octave);
	MOVLW       ?lstr12_frequencyToNoteConverter+0
	MOVWF       FARG_printTone_note+0 
	MOVLW       hi_addr(?lstr12_frequencyToNoteConverter+0)
	MOVWF       FARG_printTone_note+1 
	MOVF        FARG_lookupAndPrintTone_octave+0, 0 
	MOVWF       FARG_printTone_octave+0 
	CALL        _printTone+0, 0
;frequencyToNoteConverter.c,126 :: 		break;
	GOTO        L_lookupAndPrintTone16
;frequencyToNoteConverter.c,127 :: 		}
L_lookupAndPrintTone15:
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       0
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone17
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       1
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone18
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       2
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone19
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       3
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone20
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       4
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone21
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       5
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone22
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       6
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone23
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       7
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone24
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       8
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone25
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       9
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone26
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       10
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone27
	MOVF        FARG_lookupAndPrintTone_semitone+0, 0 
	XORLW       11
	BTFSC       STATUS+0, 2 
	GOTO        L_lookupAndPrintTone28
L_lookupAndPrintTone16:
;frequencyToNoteConverter.c,129 :: 		}
L_end_lookupAndPrintTone:
	RETURN      0
; end of _lookupAndPrintTone

_printTone:

;frequencyToNoteConverter.c,131 :: 		void printTone(char* note, unsigned short octave){
;frequencyToNoteConverter.c,134 :: 		Lcd_Out(1,8,note);
	MOVLW       1
	MOVWF       FARG_Lcd_Out_row+0 
	MOVLW       8
	MOVWF       FARG_Lcd_Out_column+0 
	MOVF        FARG_printTone_note+0, 0 
	MOVWF       FARG_Lcd_Out_text+0 
	MOVF        FARG_printTone_note+1, 0 
	MOVWF       FARG_Lcd_Out_text+1 
	CALL        _Lcd_Out+0, 0
;frequencyToNoteConverter.c,135 :: 		Lcd_Chr(1, 7, 48 + octave);
	MOVLW       1
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       7
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        FARG_printTone_octave+0, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencyToNoteConverter.c,136 :: 		}
L_end_printTone:
	RETURN      0
; end of _printTone

_printMismatch:

;frequencyToNoteConverter.c,138 :: 		void printMismatch(int mismatch){
;frequencyToNoteConverter.c,143 :: 		if(mismatch > 0){
	MOVLW       128
	MOVWF       R0 
	MOVLW       128
	XORWF       FARG_printMismatch_mismatch+1, 0 
	SUBWF       R0, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__printMismatch52
	MOVF        FARG_printMismatch_mismatch+0, 0 
	SUBLW       0
L__printMismatch52:
	BTFSC       STATUS+0, 0 
	GOTO        L_printMismatch29
;frequencyToNoteConverter.c,144 :: 		symbol = '+';
	MOVLW       43
	MOVWF       printMismatch_symbol_L0+0 
;frequencyToNoteConverter.c,145 :: 		numberOfSymbols = getNumberOfSymbols(mismatch);
	MOVF        FARG_printMismatch_mismatch+0, 0 
	MOVWF       FARG_getNumberOfSymbols_mismatch+0 
	MOVF        FARG_printMismatch_mismatch+1, 0 
	MOVWF       FARG_getNumberOfSymbols_mismatch+1 
	CALL        _getNumberOfSymbols+0, 0
	MOVF        R0, 0 
	MOVWF       printMismatch_numberOfSymbols_L0+0 
;frequencyToNoteConverter.c,146 :: 		} else {
	GOTO        L_printMismatch30
L_printMismatch29:
;frequencyToNoteConverter.c,147 :: 		symbol = '-';
	MOVLW       45
	MOVWF       printMismatch_symbol_L0+0 
;frequencyToNoteConverter.c,148 :: 		numberOfSymbols = getNumberOfSymbols(-1 * mismatch);
	MOVLW       255
	MOVWF       R0 
	MOVLW       255
	MOVWF       R1 
	MOVF        FARG_printMismatch_mismatch+0, 0 
	MOVWF       R4 
	MOVF        FARG_printMismatch_mismatch+1, 0 
	MOVWF       R5 
	CALL        _Mul_16x16_U+0, 0
	MOVF        R0, 0 
	MOVWF       FARG_getNumberOfSymbols_mismatch+0 
	MOVF        R1, 0 
	MOVWF       FARG_getNumberOfSymbols_mismatch+1 
	CALL        _getNumberOfSymbols+0, 0
	MOVF        R0, 0 
	MOVWF       printMismatch_numberOfSymbols_L0+0 
;frequencyToNoteConverter.c,149 :: 		}
L_printMismatch30:
;frequencyToNoteConverter.c,151 :: 		for(i = 0; i< numberOfSymbols; i++){
	CLRF        printMismatch_i_L0+0 
L_printMismatch31:
	MOVF        printMismatch_numberOfSymbols_L0+0, 0 
	SUBWF       printMismatch_i_L0+0, 0 
	BTFSC       STATUS+0, 0 
	GOTO        L_printMismatch32
;frequencyToNoteConverter.c,152 :: 		Lcd_Chr(1, 12 + i, symbol);
	MOVLW       1
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        printMismatch_i_L0+0, 0 
	ADDLW       12
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        printMismatch_symbol_L0+0, 0 
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencyToNoteConverter.c,151 :: 		for(i = 0; i< numberOfSymbols; i++){
	INCF        printMismatch_i_L0+0, 1 
;frequencyToNoteConverter.c,153 :: 		}
	GOTO        L_printMismatch31
L_printMismatch32:
;frequencyToNoteConverter.c,154 :: 		for(i; i<3; i++){
L_printMismatch34:
	MOVLW       3
	SUBWF       printMismatch_i_L0+0, 0 
	BTFSC       STATUS+0, 0 
	GOTO        L_printMismatch35
;frequencyToNoteConverter.c,155 :: 		Lcd_Chr(1, 12 + i, ' ');
	MOVLW       1
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        printMismatch_i_L0+0, 0 
	ADDLW       12
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVLW       32
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;frequencyToNoteConverter.c,154 :: 		for(i; i<3; i++){
	INCF        printMismatch_i_L0+0, 1 
;frequencyToNoteConverter.c,156 :: 		}
	GOTO        L_printMismatch34
L_printMismatch35:
;frequencyToNoteConverter.c,157 :: 		}
L_end_printMismatch:
	RETURN      0
; end of _printMismatch

_getNumberOfSymbols:

;frequencyToNoteConverter.c,159 :: 		unsigned short getNumberOfSymbols(int mismatch){
;frequencyToNoteConverter.c,161 :: 		if(mismatch >200){
	MOVLW       128
	MOVWF       R0 
	MOVLW       128
	XORWF       FARG_getNumberOfSymbols_mismatch+1, 0 
	SUBWF       R0, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__getNumberOfSymbols54
	MOVF        FARG_getNumberOfSymbols_mismatch+0, 0 
	SUBLW       200
L__getNumberOfSymbols54:
	BTFSC       STATUS+0, 0 
	GOTO        L_getNumberOfSymbols37
;frequencyToNoteConverter.c,162 :: 		return 3;
	MOVLW       3
	MOVWF       R0 
	GOTO        L_end_getNumberOfSymbols
;frequencyToNoteConverter.c,163 :: 		} else if(mismatch > 100) {
L_getNumberOfSymbols37:
	MOVLW       128
	MOVWF       R0 
	MOVLW       128
	XORWF       FARG_getNumberOfSymbols_mismatch+1, 0 
	SUBWF       R0, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__getNumberOfSymbols55
	MOVF        FARG_getNumberOfSymbols_mismatch+0, 0 
	SUBLW       100
L__getNumberOfSymbols55:
	BTFSC       STATUS+0, 0 
	GOTO        L_getNumberOfSymbols39
;frequencyToNoteConverter.c,164 :: 		return 2;
	MOVLW       2
	MOVWF       R0 
	GOTO        L_end_getNumberOfSymbols
;frequencyToNoteConverter.c,165 :: 		} else if(mismatch > 50) {
L_getNumberOfSymbols39:
	MOVLW       128
	MOVWF       R0 
	MOVLW       128
	XORWF       FARG_getNumberOfSymbols_mismatch+1, 0 
	SUBWF       R0, 0 
	BTFSS       STATUS+0, 2 
	GOTO        L__getNumberOfSymbols56
	MOVF        FARG_getNumberOfSymbols_mismatch+0, 0 
	SUBLW       50
L__getNumberOfSymbols56:
	BTFSC       STATUS+0, 0 
	GOTO        L_getNumberOfSymbols41
;frequencyToNoteConverter.c,166 :: 		return 1;
	MOVLW       1
	MOVWF       R0 
	GOTO        L_end_getNumberOfSymbols
;frequencyToNoteConverter.c,167 :: 		}
L_getNumberOfSymbols41:
;frequencyToNoteConverter.c,168 :: 		return 0;
	CLRF        R0 
;frequencyToNoteConverter.c,170 :: 		}
L_end_getNumberOfSymbols:
	RETURN      0
; end of _getNumberOfSymbols
