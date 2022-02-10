
#ifndef __LK_GAPHICS__
#define __LK_GAPHICS__



extern const uint16_t cki_player1Sheet ;
extern const uint16_t cki_player2Sheet ;
extern const uint16_t cki_player3Sheet ;
extern const uint16_t cki_player4Sheet ;
extern const uint16_t cki_itemsSheet ;

#define cki_spritenewline 64

#define cki_itemsOffset  0

#define cki_playerGraphicsStart 16

#define cki_player1Offset (16*32*cki_playerGraphicsStart)
#define cki_player2Offset (16*32*(cki_playerGraphicsStart+1))
#define cki_player3Offset (16*32*(cki_playerGraphicsStart+2)) //(64*10*16)// (16*32*8)+256//256+80+64
#define cki_player4Offset (16*32*(cki_playerGraphicsStart+3))// (16*32*8)+386//384+80+64

#define cki_playerSheetWidth 752

//(16*32*2) + 64

extern const uint16_t cki_nextLineOff;

extern volatile uint16_t* GBA_BG0_Tiles ;
extern volatile uint16_t* GBA_BG0_Map   ;

extern volatile uint16_t* GBA_BG1_Tiles ;
extern volatile uint16_t* GBA_BG1_Map   ;

extern volatile uint16_t* GBA_BG2_Tiles ;
extern volatile uint16_t* GBA_BG2_Map   ;

extern volatile uint16_t* GBA_BG3_Tiles ;
extern volatile uint16_t* GBA_BG3_Map   ;

extern volatile uint16_t* GBA_BG_Palette;

extern const unsigned char tilescreen_data[];
extern const unsigned char timelabs_data[];

#define CK_GBA_BLOCK0 0x00
#define CK_GBA_BLOCK1 0x04
#define CK_GBA_BLOCK2 0x08
#define CK_GBA_BLOCK3 0x0C

#define CK_GBA_MAP0 0x1000
#define CK_GBA_MAP1 0x1100
#define CK_GBA_MAP2 0x1200
#define CK_GBA_MAP3 0x1300


void LK_CA_SetupGraphics();
void LK_CA_CopySpriteSheet();
void LK_CA_SetPlayerSprite(int sprId, int sheetId);
void LK_CA_HackPlayerSprite(int sprId1, int sprId2, int sprId3, int sheetId,int sheetrotation, int pid,int white);

#endif

