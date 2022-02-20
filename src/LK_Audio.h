
#ifndef __LK_AUDIO_
#define __LK_AUDIO__

#ifndef LK_MULTIBOOT_ROM

#define LK_SOUND_ENABLED
#define LK_MUSIC_ENABLED

#endif

#ifdef LK_MUSIC_ENABLED
extern const GBA_SoundSample *CK_Music[];
#endif

#ifdef LK_SOUND_ENABLED
extern const GBA_SoundSample *CK_Sounds[CK_NUM_SOUNDS];
#endif


#endif

