/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"


void LK_SD_PlaySound(GBA_SoundSample * sample){
	if(ck_localGameState.sound_enabled){
		// We use channel B so that channel A can mix audio
		GBA_StopChannel(GBA_CHANNEL_B);
		GBA_PlaySample(sample, GBA_NOLOOP_SAMPLE, GBA_CHANNEL_B);
	}
};



void LK_SD_StopSounds(){
	GBA_StopChannel(GBA_CHANNEL_B);	
};




