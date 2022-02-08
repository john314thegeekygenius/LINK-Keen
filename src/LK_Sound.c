/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"



void LK_SD_PlaySound(int id){
	#ifdef LK_SOUND_ENABLED
	if(ck_localGameState.sound_enabled){
		// We use channel B so that channel A can play music
		GBA_StopChannel(GBA_CHANNEL_B);
		GBA_PlaySample(CK_Sounds[id], GBA_NOLOOP_SAMPLE, GBA_CHANNEL_B);
	}
	#endif
};



void LK_SD_StopSounds(){
	#ifdef LK_SOUND_ENABLED
	GBA_StopChannel(GBA_CHANNEL_B);	
	#endif
};




void LK_SD_PlayMusic(int id, int loop){
	#ifdef LK_MUSIC_ENABLED
	if(ck_localGameState.music_enabled){
		GBA_StopChannel(GBA_CHANNEL_A);
		GBA_PlaySample(CK_Music[id], loop, GBA_CHANNEL_A);
	}
	#endif
};



void LK_SD_StopMusic(){
	#ifdef LK_MUSIC_ENABLED
	GBA_StopChannel(GBA_CHANNEL_A);	
	#endif
};



