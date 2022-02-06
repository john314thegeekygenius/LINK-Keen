
#ifndef __LK_GAME__
#define __LK_GAME__



typedef struct ck_gameState {
	unsigned char level_id; // What level is selected to play
	unsigned char player_id; // What is the ID of the player 0-3
	unsigned char player_pics[4]; // what player pictures are used
	boolean in_game; // Is the player in a game
	boolean game_over; // Is the round over
	boolean multiplayerAvailable; // Is another GBA connected
	boolean multiplayerGame; // Is the current match a multiplayer one
	short num_players; // How many players are gona play
	unsigned short player_ips[4]; // IP of players

	// Keen stuff
	short ck_shots; // How many shots does the player have
	short ck_lives; // How many lives does the player have
	short ck_bombs; // How many bombs does the player have
	short ck_score; // How many points does the player have

	boolean is_pogoing; // is the player pogoing
	boolean on_ground; // is the player on the ground
	boolean has_shot;  // has the player just shot a shot
	boolean has_thrown; // has the player started throwing
	boolean thrown_bomb; // has the player thrown a bomb

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
#define CK_DATA_PACKET 0x2000
#define CK_OBJ_KEEN_PACKET 0x8000
#define CK_OBJ_SHOT_PACKET 0x8100

extern ck_gameState ck_localGameState;

extern unsigned short ck_randomTick;

extern uint16_t CK_GBA_BUTTON_SET[];

void LK_Init(void);
void LK_InitGame(boolean singleplayer);
void LK_DoGameLoop(void);

#endif

