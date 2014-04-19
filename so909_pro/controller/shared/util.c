char UTIL_getFolderNum(char patternNum){
  return patternNum >> 4;
}

char UTIL_getFileNum(char patternNum){
  return patternNum & 0x0F;
}

char UTIL_getPatternNum(char folderNum, char fileNum){
  return fileNum | (folderNum << 4);
}

int UTIL_getPrintablePatternNum(char patternNum){
  return (patternNum >> 4) * 100 + (patternNum & 0x0F);
}

int UTIL_getPrintablePatternNumFromFolderFile(char folderNum, char fileNum){
  return folderNum * 100 + fileNum;
}

char UTIL_getPatternNumFromPrintable(int printablePatternNum){
  char folderNum, fileNum;
  folderNum = printablePatternNum / 100;
  fileNum = printablePatternNum - folderNum * 100;
  return UTIL_getPatternNum(folderNum, fileNum);
}