
// Include some things

#include "GBA_Defs.h"



void GBA_Delay(uint32_t ms){
	ms *= 250;
delay_loop:
	asm("nop");
	--ms;
	if(ms)
		goto delay_loop;
};

int GBA_StrLen(char*str){
	int count = 0;
	while(*(str++)){count+=1;}
	return count;
};

uint32_t GBA_RandSeed = 42;

void GBA_SRand(uint32_t s){
	GBA_RandSeed = s;
};

uint32_t GBA_Rand(){
	GBA_RandSeed = 1664525 * GBA_RandSeed + 1013904223;
	return (GBA_RandSeed>>16)&0x7FFF;
};

// Copy data using DMA 
void GBA_DMA_Copy16(uint16_t* dest, uint16_t* source, int amount) {
    *(volatile unsigned int*)GBA_DMA_SRC   = (unsigned int) source;
    *(volatile unsigned int*)GBA_DMA_DEST  = (unsigned int) dest;
    *(volatile unsigned int*)GBA_DMA_COUNT = amount | GBA_DMA_16 | GBA_DMA_ENABLE;
};

void GBA_DMA_Copy32(uint32_t* dest, uint32_t* source, int amount) {
    *(volatile unsigned int*)GBA_DMA_SRC   = (unsigned int) source;
    *(volatile unsigned int*)GBA_DMA_DEST  = (unsigned int) dest;
    *(volatile unsigned int*)GBA_DMA_COUNT = amount | GBA_DMA_32 | GBA_DMA_ENABLE;
};

void GBA_DMA_MemSet16(uint16_t* dest, uint16_t val, int len){
    *(volatile unsigned int*)GBA_DMA_SRC   = (unsigned int) &val;
    *(volatile unsigned int*)GBA_DMA_DEST  = (unsigned int) dest;
    *(volatile unsigned int*)GBA_DMA_COUNT = len | GBA_DMA_16 | GBA_DMA_SRC_FIXED | GBA_DMA_ENABLE;
};

void GBA_DMA_MemSet32(uint32_t* dest, uint32_t val, int len){
    *(volatile unsigned int*)GBA_DMA_SRC   = (unsigned int) &val;
    *(volatile unsigned int*)GBA_DMA_DEST  = (unsigned int) dest;
    *(volatile unsigned int*)GBA_DMA_COUNT = len | GBA_DMA_32 | GBA_DMA_SRC_FIXED | GBA_DMA_ENABLE;
};


// Copy using ASM

void GBA_ASM_Copy16(uint16_t* dest, uint16_t* source, int amount) {
copyloop:
	*(dest++) = *(source++);
	if(--amount)
		goto copyloop;
};

GBA_Sprite GBA_SpriteList[GBA_NUM_SPRITES];
uint16_t GBA_SpriteIndex = 0;


// Moves all the sprites off the screen to hide them
void GBA_ResetSprites(){
	int i;
	for(i = 0; i < GBA_NUM_SPRITES; i++){
		GBA_SpriteList[i].a0 = 0xF0;
		GBA_SpriteList[i].a1 = 0xA0;
		GBA_SpriteList[i].a2 = 0;
	}
	GBA_SpriteIndex = 0;
	GBA_UPDATE_SPRITES()
};


// Removes a sprite (moves it off the screen)
void GBA_RemoveSprite(uint16_t id){
	if(id > 127) return;
	GBA_SpriteList[id].a0 = 0xF0;
	GBA_SpriteList[id].a1 = 0xA0;
	GBA_SpriteList[id].a2 = 0;
	GBA_UPDATE_SPRITES()
};

// Function to create a new sprite in the list
// returns index into sprite array
GBA_SpriteIndex_t GBA_CreateSprite(int x, int y, GBA_SpriteSizes size, uint16_t tileIndex, int zLayer){
	uint16_t size_bits = 0, shape_bits = 0;
	uint16_t index = GBA_SpriteIndex; // Get the current index
	uint16_t palette = 0;
	uint16_t i = 0;

	// Set the sprite attributes
	size_bits = size&0x3;//((int)size%4);
	shape_bits = (size >> 2);

	for(i = 0; i < GBA_SpriteIndex; i++){
		if(GBA_SpriteList[i].a0 == 0xF0 && GBA_SpriteList[i].a1 == 0xA0 && GBA_SpriteList[i].a2 == 0){
			GBA_SpriteList[i].a0 = y | GBA_SPRITE_256 | (shape_bits<<14);
			GBA_SpriteList[i].a1 = x | (size_bits<<14);
			GBA_SpriteList[i].a2 = tileIndex | zLayer | (palette<<12);

			return i;
		}
	}

	++GBA_SpriteIndex; // Increment the index by one
	if(GBA_SpriteIndex>127){
		GBA_SpriteIndex = 127; // lock in place
	}

	GBA_SpriteList[index].a0 = y | GBA_SPRITE_256 | (shape_bits<<14);
	GBA_SpriteList[index].a1 = x | (size_bits<<14);
	GBA_SpriteList[index].a2 = tileIndex | zLayer | (palette<<12);

	return index;
};

void GBA_RemakeSprite(GBA_SpriteIndex_t index, int x, int y, GBA_SpriteSizes size, uint16_t tileIndex, int zLayer){
	uint16_t size_bits = 0, shape_bits = 0;
	uint16_t palette = 0;

	// Set the sprite attributes
	size_bits = size&0x3;//((int)size%4);
	shape_bits = (size >> 2);

	if(index<0||index>127) return; // bad sprite ID
	GBA_SpriteList[index].a0 = y | GBA_SPRITE_256 | (shape_bits<<14);
	GBA_SpriteList[index].a1 = x | (size_bits<<14);
	GBA_SpriteList[index].a2 = tileIndex | zLayer | (palette<<12);
};



unsigned short GBA_GameBoysConnected[4];
unsigned short GBA_NGameBoysConnected = 0;
unsigned short GBA_SerialData[4];
unsigned short GBA_SerialID = 0;
unsigned short GBA_BaudRate = 0;
unsigned char GBA_SerialError = 0;
unsigned short GBA_TimeOutTick = 0;
unsigned short GBA_SerialAvailable = 0;

void GBA_InitSerial(unsigned short baud){
	// Set this to 0 (Enable serial communication)
	*(volatile uint16_t*)GBA_REG_RCNT = 0;

	// Setup the baud rate and com type
	*(volatile uint16_t*)GBA_REG_SCCNT_L = GBA_BaudRate | GBA_COM_MULTI;
	GBA_BaudRate = baud;
	
	GBA_SerialAvailable = 1;

	GBA_FindGBAs();
};


void GBA_FindGBAs(){
	int i;

	// Clear some variables
	GBA_SerialID = 0;
	GBA_SerialError = 0;

	GBA_GameBoysConnected[0] = 
	GBA_GameBoysConnected[1] = 
	GBA_GameBoysConnected[2] = 
	GBA_GameBoysConnected[3] = 0;		

	// Make sure GBAs are connected

	// Wait for all GameBoys to connect
	GBA_TimeOutTick = 0;
	while((*(volatile uint16_t*)GBA_REG_SCCNT_L & GBA_COM_STATUS)==0){
		GBA_TimeOutTick += 1;
		if(GBA_TimeOutTick > GBA_SERIAL_TIMEOUT){
			GBA_TimeOutTick = 0;
			// No link connected
			GBA_SerialAvailable = 0;
			return;
		}
		if(*(volatile uint16_t*)GBA_REG_SCCNT_L & GBA_COM_ERROR){
			// No link connected
			GBA_SerialAvailable = 0;
			return;
		}
	}

	// Put init packet head into register
	*(volatile uint16_t*)GBA_REG_SCCNT_H = 0xAAAA;

	// Send start signal
	*(volatile uint16_t*)GBA_REG_SCCNT_L |= GBA_COM_BUSY; 

	// Wait for transfer to end
	if(GBA_WaitSerial()){

		// Get GBA ID
		GBA_SerialID = (*(volatile uint16_t*)GBA_REG_SCCNT_L & GBA_COM_ID)>>4;

		// Master Gamboy is always connected
		GBA_GameBoysConnected[0] = 1;

		// Send data
		GBA_Serial_SendWord(0x1234);

		GBA_UpdateSerial();

		// Find out who's online
		GBA_NGameBoysConnected = 0;
		for(i = 0; i < 4; i++){
			if(GBA_SerialData[i]==0x1234){
				GBA_GameBoysConnected[i] = 1;
				GBA_NGameBoysConnected += 1;
			}
		}
		GBA_SerialAvailable = 1;
	}

};

short GBA_WaitSerial(void){
	if(!GBA_SerialAvailable) return;
	if(GBA_SerialID != 0){
		// Wait for transfer to start
		while((*(volatile uint16_t*)GBA_REG_SCCNT_L & GBA_COM_BUSY) != GBA_COM_BUSY); 
	}
	// Wait for transfer to end
//	while((*(volatile uint16_t*)GBA_REG_SCCNT_L & GBA_COM_BUSY) == GBA_COM_BUSY);
	GBA_TimeOutTick = 0;
	while((*(volatile uint16_t*)GBA_REG_SCCNT_L & GBA_COM_BUSY) == GBA_COM_BUSY){
		GBA_TimeOutTick += 1;
		if(GBA_TimeOutTick > (GBA_SERIAL_TIMEOUT<<1)){
			GBA_TimeOutTick = 0;
			GBA_SerialError = 1;
			return 0;
		}

		if(*(volatile uint16_t*)GBA_REG_SCCNT_L & GBA_COM_ERROR){
			GBA_SerialError = 1;
			return 0;
		}
	};
	return 1;
};

void GBA_UpdateSerial(void){
	if(!GBA_SerialAvailable) return;
	if(GBA_WaitSerial()){
		// Get the data
		GBA_SerialData[0] = *(volatile uint16_t*)GBA_REG_SCD0;
		GBA_SerialData[1] = *(volatile uint16_t*)GBA_REG_SCD1;
		GBA_SerialData[2] = *(volatile uint16_t*)GBA_REG_SCD2;
		GBA_SerialData[3] = *(volatile uint16_t*)GBA_REG_SCD3;
	}
};

int GBA_Synced = 0;

void GBA_Serial_SendWord(unsigned short word){
	if(!GBA_SerialAvailable) return;

	// Put data into register
	*(volatile uint16_t*)GBA_REG_SCCNT_H = word;

	if(GBA_SerialID == 0){
/*		if(GBA_Synced == 0){
			GBA_Synced = 1;
			GBA_Delay(GBA_S_MAX_WAIT); // Wait for a bit
		}*/
		GBA_Delay(10); // Wait for a bit
		*(volatile uint16_t*)GBA_REG_SCCNT_L |= GBA_COM_BUSY; // Send start signal	
	}
	
	/*
	if(GBA_WaitSerial()){
		// Get the data
		GBA_SerialData[0] = *(volatile uint16_t*)GBA_REG_SCD0;
		GBA_SerialData[1] = *(volatile uint16_t*)GBA_REG_SCD1;
		GBA_SerialData[2] = *(volatile uint16_t*)GBA_REG_SCD2;
		GBA_SerialData[3] = *(volatile uint16_t*)GBA_REG_SCD3;
	}*/
};

void GBA_ClearSerial(){
	// Clear with 0's
	GBA_SerialData[0] = 0x00;
	GBA_SerialData[1] = 0x00;
	GBA_SerialData[2] = 0x00;
	GBA_SerialData[3] = 0x00;

};


//////////// Sound Stuff


int GBA_Channel_A_VBlanks = 0;
int GBA_Channel_A_Samples = 0;
char GBA_Loop_Channel_A = 0;
unsigned char *GBA_Channel_A_Src = (void*)0;

int GBA_Channel_B_VBlanks = 0;
int GBA_Channel_B_Samples = 0;
char GBA_Loop_Channel_B = 0;
unsigned char *GBA_Channel_B_Src = (void*)0;


// Mixer variables
#ifdef GBA_MIX_MY_AUDIO


// structure to store all the frequency settings we allow
typedef struct _FREQ_TABLE
{
	uint16_t timer;
	uint16_t freq;
	uint16_t bufSize;

} FREQ_TABLE;

static const FREQ_TABLE mixerFreqTable[MIX_FREQ_NUM] =
{
	// timer, frequency, and buffer size for frequencies 
	// that time out perfectly with VBlank.
	{ 62610, 5734, 96 }, 
	{ 63940, 10512, 176 }, 
	{ 64282, 13379, 224 }, 
	{ 64612, 18157, 304 }, 
	{ 64738, 21024, 352 }, 
	{ 64909, 26758, 448 }, 
	{ 65004, 31536, 528 }, 
	{ 65073, 36314, 608 }, 
	{ 65118, 40137, 672 }, 
	{ 65137, 42048, 704 }, 
	{ 65154, 43959, 736 }
};



MIXER_CHANNEL mixerChannel[MIX_MAX_CHANNELS];
MIXER_VARS mixerVars;

// This is the actual double buffer memory. The size is taken from 
// the highest entry in the frequency table above.
signed char mixerBuffer[736*2] GBA_IN_EWRAM;

#endif

unsigned int GBA_VSyncCounter = 0;

void GBA_VSyncIRQ() {
	unsigned short temp;
		
	/* disable interrupts for now and save current state of interrupt */
	*(volatile uint16_t*)GBA_INT_ENABLE = 0;

	temp = *(volatile uint16_t*)GBA_INT_STATE;


    // look for vertical refresh 
    if ((*(volatile uint16_t*)GBA_INT_STATE & GBA_INT_VBLANK) == GBA_INT_VBLANK) {
		
		GBA_VSyncCounter += 1; // Update the number of VBlanks

		// Force this here to always mix the audio
//		#ifdef GBA_MIX_MY_AUDIO
	//	GBA_MixAudio();
		//#endif

		// Audio Mixer Stuff
		#ifdef GBA_MIX_MY_AUDIO

		if(mixerVars.activeBuffer == 1)	// buffer 1 just got over
		{
			// Start playing buffer 0
			*(volatile unsigned int*)GBA_DMA1_COUNT = 0;
			*(volatile unsigned int*)GBA_DMA1_SRC = (unsigned int)mixerVars.mixBufferBase;
			*(volatile unsigned int*)GBA_DMA1_COUNT =	GBA_DMA_DEST_FIXED | GBA_DMA_REPEAT | GBA_DMA_32 | 
							GBA_DMA_SNC_TO_TIMER | GBA_DMA_ENABLE;

				// Set the current buffer pointer to the start of buffer 1
			mixerVars.curMixBuffer = mixerVars.mixBufferBase + mixerVars.mixBufferSize;
			mixerVars.activeBuffer = 0;
		}
		else	// buffer 0 just got over
		{
				// DMA points to buffer 1 already, so don't bother stopping and resetting it

				// Set the current buffer pointer to the start of buffer 0
			mixerVars.curMixBuffer = mixerVars.mixBufferBase;
			mixerVars.activeBuffer = 1;
		}
		#else
	
		// Update channel A 
		if (GBA_Channel_A_VBlanks == 0) {
			// restart the sound again when it runs out
			if((GBA_Channel_A_Src!=(void*)0) && GBA_Loop_Channel_A){
				GBA_Channel_A_VBlanks = GBA_Channel_A_Samples;
				*(volatile unsigned int*)GBA_DMA1_COUNT = 0;
				*(volatile unsigned int*)GBA_DMA1_SRC   = (unsigned int) GBA_Channel_A_Src;
				*(volatile unsigned int*)GBA_DMA1_COUNT = GBA_DMA_DEST_FIXED | GBA_DMA_REPEAT | GBA_DMA_32 | GBA_DMA_SNC_TO_TIMER | GBA_DMA_ENABLE;
			}else{
				// Disable the sound and DMA transfer on channel A
				*(volatile uint16_t*)GBA_SOUNDCNT_H &= ~(GBA_DSND_A_RIGHT | GBA_DSND_A_LEFT | GBA_DSND_A_FIFO_RESET | GBA_DSND_TIMER0 | GBA_DSND_A_RATIO);
				*(volatile unsigned int*)GBA_DMA1_COUNT = 0;
			}
		} else {
			--GBA_Channel_A_VBlanks;
		}
		#endif

        // Update channel B 
        if (GBA_Channel_B_VBlanks == 0) {
            // restart the sound again when it runs out
            if(GBA_Channel_B_Src!=(void*)0&&GBA_Loop_Channel_B){
	            GBA_Channel_B_VBlanks = GBA_Channel_B_Samples;
			    *(volatile unsigned int*)GBA_DMA2_COUNT = 0;
				*(volatile unsigned int*)GBA_DMA2_SRC   = (unsigned int) GBA_Channel_B_Src;
				*(volatile unsigned int*)GBA_DMA2_COUNT = GBA_DMA_DEST_FIXED | GBA_DMA_REPEAT | GBA_DMA_32 | GBA_DMA_SNC_TO_TIMER | GBA_DMA_ENABLE;
			}else{
	            // Disable the sound and DMA transfer on channel B
	            *(volatile uint16_t*)GBA_SOUNDCNT_H &= ~(GBA_DSND_B_RIGHT | GBA_DSND_B_LEFT | GBA_DSND_B_FIFO_RESET | GBA_DSND_TIMER1 | GBA_DSND_B_RATIO);
	            *(volatile unsigned int*)GBA_DMA2_COUNT = 0;
			}
        }
        else {
            --GBA_Channel_B_VBlanks;
        }
    }

    // restore/enable interrupts 
    *(volatile uint16_t*)GBA_INT_STATE = temp;
	*(volatile uint16_t*)GBA_INT_ENABLE = 1;
};



void GBA_InitAudio(void){
	// Setup sample stuff
	GBA_Channel_A_VBlanks = 0;
	GBA_Channel_A_Samples = 0;
	GBA_Loop_Channel_A = 0;
	GBA_Channel_A_Src = (void*)0;

	GBA_Channel_B_VBlanks = 0;
	GBA_Channel_B_Samples = 0;
	GBA_Loop_Channel_B = 0;
	GBA_Channel_B_Src = (void*)0;


	// Setup interupt to stop sounds
	*(volatile uint16_t*)GBA_INT_ENABLE = 0;
    *(volatile unsigned int *)GBA_INT_CALLBACK = (unsigned int) &GBA_VSyncIRQ;
    *(volatile uint16_t*)GBA_INT_SELECT |= GBA_INT_VBLANK;
    *(volatile uint16_t*)GBA_DISP_INT |= 0x08;
	*(volatile uint16_t*)GBA_INT_ENABLE = 1;

	// Enable sound
	*(volatile uint16_t*)GBA_SOUNDCNT_X = GBA_SOUND_MASTER_ENABLE;

	GBA_SetGlobalVolume(GBA_VOL_MAX,GBA_VOL_MAX);

	// Enable FIFO audio
	#ifdef GBA_MIX_MY_AUDIO
	*(volatile uint16_t*)GBA_SOUNDCNT_H |= GBA_DSND_A_RIGHT | GBA_DSND_A_LEFT | GBA_DSND_A_FIFO_RESET | GBA_DSND_TIMER0 | GBA_DSND_A_RATIO;
	#endif
};

void GBA_SetGlobalVolume(short L_Volume, short R_Volume){

	*(volatile uint16_t*)GBA_SOUNDCNT_L &= 0xFF00;
	*(volatile uint16_t*)GBA_SOUNDCNT_L |= L_Volume | (R_Volume<<4);

	// I'm not sure what use this is
	*(volatile uint16_t*)GBA_SOUNDCNT_H |= GBA_SOUND_RATIO_100;

};


void GBA_PlaySound(short id){
	//GBA_ResetSound(id);
	switch(id){
		case 1:
			*(volatile uint16_t*)GBA_SOUNDCNT_L |= GBA_SND_1L | GBA_SND_1R;
			break;
		case 2:
			*(volatile uint16_t*)GBA_SOUNDCNT_L |= GBA_SND_2L | GBA_SND_2R;
			break;
		case 3:
			*(volatile uint16_t*)GBA_SOUNDCNT_L |= GBA_SND_3L | GBA_SND_3R;
			break;
		case 4:
			*(volatile uint16_t*)GBA_SOUNDCNT_L |= GBA_SND_4L | GBA_SND_4R;
			break;
	}

};

void GBA_StopSound(short id){
	switch(id){
		case 1:
			*(volatile uint16_t*)GBA_SOUNDCNT_L &= 0xEEFF;
			break;
		case 2:
			*(volatile uint16_t*)GBA_SOUNDCNT_L &= 0xDDFF;
			break;
		case 3:
			*(volatile uint16_t*)GBA_SOUNDCNT_L &= 0xBBFF;
			break;
		case 4:
			*(volatile uint16_t*)GBA_SOUNDCNT_L &= 0x77FF;
			break;
	}
	GBA_ResetSound(id);
};

void GBA_ResetSound(short id){

	switch(id){
		case 1:
			*(volatile uint16_t*)GBA_SOUND1_X |= GBA_RESET_SOUND;
			break;
		case 2:
			*(volatile uint16_t*)GBA_SOUND2_H |= GBA_RESET_SOUND;
			break;
		case 3:
			*(volatile uint16_t*)GBA_SOUND3_X |= GBA_RESET_SOUND;
			break;
		case 4:
			*(volatile uint16_t*)GBA_SOUND4_H |= GBA_RESET_SOUND;
			break;
	}
};

void GBA_SetSoundSqWav(short id){

	// Set the sound to a square wave
	switch(id){
		case 1:
			*(volatile uint16_t*)GBA_SOUND1_L = 0 | GBA_SWEEP_NONE;
			*(volatile uint16_t*)GBA_SOUND1_H = GBA_SOUND1_DUTY_50 | GBA_ENV_0 | (0xF<<12);
//			*(volatile uint16_t*)GBA_SOUND1_L = 6 | GBA_SWEEP_31;
//			*(volatile uint16_t*)GBA_SOUND1_H = GBA_SOUND1_DUTY_50 | GBA_ENV_7 | (0xF<<12);
			break;
		case 2:
			*(volatile uint16_t*)GBA_SOUND2_L = 0 | GBA_SWEEP_NONE | GBA_SOUND1_DUTY_50 | GBA_ENV_0 | (0xF<<12);
			break;
		case 3:
			return; // Cannot play a square wave
			break;
		case 4:
			return; // Cannot play a square wave
			break;
	}

};


void GBA_SetSoundFreq(short id, int freq){
	int f = freq;

	// Set the frequency
	switch(id){
		case 1:
			*(volatile uint16_t*)GBA_SOUND1_X &= 0x7FF;
			*(volatile uint16_t*)GBA_SOUND1_X = f | GBA_RESET_SOUND;
			break;
		case 2:
			*(volatile uint16_t*)GBA_SOUND2_H &= 0x7FF;
			*(volatile uint16_t*)GBA_SOUND2_H = f | GBA_RESET_SOUND;
			break;
		case 3:
			*(volatile uint16_t*)GBA_SOUND3_X &= 0x7FF;
			*(volatile uint16_t*)GBA_SOUND3_X = f;
			break;
		case 4:
			*(volatile uint16_t*)GBA_SOUND4_H &= 0x7FF;
			*(volatile uint16_t*)GBA_SOUND4_H = f;
			break;
	}

};



void GBA_PlaySample(GBA_SoundSample *sample, char loop, char channel){
	if(sample==NULL) return; // Don't play bad samples!
	#ifndef GBA_MIX_MY_AUDIO
	if(channel==GBA_CHANNEL_A){
		GBA_Channel_A_VBlanks = sample->num_samples * sample->sample_rate;
		GBA_Channel_A_Samples = GBA_Channel_A_VBlanks;
		GBA_Loop_Channel_A = loop;
		GBA_Channel_A_Src = sample->sample;

		// Enable FIFO audio
	    *(volatile uint16_t*)GBA_SOUNDCNT_H |= GBA_DSND_A_RIGHT | GBA_DSND_A_LEFT | GBA_DSND_A_FIFO_RESET | GBA_DSND_TIMER0 | GBA_DSND_A_RATIO;

		*(volatile unsigned int*)GBA_DMA1_SRC   = (unsigned int) GBA_Channel_A_Src;
		*(volatile unsigned int*)GBA_DMA1_DEST  = (unsigned int) GBA_FIFO_BUFF_A;
		*(volatile unsigned int*)GBA_DMA1_COUNT = GBA_DMA_DEST_FIXED | GBA_DMA_REPEAT | GBA_DMA_32 | GBA_DMA_SNC_TO_TIMER | GBA_DMA_ENABLE ;

		*(volatile unsigned short*)GBA_TIMER0_DATA = 0x10000 - sample->rate;

		*(volatile unsigned short*)GBA_TIMER0_CONTROL = GBA_TIMER_ENABLE | GBA_TIMER_FREQ_1;
	}
	else 
	#endif
	if(channel==GBA_CHANNEL_B){
		GBA_Channel_B_VBlanks = sample->num_samples * sample->sample_rate;
		GBA_Channel_B_Samples = GBA_Channel_B_VBlanks;
		GBA_Loop_Channel_B = loop;
		GBA_Channel_B_Src = sample->sample;

		// Enable FIFO audio
	    *(volatile uint16_t*)GBA_SOUNDCNT_H |= GBA_DSND_B_RIGHT | GBA_DSND_B_LEFT | GBA_DSND_B_FIFO_RESET | GBA_DSND_TIMER1 | GBA_DSND_B_RATIO;

		*(volatile unsigned int*)GBA_DMA2_SRC   = (unsigned int) GBA_Channel_B_Src;
		*(volatile unsigned int*)GBA_DMA2_DEST  = (unsigned int) GBA_FIFO_BUFF_B;
		*(volatile unsigned int*)GBA_DMA2_COUNT = GBA_DMA_DEST_FIXED | GBA_DMA_REPEAT | GBA_DMA_32 | GBA_DMA_SNC_TO_TIMER | GBA_DMA_ENABLE ;

		*(volatile unsigned short*)GBA_TIMER1_DATA = 0x10000 - sample->rate;

		*(volatile unsigned short*)GBA_TIMER1_CONTROL = GBA_TIMER_ENABLE | GBA_TIMER_FREQ_1;
	}

};

void GBA_StopChannel(char channel){
	
	#ifndef GBA_MIX_MY_AUDIO
	if(channel==GBA_CHANNEL_A){
        // Disable the sound and DMA transfer on channel A
        *(volatile uint16_t*)GBA_SOUNDCNT_H &= ~(GBA_DSND_A_RIGHT | GBA_DSND_A_LEFT | GBA_DSND_A_FIFO_RESET | GBA_DSND_TIMER0 | GBA_DSND_A_RATIO);
        *(volatile unsigned int*)GBA_DMA1_COUNT = 0;

		GBA_Channel_A_VBlanks = 0;
		GBA_Channel_A_Samples = 0;
		GBA_Loop_Channel_A = 0;
		GBA_Channel_A_Src = (void*)0;

	}else 
	#endif
	if(channel==GBA_CHANNEL_B){
        // Disable the sound and DMA transfer on channel B
        *(volatile uint16_t*)GBA_SOUNDCNT_H &= ~(GBA_DSND_B_RIGHT | GBA_DSND_B_LEFT | GBA_DSND_B_FIFO_RESET | GBA_DSND_TIMER1 | GBA_DSND_B_RATIO);
        *(volatile unsigned int*)GBA_DMA2_COUNT = 0;
		GBA_Channel_B_VBlanks = 0;
		GBA_Channel_B_Samples = 0;
		GBA_Loop_Channel_B = 0;
		GBA_Channel_B_Src = (void*)0;
	}
};



/////////// Audio Mixer


void GBA_InitMixer(MIX_FREQ sampleRate){
	#ifdef GBA_MIX_MY_AUDIO

	int i;
	// clear the whole buffer area
	GBA_DMA_MemSet32(mixerBuffer,0, 736/2);

	// initialize main sound variables
	mixerVars.mixBufferSize	= mixerFreqTable[sampleRate].bufSize;
	mixerVars.mixBufferBase	= mixerBuffer;
	mixerVars.curMixBuffer	= mixerVars.mixBufferBase;
	mixerVars.activeBuffer	= 1;	// 1 so first swap will start DMA

	// initialize channel structures
	for(i = 0; i < MIX_MAX_CHANNELS; i++){
		mixerChannel[i].data		= 0;
		mixerChannel[i].pos			= 0;
		mixerChannel[i].inc			= 0;
		mixerChannel[i].vol			= 0;
		mixerChannel[i].length		= 0;
		mixerChannel[i].loopLength	= 0;
	}

	// start up the timer we will be using
	*(volatile unsigned short*)GBA_TIMER0_DATA = mixerFreqTable[sampleRate].timer;
	*(volatile unsigned short*)GBA_TIMER0_CONTROL = GBA_TIMER_ENABLE;

	// set up the DMA settings, but let the VBlank interrupt 
	// actually start it up, so the timing is right
	*(volatile unsigned int*)GBA_DMA1_COUNT = 0;
	*(volatile unsigned int*)GBA_DMA1_DEST = (unsigned int) GBA_FIFO_BUFF_A;

	#endif
};



#ifdef GBA_MIX_MY_AUDIO
// Moved this out of the function
short mixer_tempBuffer[736];
#endif

void GBA_MixAudio(){
	#ifdef GBA_MIX_MY_AUDIO
	int i, currentChannel;

	// zero the buffer
	// Simply do this here so it's faster
	i = 0;
    *(volatile unsigned int*)GBA_DMA_SRC   = (unsigned int) &i;
    *(volatile unsigned int*)GBA_DMA_DEST  = (unsigned int) mixer_tempBuffer;
    *(volatile unsigned int*)GBA_DMA_COUNT = (mixerVars.mixBufferSize>>1) | GBA_DMA_32 | GBA_DMA_SRC_FIXED | GBA_DMA_ENABLE;


	for(currentChannel = 0; currentChannel < MIX_MAX_CHANNELS; currentChannel++)
	{
		// check special active flag value
		if(mixerChannel[currentChannel].data == NULL)
			continue;
		if(mixerChannel[currentChannel].pos < mixerChannel[currentChannel].endLength)
		{
			for(i = 0; i < mixerVars.mixBufferSize; i++)
			{
				// this channel is active, so mix its data into the intermediate buffer
				// mix a sample into the intermediate buffer
				mixer_tempBuffer[i] += mixerChannel[currentChannel].data[ mixerChannel[currentChannel].pos>>12 ];// << mixerChannel[currentChannel].vol;
				mixerChannel[currentChannel].pos += mixerChannel[currentChannel].inc;
			}
		}else{
			for(i = 0; i < mixerVars.mixBufferSize; i++)
			{
				// this channel is active, so mix its data into the intermediate buffer
				// mix a sample into the intermediate buffer
				mixer_tempBuffer[i] += mixerChannel[currentChannel].data[ mixerChannel[currentChannel].pos>>12 ];// << mixerChannel[currentChannel].vol;
				mixerChannel[currentChannel].pos += mixerChannel[currentChannel].inc;

				// loop the sound if it hits the end
				if(mixerChannel[currentChannel].pos >= mixerChannel[currentChannel].length){
					// check special loop on/off flag value
					if(mixerChannel[currentChannel].loopLength == 0)
					{
						// disable the channel and break from the i loop
						mixerChannel[currentChannel].data = NULL;
						i = mixerVars.mixBufferSize;
					}
					else
					{
						// loop back
						mixerChannel[currentChannel].pos = 0;
					}
				}
			}
		}
	}
	// Copy the buffer
    *(volatile unsigned int*)GBA_DMA_SRC   = (unsigned int) mixer_tempBuffer;
    *(volatile unsigned int*)GBA_DMA_DEST  = (unsigned int) mixerVars.curMixBuffer;
    *(volatile unsigned int*)GBA_DMA_COUNT = mixerVars.mixBufferSize | GBA_DMA_16 | GBA_DMA_ENABLE;

/*	
	// now downsample the 16-bit buffer and copy it into the actual playing buffer
	for(i = 0; i < mixerVars.mixBufferSize; i++){
		// >>6 to divide off the volume, >>2 to divide by 4 channels 
		mixerVars.curMixBuffer[i] = mixer_tempBuffer[i];// >> 6;//(6+(MIX_MAX_CHANNELS>>1));
	}*/
	#endif

};

/*

void GBA_MixAudio(){
	int i, currentChannel;

	// zero the buffer
//	GBA_MemSet32(mixer_tempBuffer,0, (mixerVars.mixBufferSize*sizeof(signed short)/4));
	// Simply do this here so it's faster
	i = 0;
    *(volatile unsigned int*)GBA_DMA_SRC   = (unsigned int) &i;
    *(volatile unsigned int*)GBA_DMA_DEST  = (unsigned int) mixer_tempBuffer;
    *(volatile unsigned int*)GBA_DMA_COUNT = (mixerVars.mixBufferSize>>1) | GBA_DMA_32 | GBA_DMA_SRC_FIXED | GBA_DMA_ENABLE;

	for(currentChannel = 0; currentChannel < MIX_MAX_CHANNELS; currentChannel++)
	{
		MIXER_CHANNEL *chnPtr = &mixerChannel[currentChannel];

		// check special active flag value
		if(chnPtr->data != NULL)
		{
			// this channel is active, so mix its data into the intermediate buffer
			for(i = 0; i < mixerVars.mixBufferSize; i++)
			{
				// mix a sample into the intermediate buffer
				mixer_tempBuffer[i] += chnPtr->data[ chnPtr->pos>>12 ] * chnPtr->vol;
				chnPtr->pos += chnPtr->inc;

				// loop the sound if it hits the end
				if(chnPtr->pos >= chnPtr->length)
				{
					// check special loop on/off flag value
					if(chnPtr->loopLength == 0)
					{
						// disable the channel and break from the i loop
						chnPtr->data = NULL;
						i = mixerVars.mixBufferSize;
					}
					else
					{
						// loop back
						while(chnPtr->pos >= chnPtr->length)
						{
							chnPtr->pos -= chnPtr->loopLength;
						}
					}
				}
			}
		}
	}

	// now downsample the 16-bit buffer and copy it into the actual playing buffer
	for(i = 0; i < mixerVars.mixBufferSize; i++){
		// >>6 to divide off the volume, >>2 to divide by 4 channels 
		mixerVars.curMixBuffer[i] = mixer_tempBuffer[i] >> 6;//(6+(MIX_MAX_CHANNELS>>1));
	}
	

};



*/

void GBA_MixerPlaySample(int channel, GBA_SoundSample * sample, char loop){
	#ifdef GBA_MIX_MY_AUDIO
	if(channel<0||channel>=MIX_MAX_CHANNELS)
		return; // Don't play in a channel we can't!
		
	// make sure the channel is disabled first
	mixerChannel[channel].data = NULL;

	// Start at the start
	mixerChannel[channel].pos			= 0;

	// Calculate the increment. 
	// also, it is 12-bit fixed-point, so shift up before the divide
	mixerChannel[channel].inc			= 0x1340;//2<<12;//(8363<<12)/18157;

	// Set the volume to maximum
	mixerChannel[channel].vol			= 6; // 64 = 1<<4

	// The length of the original sample (also 12-bit fixed-point)
	// This will go into our sample info table too
	mixerChannel[channel].length		= sample->num_samples<<12;//10516<<12;
	
	mixerChannel[channel].endLength = mixerChannel[channel].length - (mixerVars.mixBufferSize*mixerChannel[channel].inc);

	// Set the loop length to the special no-loop marker value
	if(loop)
		mixerChannel[channel].loopLength	= (sample->num_samples)<<12;
	else
		mixerChannel[channel].loopLength	= 0;

	// Set the data. This will actually start the channel up so 
	// it will be processed next time GBA_MixAudio() is called
	mixerChannel[channel].data			= (signed char*)sample->sample;
	#endif
};



