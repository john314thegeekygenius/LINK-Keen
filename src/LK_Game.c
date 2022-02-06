/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"

ck_gameState ck_localGameState;

unsigned short ck_randomTick = 0;

uint16_t CK_GBA_BUTTON_SET[] = {
	GBA_BUTTON_A,
	GBA_BUTTON_B,
	GBA_BUTTON_RSHOLDER,
	GBA_BUTTON_LSHOLDER,
	GBA_BUTTON_UP | GBA_BUTTON_A,
	GBA_BUTTON_UP | GBA_BUTTON_B,
	GBA_BUTTON_DOWN | GBA_BUTTON_A,
	GBA_BUTTON_DOWN | GBA_BUTTON_B,
};


void LK_Init(void){
	// Setup a default game state

	ck_localGameState.level_id = 0;
	ck_localGameState.player_id = 0;
	ck_localGameState.player_pics[0] = 0;
	ck_localGameState.in_game = 0;
	ck_localGameState.game_over = 0;
	ck_localGameState.multiplayerAvailable = false;
	ck_localGameState.num_players = 2; // default 2

	ck_localGameState.ck_shots = 0;
	ck_localGameState.ck_lives = 0;
	ck_localGameState.ck_bombs = 0;
	ck_localGameState.ck_score = 0;

	ck_localGameState.is_pogoing = false;
	ck_localGameState.on_ground = false;
	ck_localGameState.has_shot = false;
	ck_localGameState.has_thrown = false;
	

	ck_localGameState.jump_set = 0;
	ck_localGameState.shoot_set = 1;
	ck_localGameState.pogo_set = 2;
	ck_localGameState.throw_set = 3;

	ck_localGameState.music_enabled = true;
	ck_localGameState.sound_enabled = true;

	LK_CA_SetupGraphics();

	GBA_InitAudio();
	GBA_InitSerial(GBA_COM_BAUD_38400);
};

char * waitingtext[3] = {
	"Waiting for players 1 of 3",
	"Waiting for players 2 of 3",
	"Waiting for players 1 of 4",
	"Waiting for players 2 of 4",
	"Waiting for players 3 of 4",
};

void LK_InitGame(boolean singleplayer){
	int i;
	int playerCount = 1; // 1 because we are in the game... right?

	// Our IP is GBA_SerialID
	ck_localGameState.player_ips[0] = GBA_SerialID;
	// Fix image
	ck_localGameState.player_pics[GBA_SerialID] = ck_localGameState.player_pics[0];

	ck_localGameState.multiplayerGame = false;
	
	// Wait for other GBA to connect
	if(ck_localGameState.multiplayerAvailable&&!singleplayer){
		ck_localGameState.multiplayerGame = true;
		// Render a cyan screen with text
		GBA_DMA_MemSet16((uint16_t*)GBA_BG0_Map,0x96,32*32);
		GBA_DMA_MemSet16((uint16_t*)GBA_BG1_Map,0,32*32);
		// Render text
		US_TextX = 1; US_TextY = 8;
		if(ck_localGameState.num_players==2){
			LK_US_TextBox("Waiting for players 1 of 2");
		}else if(ck_localGameState.num_players==3){
			LK_US_TextBox(waitingtext[playerCount]);
		}else if(ck_localGameState.num_players==3){
			LK_US_TextBox(waitingtext[playerCount+2]);
		}

		while(playerCount<ck_localGameState.num_players){
			// Find players
			GBA_UpdateSerial();
			for(i=1;i<ck_localGameState.num_players;i++){
				if(GBA_SerialData[i] == CK_LOCATE_PACKET){
					ck_localGameState.player_ips[playerCount] = i;
					// Found a player!
					playerCount ++;
				}
			}
			// Wait for 2 vblanks
			GBA_WAIT_VBLANK
			// Send data
			GBA_Serial_SendWord(CK_LOCATE_PACKET);
			GBA_WAIT_VBLANK
		};
		GBA_WAIT_VBLANK
		// Send data
		GBA_Serial_SendWord(CK_DATA_PACKET | 
			(ck_localGameState.player_pics[GBA_SerialID]&0xF) | 
			((ck_localGameState.level_id&0xFF)<<4)
			);
		GBA_WAIT_VBLANK
		GBA_Delay(50); // Wait for a bit
		// Collect information
		GBA_UpdateSerial();
		for(i=0;i<ck_localGameState.num_players;i++){
			if((GBA_SerialData[i]&0xF000) == CK_DATA_PACKET){
				ck_localGameState.player_pics[i] = GBA_SerialData[i]&0xF;
				if(i==0)
					ck_localGameState.level_id = (GBA_SerialData[i]>>4)&0xFF;
			}else{
				CK_US_ThrowError("Bad Packet");
			}
		}
	}

	// Reset some things and load the data

	ck_localGameState.ck_shots = 5;
	ck_localGameState.ck_lives = 3;
	ck_localGameState.ck_bombs = 3;
	ck_localGameState.ck_score = 0;

	ck_localGameState.game_over = 0;
	ck_randomTick = 0;

	// Setup the sprites
	LK_SetupObjects();
	GBA_ResetSprites();

	// Load the map
	LK_MP_LoadMap(ck_localGameState.level_id);

	// Start the music
	
#ifdef LK_MUSIC_ENABLED
	GBA_StopChannel(GBA_CHANNEL_A);
	GBA_PlaySample(CK_Music[ck_level_music], GBA_LOOP_SAMPLE, GBA_CHANNEL_A);
#endif

	ck_localGameState.in_game = true;
};

void LK_DoGameLogic(void){

	// Get Input
	LK_IN_ReadInput();
	// Update Map
	LK_MP_UpdateMap();
	// Update Objects
	LK_UpdateObjects();
	// Update Camera
	LK_MP_UpdateCamera();
	// Handle Game State

	// Render Background
	LK_MP_RenderMap();
	// Render Foreground
	// Render Objects
	LK_RenderObjects();
	
};

unsigned int nextVSync = 0;

const int CK_VRBs = 3;
boolean CK_UpdatePackets = false;

void LK_DoGameLoop(void){
	int i;
	nextVSync = GBA_VSyncCounter+CK_VRBs;
	while(1){
		if(GBA_VSyncCounter >= nextVSync){
			if(ck_localGameState.in_game){
				LK_DoGameLogic();
				++ck_randomTick;
			}else{
				LK_US_DrawControlPannel();
			}
			//nextVSync -= GBA_VSyncCounter;
			nextVSync += CK_VRBs;
		}

		GBA_WAIT_VBLANK
		if(GBA_NGameBoysConnected>1){
			ck_localGameState.multiplayerAvailable = true;
		}else{
			ck_localGameState.multiplayerAvailable = false;
		}

		// Update multiplayer
		if(CK_UpdatePackets ==false){
			CK_UpdatePackets = true;
		}else{
			if(ck_localGameState.multiplayerGame){
				GBA_Serial_SendWord(CK_OBJ_KEEN_PACKET | 
				(((ck_keen_objs[GBA_SerialID]->dir_x+1)&0x3)<<6) | (((ck_keen_objs[GBA_SerialID]->dir_y+1)&0x3)<<4));
				GBA_Delay(50); // Wait for a bit
				// Collect information
				GBA_UpdateSerial();
				for(i=0;i<ck_localGameState.num_players;i++){
					if(i==GBA_SerialID) continue; // We don't need our own data
					ck_keen_objs[i]->dir_x = ((GBA_SerialData[i]&0xC0)>>6)-1;
					ck_keen_objs[i]->dir_y = ((GBA_SerialData[i]&0x30)>>4)-1;
				}
				GBA_Delay(50); // Wait for a bit
				GBA_Serial_SendWord(CK_OBJ_KEEN_PACKET | ((ck_keen_objs[GBA_SerialID]->pos_x>>1)));
				GBA_Delay(50); // Wait for a bit
				// Collect information
				GBA_UpdateSerial();
				for(i=0;i<ck_localGameState.num_players;i++){
					if(i==GBA_SerialID) continue; // We don't need our own data
					ck_keen_objs[i]->pos_x = (GBA_SerialData[i]&0xFF)<<1;
				}
				GBA_Delay(50); // Wait for a bit
				GBA_Serial_SendWord(CK_OBJ_KEEN_PACKET | ((ck_keen_objs[GBA_SerialID]->pos_y>>1)));
				GBA_Delay(50); // Wait for a bit
				// Collect information
				GBA_UpdateSerial();
				for(i=0;i<ck_localGameState.num_players;i++){
					if(i==GBA_SerialID) continue; // We don't need our own data
					ck_keen_objs[i]->pos_y = (GBA_SerialData[i]&0xFF)<<1;
				}
			}
		}
	}
};

