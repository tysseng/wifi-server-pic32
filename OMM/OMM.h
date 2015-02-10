

void printSignedShort(unsigned short row, unsigned short col, short in){
  unsigned short inExpanded;
  unsigned short rest;
  if(in < 0){
    Lcd_Chr(row, col, '-');
    inExpanded = -in;
  } else {
    Lcd_Chr(row, col, ' ');
    inExpanded = in;
  }

  rest = inExpanded % 10;
  Lcd_Chr(row, col+3, 48 + rest);

  rest = (inExpanded / 10) % 10;
  Lcd_Chr(row, col+2, 48 + rest);

  rest = (inExpanded / 100) % 10;
  Lcd_Chr(row, col+1, 48 + rest);
}

