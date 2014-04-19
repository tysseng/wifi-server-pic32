#define CHAR_FIRST_ADDRESS 0x80
#define CHAR_LAST_ADDRESS 0xFF
#define MEMMAP_ADDR_CHARACTER_CHIP 0xA2
#define MEMMAP_POS_CHAR 0

//possible track names
#define TIDpattern   0
#define TIDgeneral   1
#define TIDblank     2
#define TIDbass      3
#define TIDsnare     4
#define TIDlo        5
#define TIDmid       6
#define TIDhi        7
#define TIDrimshot   8
#define TIDhandclap  9
#define TIDhhclosed 10
#define TIDhhopen   11
#define TIDcrash    12
#define TIDride     13
#define TIDtrack1   14
#define TIDtrack2   15
#define TIDtrack3   16
#define TIDtrack4   17
#define TIDtrack5   18
#define TIDtrack6   19
#define TIDtrack7   20
#define TIDtrack8   21
#define TIDtrack9   22
#define TIDtrack10  23
#define TIDtrack11  24
#define TIDtrack12  25

//character addresses
//not in use: 90, 91, 92
#define gridL       0x80
#define gridMOn     0x81
#define gridMOff    0x82
#define gridROn     0x83
#define gridROff    0x84
#define gridMOnInv  0x89
#define gridMOffInv 0x8A
#define gridROnInv  0x8B
#define gridROffInv 0x8C
#define gridHead    0x8D
#define gridHeadC   0x8E /* corner */

//very important that gridHead0 - 10 have consecutive addresses
#define gridHead0   0xBB
#define gridHead1   0xBC
#define gridHead2   0xBD
#define gridHead3   0xBE
#define gridHead4   0xBF
#define gridHead5   0xC0
#define gridHead6   0xC1
#define gridHead7   0xC2
#define gridHead8   0xC3
#define gridHead9   0xC4
#define gridHead10  0xC5

#define solo1       0x85
#define solo2       0x86
#define mute1       0x87
#define mute2       0x88
#define insSnare1   0x93
#define insSnare2   0x94
#define insSnare3   0x95
#define insSnare4   0x96
#define insBass1    0x97
#define insBass2    0x98
#define insBass3    0x99
#define insHigh1    0x9A
#define insHigh2    0x9B
#define insHigh3    0x9C
#define insMid1     0x9D
#define insMid2     0x9E
#define insLow1     0x9F
#define insLow2     0xA0
#define insLow3     0xA1
#define insCrash1   0xA2
#define insCrash2   0xA3
#define insCrash3   0xA4
#define insRide1    0xA5
#define insRide2    0xA6
#define insRide3    0xA7
#define insHH1      0xA8
#define insHH2      0xA9
#define insHH3      0xAA
#define insHH4      0xAB
#define insHH5      0xAC
#define insHH6      0xAD
#define insHH7      0xAE
#define insHH8      0xAF
#define insRim1     0xB0
#define insRim2     0xB1
#define insRim3     0xB2
#define insRim4     0xB3
#define insRim5     0xB4
#define insHand1    0xB5
#define insHand2    0xB6
#define insHand3    0xB7
#define insHand4    0xB8
#define insHand5    0xB9
#define rightArrow  0xBA
#define leftArrow   0xCC
#define divider     0xCD
#define play        0xCE
#define stop        0xCF
#define pause       0xD0
#define rec         0xD1
#define extSynch    0xC6
#define loop        0xC7
#define notok       0x8F

#define setupLine   0xC8
#define setupLineTop 0xCB
#define setupLineTopDotted 0xC9
#define setupLineTopTDotted 0xCA

#define gridBar1    0xD2
#define gridBar2    0xD3
#define gridBar3    0xD4
#define gridBar4    0xD5
#define gridBar5    0xD6
#define gridBar6    0xD7
#define gridBar7    0xD8
#define gridBar8    0xD9

#define insPatt1    0xDA
#define insPatt2    0xDB
#define insPatt3    0xDC
#define insPatt4    0xDD
#define insPatt5    0xDE

#define insGen1     0xDF
#define insGen2     0xE0
#define insGen3     0xE1
#define insGen4     0xE2
#define insGen5     0xE3

#define insTrack1   0xE4
#define insTrack2   0xE5
#define insTrack3   0xE6

#define ins0        0xE7
#define ins1        0xE8
#define ins2        0xE9
#define ins3        0xEA
#define ins4        0xEB
#define ins5        0xEC
#define ins6        0xED
#define ins7        0xEE
#define ins8        0xEF
#define ins9        0xF0

#define flam0       0xF1
#define flam1       0xF2
#define flam2       0xF3
#define flam3       0xF4
#define flam4       0xF5
#define flam5       0xF6
#define flam6       0xF7

//Borders for dialog boxes:
// U = upper, L = left, R = right, L = lower, M = middle
#define borderUL    0xF8
#define borderUM    0xF9
#define borderUR    0xFA
#define borderL     0xFB
#define borderR     0xFC
#define borderLL    0xFD
#define borderLM    0xFE
#define borderLR    0xFF
