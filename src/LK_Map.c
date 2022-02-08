/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"


// Local map
unsigned char ck_levelbuff[MAX_LEVEL_WIDTH*MAX_LEVEL_HEIGHT*2];
uint16_t ck_map_animations[MAX_LEVEL_ANIMATIONS*2];

uint16_t ck_level_width = 32;
uint16_t ck_level_height = 32;
unsigned char **ck_level_tileset = NULL;
uint16_t ck_level_music = 0;
uint16_t **ck_tileinfo;
int ck_level_size = 0;

boolean ck_mapneeds_updated = false;

// Camera stuffs

short ck_cam_minx = 16; // Start with not allowing border
short ck_cam_miny = 16; // Start with not allowing border
int ck_cam_x = 0;
int ck_cam_y = 0;
int ck_last_cam_x = 0;
int ck_last_cam_y = 0;
int ck_cam_scrollx = 0;
int ck_cam_scrolly = 0;

void LK_MP_SetTile(uint16_t x, uint16_t y, uint16_t tile, uint16_t plane){
	if(x<0||y<0||x>=ck_level_width||y>=ck_level_height)
		return;
	ck_levelbuff[(y*ck_level_width)+x+(ck_level_size*plane)] = tile;
	// Yes?
	ck_mapneeds_updated = true;
};

void LK_MP_LoadMap(uint16_t mapid){
	int i = 0, animationID = 0;
	int tx = 0, ty = 0;
	int playersSpawned = 0;
	int playerx = 0, playery = 0;
	int playerx2 = 0, playery2 = 0;
	
	uint16_t *level = ck_levels_data[mapid];

	ck_level_width = ck_levels_width[mapid];
	ck_level_height = ck_levels_height[mapid];
	ck_level_tileset = ck_levels_tileset[mapid];
	ck_level_music = ck_levels_music[mapid];

	ck_tileinfo = &ck_levels_tileinfo[mapid];

	ck_level_size = ck_level_width*ck_level_height;

	GBA_DMA_MemSet16(ck_map_animations,0,MAX_LEVEL_ANIMATIONS*2);

	// Copy the map into the buffer
	animationID = 0;
	for(i = 0; i < ck_level_size; i++){
		ck_levelbuff[i] = level[i*3];
		ck_levelbuff[i+ck_level_size] = level[(i*3)+1];

		// Spawn in objects
		ty = i/ck_level_width;
		tx = i - (ty*ck_level_width);
		switch(level[(i*3)+2]){
			case 1:
			LK_SpawnKeen(tx<<3,(ty<<3)-24,0);
			playerx = tx; playery = ty;
			if(playerx2==0 && playery2==0){
				playerx2 = tx;
				playery2 = ty;
			}
			playersSpawned += 1;
			if(GBA_SerialID == 0){
				ck_cam_x = (tx<<3) - (GBA_SCREEN_WIDTH>>1);
				ck_cam_y = (ty<<3) - (GBA_SCREEN_HEIGHT>>1);
			}
			break;
			case 2:
			if(ck_localGameState.multiplayerGame ){
				if(ck_localGameState.num_players>=2){
					LK_SpawnKeen(tx<<3,(ty<<3)-24,1);
					playerx2 = tx; playery2 = ty;
					playersSpawned += 1;
					if(GBA_SerialID == 1){
						ck_cam_x = (tx<<3) - (GBA_SCREEN_WIDTH>>1);
						ck_cam_y = (ty<<3) - (GBA_SCREEN_HEIGHT>>1);
					}
				}
			}
			break;
			case 3:
			if(ck_localGameState.multiplayerGame ){
				if(ck_localGameState.num_players>=3){
					LK_SpawnKeen(tx<<3,(ty<<3)-24,2);
					if(GBA_SerialID == 2){
						ck_cam_x = (tx<<3) - (GBA_SCREEN_WIDTH>>1);
						ck_cam_y = (ty<<3) - (GBA_SCREEN_HEIGHT>>1);
					}
					playersSpawned += 1;
				}
			}
			break;
			case 4:
			if(ck_localGameState.multiplayerGame ){
				if(ck_localGameState.num_players>=4){
					LK_SpawnKeen(tx<<3,(ty<<3)-24,3);
					if(GBA_SerialID == 3){
						ck_cam_x = (tx<<3) - (GBA_SCREEN_WIDTH>>1);
						ck_cam_y = (ty<<3) - (GBA_SCREEN_HEIGHT>>1);
					}
					playersSpawned += 1;
				}
			}
			break;
		}
		// Add background animations
		if((*ck_tileinfo)[(ck_levelbuff[i]*3)+2]){
			ck_map_animations[(animationID<<1)] = i;
			ck_map_animations[(animationID<<1)+1] = 0; // set time to 0
			animationID ++;
		}
		// Add foreground animations
		if((*ck_tileinfo)[512+(ck_levelbuff[i+ck_level_size]*3)+2]){
			ck_map_animations[(animationID<<1)] = i;
			ck_map_animations[(animationID<<1)+1] = 0x8000; // set time to 0
			animationID ++;
		}
		if(animationID>=MAX_LEVEL_ANIMATIONS) animationID = 0; // Uhhh :o
	}
	
	// Add players if they never spawned! :(
	if(ck_localGameState.multiplayerGame){
		if(playersSpawned<ck_localGameState.num_players){
			if(ck_localGameState.num_players==2){
				// Spawn player 2 with player 1
				LK_SpawnKeen(playerx<<3,(playery<<3)-24,1);
				if(GBA_SerialID == 1){
					ck_cam_x = (playerx<<3) - (GBA_SCREEN_WIDTH>>1);
					ck_cam_y = (playery<<3) - (GBA_SCREEN_HEIGHT>>1);
				}
			}else{
				if(playersSpawned==1){
					// Spawn all players with player 1
					for(i = 1; i < ck_localGameState.num_players; i++){
						LK_SpawnKeen(playerx<<3,(playery<<3)-24,i);
					}
					ck_cam_x = (playerx<<3) - (GBA_SCREEN_WIDTH>>1);
					ck_cam_y = (playery<<3) - (GBA_SCREEN_HEIGHT>>1);

				}else if (playersSpawned==2){
					// Spawn player 3 with player 1
					LK_SpawnKeen(playerx<<3,(playery<<3)-24,2);
					if(GBA_SerialID == 2){
						ck_cam_x = (playerx<<3) - (GBA_SCREEN_WIDTH>>1);
						ck_cam_y = (playery<<3) - (GBA_SCREEN_HEIGHT>>1);
					}
					if(ck_localGameState.num_players==4){
						// Spawn player 4 with player 2
						LK_SpawnKeen(playerx2<<3,(playery2<<3)-24,3);
						if(GBA_SerialID == 3){
							ck_cam_x = (playerx2<<3) - (GBA_SCREEN_WIDTH>>1);
							ck_cam_y = (playery2<<3) - (GBA_SCREEN_HEIGHT>>1);
						}
					}
				}
			}
		}
	}

	// Clear the video maps
	GBA_DMA_MemSet16(GBA_BG0_Map,0,32*24);
	GBA_DMA_MemSet16(GBA_BG1_Map,0,32*24);
	GBA_DMA_MemSet16(GBA_BG2_Map,0,32*24);

	// Copy the tileset into first block
	GBA_DMA_Copy16((uint16_t*)GBA_BG0_Tiles,(uint16_t*)*ck_level_tileset,16384);

	// Finish the render of the background
	GBA_FINISH_BG0(GBA_BG_BACK | CK_GBA_BLOCK0 | CK_GBA_MAP0 | GBA_BG_SIZE_64x64);

	// Finish the render of the background
	GBA_FINISH_BG1(GBA_BG_MID | CK_GBA_BLOCK1 | CK_GBA_MAP1 | GBA_BG_SIZE_64x64);

	// Finish the render of the background
	GBA_FINISH_BG2(GBA_BG_TOP | CK_GBA_BLOCK1 | CK_GBA_MAP2 | GBA_BG_SIZE_64x64);


	// Can't go beyond border!
	ck_cam_minx = 16;
	ck_cam_miny = 16;
	
	ck_cam_scrollx = 0;
	ck_cam_scrolly = 0;

	*(volatile short*)GBA_REG_BG0HOFS = 0;
	*(volatile short*)GBA_REG_BG0VOFS = 0;
	*(volatile short*)GBA_REG_BG1HOFS = 0;
	*(volatile short*)GBA_REG_BG1VOFS = 0;
	*(volatile short*)GBA_REG_BG2HOFS = 0;
	*(volatile short*)GBA_REG_BG2VOFS = 0;
	
	ck_mapneeds_updated = true;

};


const short CK_CAM_SPEED = 4;

void LK_MP_UpdateCamera(){

	short keenXloc = globalCK_X - (GBA_SCREEN_WIDTH>>1);
	short keenYloc = globalCK_Y - (GBA_SCREEN_HEIGHT>>1);

	if(globalCK_X > (16<<3)+ck_cam_x){
		ck_cam_x += 4;
	}
	if(globalCK_X < (14<<3)+ck_cam_x){
		ck_cam_x -= 4;
	}

	if(globalCK_Y > (16<<3)+ck_cam_y){
		ck_cam_y += 4;
	}
	if(globalCK_Y < (2<<3)+ck_cam_y){
		ck_cam_y -= 4;
	}

	if(globalCK_UpdateCam){
		int dist = keenYloc - ck_cam_y;
		if(dist > 12)
			dist = 12;
		if(ck_cam_y < keenYloc){
			ck_cam_y += dist;
		}
		if(ck_cam_y > keenYloc){
			ck_cam_y += dist;
		}
		globalCK_UpdateCam = false;
	}

//	ck_cam_y = globalCK_Y;

	/*
	if(ck_cam_x >= keenXloc - CK_CAM_SPEED && ck_cam_x <= keenXloc + CK_CAM_SPEED)
		ck_cam_x = keenXloc;
		
	if(ck_cam_x < keenXloc)
		ck_cam_x += CK_CAM_SPEED;
	if(ck_cam_x > keenXloc)
		ck_cam_x -= CK_CAM_SPEED;

	if(ck_cam_y >= keenYloc - CK_CAM_SPEED && ck_cam_y <= keenYloc + CK_CAM_SPEED)
		ck_cam_y = keenYloc;

	if(ck_cam_y < keenYloc)
		ck_cam_y += CK_CAM_SPEED;
	if(ck_cam_y > keenYloc)
		ck_cam_y -= CK_CAM_SPEED;
*/

	if(ck_cam_x<ck_cam_minx){
		ck_cam_x = ck_cam_minx;
	}
	if(ck_cam_y<ck_cam_miny){
		ck_cam_y = ck_cam_miny;
	}
	if(ck_cam_x >= (ck_level_width-32)<<3){
		ck_cam_x = (ck_level_width-32)<<3; // Don't go beyond
	}
	if(ck_cam_y >= (ck_level_height-22)<<3){
		ck_cam_y = (ck_level_height-22)<<3; // Don't go beyond
	}

	ck_cam_scrollx = ck_cam_x - ((ck_cam_x>>3)<<3);
	ck_cam_scrolly = ck_cam_y - ((ck_cam_y>>3)<<3);

	if(ck_last_cam_x != (ck_cam_x>>3)){
		ck_last_cam_x = (ck_cam_x>>3);
		ck_mapneeds_updated = true;
	}

	if(ck_last_cam_y != (ck_cam_y>>3)){
		ck_last_cam_y = (ck_cam_y>>3);
		ck_mapneeds_updated = true;
	}
};




void LK_MP_UpdateMap(){
	int i = 0;
	uint16_t infob,infoc;
	uint16_t tile, tileoff;
	uint16_t tx, ty, tv, vy;
	
	for(i = 0; i < MAX_LEVEL_ANIMATIONS; i++){
		tileoff = ck_map_animations[(i<<1)];
		//if(tileoff == 0) return; // Don't do the rest?
		tv = ck_map_animations[(i<<1)+1];
		vy = (tileoff/ck_level_width);
		if(tv&0x8000 == 0x8000){
			tv -= 0x8000;
			tileoff += ck_level_size;
			tile = (ck_levelbuff[tileoff]);
			tile *= 3;
			tile += 512;
			infob = (*ck_tileinfo)[tile+1];
			infoc = (*ck_tileinfo)[tile+2];
		}else{
			tile = (ck_levelbuff[tileoff]);
			tile <<= 1;
			infob = (*ck_tileinfo)[tile];
			infoc = (*ck_tileinfo)[tile+1];
		}
		if(infoc && tv >= infoc){
			if(vy<=ck_cam_y+24&&(i-(vy*ck_level_width))<=ck_cam_x+34)
				ck_mapneeds_updated = true;
			ck_levelbuff[tileoff] = ((infob)>>7)&0xFF;
			ck_map_animations[(i<<1)+1] -= infoc;
		}
		ck_map_animations[(i<<1)+1] += 0x0001;
	}
};

void LK_MP_RenderMap(){
	int bgoffset = 0;
	int lvlx = (ck_cam_x>>3);
	int lvly = (ck_cam_y>>3);
	int i = 0, e = 0, ftile = 0, mvi = 0;
	int leveloff = ck_level_width-32;

	GBA_WAIT_VBLANK
	*(volatile short*)GBA_REG_BG0HOFS = (ck_cam_scrollx);
	*(volatile short*)GBA_REG_BG0VOFS = (ck_cam_scrolly);
	*(volatile short*)GBA_REG_BG1HOFS = (ck_cam_scrollx);
	*(volatile short*)GBA_REG_BG1VOFS = (ck_cam_scrolly);
	*(volatile short*)GBA_REG_BG2HOFS = (ck_cam_scrollx);
	*(volatile short*)GBA_REG_BG2VOFS = (ck_cam_scrolly);

	if(ck_mapneeds_updated){
		ck_mapneeds_updated = false;
		// Copy a region of the map to the screen memory
		bgoffset = (lvly*ck_level_width)+lvlx;
		for(i = 0; i < 22; ++i){
			for(e = 0; e < 32; e++){
				mvi = (i<<5)+e;
				*(uint16_t*)(GBA_BG0_Map+mvi) = ck_levelbuff[bgoffset];
				ftile = ck_levelbuff[bgoffset+ck_level_size];
				*(uint16_t*)(GBA_BG1_Map+mvi) = ftile;
				if(((*ck_tileinfo)[512+(ftile*3)+1]&0x8000)!=0x8000 )
					ftile = 0;
				*(uint16_t*)(GBA_BG2_Map+mvi) = ftile;
				++bgoffset;
			}
			bgoffset += leveloff;
		}
	}

};





