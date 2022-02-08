/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"


// Include all the music
#ifdef LK_MUSIC_ENABLED
#include "CK_Audio/ck_music_0.h"
#include "CK_Audio/ck_music_1.h"
#include "CK_Audio/ck_music_2.h"
#endif

#ifdef LK_SOUND_ENABLED
// Include all the sounds
#include "CK_Audio/ck_snd_0.h"
#include "CK_Audio/ck_snd_1.h"
#include "CK_Audio/ck_snd_2.h"
#include "CK_Audio/ck_snd_3.h"
#include "CK_Audio/ck_snd_4.h"
#include "CK_Audio/ck_snd_5.h"
#include "CK_Audio/ck_snd_6.h"
#endif

// List of samples

// Music
#ifdef LK_MUSIC_ENABLED
const GBA_SoundSample CK_Music_0 = { &ck_music_0 , ck_music_0_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Music_1 = { &ck_music_1 , ck_music_1_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Music_2 = { &ck_music_2 , ck_music_2_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };

const GBA_SoundSample *CK_Music[] = {
	&CK_Music_0,
	&CK_Music_1,
	&CK_Music_2,
	&CK_Music_0,
	&CK_Music_0,
};
#endif

// Sounds
#ifdef LK_SOUND_ENABLED
const GBA_SoundSample CK_Sound_Keen_Jump = { &ck_snd_0 , ck_snd_0_bytes, GBA_INV_22050, GBA_SAMP_RATE_22050 };
const GBA_SoundSample CK_Sound_Keen_Pogo = { &ck_snd_1 , ck_snd_1_bytes, GBA_INV_22050, GBA_SAMP_RATE_22050 };
const GBA_SoundSample CK_Sound_Keen_Die  = { &ck_snd_2 , ck_snd_2_bytes, GBA_INV_22050, GBA_SAMP_RATE_22050 };
const GBA_SoundSample CK_Sound_Shot_Hit  = { &ck_snd_5 , ck_snd_5_bytes, GBA_INV_22050, GBA_SAMP_RATE_22050 };
const GBA_SoundSample CK_Sound_Keen_Shot = { &ck_snd_6 , ck_snd_6_bytes, GBA_INV_22050, GBA_SAMP_RATE_22050 };
#else
// We don't play them anyway so...
GBA_SoundSample CK_Sound_Keen_Jump,
				CK_Sound_Keen_Pogo,
				CK_Sound_Keen_Die, 
				CK_Sound_Shot_Hit, 
				CK_Sound_Keen_Shot ;
#endif
const GBA_SoundSample *CK_Sounds[CK_NUM_SOUNDS] = {
	NULL,
	&CK_Sound_Keen_Jump,
	NULL,
	&CK_Sound_Keen_Pogo,
	&CK_Sound_Keen_Die,
	&CK_Sound_Shot_Hit,
	&CK_Sound_Keen_Shot,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};


/*
item points CK4
keen switch click
shot hit wall
keen shot snd

*/



