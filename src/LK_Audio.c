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
#include "CK_Audio/ck_snd_7.h"
#include "CK_Audio/ck_snd_8.h"
#include "CK_Audio/ck_snd_9.h"
#include "CK_Audio/ck_snd_10.h"
#include "CK_Audio/ck_snd_11.h"
#include "CK_Audio/ck_snd_12.h"
#include "CK_Audio/ck_snd_13.h"
#include "CK_Audio/ck_snd_14.h"
#include "CK_Audio/ck_snd_15.h"
#include "CK_Audio/ck_snd_16.h"
#include "CK_Audio/ck_snd_17.h"
#include "CK_Audio/ck_snd_18.h"
#include "CK_Audio/ck_snd_19.h"
#include "CK_Audio/ck_snd_20.h"
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
const GBA_SoundSample CK_Sound_0 = { &ck_snd_0 , ck_snd_0_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_1 = { &ck_snd_1 , ck_snd_1_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_2 = { &ck_snd_2 , ck_snd_2_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_3 = { &ck_snd_3 , ck_snd_3_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_4 = { &ck_snd_4 , ck_snd_4_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_5 = { &ck_snd_5 , ck_snd_5_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_6 = { &ck_snd_6 , ck_snd_6_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_7 = { &ck_snd_7 , ck_snd_7_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_8 = { &ck_snd_8 , ck_snd_8_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_9 = { &ck_snd_9 , ck_snd_9_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_10 = { &ck_snd_10 , ck_snd_10_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_11 = { &ck_snd_11 , ck_snd_11_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_12 = { &ck_snd_12 , ck_snd_12_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_13 = { &ck_snd_13 , ck_snd_13_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_14 = { &ck_snd_14 , ck_snd_14_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_15 = { &ck_snd_15 , ck_snd_15_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_16 = { &ck_snd_16 , ck_snd_16_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_17 = { &ck_snd_17 , ck_snd_17_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_18 = { &ck_snd_18 , ck_snd_18_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_19 = { &ck_snd_19 , ck_snd_19_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };
const GBA_SoundSample CK_Sound_20 = { &ck_snd_20 , ck_snd_20_bytes, GBA_INV_11025, GBA_SAMP_RATE_11025 };

const GBA_SoundSample *CK_Sounds[CK_NUM_SOUNDS] = {
	&CK_Sound_0,
	&CK_Sound_1,
	&CK_Sound_2,
	&CK_Sound_3,
	&CK_Sound_4,
	&CK_Sound_5,
	&CK_Sound_6,
	&CK_Sound_7,
	&CK_Sound_8,
	&CK_Sound_9,
	&CK_Sound_10,
	&CK_Sound_11,
	&CK_Sound_12,
	&CK_Sound_13,
	&CK_Sound_14,
	&CK_Sound_15,
	&CK_Sound_16,
	&CK_Sound_17,
	&CK_Sound_18,
	&CK_Sound_19,
	&CK_Sound_20,
};
#endif


/*
item points CK4
keen switch click
shot hit wall
keen shot snd

*/



