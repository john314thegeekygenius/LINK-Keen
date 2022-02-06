
#ifndef __LK_AUDIO_
#define __LK_AUDIO__


//#define CK_MUSIC_SAMP(n,id) GBA_SoundSample CK_Music_##n## = { &ck_music_##id## , ck_music_ ##id##_bytes, GBA_INV_44100, GBA_SAMP_RATE_44100 };
//#define CK_SOUND_SAMP(n,id) GBA_SoundSample CK_Sound_##n## = { &ck_snd_##id## , ck_snd_##id##_bytes, GBA_INV_22050, GBA_SAMP_RATE_22050 };

//#define EXT_CK_MUSIC_SAMP(n) extern GBA_SoundSample CK_Music_##n 
//#define EXT_CK_SOUND_SAMP(n) extern GBA_SoundSample CK_Sound_##n 

#ifdef LK_MUSIC_ENABLED
extern GBA_SoundSample *CK_Music[];
#endif

extern GBA_SoundSample CK_Sound_Keen_Jump;
extern GBA_SoundSample CK_Sound_Keen_Pogo;
extern GBA_SoundSample CK_Sound_Keen_Die;
extern GBA_SoundSample CK_Sound_Keen_Shot;
extern GBA_SoundSample CK_Sound_Shot_Hit;

/*
item points CK4
keen switch click
shot hit wall
keen shot snd

*/

#endif

