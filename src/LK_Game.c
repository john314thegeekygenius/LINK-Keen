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

void LK_EndMatch(){
	int button = 0;
	int i,e;
	int boxX = 2;
	int boxY = 2;
	int boxW = 26;
	int boxH = 16;
	int txtX = 15-(boxW>>1);
	int txtY = 10-(boxH>>1);
	int numconnected = -1;
		
	// Stop the music
	LK_SD_StopMusic();

	// Stop any stray sounds
	LK_SD_StopSounds();
	
	// Play the "Exit Level" sound
	LK_SD_PlaySound(CK_SND_END_LEVEL);
	
	// Get rid of unecesary sprites
	LK_RemoveNonKeenObjs();
	
	LK_US_ResetTiles();

	// Render a cyan screen with text
	GBA_DMA_MemSet16((uint16_t*)GBA_BG0_Map,0x96,32*32);
	GBA_DMA_MemSet16((uint16_t*)GBA_BG1_Map,0,32*32);
	GBA_DMA_MemSet16((uint16_t*)GBA_BG2_Map,0,32*32);
	GBA_DMA_MemSet16((uint16_t*)GBA_BG3_Map,0,32*32);

	LK_MP_ResetScroll();

	{

		
		boxX -= 1;
		boxY -= 1;
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
		US_TextX = boxX + (boxW>>1) - 8;
		US_TextY = boxY + 1;
		
		numconnected = -1;
		for(i = 0; i < ck_localGameState.num_players; i++){
			if(ck_localGameState.player_connected[i]){
				numconnected = i;
			}
		}

		if(ck_localGameState.ck_lives[0] >= ck_localGameState.end_kills || numconnected == 1){
			LK_US_PrintXY("Player 1 Wins");
		}else
		if(ck_localGameState.ck_lives[1] >= ck_localGameState.end_kills || numconnected == 2){
			LK_US_PrintXY("Player 2 Wins");
		}else
		if(ck_localGameState.ck_lives[2] >= ck_localGameState.end_kills || numconnected == 3){
			LK_US_PrintXY("Player 3 Wins");
		}else
		if(ck_localGameState.ck_lives[3] >= ck_localGameState.end_kills || numconnected == 4){
			LK_US_PrintXY("Player 4 Wins");
		}else{
			US_TextX = 6;
			LK_US_PrintXY("Single Match Ended");
		}

	}

	US_TextX = 5;
	US_TextY = 16;
	LK_US_PrintXY("Press A to continue");
	// Draw the player sprites
	ck_cam_x = 0;
	ck_cam_y = 0;
	
	if(ck_localGameState.multiplayerGame){
		for(i = 0; i < ck_localGameState.num_players; i++){
			if(ck_keen_objs[i]!=NULL){
				ck_keen_objs[i]->var1 = 0; // Stop from blinking
				ck_keen_objs[i]->var2 = 0; 
				ck_keen_objs[i]->pos_x = 40+(i*40);
				ck_keen_objs[i]->pos_y = 64;
				ck_keen_objs[i]->ck_render(ck_keen_objs[i]); // Update the sprite positions
			}
			US_TextX = 6 + (i*5);
			US_TextY = 12;
			LK_US_PrintXY(LK_US_Itoa(ck_localGameState.ck_lives[i]));
		}
	}else{
		ck_keen_objs[0]->var1 = 0; // Stop from blinking
		ck_keen_objs[0]->var2 = 0; 
		ck_keen_objs[0]->pos_x = ((GBA_SCREEN_WIDTH>>1)-12);
		ck_keen_objs[0]->pos_y = 64;
		ck_keen_objs[0]->ck_render(ck_keen_objs[0]); // Update the sprite positions
	}
	GBA_UPDATE_SPRITES()

	while((~GBA_BUTTONS));
	while(!((~GBA_BUTTONS)&GBA_BUTTON_A)){
		if(GBA_SerialID==0)
			GBA_UpdateSerial();
	}
	while(((~GBA_BUTTONS)&GBA_BUTTON_A));
	LK_US_ResetROM();

};

#define CK_TIMEOUT_TIMER 0xFFFF

void LK_IntroDemo(){
	int i,e,f, timeout;

	volatile uint16_t* GBA_BG_Map   = (volatile uint16_t*)GBA_SCREEN_BLOCK(20);

	// Set custom tiles (675 tiles)
	// 168 tiles per mapset

	// Copy the tileset into the block
	GBA_DMA_Copy16((uint16_t*)GBA_BG0_Tiles,(uint16_t*)timelabs_data,(43200));

	// Finish the render of the background
	GBA_FINISH_BG0(GBA_BG_TOP | CK_GBA_BLOCK0 | 0x1400 | GBA_BG_SIZE_32x32);

	// Render the screen
	e = 0;
	f = 0;
	for(i = 0; i < 120*180;i++){
		GBA_BG_Map[e++] = i;
		++f;
		if(f==30) {
			f = 0;
			e += 2;
		}
	}
	
	timeout = 0;
	while(!((~GBA_BUTTONS)&GBA_BUTTON_A)){
		++timeout;
		if(timeout>CK_TIMEOUT_TIMER){
			goto done1;
		}
		GBA_WAIT_VBLANK
	}
	while(((~GBA_BUTTONS)&GBA_BUTTON_A)){
		++timeout;
		if(timeout>CK_TIMEOUT_TIMER){
			goto done1;
		}
		GBA_WAIT_VBLANK
	}

done1:
	// Copy the title screen picture
	GBA_DMA_Copy16((uint16_t*)GBA_BG0_Tiles,(uint16_t*)tilescreen_data,(43200));

	// Render the screen (cause it got reset???)
	e = 0;
	f = 0;
	for(i = 0; i < 120*180;i++){
		GBA_BG_Map[e++] = i;
		++f;
		if(f==30) {
			f = 0;
			e += 2;
		}
	}
	
	while(!((~GBA_BUTTONS)&GBA_BUTTON_A));
	while(((~GBA_BUTTONS)&GBA_BUTTON_A));

};


void LK_ResetGameState(){
	int i;
	ck_localGameState.in_game = 0;
	ck_localGameState.game_over = 0;

	for(i = 0; i < 4; i++){
		ck_localGameState.ck_shots[i] = 0;
		ck_localGameState.ck_lives[i] = 0;
		ck_localGameState.ck_bombs[i] = 0;
		ck_localGameState.ck_score[i] = 0;
		ck_localGameState.ck_health[i] = 0;

		ck_localGameState.is_pogoing[i] = false;
		ck_localGameState.on_ground[i] = false;
		ck_localGameState.has_shot[i] = false;
		ck_localGameState.has_thrown[i] = false;
		ck_localGameState.ck_keeninputs[i] = 0;
		ck_localGameState.ck_lastkeeninputs[i] = 0;
	}

};

void LK_NukeGameState(){
	ck_localGameState.level_id = 0;
	ck_localGameState.player_id = 0;
	ck_localGameState.player_pics[0] = 0;
	ck_localGameState.multiplayerAvailable = false;
	ck_localGameState.num_players = 2; // default 2

	ck_localGameState.start_shots = 5;
	ck_localGameState.start_bombs = 0;
	ck_localGameState.start_health = 100;
	ck_localGameState.end_score = 16000;
	ck_localGameState.end_kills = 5;
		
	ck_localGameState.jump_set = 0;
	ck_localGameState.shoot_set = 1;
	ck_localGameState.pogo_set = 2;
	ck_localGameState.throw_set = 3;

	ck_localGameState.ck_status_type = 2; // Make it the smaller one
	ck_localGameState.ck_status_loc = 1;  // and put it on the bottom of the screen

	ck_localGameState.music_enabled = true;
	ck_localGameState.sound_enabled = true;

	ck_localGameState.music_enabled = false; // HACKED: To stop the music from making me go insane

};

void LK_Init(void){
	int  i;

	GBA_InitAudio();
	GBA_InitSerial(GBA_COM_BAUD_9600);

	LK_CA_SetupGraphics();
	LK_IntroDemo();
	
	// Setup a default game state
	LK_ResetGameState();
	LK_NukeGameState();
	LK_US_ResetTiles();
	LK_CA_CopySpriteSheet();
	
	
	ck_localGameState.num_players = GBA_NGameBoysConnected;
};

void LK_InitGame(boolean singleplayer){
	int i,e;
	
	ck_localGameState.multiplayerGame = false;
	
	if(ck_localGameState.multiplayerAvailable&&!singleplayer){
		// Make the game a multiplayer one!
		ck_localGameState.multiplayerGame = true;
		// Wait for other GBA to connect
		if(LK_NT_InitMatch()){
			LK_US_ResetROM();
			return;
		}
	}

	// Reset some things and load the data
	for(i = 0; i < 4; i++){
		ck_localGameState.ck_shots[i] = ck_localGameState.start_shots;
		ck_localGameState.ck_lives[i] = 0; // Would be 3 if normal keen game
		ck_localGameState.ck_bombs[i] = ck_localGameState.start_bombs;
		ck_localGameState.ck_score[i] = 0;
		ck_localGameState.ck_health[i] = ck_localGameState.start_health;
		ck_localGameState.ck_keeninputs[i] = 0;
		ck_localGameState.ck_lastkeeninputs[i] = 0;

		ck_localGameState.is_pogoing[i] = false;
		ck_localGameState.on_ground[i] = false;
		ck_localGameState.has_shot[i] = false;
		ck_localGameState.has_thrown[i] = false;
	
	}
	
	ck_localGameState.update_scorebox = true;
	
	// Assume all players are available
	for(i =0 ; i < ck_localGameState.num_players; i++){
		ck_localGameState.player_connected[i] = true;
	}

	ck_localGameState.game_over = 0;
	ck_randomTick = 0;

	// Setup the sprites
	LK_SetupObjects();
	GBA_ResetSprites();
	
	// Load the map
	LK_MP_LoadMap(ck_localGameState.level_id);

	// Start the music
	LK_SD_PlayMusic(ck_level_music, 1);

	ck_localGameState.in_game = true;
};

void LK_DoGameLogic(void){
	int i, numconnected;
	// Check for special buttons
	if(ck_localGameState.multiplayerGame){
		// Send the current buttons
		LK_NT_Send(CK_INPUT_PACKET | (ck_curInput&0x3FF));

		if(ck_curInput & GBA_BUTTON_START){
			// Send quit packet
			LK_NT_Send(CK_QUIT_MATCH);
		}
		// Update multiplayer
		LK_NT_UpdatePackets();
		if(!ck_localGameState.in_game)
			return;
	}else{
		// Make sure we can quit
		if(ck_curInput & GBA_BUTTON_START){
			ck_localGameState.in_game = false;
			LK_EndMatch();
			return;
		}

	}

	
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
	
	// Check for win state
	numconnected = 0;
	for(i = 0; i < ck_localGameState.num_players; i++){
		if(ck_localGameState.player_connected[i]){
			numconnected += 1;
			if(ck_localGameState.ck_lives[i] >= ck_localGameState.end_kills){
				// Win the match!
				LK_EndMatch();
				return;
			}
		}
	}
	/*
	if(ck_localGameState.multiplayerGame){
		if(numconnected==1){
			// Win the match!
			LK_EndMatch();
			return;
		}
	}*/
};

unsigned int nextVSync = 0;
unsigned int VSyncCount = 0;

const int LK_VRBs = 3;

void LK_DoGameLoop(void){
	int i, updateGame = 0;
	// Changed to relitive because it was causing problems with multiplayer
	if(ck_localGameState.multiplayerGame){
		nextVSync = LK_VRBs; 
	}else{
		nextVSync = GBA_VSyncCounter+LK_VRBs;
	}
	updateGame = 1;
	while(1){
		
		// Move out of vsync?
		// Get Input
		LK_IN_ReadInput();

		if(GBA_NGameBoysConnected>1){
			ck_localGameState.multiplayerAvailable = true;
		}else{
			ck_localGameState.multiplayerAvailable = false;
		}
		
		if(ck_localGameState.multiplayerGame){
			if(VSyncCount++ >= LK_VRBs*0x40){
				updateGame = 1;
				VSyncCount = 0;
			}
		}else{
			if(GBA_VSyncCounter >= nextVSync){
				updateGame = 1;
				nextVSync += LK_VRBs;
			}
		}
		if(updateGame){
			updateGame = 0;
			if(ck_localGameState.in_game){
				LK_DoGameLogic();
				++ck_randomTick;

			}else{
				LK_US_DrawControlPannel();
			}
		}
		if(!ck_localGameState.multiplayerGame){
			GBA_WAIT_VBLANK
		}
		if(GBA_SerialError){
			int d = GBA_SerialError;
			// Uh oh!
			LK_US_ThrowError("Link Error");
			LK_US_ThrowError(LK_US_Htoa16(d));

			ck_localGameState.multiplayerGame = false;
			GBA_NGameBoysConnected = 1;
			GBA_SerialAvailable = 0;
			GBA_SerialError = 0;
			GBA_ClearSerial();
		}


		
	}
};

