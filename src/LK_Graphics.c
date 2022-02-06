/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"

#include "CK_Data/keen_sheet.h"
#include "CK_Data/mort_sheet.h"
#include "CK_Data/lindsay_sheet.h"
#include "CK_Data/spot_sheet.h"
#include "CK_Data/blaster_sheet.h"

#include "CK_Data/ck5_sprites.h"

const unsigned short ck_graphics_palette [] = {
	0x63f8, 0x5000, 0x280, 0x5280, 0x14, 0x5014, 0x154, 0x5294, 0x294a, 0x794a, 0x2bca, 0x7bca, 0x295e, 0x795e, 0x2bde, 0x7bde, 
	0x63f8, 0x4400, 0x220, 0x4620, 0x11, 0x4411, 0x111, 0x4631, 0x2108, 0x6908, 0x2348, 0x6b48, 0x211a, 0x691a, 0x235a, 0x6b5a, 
	0x63f8, 0x3c00, 0x1e0, 0x3de0, 0xf, 0x3c0f, 0xef, 0x3def, 0x1ce7, 0x5ce7, 0x1ee7, 0x5ee7, 0x1cf7, 0x5cf7, 0x1ef7, 0x5ef7, 
	0x63f8, 0x3400, 0x1a0, 0x35a0, 0xd, 0x340d, 0xcd, 0x35ad, 0x18c6, 0x50c6, 0x1a86, 0x5286, 0x18d4, 0x50d4, 0x1a94, 0x5294, 
	0x63f8, 0x2c00, 0x160, 0x2d60, 0xb, 0x2c0b, 0xab, 0x2d6b, 0x14a5, 0x44a5, 0x1625, 0x4625, 0x14b1, 0x44b1, 0x1631, 0x4631, 
	0x63f8, 0x2800, 0x140, 0x2940, 0xa, 0x280a, 0xaa, 0x294a, 0x14a5, 0x3ca5, 0x15e5, 0x3de5, 0x14af, 0x3caf, 0x15ef, 0x3def, 
	0x63f8, 0x2400, 0x120, 0x2520, 0x9, 0x2409, 0x89, 0x2529, 0x1084, 0x3484, 0x11a4, 0x35a4, 0x108d, 0x348d, 0x11ad, 0x35ad, 
	0x63f8, 0x2000, 0x100, 0x2100, 0x8, 0x2008, 0x88, 0x2108, 0x1084, 0x2c84, 0x1164, 0x2d64, 0x108b, 0x2c8b, 0x116b, 0x2d6b, 
	0x294a, 0x71ad, 0x378d, 0x738d, 0x35bc, 0x71bc, 0x369c, 0x739c, 0x5294, 0x7e94, 0x53f4, 0x7ff4, 0x529f, 0x7e9f, 0x53ff, 0x7fff, 
	0x2108, 0x69ad, 0x374d, 0x6b4d, 0x35ba, 0x69ba, 0x367a, 0x6b5a, 0x4e73, 0x7e73, 0x4ff3, 0x7ff3, 0x4e7f, 0x7e7f, 0x4fff, 0x7fff, 
	0x18c6, 0x61ad, 0x370d, 0x630d, 0x35b8, 0x61b8, 0x3678, 0x6318, 0x4e73, 0x7a73, 0x4fd3, 0x7bd3, 0x4e7e, 0x7a7e, 0x4fde, 0x7bde, 
	0x14a5, 0x5dad, 0x36ed, 0x5eed, 0x35b7, 0x5db7, 0x3657, 0x5ef7, 0x4a52, 0x7252, 0x4b92, 0x7392, 0x4a5c, 0x725c, 0x4b9c, 0x739c, 
	0x14a5, 0x59ad, 0x36cd, 0x5acd, 0x35b6, 0x59b6, 0x3636, 0x5ad6, 0x4631, 0x6a31, 0x4751, 0x6b51, 0x463a, 0x6a3a, 0x475a, 0x6b5a, 
	0x1084, 0x55ad, 0x36ad, 0x56ad, 0x35b5, 0x55b5, 0x3635, 0x56b5, 0x4631, 0x6231, 0x4711, 0x6311, 0x4638, 0x6238, 0x4718, 0x6318, 
	0x842, 0x51ad, 0x368d, 0x528d, 0x35b4, 0x51b4, 0x3614, 0x5294, 0x4210, 0x5e10, 0x42f0, 0x5ef0, 0x4217, 0x5e17, 0x42f7, 0x5ef7, 
	0x0, 0x4dad, 0x366d, 0x4e6d, 0x35b3, 0x4db3, 0x3613, 0x4e73, 0x4210, 0x5a10, 0x42d0, 0x5ad0, 0x4216, 0x5a16, 0x42d6, 0x5ad6, 
};


const uint16_t cki_player1Sheet = 0;
const uint16_t cki_player2Sheet = cki_player1Sheet + (128*24);
const uint16_t cki_player3Sheet = cki_player2Sheet + (128*24);
const uint16_t cki_player4Sheet = cki_player3Sheet + (128*24);
const uint16_t cki_itemsSheet   = 0;//(16*48*2)-(16*32);


const uint16_t cki_nextLineOff = 0x20;


volatile uint16_t* GBA_BG0_Tiles = (volatile uint16_t*)GBA_CHAR_BLOCK(0);
volatile uint16_t* GBA_BG0_Map   = (volatile uint16_t*)GBA_SCREEN_BLOCK(16);

volatile uint16_t* GBA_BG1_Tiles = (volatile uint16_t*)GBA_CHAR_BLOCK(1);
volatile uint16_t* GBA_BG1_Map   = (volatile uint16_t*)GBA_SCREEN_BLOCK(17);

volatile uint16_t* GBA_BG2_Tiles = (volatile uint16_t*)GBA_CHAR_BLOCK(2);
volatile uint16_t* GBA_BG2_Map   = (volatile uint16_t*)GBA_SCREEN_BLOCK(18);

volatile uint16_t* GBA_BG3_Tiles = (volatile uint16_t*)GBA_CHAR_BLOCK(3);
volatile uint16_t* GBA_BG3_Map   = (volatile uint16_t*)GBA_SCREEN_BLOCK(19);

volatile uint16_t* GBA_BG_Palette = (uint16_t*)GBA_PAL_BG_START;



void LK_CA_SetupGraphics(){
	int i ;
	// Setup the video
	*(volatile uint16_t*)GBA_REG_DISPCNT = GBA_MODE_0 | GBA_ENABLE_BG0 | GBA_ENABLE_BG1 | GBA_ENABLE_BG2 | GBA_SPRITE_ENABLE;// | GBA_SPRITE_MAP_1D;

	// Copy the palette
	GBA_DMA_Copy16((uint16_t*)GBA_BG_Palette,(uint16_t*)ck_graphics_palette,GBA_PAL_SIZE);

	// Clear the maps
	for(i = 0; i < 32*32; i++){
		GBA_BG0_Map[i] = 0;
		GBA_BG1_Map[i] = 0;
		GBA_BG2_Map[i] = 0;
	}

	// Stop all backgrounds from showing
	GBA_FINISH_BG0(0);
	GBA_FINISH_BG1(0);
	GBA_FINISH_BG2(0);

	//GBA_BLEND_BGS(GBA_BLD_S_BG1 | GBA_BLDM_APLHA | GBA_BLD_D_BG0)


	// "Hide" all the sprites
	GBA_ResetSprites();

	// Load the sprite data

	// Palette
	GBA_DMA_Copy16((uint16_t*)GBA_PAL_SPR_START,(uint16_t*)ck_graphics_palette,GBA_PAL_SIZE);
	// Data
	GBA_DMA_Copy16((uint16_t*)GBA_SPRGFX_START+cki_itemsSheet,(uint16_t*)ck5_sprites_data,(ck5_sprites_size)>>1);
//	GBA_DMA_Copy16((uint16_t*)GBA_SPRGFX_START+cki_bitsSheet,(uint16_t*)bits_sheet_data,(bits_sheet_size)>>1);
/*
	LK_CA_SetPlayerSprite(cki_player1Sheet,0);
	LK_CA_SetPlayerSprite(cki_player2Sheet,1);
	LK_CA_SetPlayerSprite(cki_player3Sheet,2);
	LK_CA_SetPlayerSprite(cki_player4Sheet,3);
*/	
	// Show the sprite
	GBA_UPDATE_SPRITES()

};

void LK_CA_SetPlayerSprite(int sprId, int sheetId){
	uint16_t *data = NULL;
	return; // This should never be used??
	switch(sheetId){
		case 0:
		data = (uint16_t*)&keen_sheet_data;
		break;
		case 1:
		data = (uint16_t*)&mort_sheet_data;
		break;
		case 2:
		data = (uint16_t*)&lindsay_sheet_data;
		break;
		case 3:
		data = (uint16_t*)&spot_sheet_data;
		break;
		case 4:
		data = (uint16_t*)&blaster_sheet_data;
		break;
		default:
		data = (uint16_t*)&keen_sheet_data;
		break;
	}
	GBA_DMA_Copy16((uint16_t*)GBA_SPRGFX_START+sprId,data,(keen_sheet_size)>>1);
};


int lastSprID[8];

void LK_CA_HackPlayerSprite(int sprId1, int sprId2, int sprId3, int sheetId,int sheetrotation, int pid){
	uint16_t *data = NULL;
	int sprOff = 0;
	switch(pid){
		case 0:
		sprOff = (cki_player1Offset);
		break;
		case 1:
		sprOff = (cki_player2Offset);
		break;
		case 2:
		sprOff = cki_player3Offset;
		break;
		case 3:
		sprOff = cki_player4Offset;
		break;
		default:
		sprOff = (cki_player1Offset);
		break;
	}
	switch(sheetId){
		case 0:
		data = (uint16_t*)&keen_sheet_data;
		break;
		case 1:
		data = (uint16_t*)&mort_sheet_data;
		break;
		case 2:
		data = (uint16_t*)&lindsay_sheet_data;
		break;
		case 3:
		data = (uint16_t*)&spot_sheet_data;
		break;
		case 4:
		data = (uint16_t*)&blaster_sheet_data;
		break;
		default:
		data = (uint16_t*)&keen_sheet_data;
		break;
	}
	// Clear the memory
	
	if(sprId2 != lastSprID[sheetId]){
		lastSprID[sheetId] =  sprId2;
		GBA_DMA_MemSet32((uint16_t*)GBA_SPRGFX_START+sprOff+(32*16),0,32*3);
	}
	GBA_DMA_Copy32((uint16_t*)GBA_SPRGFX_START+sprOff,data+(sprId1*32),32); // Top
	GBA_DMA_Copy32((uint16_t*)GBA_SPRGFX_START+sprOff+(32*16),data+(sprId1*32)+(cki_playerSheetWidth<<2),32); // Middle
	GBA_DMA_Copy32((uint16_t*)GBA_SPRGFX_START+sprOff+(64),data+(sprId2*32)+(cki_playerSheetWidth<<2),32); // Bottom

	if(sheetrotation==1){
		GBA_DMA_Copy32((uint16_t*)GBA_SPRGFX_START+sprOff+(128),data+(sprId3*32),32); // Extra (Horizontal)
	}else{
		GBA_DMA_Copy32((uint16_t*)GBA_SPRGFX_START+sprOff+(128),data+(sprId3*32),16); // Extra (Virtical)
		GBA_DMA_Copy32((uint16_t*)GBA_SPRGFX_START+sprOff+(128)+(32*16),data+(sprId3*32)+32,16); // Extra (Virtical)
	}


};


