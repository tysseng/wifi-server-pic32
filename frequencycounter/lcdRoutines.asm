
_Lcd_Out_ULong:

;lcdRoutines.c,3 :: 		void Lcd_Out_ULong(unsigned short row, unsigned short col, unsigned long number){
;lcdRoutines.c,5 :: 		digits[0] = number % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+0 
;lcdRoutines.c,6 :: 		digits[1] = (number / 10) % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+1 
;lcdRoutines.c,7 :: 		digits[2] = (number / 100) % 10;
	MOVLW       100
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+2 
;lcdRoutines.c,8 :: 		digits[3] = (number / 1000) % 10;
	MOVLW       232
	MOVWF       R4 
	MOVLW       3
	MOVWF       R5 
	MOVLW       0
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+3 
;lcdRoutines.c,9 :: 		digits[4] = (number / 10000) % 10;
	MOVLW       16
	MOVWF       R4 
	MOVLW       39
	MOVWF       R5 
	MOVLW       0
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+4 
;lcdRoutines.c,10 :: 		digits[5] = (number / 100000) % 10;
	MOVLW       160
	MOVWF       R4 
	MOVLW       134
	MOVWF       R5 
	MOVLW       1
	MOVWF       R6 
	MOVLW       0
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+5 
;lcdRoutines.c,11 :: 		digits[6] = (number / 1000000) % 10;
	MOVLW       64
	MOVWF       R4 
	MOVLW       66
	MOVWF       R5 
	MOVLW       15
	MOVWF       R6 
	MOVLW       0
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+6 
;lcdRoutines.c,12 :: 		digits[7] = (number / 10000000) % 10;
	MOVLW       128
	MOVWF       R4 
	MOVLW       150
	MOVWF       R5 
	MOVLW       152
	MOVWF       R6 
	MOVLW       0
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+7 
;lcdRoutines.c,13 :: 		digits[8] = (number / 100000000) % 10;
	MOVLW       0
	MOVWF       R4 
	MOVLW       225
	MOVWF       R5 
	MOVLW       245
	MOVWF       R6 
	MOVLW       5
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+8 
;lcdRoutines.c,14 :: 		digits[9] = (number / 1000000000) % 10;
	MOVLW       0
	MOVWF       R4 
	MOVLW       202
	MOVWF       R5 
	MOVLW       154
	MOVWF       R6 
	MOVLW       59
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_ULong_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_ULong_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_ULong_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_ULong_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_ULong_digits_L0+9 
;lcdRoutines.c,16 :: 		Lcd_Chr(row, col+9, 48 + digits[0]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       9
	ADDWF       FARG_Lcd_Out_ULong_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+0, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,17 :: 		Lcd_Chr(row, col+8, 48 + digits[1]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       8
	ADDWF       FARG_Lcd_Out_ULong_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+1, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,18 :: 		Lcd_Chr(row, col+7, 48 + digits[2]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       7
	ADDWF       FARG_Lcd_Out_ULong_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+2, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,19 :: 		Lcd_Chr(row, col+6, 48 + digits[3]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       6
	ADDWF       FARG_Lcd_Out_ULong_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+3, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,20 :: 		Lcd_Chr(row, col+5, 48 + digits[4]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       5
	ADDWF       FARG_Lcd_Out_ULong_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+4, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,21 :: 		Lcd_Chr(row, col+4, 48 + digits[5]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       4
	ADDWF       FARG_Lcd_Out_ULong_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+5, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,22 :: 		Lcd_Chr(row, col+3, 48 + digits[6]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       3
	ADDWF       FARG_Lcd_Out_ULong_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+6, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,23 :: 		Lcd_Chr(row, col+2, 48 + digits[7]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       2
	ADDWF       FARG_Lcd_Out_ULong_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+7, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,24 :: 		Lcd_Chr(row, col+1, 48 + digits[8]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_ULong_col+0, 0 
	ADDLW       1
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+8, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,25 :: 		Lcd_Chr(row, col  , 48 + digits[9]);
	MOVF        FARG_Lcd_Out_ULong_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_ULong_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_ULong_digits_L0+9, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,26 :: 		}
L_end_Lcd_Out_ULong:
	RETURN      0
; end of _Lcd_Out_ULong

_Lcd_Out_UInt:

;lcdRoutines.c,28 :: 		void Lcd_Out_UInt(unsigned short row, unsigned short col, unsigned int number){
;lcdRoutines.c,30 :: 		digits[0] = number % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_UInt_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_UInt_number+1, 0 
	MOVWF       R1 
	CALL        _Div_16x16_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_UInt_digits_L0+0 
;lcdRoutines.c,31 :: 		digits[1] = (number / 10) % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_UInt_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_UInt_number+1, 0 
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
	MOVWF       Lcd_Out_UInt_digits_L0+1 
;lcdRoutines.c,32 :: 		digits[2] = (number / 100) % 10;
	MOVLW       100
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_UInt_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_UInt_number+1, 0 
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
	MOVWF       Lcd_Out_UInt_digits_L0+2 
;lcdRoutines.c,33 :: 		digits[3] = (number / 1000) % 10;
	MOVLW       232
	MOVWF       R4 
	MOVLW       3
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_UInt_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_UInt_number+1, 0 
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
	MOVWF       Lcd_Out_UInt_digits_L0+3 
;lcdRoutines.c,34 :: 		digits[4] = (number / 10000) % 10;
	MOVLW       16
	MOVWF       R4 
	MOVLW       39
	MOVWF       R5 
	MOVF        FARG_Lcd_Out_UInt_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_UInt_number+1, 0 
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
	MOVWF       Lcd_Out_UInt_digits_L0+4 
;lcdRoutines.c,36 :: 		Lcd_Chr(row, col+4, 48 + digits[0]);
	MOVF        FARG_Lcd_Out_UInt_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       4
	ADDWF       FARG_Lcd_Out_UInt_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_UInt_digits_L0+0, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,37 :: 		Lcd_Chr(row, col+3, 48 + digits[1]);
	MOVF        FARG_Lcd_Out_UInt_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       3
	ADDWF       FARG_Lcd_Out_UInt_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_UInt_digits_L0+1, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,38 :: 		Lcd_Chr(row, col+2, 48 + digits[2]);
	MOVF        FARG_Lcd_Out_UInt_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       2
	ADDWF       FARG_Lcd_Out_UInt_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_UInt_digits_L0+2, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,39 :: 		Lcd_Chr(row, col+1, 48 + digits[3]);
	MOVF        FARG_Lcd_Out_UInt_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_UInt_col+0, 0 
	ADDLW       1
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_UInt_digits_L0+3, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,40 :: 		Lcd_Chr(row, col  , 48 + digits[4]);
	MOVF        FARG_Lcd_Out_UInt_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_UInt_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_UInt_digits_L0+4, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,41 :: 		}
L_end_Lcd_Out_UInt:
	RETURN      0
; end of _Lcd_Out_UInt

_Lcd_Out_UShort:

;lcdRoutines.c,44 :: 		void Lcd_Out_UShort(unsigned short row, unsigned short col, unsigned short number){
;lcdRoutines.c,46 :: 		digits[0] = number % 10;
	MOVLW       10
	MOVWF       R4 
	MOVF        FARG_Lcd_Out_UShort_number+0, 0 
	MOVWF       R0 
	CALL        _Div_8x8_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_UShort_digits_L0+0 
;lcdRoutines.c,47 :: 		digits[1] = (number / 10) % 10;
	MOVLW       10
	MOVWF       R4 
	MOVF        FARG_Lcd_Out_UShort_number+0, 0 
	MOVWF       R0 
	CALL        _Div_8x8_U+0, 0
	MOVLW       10
	MOVWF       R4 
	CALL        _Div_8x8_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_UShort_digits_L0+1 
;lcdRoutines.c,48 :: 		digits[2] = (number / 100) % 10;
	MOVLW       100
	MOVWF       R4 
	MOVF        FARG_Lcd_Out_UShort_number+0, 0 
	MOVWF       R0 
	CALL        _Div_8x8_U+0, 0
	MOVLW       10
	MOVWF       R4 
	CALL        _Div_8x8_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_UShort_digits_L0+2 
;lcdRoutines.c,50 :: 		Lcd_Chr(row, col+2, 48 + digits[0]);
	MOVF        FARG_Lcd_Out_UShort_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       2
	ADDWF       FARG_Lcd_Out_UShort_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_UShort_digits_L0+0, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,51 :: 		Lcd_Chr(row, col+1, 48 + digits[1]);
	MOVF        FARG_Lcd_Out_UShort_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_UShort_col+0, 0 
	ADDLW       1
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_UShort_digits_L0+1, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,52 :: 		Lcd_Chr(row, col  , 48 + digits[2]);
	MOVF        FARG_Lcd_Out_UShort_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_UShort_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_UShort_digits_L0+2, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,53 :: 		}
L_end_Lcd_Out_UShort:
	RETURN      0
; end of _Lcd_Out_UShort

_Lcd_Out_Timer:

;lcdRoutines.c,58 :: 		void Lcd_Out_Timer(unsigned short row, unsigned short col, unsigned int number){
;lcdRoutines.c,60 :: 		digits[0] = number % 10;
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
;lcdRoutines.c,61 :: 		digits[1] = (number / 10) % 10;
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
;lcdRoutines.c,62 :: 		digits[2] = (number / 100) % 10;
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
;lcdRoutines.c,63 :: 		digits[3] = (number / 1000) % 10;
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
;lcdRoutines.c,64 :: 		digits[4] = (number / 10000) % 10;
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
;lcdRoutines.c,66 :: 		Lcd_Chr(row, col+4, 48 + digits[0]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       4
	ADDWF       FARG_Lcd_Out_Timer_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+0, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,67 :: 		Lcd_Chr(row, col+3, 48 + digits[1]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       3
	ADDWF       FARG_Lcd_Out_Timer_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+1, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,68 :: 		Lcd_Chr(row, col+2, 48 + digits[2]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       2
	ADDWF       FARG_Lcd_Out_Timer_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+2, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,69 :: 		Lcd_Chr(row, col+1, 48 + digits[3]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Timer_col+0, 0 
	ADDLW       1
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+3, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,70 :: 		Lcd_Chr(row, col, 48 + digits[4]);
	MOVF        FARG_Lcd_Out_Timer_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Timer_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Timer_digits_L0+4, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,71 :: 		}
L_end_Lcd_Out_Timer:
	RETURN      0
; end of _Lcd_Out_Timer

_Lcd_Out_Freq:

;lcdRoutines.c,73 :: 		void Lcd_Out_Freq(unsigned short row, unsigned short col, unsigned long number){
;lcdRoutines.c,75 :: 		digits[0] = number % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_Freq_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_Freq_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+0 
;lcdRoutines.c,76 :: 		digits[1] = (number / 10) % 10;
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_Freq_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_Freq_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+1 
;lcdRoutines.c,77 :: 		digits[2] = (number / 100) % 10;
	MOVLW       100
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_Freq_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_Freq_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+2 
;lcdRoutines.c,78 :: 		digits[3] = (number / 1000) % 10;
	MOVLW       232
	MOVWF       R4 
	MOVLW       3
	MOVWF       R5 
	MOVLW       0
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_Freq_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_Freq_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+3 
;lcdRoutines.c,79 :: 		digits[4] = (number / 10000) % 10;
	MOVLW       16
	MOVWF       R4 
	MOVLW       39
	MOVWF       R5 
	MOVLW       0
	MOVWF       R6 
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_Freq_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_Freq_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+4 
;lcdRoutines.c,80 :: 		digits[5] = (number / 100000) % 10;
	MOVLW       160
	MOVWF       R4 
	MOVLW       134
	MOVWF       R5 
	MOVLW       1
	MOVWF       R6 
	MOVLW       0
	MOVWF       R7 
	MOVF        FARG_Lcd_Out_Freq_number+0, 0 
	MOVWF       R0 
	MOVF        FARG_Lcd_Out_Freq_number+1, 0 
	MOVWF       R1 
	MOVF        FARG_Lcd_Out_Freq_number+2, 0 
	MOVWF       R2 
	MOVF        FARG_Lcd_Out_Freq_number+3, 0 
	MOVWF       R3 
	CALL        _Div_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVLW       0
	MOVWF       R5 
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Div_32x32_U+0, 0
	MOVF        R8, 0 
	MOVWF       R0 
	MOVF        R9, 0 
	MOVWF       R1 
	MOVF        R10, 0 
	MOVWF       R2 
	MOVF        R11, 0 
	MOVWF       R3 
	MOVF        R0, 0 
	MOVWF       Lcd_Out_Freq_digits_L0+5 
;lcdRoutines.c,82 :: 		Lcd_Chr(row, col+6, 48 + digits[0]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       6
	ADDWF       FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+0, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,83 :: 		Lcd_Chr(row, col+5, 48 + digits[1]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       5
	ADDWF       FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+1, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,84 :: 		Lcd_Chr(row, col+4, 46);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       4
	ADDWF       FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVLW       46
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,85 :: 		Lcd_Chr(row, col+3, 48 + digits[2]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       3
	ADDWF       FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+2, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,86 :: 		Lcd_Chr(row, col+2, 48 + digits[3]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVLW       2
	ADDWF       FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+3, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,87 :: 		Lcd_Chr(row, col+1, 48 + digits[4]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Freq_col+0, 0 
	ADDLW       1
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+4, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,88 :: 		if(digits[5] == 0){
	MOVF        Lcd_Out_Freq_digits_L0+5, 0 
	XORLW       0
	BTFSS       STATUS+0, 2 
	GOTO        L_Lcd_Out_Freq0
;lcdRoutines.c,89 :: 		Lcd_Chr(row, col, 32);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVLW       32
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,90 :: 		} else {
	GOTO        L_Lcd_Out_Freq1
L_Lcd_Out_Freq0:
;lcdRoutines.c,91 :: 		Lcd_Chr(row, col, 48 + digits[5]);
	MOVF        FARG_Lcd_Out_Freq_row+0, 0 
	MOVWF       FARG_Lcd_Chr_row+0 
	MOVF        FARG_Lcd_Out_Freq_col+0, 0 
	MOVWF       FARG_Lcd_Chr_column+0 
	MOVF        Lcd_Out_Freq_digits_L0+5, 0 
	ADDLW       48
	MOVWF       FARG_Lcd_Chr_out_char+0 
	CALL        _Lcd_Chr+0, 0
;lcdRoutines.c,92 :: 		}
L_Lcd_Out_Freq1:
;lcdRoutines.c,93 :: 		}
L_end_Lcd_Out_Freq:
	RETURN      0
; end of _Lcd_Out_Freq
