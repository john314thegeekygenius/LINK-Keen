/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"


// Local map
uint16_t ck_levelbuff[MAX_LEVEL_WIDTH*MAX_LEVEL_HEIGHT*2];
uint16_t ck_map_animations[MAX_LEVEL_ANIMATIONS*2];

uint16_t * ck_level_data;
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

uint16_t LK_MP_GetTile(uint16_t x, uint16_t y, uint16_t plane){
	if(x<0||y<0||x>=ck_level_width||y>=ck_level_height)
		return 0;
	if(plane==2)
		return ck_level_data[(((y*ck_level_width)+x)*3)+2];
	else
		return ck_levelbuff[(y*ck_level_width)+x+(ck_level_size*plane)];
};

void LK_MP_LoadMap(uint16_t mapid){
	int i = 0, animationID = 0, e = 0;
	int tx = 0, ty = 0, tileid = 0;
	int playersSpawned = 0;
	int playerx = 0, playery = 0;
	int playerx2 = 0, playery2 = 0;
	
	ck_level_data = ck_levels_data[mapid];

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
		ck_levelbuff[i] = ck_level_data[i*3];
		ck_levelbuff[i+ck_level_size] = ck_level_data[(i*3)+1];

		// Spawn in objects
		ty = i/ck_level_width;
		tx = i - (ty*ck_level_width);
		tileid = ck_level_data[(i*3)+2];
		switch(tileid){
			// Players
			case 1:
			LK_SpawnKeen(tx<<3,(ty<<3)-16,0);
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
					LK_SpawnKeen(tx<<3,(ty<<3)-16,1);
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
					LK_SpawnKeen(tx<<3,(ty<<3)-16,2);
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
					LK_SpawnKeen(tx<<3,(ty<<3)-16,3);
					if(GBA_SerialID == 3){
						ck_cam_x = (tx<<3) - (GBA_SCREEN_WIDTH>>1);
						ck_cam_y = (ty<<3) - (GBA_SCREEN_HEIGHT>>1);
					}
					playersSpawned += 1;
				}
			}
			break;
			// Items
			case 5:
				// Stunner
				LK_ACT_SpawnItem(tx<<3,ty<<3,0);
			break;
			case 6:
				// Bomb
				LK_ACT_SpawnItem(tx<<3,ty<<3,1);
			break;
			case 0x2A:
				// Health
				LK_ACT_SpawnItem(tx<<3,ty<<3,2);
			break;
			case 0x20:
			case 0x21:
			case 0x22:
			case 0x23:
//				LK_ACT_SpawnKey(tx<<3,ty<<3,tileid-0x20);
			break;
			case 0x24:
//				LK_ACT_SpawnCandy(tx<<3,ty<<3);
			break;
			case 0x27:
//				LK_ACT_SpawnPixiBlue(tx<<3,ty<<3);
			break;
			
			
		}
		// Add background animations
		if((*ck_tileinfo)[ck_levelbuff[i]]>>9){
			ck_map_animations[(animationID<<1)] = i;
			ck_map_animations[(animationID<<1)+1] = 0; // set time to 0
			animationID ++;
		}
		// Add foreground animations
		if((*ck_tileinfo)[256+(ck_levelbuff[i+ck_level_size]*3)+2]>>9){
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
				LK_SpawnKeen(playerx<<3,(playery<<3)-16,1);
				if(GBA_SerialID == 1){
					ck_cam_x = (playerx<<3) - (GBA_SCREEN_WIDTH>>1);
					ck_cam_y = (playery<<3) - (GBA_SCREEN_HEIGHT>>1);
				}
			}else{
				if(playersSpawned==1){
					// Spawn all players with player 1
					for(i = 1; i < ck_localGameState.num_players; i++){
						LK_SpawnKeen(playerx<<3,(playery<<3)-16,i);
					}
					ck_cam_x = (playerx<<3) - (GBA_SCREEN_WIDTH>>1);
					ck_cam_y = (playery<<3) - (GBA_SCREEN_HEIGHT>>1);

				}else if (playersSpawned==2){
					// Spawn player 3 with player 1
					LK_SpawnKeen(playerx<<3,(playery<<3)-16,2);
					if(GBA_SerialID == 2){
						ck_cam_x = (playerx<<3) - (GBA_SCREEN_WIDTH>>1);
						ck_cam_y = (playery<<3) - (GBA_SCREEN_HEIGHT>>1);
					}
					if(ck_localGameState.num_players==4){
						// Spawn player 4 with player 2
						LK_SpawnKeen(playerx2<<3,(playery2<<3)-16,3);
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
	GBA_DMA_MemSet16((uint16_t*)GBA_BG0_Map,0,32*24);
	GBA_DMA_MemSet16((uint16_t*)GBA_BG1_Map,0,32*24);
	GBA_DMA_MemSet16((uint16_t*)GBA_BG2_Map,0,32*24);

	GBA_DMA_MemSet16((uint16_t*)GBA_BG3_Map,0xD0,32*24);
	
	// Draw the status bar
	if(ck_localGameState.ck_status_type==1){
		int statusx = 1;
		int statusy = 0;
		int val, val1 , val2;
		if(ck_localGameState.ck_status_loc)
			statusy = 18;
		for(e = 0; e < 4; e++){
			GBA_BG3_Map[(statusy*32)+statusx] = 0xE0;
			GBA_BG3_Map[((statusy+1)*32)+statusx] = 0xF0;

			GBA_BG3_Map[(statusy*32)+statusx+3] = 0xE9;
			GBA_BG3_Map[((statusy+1)*32)+statusx+3] = 0xF9;
			GBA_BG3_Map[(statusy*32)+statusx+4] = 0xE9;
			GBA_BG3_Map[((statusy+1)*32)+statusx+4] = 0xF9;
			GBA_BG3_Map[(statusy*32)+statusx+5] = 0xE9;
			GBA_BG3_Map[((statusy+1)*32)+statusx+5] = 0xF9;

			GBA_BG3_Map[(statusy*32)+statusx+6] = 0xEA;
			GBA_BG3_Map[((statusy+1)*32)+statusx+6] = 0xFA;

			GBA_BG3_Map[(statusy*32)+statusx+1] = 0xE1+(e<<1);
			GBA_BG3_Map[((statusy+1)*32)+statusx+1] = 0xF1+(e<<1);
			GBA_BG3_Map[(statusy*32)+statusx+2] = 0xE2+(e<<1);
			GBA_BG3_Map[((statusy+1)*32)+statusx+2] = 0xF2+(e<<1);

			statusx += 7;
		}
	}
	if(ck_localGameState.ck_status_type==2){
		int statusx = 3;
		int statusy = 0;
		int val, val1 , val2;
		if(ck_localGameState.ck_status_loc)
			statusy = 19;
		for(e = 0; e < 4; e++){
			// The icon
			GBA_BG3_Map[(statusy*32)+statusx+1] = 0xD3+e;
			// The number
			GBA_BG3_Map[(statusy*32)+statusx+2] = 0xD2;
			GBA_BG3_Map[(statusy*32)+statusx+3] = 0xD2;
			GBA_BG3_Map[(statusy*32)+statusx+4] = 0xD2;

			statusx += 6;
		}
	}
	

	// Copy the tileset into first block
	GBA_DMA_Copy16((uint16_t*)GBA_BG0_Tiles,(uint16_t*)*ck_level_tileset,24576);

	// Finish the render of the background
	GBA_FINISH_BG0(GBA_BG_BACK | CK_GBA_BLOCK0 | CK_GBA_MAP0 | GBA_BG_SIZE_64x32);

	// Finish the render of the background
	GBA_FINISH_BG1(GBA_BG_MID | CK_GBA_BLOCK1 | CK_GBA_MAP1 | GBA_BG_SIZE_64x32);

	// Finish the render of the background
	GBA_FINISH_BG2(GBA_BG_FRONT | CK_GBA_BLOCK1 | CK_GBA_MAP2 | GBA_BG_SIZE_64x32);

	// Finish the render of the background
	GBA_FINISH_BG3(GBA_BG_TOP | CK_GBA_BLOCK0 | CK_GBA_MAP3 | GBA_BG_SIZE_32x32);
	
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
	*(volatile short*)GBA_REG_BG3HOFS = 0;
	*(volatile short*)GBA_REG_BG3VOFS = 0;
	
	ck_mapneeds_updated = true;
};


const short CK_CAM_SPEED = 4;
//short ck_mapdir = 0;

void LK_MP_UpdateCamera(){

	short keenXloc = globalCK_X - (GBA_SCREEN_WIDTH>>1);
	short keenYloc = globalCK_Y - (GBA_SCREEN_HEIGHT>>1);

//	ck_mapdir = 0;
	if(globalCK_X > (16<<3)+ck_cam_x){
		ck_cam_x += 4;
//		ck_mapneeds_updated = true;
//		ck_mapdir |= 1;
	}
	if(globalCK_X < (14<<3)+ck_cam_x){
		ck_cam_x -= 4;
//		ck_mapneeds_updated = true;
//		ck_mapdir |= 2;
	}

	if(globalCK_Y > (16<<3)+ck_cam_y){
		ck_cam_y += 8;
//		ck_mapneeds_updated = true;
//		ck_mapdir |= 4;
	}
	if(globalCK_Y < (2<<3)+ck_cam_y){
		ck_cam_y -= 8;
//		ck_mapneeds_updated = true;
//		ck_mapdir |= 8;
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

	ck_cam_scrollx = ck_cam_x&0x7;//ck_cam_x - ((ck_cam_x>>3)<<3);
	ck_cam_scrolly = ck_cam_y&0x7;//ck_cam_y - ((ck_cam_y>>3)<<3);

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
			tile += 256;
			infob = (*ck_tileinfo)[tile+2];
			infoc = (*ck_tileinfo)[tile+2];
		}else{
			tile = (ck_levelbuff[tileoff]);
			infob = (*ck_tileinfo)[tile];
			infoc = (*ck_tileinfo)[tile];
		}
		if((infoc>>9) && tv >= (infoc>>9)){
			if(vy<=ck_cam_y+24&&(i-(vy*ck_level_width))<=ck_cam_x+34)
				ck_mapneeds_updated = true;
			ck_levelbuff[tileoff] = (infob)&0x1FF;
			ck_map_animations[(i<<1)+1] -= (infoc>>9);
		}
		ck_map_animations[(i<<1)+1] += 0x0001;
	}
};

int ck_mapbuffer = 0;

void LK_MP_RenderMap(){
	int bgoffset = 0;
	int lvlx = (ck_cam_x>>3);
	int lvly = (ck_cam_y>>3);
	int i = 0, e = 0, mvi = 0;
	uint16_t * ftile = NULL;
	int leveloff = ck_level_width-32;

	if(ck_mapneeds_updated){
		// Swap the map buffer
		if(ck_mapbuffer==0){
			ck_mapbuffer = 32;
		}else{
			ck_mapbuffer = 0;
		}

		ck_mapneeds_updated = false;
		// Copy a region of the map to the screen memory
		bgoffset = (lvly*ck_level_width)+lvlx;
		mvi = (ck_mapbuffer<<5);
		ftile = ( uint16_t*)&ck_levelbuff + (bgoffset + ck_level_size);
		for(i = 0; i < 22; ++i){
			for(e = 0; e < 32; e++){
				*(uint16_t*)(GBA_BG0_Map+mvi) = (uint16_t)ck_levelbuff[bgoffset];
				*(uint16_t*)(GBA_BG1_Map+mvi) = *ftile;
				*(uint16_t*)(GBA_BG2_Map+mvi) = 0;
				if(((*ck_tileinfo)[256+((*ftile)*3)+1]&0x8000))
					*(uint16_t*)(GBA_BG2_Map+mvi) = *ftile;

				++bgoffset;
				++mvi;
				++ftile;
			}
			ftile += leveloff;
			bgoffset += leveloff;
		}
	}

	if(ck_mapbuffer){
		int soff = (ck_cam_scrollx)+(32<<3);
		GBA_WAIT_VBLANK
		*(volatile short*)GBA_REG_BG0HOFS = soff;
		*(volatile short*)GBA_REG_BG1HOFS = soff;
		*(volatile short*)GBA_REG_BG2HOFS = soff;
		
		*(volatile short*)GBA_REG_BG0VOFS = (ck_cam_scrolly);
		*(volatile short*)GBA_REG_BG1VOFS = (ck_cam_scrolly);
		*(volatile short*)GBA_REG_BG2VOFS = (ck_cam_scrolly);
	}else{
		GBA_WAIT_VBLANK
		*(volatile short*)GBA_REG_BG0HOFS = (ck_cam_scrollx);
		*(volatile short*)GBA_REG_BG1HOFS = (ck_cam_scrollx);
		*(volatile short*)GBA_REG_BG2HOFS = (ck_cam_scrollx);
		
		*(volatile short*)GBA_REG_BG0VOFS = (ck_cam_scrolly);
		*(volatile short*)GBA_REG_BG1VOFS = (ck_cam_scrolly);
		*(volatile short*)GBA_REG_BG2VOFS = (ck_cam_scrolly);
	}

	// Update the status bar
	if(!ck_localGameState.update_scorebox)
		return;

	if(ck_localGameState.update_scorebox){
		int statusx = 1;
		int statusy = 0;
		int val, val0, val1 , val2;
		if(ck_localGameState.ck_status_loc)
			statusy = 18;
		ck_localGameState.update_scorebox = false;
		
		if(ck_localGameState.ck_status_type==1){
			for(e = 0; e < 4; e++){

				// Write the correct values
				if(e==0) val = ck_localGameState.ck_lives[GBA_SerialID];
				if(e==1) val = ck_localGameState.ck_health[GBA_SerialID];
				if(e==2) val = ck_localGameState.ck_shots[GBA_SerialID];
				if(e==3) val = ck_localGameState.ck_bombs[GBA_SerialID];
				if(val<0){
					val = -val;
					GBA_BG3_Map[(statusy*32)+statusx+3] = 0xCA;
					GBA_BG3_Map[((statusy+1)*32)+statusx+3] = 0xDA;
				}else{
					GBA_BG3_Map[(statusy*32)+statusx+3] = 0xE9;
					GBA_BG3_Map[((statusy+1)*32)+statusx+3] = 0xF9;
				}
				if(e==1){
					if(val>200) val = 200; // Should never happen
				}else if(e==3){
					if(val>9) val = 9; // Should never happen
				}else{
					if(val>99) val = 99;
				}
				val0 = val / 100;
				val1 = (val-(val0*100))/10;
				val2 = val%10;

				if(e==1){
					if(val0){
						if(val0<5){
							GBA_BG3_Map[(statusy*32)+statusx+3] = 0xCB+val0;
							GBA_BG3_Map[((statusy+1)*32)+statusx+3] = 0xDB+val0;
						}else{
							GBA_BG3_Map[(statusy*32)+statusx+3] = 0xEB+(val0-5);
							GBA_BG3_Map[((statusy+1)*32)+statusx+3] = 0xFB+(val0-5);
						}
					}
				}
				
				if(val1||val0){
					if(val1<5){
						GBA_BG3_Map[(statusy*32)+statusx+4] = 0xCB+val1;
						GBA_BG3_Map[((statusy+1)*32)+statusx+4] = 0xDB+val1;
					}else{
						GBA_BG3_Map[(statusy*32)+statusx+4] = 0xEB+(val1-5);
						GBA_BG3_Map[((statusy+1)*32)+statusx+4] = 0xFB+(val1-5);
					}
				}else{
					GBA_BG3_Map[(statusy*32)+statusx+4] = 0xE9;
					GBA_BG3_Map[((statusy+1)*32)+statusx+4] = 0xF9;
				}
				
				if(val2<5){
					GBA_BG3_Map[(statusy*32)+statusx+5] = 0xCB+val2;
					GBA_BG3_Map[((statusy+1)*32)+statusx+5] = 0xDB+val2;
				}else{
					GBA_BG3_Map[(statusy*32)+statusx+5] = 0xEB+(val2-5);
					GBA_BG3_Map[((statusy+1)*32)+statusx+5] = 0xFB+(val2-5);
				}
				statusx += 7;
			}
		}
		else if(ck_localGameState.ck_status_type==2){
			statusx = 3;
			if(ck_localGameState.ck_status_loc)
				statusy = 19;
				
			for(e = 0; e < 4; e++){
				
				// Write the correct values
				if(e==0) val = ck_localGameState.ck_lives[GBA_SerialID];
				if(e==1) val = ck_localGameState.ck_health[GBA_SerialID];
				if(e==2) val = ck_localGameState.ck_shots[GBA_SerialID];
				if(e==3) val = ck_localGameState.ck_bombs[GBA_SerialID];
				if(val<0){
					val = -val;
					GBA_BG3_Map[(statusy*32)+statusx+2] = 0xD1;
				}else{
					GBA_BG3_Map[(statusy*32)+statusx+2] = 0xD2;
				}
				if(e==1){
					if(val>200) val = 200; // Should never happen
				}else if(e==3){
					if(val>9) val = 9; // Should never happen
				}else{
					if(val>99) val = 99;
				}
				val0 = val / 100;
				val1 = (val-(val0*100))/10;
				val2 = val%10;

				if(e==1){
					if(val0){
						GBA_BG3_Map[(statusy*32)+statusx+2] = 0xC0+val0;
					}
				}
				
				if(val1||val0){
					GBA_BG3_Map[(statusy*32)+statusx+3] = 0xC0+val1;
				}else{
					GBA_BG3_Map[(statusy*32)+statusx+3] = 0xD2;
				}
				
				GBA_BG3_Map[(statusy*32)+statusx+4] = 0xC0+val2;
				statusx += 6;
			}
		}
	}

};

void LK_MP_ResetScroll(){
	GBA_WAIT_VBLANK
	*(volatile short*)GBA_REG_BG0HOFS = (0);
	*(volatile short*)GBA_REG_BG1HOFS = (0);
	*(volatile short*)GBA_REG_BG2HOFS = (0);
	
	*(volatile short*)GBA_REG_BG0VOFS = (0);
	*(volatile short*)GBA_REG_BG1VOFS = (0);
	*(volatile short*)GBA_REG_BG2VOFS = (0);

};



