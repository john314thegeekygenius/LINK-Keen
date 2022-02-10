
#ifndef __LK_SOUND__
#define __LK_SOUND__


// List of CK Sounds


#define CK_SND_WALK1 			0
#define CK_SND_WALK2 			1
#define CK_SND_JUMP 			2
#define CK_SND_FALL 			3
#define CK_SND_LAND 			4
#define CK_SND_POGO 			5
#define CK_SND_DIE  			6
#define CK_SND_KEEN_SHOT 		7
#define CK_SND_SHOT_HIT 		8
#define CK_SND_HIT_HEAD 		9
#define CK_SND_CLICK 			10
#define CK_SND_ESHOT 			11
#define CK_SND_ESHOT_HIT 		12
#define CK_SND_END_LEVEL 		13
#define CK_SND_PICKUP_POINT 	14
#define CK_SND_PICKUP_STUNNER 	15
#define CK_SND_PICKUP_SPECIAL 	16
#define CK_SND_PICKUP_HEALTH 	17
#define CK_SND_PICKUP_KEY 		18
#define CK_SND_NO_WAY			19
#define CK_SND_DORR_OPEN		20

#define CK_NUM_SOUNDS 21
#define CK_NUM_SONGS 16

void LK_SD_PlaySound(int id);
void LK_SD_StopSounds();
void LK_SD_StopMusic();
void LK_SD_PlayMusic(int id, int loop);


#endif

