#ifndef _PAGE_SONG_H
#define _PAGE_SONG_H
void PGSO_initPage();
void PGSO_showPage();
char PGSO_shouldSwitchToNextPattern();
void PGSO_loadFirstNonEmptyPattern(char pattToLoad);
void PGSO_incrementSongPosition();
char PGSO_getNextPatternAndIncrementIfNewPattern();
void PGSO_playOrLoad();
void PGSO_stopAndReset();
void PGSO_keyEventHandler(char key);
#endif