
#ifndef __LK_SOUND__
#define __LK_SOUND__


// List of CK Sounds


#define CK_SND_WALK 		0
#define CK_SND_JUMP 		1
#define CK_SND_FALL 		2
#define CK_SND_POGO 		3
#define CK_SND_DIE  		4
#define CK_SND_SHOT_HIT 	5
#define CK_SND_KEEN_SHOT 	6
#define CK_SND_KEEN_NO_SHOT 7
#define CK_SND_BAD_SHOT 	8
#define CK_SND_END_LEVEL 	9
#define CK_SND_SWITCH 		10
#define CK_SND_ITEM 		11
#define CK_SND_STUNNER 		12
#define CK_SND_BOMB 		13

#define CK_NUM_SOUNDS 14


void LK_SD_PlaySound(int id);
void LK_SD_StopSounds();
void LK_SD_StopMusic();
void LK_SD_PlayMusic(int id, int loop);


#endif

