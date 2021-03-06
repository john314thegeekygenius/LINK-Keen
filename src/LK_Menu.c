/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"

// Include the graphics

#include "CK_Data/cwrist_map.h"
#include "CK_Data/cwrist_tiles.h"
#include "CK_Data/cwrist_font.h"

#ifndef LK_MULTIBOOT_ROM
// Include the ROM here???
#include "ROM/BKEEN.h"
#endif

// Define some menus


char* ck_SNewGame[] = {"Start Solo","Start Multi","Character","Map","Game Settings"};
char* ck_SOptions[] = {"Music","Sound","Controls","Graphics","Link Settings"};
char* ck_SGame[]    = {"Players","Bombs","Shots","Lives","Teams"};
#ifdef LK_MULTIBOOT_ROM
char* ck_SMainMenu[] = {"New Game","Options","Reconnect"};
#else
char* ck_SMainMenu[] = {"New Game","Options","Reconnect","Send Game"};
#endif
char* ck_SGraphics[] = {"Map Renderer","Scorebox"};


char ck_BNewGame[] = {1,0,1,1,1};
char ck_BOptions[] = {1,1,1,1,1};
char ck_BGame[] = {1,1,1,1,1};
char ck_BMainMenu[] = {1,1,1,0};
char ck_BGraphics[] = {1,1};

ck_Folder ck_FMainMenu,ck_FNewGame,ck_FOptions,ck_FSettings,ck_FReconnect,ck_FSendGame,
			ck_FNPlayers, ck_FNBombs, ck_FNShots, ck_FLives, ck_FTeams, ck_FGraphics;

//////// New Game
ck_Folder ck_FSoloGame  = {ck_CSoloGame, NULL,  NULL,  0, &ck_FNewGame,
	{NULL}};
ck_Folder ck_FJoinGame  = {ck_CJoinGame, NULL,  NULL,  0, &ck_FNewGame,
	{NULL}};
ck_Folder ck_FCharacter  = {ck_CCharacter, NULL,  NULL,  0, &ck_FNewGame,
	{NULL}};
ck_Folder ck_FMap  = {ck_CMap, NULL,  NULL,  0, &ck_FNewGame,
	{NULL}};

//////// Game Settings
ck_Folder ck_FSettings  = {ck_CSettings, &ck_BGame,  &ck_SGame,  4, &ck_FNewGame,
	{&ck_FNPlayers, &ck_FNBombs, &ck_FNShots, &ck_FLives, &ck_FTeams, NULL}};
ck_Folder ck_FNPlayers  = {ck_CNPlayers, NULL,  NULL,  0, &ck_FSettings,
	{NULL}};
ck_Folder ck_FNBombs  = {ck_CNBombs, NULL,  NULL,  0, &ck_FSettings,
	{NULL}};
ck_Folder ck_FNShots  = {ck_CNShots, NULL,  NULL,  0, &ck_FSettings,
	{NULL}};
ck_Folder ck_FLives  = {ck_CLives, NULL,  NULL,  0, &ck_FSettings,
	{NULL}};
ck_Folder ck_FTeams  = {ck_CTeams, NULL,  NULL,  0, &ck_FSettings,
	{NULL}};


//// Options

ck_Folder ck_FMusic  = {ck_CMusic, NULL,  NULL,  0, &ck_FOptions,
	{NULL}};
ck_Folder ck_FSound  = {ck_CSound, NULL,  NULL,  0, &ck_FOptions,
	{NULL}};
ck_Folder ck_FControls  = {ck_CControls, NULL,  NULL,  0, &ck_FOptions,
	{NULL}};
ck_Folder ck_FScorebox  = {ck_CScorebox, NULL,  NULL,  0, &ck_FOptions,
	{NULL}};
ck_Folder ck_FLinkSettings  = {ck_CLinkSettings, NULL,  NULL,  0, &ck_FOptions,
	{NULL}};

ck_Folder ck_FRenderer  = {ck_CRenderer, NULL,  NULL,  0, &ck_FGraphics,
	{NULL}};


#ifdef LK_MULTIBOOT_ROM
ck_Folder ck_FMainMenu = {ck_CMainMenu,&ck_BMainMenu, &ck_SMainMenu, 3, NULL,
	{&ck_FNewGame, &ck_FOptions, &ck_FReconnect, NULL}};
#else
ck_Folder ck_FMainMenu = {ck_CMainMenu,&ck_BMainMenu, &ck_SMainMenu, 4, NULL,
	{&ck_FNewGame, &ck_FOptions, &ck_FReconnect, &ck_FSendGame, NULL}};
#endif

ck_Folder ck_FNewGame  = {ck_CNewGame, &ck_BNewGame,  &ck_SNewGame,  5, &ck_FMainMenu,
	{&ck_FSoloGame, &ck_FJoinGame, &ck_FCharacter, &ck_FMap, &ck_FSettings, NULL}};

ck_Folder ck_FGraphics  = {ck_CGraphics, &ck_BGraphics,  &ck_SGraphics,  2, &ck_FOptions,
	{&ck_FRenderer,&ck_FScorebox,NULL}};

ck_Folder ck_FOptions  = {ck_COptions, &ck_BOptions,  &ck_SOptions,  4, &ck_FMainMenu,
	{&ck_FMusic,&ck_FSound,&ck_FControls,&ck_FGraphics,&ck_FLinkSettings,NULL}};

ck_Folder ck_FReconnect  = {ck_CReconnect, NULL,  NULL,  0, &ck_FMainMenu,
	{NULL}};

ck_Folder ck_FSendGame  = {ck_CSendGame, NULL,  NULL,  0, &ck_FMainMenu,
	{NULL}};


boolean ck_ControlPannelDrawn = false;
boolean ck_DrawnCharacter = false;

ck_Folder *ck_ControlPannelMenu = &ck_FMainMenu;

short ck_selectorY = 0;
short ck_selectorX = 0;
uint16_t ck_selectorGlyph = 11;
short ck_mapSelected = 0;


uint16_t chr2gliph(char c){
	// Letters are more frequent
	if(c >= 'A' && c <= 'Z'){
		return 15+(c-'A');
	}
	if(c >= 'a' && c <= 'z'){
		return 45+(c-'a');
	}
	if(c >= '0' && c <= '9'){
		return 1+(c-'0');
	}
	if(c == '-'){
		return 89;
	}
	if(c == '.'){
		return 104;
	}
	if(c == '\1') return 87;
	if(c == '\2') return 88;
	if(c == '\3') return 103;
	if(c == '\4') return 102;
	return 0;
};


uint16_t US_TextX = 0;
uint16_t US_TextY = 0;

char US_Itoa_String[32];

char *LK_US_Itoa(short v){
	short nlen = 0;
	if(v==0){
		US_Itoa_String[0] = '0';
		US_Itoa_String[1] = 0;
		return US_Itoa_String;
	}
	if(v<0){
		US_Itoa_String[0] = '-';
		v = -v;
		nlen += 1;
	}
	if(v>=0) nlen += 1;
	if(v>=10) nlen += 1;
	if(v>=100) nlen += 1;
	if(v>=1000) nlen += 1;
	if(v>=10000) nlen += 1;

	US_Itoa_String[nlen] = 0;
	nlen -= 1;
	while(v>0){
		US_Itoa_String[nlen--] = '0'+(v%10);
		v /= 10;
	}

	return US_Itoa_String;
};

char *LK_US_uItoa(unsigned short v){
	short nlen = 0;
	if(v==0){
		US_Itoa_String[0] = '0';
		US_Itoa_String[1] = 0;
		return US_Itoa_String;
	}
	if(v>=0) nlen += 1;
	if(v>=10) nlen += 1;
	if(v>=100) nlen += 1;
	if(v>=1000) nlen += 1;
	if(v>=10000) nlen += 1;

	US_Itoa_String[nlen+1] = 0;
	nlen -= 1;
	while(v>0){
		US_Itoa_String[nlen--] = '0'+(v%10);
		v /= 10;
	}

	return US_Itoa_String;
};


char *LK_US_Htoa16(short v){
	int i, o, shf;
	o = 0;
	shf = 12;
	while(shf>=0){
		for(i = 0; i < 16; i ++)
			if(((v>>shf)&0xF) == i)
				US_Itoa_String[o++] = "0123456789ABCDEF"[i];
		shf -= 4;
	}
	US_Itoa_String[o] = 0; // Terminate the string

	return US_Itoa_String;
};

char *LK_US_Htoa32(int v){
	int i, o, shf;
	o = 0;
	shf = 28;
	while(shf>=0){
		for(i = 0; i < 16; i ++)
			if(((v>>shf)&0xF) == i)
				US_Itoa_String[o++] = "0123456789ABCDEF"[i];
		shf -= 4;
	}
	US_Itoa_String[o] = 0; // Terminate the string

	return US_Itoa_String;
};


void LK_US_PrintXY(char *str){
	if(str==NULL) return;
	// Shift the y position
	US_TextY <<= 5;
	US_TextX += US_TextY;
	// Write the text to the screen
	while(*str){
		GBA_BG1_Map[US_TextX] = chr2gliph(*str);
		++str;
		++US_TextX;
	}
};


void LK_US_PrintXYMenu(char *str){
	if(str==NULL) return;
	// Shift the y position
	US_TextY <<= 5;
	US_TextX += US_TextY;
	// Write the text to the screen
	while(*str){
		GBA_BG0_Map[US_TextX] = 0x87;
		GBA_BG1_Map[US_TextX] = chr2gliph(*str);
		++str;
		++US_TextX;
	}
};

void LK_US_PrintGlyph(uint16_t g){
	// Shift the y position
	US_TextY <<= 5;
	US_TextX += US_TextY;
	// Write the glyph to the screen
	GBA_BG1_Map[US_TextX] = g;
};

uint16_t LK_US_GetGlyph(){
	// Shift the y position
	US_TextY <<= 5;
	US_TextX += US_TextY;
	// Write the glyph to the screen
	return GBA_BG1_Map[US_TextX];
};

void LK_US_TextBox(char *str){
	int boxX = US_TextX;
	int boxY = US_TextY;
	int boxW = GBA_StrLen(str)+2;
	int boxH = 3;
	int txtX = 15-(boxW>>1);
	int txtY = 10;
	int i,e;

	boxX -= 1;
	boxY -= 1;


	GBA_BG1_Map[(boxY<<5)+boxX] = 0x5A;
	for(i=1;i<=boxW;i++){
		GBA_BG1_Map[(boxY<<5)+boxX+i] = 0x5B;
		GBA_BG1_Map[((boxY+boxH+1)<<5)+boxX+i] = 0x61;
	}
	GBA_BG1_Map[(boxY<<5)+boxX+boxW+1] = 0x5C;
	GBA_BG1_Map[((boxY+boxH+1)<<5)+boxX] = 0x60;
	for(i=1;i<=boxH;i++){
		GBA_BG1_Map[((boxY+i)<<5)+boxX] = 0x5D;
		GBA_BG1_Map[((boxY+i)<<5)+boxX+boxW+1] = 0x5F;
	}
	GBA_BG1_Map[((boxY+boxH+1)<<5)+boxX+boxW+1] = 0x62;

/*	
	GBA_BG1_Map[(boxY<<5)+boxX] = 0x5A;
	for(i=1;i<=boxW;i++){
		GBA_BG1_Map[(boxY<<5)+boxX+i] = 0x5B;
		GBA_BG1_Map[((boxY+boxH+1)<<5)+boxX+i] = 0x6C;
	}
	GBA_BG1_Map[(boxY<<5)+boxX+boxW+1] = 0x5C;
	GBA_BG1_Map[((boxY+boxH+1)<<5)+boxX] = 0x6B;
	for(i=1;i<=boxH;i++){
		GBA_BG1_Map[((boxY+i)<<5)+boxX] = 0x5D;
		GBA_BG1_Map[((boxY+i)<<5)+boxX+boxW+1] = 0x6A;
	}
	GBA_BG1_Map[((boxY+boxH+1)<<5)+boxX+boxW+1] = 0x78;
*/
	
	boxX += 1;
	boxY += 1;

	boxW += boxX;
	boxH += boxY;

	for(e = boxY; e < boxH; e++){
		for(i = boxX; i < boxW; i++){
			US_TextX = (e<<5)+i;
			GBA_BG0_Map[US_TextX] = 0x97;
		}
	}
	// Write the text
	US_TextX = boxX + 1;
	US_TextY = boxY + 1;
	LK_US_PrintXY(str);
};


void LK_US_ResetTiles(){
#ifdef LK_4BPP_MAPS
	GBA_DMA_Copy16((uint16_t*)GBA_PAL_BG_START,(uint16_t*)ck_graphics_palette,GBA_PAL_SIZE);

	// Reset the map info
	GBA_BG0_Map   = (volatile uint16_t*)GBA_SCREEN_BLOCK(0);
	GBA_BG1_Map   = (volatile uint16_t*)GBA_SCREEN_BLOCK(2);
	GBA_BG2_Map   = (volatile uint16_t*)GBA_SCREEN_BLOCK(4);
	GBA_BG3_Map   = (volatile uint16_t*)GBA_SCREEN_BLOCK(6);
#endif

	// Copy the tileset into the block
	GBA_DMA_Copy16((uint16_t*)GBA_BG0_Tiles,(uint16_t*)cwrist_tiles_data,(cwrist_tiles_size)>>1);
	// Copy the tileset into the block
	GBA_DMA_Copy16((uint16_t*)GBA_BG1_Tiles,(uint16_t*)cwrist_font_data,(cwrist_font_size)>>1);
	// Copy the tileset into the block
	GBA_DMA_Copy16((uint16_t*)GBA_BG2_Tiles,(uint16_t*)cwrist_font_data,(cwrist_font_size)>>1);
	// Copy the tileset into the block
	GBA_DMA_Copy16((uint16_t*)GBA_BG3_Tiles,(uint16_t*)cwrist_font_data,(cwrist_font_size)>>1);
	// Finish the render of the background
	GBA_FINISH_BG0(GBA_BG_BACK | CK_GBA_BLOCK0 | CK_GBA_MAP0 | GBA_BG_SIZE_32x32);
	GBA_FINISH_BG1(GBA_BG_FRONT | CK_GBA_BLOCK1 | CK_GBA_MAP1 | GBA_BG_SIZE_32x32);
	GBA_FINISH_BG2(0);	
	GBA_FINISH_BG3(GBA_BG_TOP | CK_GBA_BLOCK1 | CK_GBA_MAP2 | GBA_BG_SIZE_32x32);
	
	LK_MP_ResetScroll();
};

void LK_US_ClearScreen(){
	// Render a cyan screen
	GBA_DMA_MemSet16((uint16_t*)GBA_BG0_Map,0x96,32*32);
	GBA_DMA_MemSet16((uint16_t*)GBA_BG1_Map,0,32*32);
	GBA_DMA_MemSet16((uint16_t*)GBA_BG2_Map,0,32*32);
	GBA_DMA_MemSet16((uint16_t*)GBA_BG3_Map,0,32*32);
};

void LK_US_ThrowError(char *str){
	int button = 0;
	
	GBA_ResetSprites();
	GBA_UPDATE_SPRITES()

	LK_US_ResetTiles();
	
	LK_US_ClearScreen();

	US_TextX = 15-((GBA_StrLen(str)>>1)+1); US_TextY = 8;
	LK_US_TextBox(str);
	
	while((~GBA_BUTTONS));
	while(!((~GBA_BUTTONS)&GBA_BUTTON_A));
	while(((~GBA_BUTTONS)&GBA_BUTTON_A));
	LK_US_ResetROM();
};


uint16_t playerSpriteT = 0, playerSpriteB;

// Draws the control pannel
void LK_US_DrawControlPannel(){
	int i = 0, gamelaunched = 0;
	char **map_data;


	// See if a GBA is connected
	ck_localGameState.multibootAvailable = GBA_MB_GBAConnected();
	
	if(GBA_SerialID!=0){
		ck_SNewGame[1] = "Join Game";
	}else{
		// Update the network
		GBA_UpdateSerial();
	}
		
	if(ck_ControlPannelDrawn==false){
		// Check for multiplayer
		if(ck_localGameState.multiplayerAvailable){
			if(ck_localGameState.num_players>1){
				ck_BNewGame[1] = 1;
			}else{
				ck_BNewGame[1] = 0; // :( no one to play with
			}
		}else{
			ck_BNewGame[1] = 0;
		}
		if(ck_localGameState.multibootAvailable){
			ck_BMainMenu[3] = 1; // Yay! :D
		}else{
			ck_BMainMenu[3] = 0; // :( no one to send the ROM to
		}
		
		
		// Copy the tileset into the block
//		GBA_DMA_Copy16((uint16_t*)GBA_BG0_Tiles,(uint16_t*)cwrist_tiles_data,(cwrist_tiles_size)>>1);

		// Copy the map data into the block
		GBA_DMA_Copy16((uint16_t*)GBA_BG0_Map,(uint16_t*)cwrist_map,(cwrist_map_width * cwrist_map_height));

		// Finish the render of the background
		GBA_FINISH_BG0(GBA_BG_BACK | CK_GBA_BLOCK0 | CK_GBA_MAP0 | GBA_BG_SIZE_32x32);

		// Render the menu title
		switch(ck_ControlPannelMenu->menuName){
			case ck_CSendGame:
			case ck_CMainMenu:
			for(i = 0; i < 9; i++)
				GBA_BG0_Map[167+i] = 79+i;
			break;
			case ck_CSoloGame:
			case ck_CJoinGame:
			case ck_CNewGame:
			for(i = 0; i < 7; i++)
				GBA_BG0_Map[167+i] = 94+i;
			for(i = 0; i < 5; i++)
				GBA_BG0_Map[167+7+i] = 83+i;
			break;
			case ck_CSettings:
			case ck_CNPlayers:
			case ck_CNBombs:
			case ck_CNShots:
			case ck_CLives:
			case ck_CTeams:
			case ck_CMusic:
			case ck_CSound:
			case ck_CControls:
			case ck_CScorebox:
			case ck_CLinkSettings:
			case ck_CReconnect:
			case ck_CGraphics:
			case ck_CRenderer:
			case ck_COptions:
			for(i = 0; i < 6; i++)
				GBA_BG0_Map[167+i] = 109+i;
			for(i = 0; i < 5; i++)
				GBA_BG0_Map[167+6+i] = 83+i;
			break;
			case ck_CCharacter:
			for(i = 0; i < 8; i++)
				GBA_BG0_Map[167+i] = 124+i;
			for(i = 0; i < 5; i++)
				GBA_BG0_Map[167+8+i] = 83+i;
			break;
			case ck_CMap:
			for(i = 0; i < 4; i++)
				GBA_BG0_Map[167+i] = 101+i;
			for(i = 0; i < 5; i++)
				GBA_BG0_Map[167+4+i] = 83+i;
			break;
		}
/*
		// Copy the tileset into the block
		GBA_DMA_Copy16((uint16_t*)GBA_BG1_Tiles,(uint16_t*)cwrist_font_data,(cwrist_font_size)>>1);
*/
		// Clear the maps
		for(i = 0; i < 32*32; i++){
			GBA_BG1_Map[i] = 0;
			GBA_BG2_Map[i] = 0;
		}

		// Write the menu options
		i = 0;
		while(ck_ControlPannelMenu->folders[i] != NULL){
			US_TextX = 7; US_TextY = 7+i;
			if(ck_ControlPannelMenu->available[i]==1)
				LK_US_PrintGlyph(11);
			else
				LK_US_PrintGlyph(41);
			US_TextX = 9; US_TextY = 7+i;
			LK_US_PrintXY(ck_ControlPannelMenu->names[i++]);
		}
		ck_ControlPannelMenu->menuLength = i;




		if(ck_ControlPannelMenu->menuName==ck_CMainMenu){
			US_TextX = 7; US_TextY = 19;
			if(ck_localGameState.multiplayerAvailable){
				LK_US_PrintXYMenu("Link Connected");
//				US_TextX = 7; US_TextY = 15;
//				LK_US_PrintXY(LK_US_Itoa(GBA_SerialID));
			}
		}

		// Finish the render of the background
		GBA_FINISH_BG1(GBA_BG_MID | CK_GBA_BLOCK1 | CK_GBA_MAP1 | GBA_BG_SIZE_32x32);
		GBA_FINISH_BG2(GBA_BG_MID | CK_GBA_BLOCK1 | CK_GBA_MAP2 | GBA_BG_SIZE_32x32);
		GBA_FINISH_BG3(GBA_BG_TOP | CK_GBA_BLOCK1 | CK_GBA_MAP2 | GBA_BG_SIZE_32x32);

		//GBA_BLEND_BGS(GBA_BLD_S_BG1 | GBA_BLDM_APLHA | GBA_BLD_D_BG0)

		ck_ControlPannelDrawn = true;
	}

	// Get controls
	LK_IN_ReadInput();

	if(ck_lastInput & GBA_BUTTON_B){
		if(ck_ControlPannelMenu->returnto!=NULL){
			GBA_ResetSprites();
			// Go into the folder
			ck_ControlPannelMenu = ck_ControlPannelMenu->returnto;
			// Reset some variables
			ck_selectorY = 0;
			ck_selectorGlyph = 11;
			// We need to redraw the scene
			ck_ControlPannelDrawn = false;
			// We might need to redraw the character later
			ck_DrawnCharacter = false;
			// Clear the flag
			ck_lastInput = 0;
			return;
		}
	}

	// ------ Custom Menus ------

	switch(ck_ControlPannelMenu->menuName){
		case ck_CSoloGame:
			LK_InitGame(true);
		break;
#ifndef LK_MULTIBOOT_ROM
		case ck_CSendGame:
			LK_US_ClearScreen();
			US_TextX = 6; US_TextY = 8;
			LK_US_TextBox("Sending ROM...");
			// Send the ROM
			GBA_MB_SendGameROM((uint8_t*)BKEEN_data,BKEEN_size);
			GBA_MB_StopMultiboot();
			// Continue as normal
			LK_US_ClearScreen();
			US_TextX = 8; US_TextY = 8;
			LK_US_TextBox("ROM Sent");
			GBA_SetMultiplayer();
			GBA_Delay(100);
			// Try to connect
			for(i=0;i<16;i++){
				GBA_InitSerial(GBA_COM_BAUD_9600);
				LK_IN_ReadInput();
				if((ck_lastInput & GBA_BUTTON_B) || GBA_SerialAvailable){
					i=16;
				}
				ck_lastInput = 0;
			}
//			GBA_RepairConnection();
			GBA_UpdateSerial();
		
			if(ck_ControlPannelMenu->returnto!=NULL){
				GBA_ResetSprites();
				// Go into the folder
				ck_ControlPannelMenu = ck_ControlPannelMenu->returnto;
				// Reset some variables
				ck_selectorY = 0;
				ck_selectorGlyph = 11;
				// We need to redraw the scene
				ck_ControlPannelDrawn = false;
				// We might need to redraw the character later
				ck_DrawnCharacter = false;
				// Clear the flag
				ck_lastInput = 0;
				return;
			}
		break;
#endif
		case ck_CJoinGame:
			LK_US_ClearScreen();
			US_TextX = 3; US_TextY = 8;
			LK_US_TextBox("Waiting for match...");
			LK_InitGame(false);
		break;
		case ck_CSetup:
			ck_localGameState.num_players = 0;
		break;
		case ck_CNPlayers:
			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Num Players:");
			US_TextX = 12; US_TextY = 8;
			LK_US_PrintXY(LK_US_Itoa(ck_localGameState.num_players));

			ck_selectorX = ck_localGameState.num_players;
			if(ck_lastInput & GBA_BUTTON_LEFT){
				ck_selectorX  -= 1;
				if(ck_selectorX<1) ck_selectorX = 1;
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}

			if(ck_lastInput & GBA_BUTTON_RIGHT){
				ck_selectorX  += 1;
				if(ck_selectorX>4) ck_selectorX = 4;
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}
			ck_localGameState.num_players = ck_selectorX;

			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("change value");

		break;
		case ck_CNBombs:
			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Starting bombs");
			US_TextX = 12; US_TextY = 8;
			LK_US_PrintXY(LK_US_Itoa(ck_localGameState.start_bombs));
			ck_selectorX = ck_localGameState.start_bombs;
			if(ck_lastInput & GBA_BUTTON_LEFT){
				ck_selectorX  -= 1;
				if(ck_selectorX<0) ck_selectorX = 0;
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}
			if(ck_lastInput & GBA_BUTTON_RIGHT){
				ck_selectorX  += 1;
				if(ck_selectorX>9) ck_selectorX = 9;
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}
			ck_localGameState.start_bombs = ck_selectorX;
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("change value");
		break;
		case ck_CNShots:
			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Starting shots");
			US_TextX = 12; US_TextY = 8;
			LK_US_PrintXY(LK_US_Itoa(ck_localGameState.start_shots));
			ck_selectorX = ck_localGameState.start_shots;
			if(ck_lastInput & GBA_BUTTON_LEFT){
				ck_selectorX  -= 5;
				if(ck_selectorX<0) ck_selectorX = 0;
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}
			if(ck_lastInput & GBA_BUTTON_RIGHT){
				ck_selectorX  += 5;
				if(ck_selectorX>99) ck_selectorX = 99;
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}
			ck_localGameState.start_shots = ck_selectorX;
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("change value");
		break;
		case ck_CLives:
			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 7; US_TextY = 9;
			LK_US_PrintGlyph(11);
			US_TextX = 7; US_TextY = 11;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Start health");
			US_TextX = 12; US_TextY = 8;
			LK_US_PrintXY(LK_US_Itoa(ck_localGameState.start_health));
			US_TextX = 9; US_TextY = 9;
			LK_US_PrintXY("Kills to win");
			US_TextX = 12; US_TextY = 10;
			LK_US_PrintXY(LK_US_Itoa(ck_localGameState.end_kills));
			US_TextX = 9; US_TextY = 11;
			LK_US_PrintXY("Hazard penalty");
			US_TextX = 12; US_TextY = 12;
			if(ck_localGameState.hazard_penalty){
				LK_US_PrintXY("Yes");
			}else{
				LK_US_PrintXY("No");
			}

			// Clear the last position
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			LK_US_PrintGlyph(ck_selectorGlyph);

			if(ck_lastInput & GBA_BUTTON_UP){
				ck_selectorY  -= 1;
				// Clear the flag
				ck_lastInput = 0;
			}
			if(ck_lastInput & GBA_BUTTON_DOWN){
				ck_selectorY  += 1;
				// Clear the flag
				ck_lastInput = 0;
			}

			if(ck_selectorY < 0){
				ck_selectorY = 0;
			}
			if(ck_selectorY >= 2){
				ck_selectorY = 2;
			}

			// Get the old glyph
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			ck_selectorGlyph = LK_US_GetGlyph();

			// Draw the selector
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			LK_US_PrintGlyph(ck_selectorGlyph+1);

			if(ck_selectorY == 0){
				ck_selectorX = ck_localGameState.start_health;


				if(ck_lastInput & GBA_BUTTON_LEFT){
					ck_selectorX  -= 10;
					if(ck_selectorX<50) ck_selectorX = 50;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}

				if(ck_lastInput & GBA_BUTTON_RIGHT){
					ck_selectorX  += 10;
					if(ck_selectorX>200) ck_selectorX = 200;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				ck_localGameState.start_health = ck_selectorX;
			}
			if(ck_selectorY == 1){
				ck_selectorX = ck_localGameState.end_kills;


				if(ck_lastInput & GBA_BUTTON_LEFT){
					ck_selectorX  -= 1;
					if(ck_selectorX<1) ck_selectorX = 1;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}

				if(ck_lastInput & GBA_BUTTON_RIGHT){
					ck_selectorX  += 1;
					if(ck_selectorX>15) ck_selectorX = 15;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				ck_localGameState.end_kills = ck_selectorX;
			}
			if(ck_selectorY == 2){
				ck_selectorX = ck_localGameState.hazard_penalty;


				if(ck_lastInput & GBA_BUTTON_LEFT){
					ck_selectorX  -= 1;
					if(ck_selectorX<0) ck_selectorX = 0;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}

				if(ck_lastInput & GBA_BUTTON_RIGHT){
					ck_selectorX  += 1;
					if(ck_selectorX>1) ck_selectorX = 1;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				ck_localGameState.hazard_penalty = ck_selectorX;
			}
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("change value");
		break;
		case ck_CTeams:

			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 7; US_TextY = 9;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Team Game:");
			US_TextX = 12; US_TextY = 8;
			if(!ck_localGameState.teamGame){
				LK_US_PrintXY("No");
			}else{
				LK_US_PrintXY("Yes");
			}
			//
			US_TextX = 9; US_TextY = 9;
			LK_US_PrintXY("Team number:");
			US_TextX = 12; US_TextY = 10;
			LK_US_PrintXY(LK_US_Itoa(ck_localGameState.player_teams[0]+1));

			// Clear the last position
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			LK_US_PrintGlyph(ck_selectorGlyph);

			if((ck_lastInput & GBA_BUTTON_UP) ){
				ck_selectorY  -= 1;
				// Clear the flag
				ck_lastInput = 0;
			}
			if((ck_lastInput & GBA_BUTTON_DOWN)){
				ck_selectorY  += 1;
				// Clear the flag
				ck_lastInput = 0;
			}

			if(ck_selectorY < 0){
				ck_selectorY = 0;
			}
			if(ck_selectorY >= 1){
				ck_selectorY = 1;
			}

			// Get the old glyph
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			ck_selectorGlyph = LK_US_GetGlyph();

			// Draw the selector
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			LK_US_PrintGlyph(ck_selectorGlyph+1);

			if(ck_selectorY == 0){
				ck_selectorX = ck_localGameState.teamGame;
				if((ck_lastInput & GBA_BUTTON_LEFT)){
					ck_selectorX  -= 1;
					if(ck_selectorX<0) ck_selectorX = 0;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				if((ck_lastInput & GBA_BUTTON_RIGHT)){
					ck_selectorX  += 1;
					if(ck_selectorX>1) ck_selectorX = 1;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				ck_localGameState.teamGame = ck_selectorX;
			}else{
				ck_selectorX = ck_localGameState.player_teams[0];
				if((ck_lastInput & GBA_BUTTON_LEFT) ){
					ck_selectorX  -= 1;
					if(ck_selectorX<0) ck_selectorX = 0;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				if((ck_lastInput & GBA_BUTTON_RIGHT) ){
					ck_selectorX  += 1;
					if(ck_selectorX>3) ck_selectorX = 3;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				ck_localGameState.player_teams[0] = ck_selectorX;
			}
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("change option");

		break;
		case ck_CCharacter:
			ck_selectorX = ck_localGameState.player_pics[0];
			// Show selected character
			if(ck_DrawnCharacter==false){
				GBA_ResetSprites();
				// Make it standing animation
				LK_CA_HackPlayerSprite(0, 94, 0, ck_selectorX, 0,0,0);
				#ifdef LK_MULTIBOOT_ROM
				playerSpriteT = GBA_CreateSprite(106, 72,GBA_SPR_16_16,0,GBA_SPRITE_ZTOP,ck_selectorX);
				playerSpriteB = GBA_CreateSprite(106, 88,GBA_SPR_16_8, 0,GBA_SPRITE_ZTOP,ck_selectorX);
				GBA_SET_SPRITE_TILES(playerSpriteT, cki_playerGraphicsStart*32);
				GBA_SET_SPRITE_TILES(playerSpriteB, (cki_playerGraphicsStart*32)+2);
				#else
				playerSpriteT = GBA_CreateSprite(106, 72,GBA_SPR_16_16,0,GBA_SPRITE_ZTOP,-1);
				playerSpriteB = GBA_CreateSprite(106, 88,GBA_SPR_16_8, 0,GBA_SPRITE_ZTOP,-1);
				GBA_SET_SPRITE_TILES(playerSpriteT, cki_playerGraphicsStart*32);
				GBA_SET_SPRITE_TILES(playerSpriteB, (cki_playerGraphicsStart*32)+4);
				#endif
				GBA_SET_FLIPH(playerSpriteT,1)
				GBA_SET_FLIPH(playerSpriteB,1)
				GBA_UPDATE_SPRITES()
				ck_DrawnCharacter = true;
			}

			if(ck_lastInput & GBA_BUTTON_LEFT){
				ck_selectorX  -= 1;
				if(ck_selectorX<0) ck_selectorX = 0;
				ck_DrawnCharacter = false;
				// Clear the flag
				ck_lastInput = 0;
			}

			if(ck_lastInput & GBA_BUTTON_RIGHT){
				ck_selectorX  += 1;
				if(ck_selectorX>=8) ck_selectorX = 7;
				ck_DrawnCharacter = false;
				// Clear the flag
				ck_lastInput = 0;
			}
			ck_localGameState.player_pics[0] = ck_selectorX;
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 6; US_TextY = 14;
			LK_US_PrintXYMenu("change character");

		break;
		case ck_CMap:

			if(ck_lastInput & GBA_BUTTON_LEFT){
				ck_mapSelected  -= 1;

				ck_ControlPannelDrawn = false;
				// Clear the flag
				ck_lastInput = 0;
			}
			if(ck_lastInput & GBA_BUTTON_RIGHT){
				ck_mapSelected  += 1;

				ck_ControlPannelDrawn = false;
				// Clear the flag
				ck_lastInput = 0;
			}

			if(ck_mapSelected<0) ck_mapSelected = 0;
			if(ck_mapSelected>CK_MAX_LEVELS) ck_mapSelected = CK_MAX_LEVELS;

			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Current Map:");
//			US_TextX = 12; US_TextY = 8;
//			LK_US_PrintXY(LK_US_Itoa(ck_mapSelected));
			map_data = LK_MP_GetMapInfo(ck_mapSelected);
			if(map_data!=NULL){
				US_TextX = 9; US_TextY = 9;
				LK_US_PrintXY(map_data[0]);
				US_TextX = 9; US_TextY = 10;
				LK_US_PrintXY(map_data[1]);
				US_TextX = 9; US_TextY = 11;
				LK_US_PrintXY(map_data[2]);
			}
			ck_localGameState.level_id = ck_mapSelected;
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 9; US_TextY = 14;
			LK_US_PrintXYMenu("change map");

		break;

		case ck_CLinkSettings:
			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Ms per packet");
			US_TextX = 12; US_TextY = 8;
			LK_US_PrintXY(LK_US_Itoa(GBA_SerialWaitTime));
			ck_selectorX = GBA_SerialWaitTime;
			if(ck_lastInput & GBA_BUTTON_LEFT){
				ck_selectorX  -= 5;
				if(ck_selectorX<0) ck_selectorX = 0;
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}
			if(ck_lastInput & GBA_BUTTON_RIGHT){
				ck_selectorX  += 5; // No upper limit???
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}
			GBA_SerialWaitTime = ck_selectorX;
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("change value");

		break;
		case ck_CReconnect:
			US_TextX = 8; US_TextY = 10;
			LK_US_PrintXY("Reconnecting");

			// Try to connect
			GBA_RepairConnection();

/*			LK_US_ClearScreen();
			US_TextX = 6; US_TextY = 8;
			LK_US_TextBox(LK_US_Itoa(GBA_NGameBoysConnected));
			GBA_Delay(2000);
*/

			ck_localGameState.num_players = GBA_NGameBoysConnected;

		
			if(ck_ControlPannelMenu->returnto!=NULL){
				GBA_ResetSprites();
				// Go into the folder
				ck_ControlPannelMenu = ck_ControlPannelMenu->returnto;
				// Reset some variables
				ck_selectorY = 0;
				ck_selectorGlyph = 11;
				// We need to redraw the scene
				ck_ControlPannelDrawn = false;
				// We might need to redraw the character later
				ck_DrawnCharacter = false;
				// Clear the flag
				ck_lastInput = 0;
				return;
			}
			/*
			// Draw some UI
			US_TextX = 8; US_TextY = 9;
			if(ck_lastInput & GBA_BUTTON_A){
				LK_US_PrintXY("Checking...");
				// Try to reconnect
				GBA_RepairConnection();
				GBA_UpdateSerial();
				ck_ControlPannelDrawn = false;
				// Clear the flag
				ck_lastInput = 0;
			}else{
				LK_US_PrintXY("Press A");
			}*/

		break;

		case ck_CMusic:

#ifdef LK_MUSIC_ENABLED
			if((ck_lastInput & GBA_BUTTON_A) || (ck_lastInput & GBA_BUTTON_LEFT) || (ck_lastInput & GBA_BUTTON_RIGHT)){
				ck_localGameState.music_enabled = !ck_localGameState.music_enabled;
				ck_ControlPannelDrawn = false;
				// Clear the flag
				ck_lastInput = 0;
			}
			// Draw some UI
			US_TextX = 8; US_TextY = 9;
			if(ck_localGameState.music_enabled){
				LK_US_PrintXY("Music Enabled");
			}else{
				LK_US_PrintXY("Music Disabled");
			}
			US_TextX = 8; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 or A");
			US_TextX = 7; US_TextY = 14;
			LK_US_PrintXYMenu("to toggle music");
#else
			US_TextX = 8; US_TextY = 9;
			LK_US_PrintXY("Music Disabled");
#endif
		break;
		case ck_CSound:
#ifdef LK_SOUND_ENABLED
			if((ck_lastInput & GBA_BUTTON_A) || (ck_lastInput & GBA_BUTTON_LEFT) || (ck_lastInput & GBA_BUTTON_RIGHT)){
				ck_localGameState.sound_enabled = !ck_localGameState.sound_enabled;
				ck_ControlPannelDrawn = false;
				// Clear the flag
				ck_lastInput = 0;
			}
			// Draw some UI
			US_TextX = 8; US_TextY = 9;
			if(ck_localGameState.sound_enabled){
				LK_US_PrintXY("Sound Enabled");
			}else{
				// Stop any stray sounds???
				LK_SD_StopSounds();

				LK_US_PrintXY("Sound Disabled");
			}
			US_TextX = 8; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 or A");
			US_TextX = 7; US_TextY = 14;
			LK_US_PrintXYMenu("to toggle sound");
#else
			// Draw some UI
			US_TextX = 8; US_TextY = 9;
			LK_US_PrintXY("Sound Disabled");
#endif
		break;
		case ck_CRenderer:
			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Map renderer");
			US_TextX = 7; US_TextY = 11;
			if(ck_localGameState.map_renderer==0)
				LK_US_PrintXY("Double Buffered");
			else if(ck_localGameState.map_renderer==1)
				LK_US_PrintXY("Fast Scrolling");

			ck_selectorX = ck_localGameState.map_renderer;
			if(ck_lastInput & GBA_BUTTON_LEFT){
				ck_selectorX  -= 1;
				if(ck_selectorX<0) ck_selectorX = 0;
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}
			if(ck_lastInput & GBA_BUTTON_RIGHT){
				ck_selectorX  += 1;
				if(ck_selectorX>1) ck_selectorX = 1;
				// Clear the flag
				ck_lastInput = 0;
				ck_ControlPannelDrawn = false;
			}
			ck_localGameState.map_renderer = ck_selectorX;
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("change value");
		break;
		case ck_CScorebox:

			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 7; US_TextY = 9;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Score box");
			US_TextX = 12; US_TextY = 8;
			if(ck_localGameState.ck_status_type==0){
				LK_US_PrintXY("None");
			}else if(ck_localGameState.ck_status_type==1){
				LK_US_PrintXY("Normal");
			}else if(ck_localGameState.ck_status_type==2){
				LK_US_PrintXY("Condensed");
			}
			US_TextX = 9; US_TextY = 9;
			if(ck_localGameState.ck_status_loc==0){
				LK_US_PrintXY("Top");
			}else {
				LK_US_PrintXY("Bottom");
			}

			// Clear the last position
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			LK_US_PrintGlyph(ck_selectorGlyph);

			if((ck_lastInput & GBA_BUTTON_UP) ){
				ck_selectorY  -= 1;
				// Clear the flag
				ck_lastInput = 0;
			}
			if((ck_lastInput & GBA_BUTTON_DOWN)){
				ck_selectorY  += 1;
				// Clear the flag
				ck_lastInput = 0;
			}

			if(ck_selectorY < 0){
				ck_selectorY = 0;
			}
			if(ck_selectorY >= 1){
				ck_selectorY = 1;
			}

			// Get the old glyph
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			ck_selectorGlyph = LK_US_GetGlyph();

			// Draw the selector
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			LK_US_PrintGlyph(ck_selectorGlyph+1);

			if(ck_selectorY == 0){
				ck_selectorX = ck_localGameState.ck_status_type;
				if((ck_lastInput & GBA_BUTTON_LEFT)){
					ck_selectorX  -= 1;
					if(ck_selectorX<0) ck_selectorX = 0;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				if((ck_lastInput & GBA_BUTTON_RIGHT)){
					ck_selectorX  += 1;
					if(ck_selectorX>2) ck_selectorX = 2;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				ck_localGameState.ck_status_type = ck_selectorX;
			}else{
				ck_selectorX = ck_localGameState.ck_status_loc;
				if((ck_lastInput & GBA_BUTTON_LEFT) ){
					ck_selectorX  -= 1;
					if(ck_selectorX<0) ck_selectorX = 0;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				if((ck_lastInput & GBA_BUTTON_RIGHT) ){
					ck_selectorX  += 1;
					if(ck_selectorX>1) ck_selectorX = 1;
					// Clear the flag
					ck_lastInput = 0;
					ck_ControlPannelDrawn = false;
				}
				ck_localGameState.ck_status_loc = ck_selectorX;
			}
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("change option");

		break;
		case ck_CControls:
			if(ck_lastInput & GBA_BUTTON_UP){
				ck_selectorY  -= 1;
				// Clear the flag
				ck_lastInput = 0;
			}
			if(ck_lastInput & GBA_BUTTON_DOWN){
				ck_selectorY  += 1;
				// Clear the flag
				ck_lastInput = 0;
			}
			if(ck_selectorY < 0) ck_selectorY = 0;
			if(ck_selectorY > 3) ck_selectorY = 3;

			if(ck_lastInput & GBA_BUTTON_LEFT){
				if(ck_selectorY==0)
					if(ck_localGameState.jump_set)
						ck_localGameState.jump_set -= 1;
				if(ck_selectorY==1)
					if(ck_localGameState.pogo_set)
						ck_localGameState.pogo_set -= 1;
				if(ck_selectorY==2)
					if(ck_localGameState.shoot_set)
						ck_localGameState.shoot_set -= 1;
				if(ck_selectorY==3)
					if(ck_localGameState.throw_set)
						ck_localGameState.throw_set -= 1;
				// Clear the flag
				ck_lastInput = 0;
			}
			if(ck_lastInput & GBA_BUTTON_RIGHT){
				if(ck_selectorY==0)
					if(ck_localGameState.jump_set<3) // 7
						ck_localGameState.jump_set += 1;
				if(ck_selectorY==1)
					if(ck_localGameState.pogo_set<3) // 7
						ck_localGameState.pogo_set += 1;
				if(ck_selectorY==2)
					if(ck_localGameState.shoot_set<3) // 7
						ck_localGameState.shoot_set += 1;
				if(ck_selectorY==3)
					if(ck_localGameState.throw_set<3) // 7
						ck_localGameState.throw_set += 1;
				// Clear the flag
				ck_lastInput = 0;
			}

			#define RENDER_SET(set,y) \
				US_TextX = 10; US_TextY = y;\
				LK_US_PrintGlyph(0);\
				US_TextX = 11; US_TextY = y;\
				LK_US_PrintGlyph(0);\
				US_TextX = 10; US_TextY = y;\
				if(set==0){\
					LK_US_PrintGlyph(15); \
				}\
				if(set==1){\
					LK_US_PrintGlyph(16); \
				}\
				if(set==2){\
					LK_US_PrintXY("Rs");\
				}\
				if(set==3){\
					LK_US_PrintXY("Ls");\
				}\
				if(set==4){\
					LK_US_PrintGlyph(88); \
					US_TextX = 11; US_TextY = y;\
					LK_US_PrintGlyph(15);\
				}\
				if(set==5){\
					LK_US_PrintGlyph(88); \
					US_TextX = 11; US_TextY = y;\
					LK_US_PrintGlyph(16);\
				}\
				if(set==6){\
					LK_US_PrintGlyph(102); \
					US_TextX = 11; US_TextY = y;\
					LK_US_PrintGlyph(15);\
				}\
				if(set==7){\
					LK_US_PrintGlyph(102); \
					US_TextX = 11; US_TextY = y;\
					LK_US_PrintGlyph(16);\
				}

			// Draw some UI
			US_TextX = 7; US_TextY = 7;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 7;
			LK_US_PrintXY("Jump");
			RENDER_SET(ck_localGameState.jump_set,8)

			US_TextX = 7; US_TextY = 9;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 9;
			LK_US_PrintXY("Pogo");
			RENDER_SET(ck_localGameState.pogo_set,10)


			US_TextX = 7; US_TextY = 11;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 11;
			LK_US_PrintXY("Shoot");

			RENDER_SET(ck_localGameState.shoot_set,12)

			US_TextX = 7; US_TextY = 13;
			LK_US_PrintGlyph(11);
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXY("Throw");

			RENDER_SET(ck_localGameState.throw_set,14)

			// Get the old glyph
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			ck_selectorGlyph = LK_US_GetGlyph();

			// Draw the selector
			US_TextX = 7; US_TextY = 7+(ck_selectorY<<1);
			LK_US_PrintGlyph(ck_selectorGlyph+1);
/*
 			// Covers up the throw option :(
			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \1 \3 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("change control");
*/
		break;

		default:

			// -------- Normal Menu -----------------

			// Clear the last position
			US_TextX = 7; US_TextY = 7+ck_selectorY;
			LK_US_PrintGlyph(ck_selectorGlyph);

			if(ck_lastInput & GBA_BUTTON_UP){
				ck_selectorY  -= 1;
				// Clear the flag
				ck_lastInput = 0;
			}
			if(ck_lastInput & GBA_BUTTON_DOWN){
				ck_selectorY  += 1;
				// Clear the flag
				ck_lastInput = 0;
			}
			if(ck_lastInput & GBA_BUTTON_A){
				if(ck_ControlPannelMenu->available[ck_selectorY]==1 && ck_ControlPannelMenu->folders[ck_selectorY]!=NULL){
					// Go into the folder
					ck_ControlPannelMenu = ck_ControlPannelMenu->folders[ck_selectorY];
					// Reset some variables
					ck_selectorY = 0;
					ck_selectorGlyph = 11;
					// We need to redraw the scene
					ck_ControlPannelDrawn = false;
					// Clear the flag
					ck_lastInput = 0;
					return;
				}else{
					LK_SD_PlaySound(CK_SND_NO_WAY);
					ck_lastInput = 0;
				}
			}

			if(ck_selectorY < 0){
				ck_selectorY = 0;
			}
			if(ck_selectorY >= ck_ControlPannelMenu->menuLength){
				ck_selectorY = ck_ControlPannelMenu->menuLength-1;
			}

			// Get the old glyph
			US_TextX = 7; US_TextY = 7+ck_selectorY;
			ck_selectorGlyph = LK_US_GetGlyph();

			// Draw the selector
			US_TextX = 7; US_TextY = 7+ck_selectorY;
			LK_US_PrintGlyph(ck_selectorGlyph+1);

			US_TextX = 9; US_TextY = 13;
			LK_US_PrintXYMenu("Use \2 \4 to");
			US_TextX = 8; US_TextY = 14;
			LK_US_PrintXYMenu("select menu");
		break;
	}
};

void LK_US_ResetControlPannel(){
	ck_ControlPannelDrawn = false;
	ck_selectorY = 0;
	ck_selectorX = 0;
	ck_ControlPannelMenu = &ck_FMainMenu;
	ck_selectorGlyph = 11;
	LK_MP_ResetScroll();
};

void LK_US_ResetROM(){
	int i, tmpA;
	// Get rid of any sound
	LK_SD_StopMusic();
	LK_SD_StopSounds();
	
	// Get rid of any sprites
	
	GBA_ResetSprites();
	GBA_UPDATE_SPRITES()
	
	// Fix the menu stuff
	LK_US_ResetControlPannel();

	// Store the current availablility of multiplayer
	tmpA = ck_localGameState.multiplayerAvailable;

	// Set the default game state
	LK_ResetGameState();

	// Try to find any GBAs connected
	//GBA_FindGBAs();

	// Fix connection
	ck_localGameState.multiplayerAvailable = tmpA;
	ck_localGameState.multiplayerGame = false;
	if(GBA_NGameBoysConnected > 1){
		ck_localGameState.num_players = GBA_NGameBoysConnected;
	}

};


