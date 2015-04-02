/**
 * An assembly version of usart write that can be used outside of the interrupt
 * routine (routes around any reentrancy problem)
 **/
void ASM_USART_Write(char usartData){
/*
  asm {
    L_ASM_USART_Write_3:
    CLRF        STACK_1, 0
    BTFSC        TXSTA, 1, 0
    INCF        STACK_1, 1, 0
    MOVF        STACK_1, 0, 0
    XORLW        0
    BNZ        L_ASM_USART_Write_4
    NOP
    BRA        L_ASM_USART_Write_3
    L_ASM_USART_Write_4:
    MOVFF        FARG_ASM_USART_Write+0, TXREG
  }*/
}