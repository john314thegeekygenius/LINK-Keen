
#ifndef __LK_GAME__
#define __LK_GAME__



typedef struct ck_gameState {
	// Game info
	unsigned char level_id; // What level is selected to play
	unsigned char player_id; // What is the ID of the player 0-3
	unsigned char player_pics[4]; // what player pictures are used
	unsigned short start_shots; // How many shots do you start with
	unsigned short start_bombs; // How many bombs do you start with
	unsigned short start_health; // How much health do you start with
	unsigned short end_score; // What score wins a match 
	unsigned short end_kills; // How many kills wins a match
	
	// Game state stuff
	boolean in_game; // Is the player in a game
	boolean game_over; // Is the round over
	boolean multiplayerAvailable; // Is another GBA connected
	boolean multiplayerGame; // Is the current match a multiplayer one
	short num_players; // How many players are gona play
	unsigned short player_ips[4]; // IP of players
	boolean player_connected[4]; // Is the player still connected

	// Keen stuff
	short ck_shots[4]; // How many shots does the player have
	short ck_lives[4]; // How many lives does the player have
	short ck_bombs[4]; // How many bombs does the player have
	short ck_score[4]; // How many points does the player have
	short ck_health[4]; // How much health does the player have

	boolean is_pogoing[4]; // is the player pogoing
	boolean on_ground[4]; // is the player on the ground
	boolean has_shot[4];  // has the player just shot a shot
	boolean has_thrown[4]; // has the player started throwing
	boolean thrown_bomb[4]; // has the player thrown a bomb
	
	uint16_t ck_keeninputs[4]; // what buttons are the players pushing
	uint16_t ck_lastkeeninputs[4]; // what buttons were the players pushing

	// Settings
	unsigned char jump_set; // What set to use for jump input
	unsigned char pogo_set; // What set to use for pogo input
	unsigned char shoot_set; // What set to use for shoot input
	unsigned char throw_set; // What set to use for throw input

	boolean music_enabled; // Is the music enabled
	boolean sound_enabled; // Is the sound enabled

}ck_gameState;


// Move to LK_Net.c ??
#define CK_LOCATE_PACKET 0x4401
#define CK_HANDSHAKE_PACKET 0x4403
#define CK_DATA_PACKET 0x2000

#define CK_INPUT_PACKET 0x8000
#define CK_SUCCESS_PACKET 0xC000

#define CK_QUIT_MATCH 0xF0F0
#define CK_ABORT_PACKET 0xA0A0

extern ck_gameState ck_localGameState;

extern unsigned short ck_randomTick;

extern uint16_t CK_GBA_BUTTON_SET[];

void LK_ResetGameState();
void LK_Init(void);
void LK_InitGame(boolean singleplayer);
void LK_DoGameLoop(void);

#endif

