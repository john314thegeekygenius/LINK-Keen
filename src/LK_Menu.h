
#ifndef __LK_MENU__
#define __LK_MENU__


typedef enum {
	ck_CMainMenu,
	ck_CNewGame,
	ck_COptions,
	ck_CSendGame,
	ck_CSoloGame,
	ck_CJoinGame,
	ck_CCharacter,
	ck_CSetup,
	ck_CNumPlayers,
	ck_CNumBombs,
	ck_CNumShots,
	ck_CMap,
	ck_CMusic,
	ck_CSound,
	ck_CControls,
	ck_CSettings,
	ck_CNPlayers,
	ck_CNBombs,
	ck_CNShots,
	ck_CNLives,
	ck_CScorebox,
	ck_CLinkSettings,
	ck_CReconnect,
	ck_CGraphics,
	ck_CRenderer,
}ck_MenuNames;

typedef struct ck_Folder {
	ck_MenuNames menuName;
	char *available;
	char **names;
	unsigned short menuLength;
	struct ck_Folder *returnto;
	struct ck_Folder *folders[];
}ck_Folder;

char *LK_US_Itoa(short v);
char *LK_US_uItoa(unsigned short v);

char *LK_US_Htoa16(short v);
char *LK_US_Htoa32(int v);

extern uint16_t US_TextX ;
extern uint16_t US_TextY ;

void LK_US_ResetTiles();

void LK_US_PrintXY(char *str);
void LK_US_TextBox(char *str);
void LK_US_ThrowError(char *str);

void LK_US_ResetROM();

void LK_US_DrawControlPannel();
void LK_US_ResetControlPannel();


#endif

