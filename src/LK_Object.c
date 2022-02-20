/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"

#include "LK_GFX.h"


//--------------- General Physics Functions ---------------------------


short ck_slopevalues[8][8] = {
	{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}, // Solid top 
	{0x1,0x1,0x2,0x2,0x3,0x3,0x4,0x4}, // Top > Middle 
	{0x5,0x5,0x6,0x6,0x7,0x7,0x8,0x8}, // Middle > Bottom 
	{0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8}, // Top > Bottom 
	{0x4,0x4,0x3,0x3,0x2,0x2,0x1,0x1}, // Middle > Top 
	{0x8,0x8,0x7,0x7,0x6,0x6,0x5,0x5}, // Bottom > Middle 
	{0x8,0x7,0x6,0x5,0x4,0x3,0x2,0x1}, // Bottom > Top 
};


// For physcollideobj()
ck_animation ck_BombRemove;

void LK_PhysCollideObj(ck_object *obj,ck_object *obj_hit){
	if(obj->type==ck_objShot && obj_hit->type==ck_objPlayer){
		if(ck_localGameState.ck_health[obj_hit->var4]>0){
			if(obj->var4 != obj_hit->var4){
				if(obj->accel_x != 0 || obj->accel_y != 0){
					if(obj_hit->var1==0){
						// Hurt the player
						ck_localGameState.ck_health[obj_hit->var4] -= 40;
						if(ck_localGameState.ck_health[obj_hit->var4]<0){
							ck_localGameState.ck_lives[obj->var4] += 1; // Add a kill
						}
						ck_localGameState.update_scorebox = true;
					}
					obj_hit->var1 = 0x10;
					obj->accel_x = 0;
					obj->accel_y = 0;					
				}
			}
		}
	}

	if(obj->type==ck_objBomb && obj_hit->type==ck_objPlayer){
		if(ck_localGameState.ck_health[obj_hit->var4]>0){
			if(obj->var4 != obj_hit->var4){
				if(obj->state == ck_objKill){
					if(obj_hit->var1==0){
						// Hurt the player
						ck_localGameState.ck_health[obj_hit->var4] -= 50;
						if(ck_localGameState.ck_health[obj_hit->var4]<0){
							ck_localGameState.ck_lives[obj->var4] += 1; // Add a kill
						}
						obj_hit->var1  = 0x10; // Make the player flash
						ck_localGameState.update_scorebox = true;
					}
					obj->var2 = 2;
					obj->accel_x = 0;
					obj->accel_y = 0;
					obj->state = ck_objIdle; // It's not a deadly thihng anymore
					obj->ck_frame = &ck_BombRemove;
				}else{
					// Explode the bomb
					if(obj->var2==0)
						obj->var2 = 1;
				}
			}
		}
	}
};


void LK_MapCollideObj(ck_object *obj){

	// Variables for later
	uint16_t tileTI, TIclipX, TIclipY,TIclipY2;
	const short mx = obj->pos_x+obj->clip_rects.clipX+(obj->clip_rects.clipW>>1);
	int m;
	
    // Collide Top
    if(obj->accel_y > 0){
		
		TIclipY = (obj->pos_y+obj->clip_rects.clipY+obj->clip_rects.clipH+obj->accel_y)>>3;
		TIclipY2 = TIclipY-1;

		for(m = 0; m < 2; m++){
			// Check for every slope in hitbox
			TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)+( (mx&0x07) < 0x04);
			
			if(TIclipX>=0 && TIclipY2>=0 && TIclipX < ck_level_width && TIclipY2 < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY2*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)]&0x7F;
				
				// Clip with sloped top
				if(tileTI>=0x02&&tileTI<=0x07){
					uint16_t dy = (mx)&0x07;
					obj->pos_y = (TIclipY2<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY-ck_slopevalues[tileTI-1][dy]);
					obj->accel_y = 0;
					break; // Get out of loop
				}
			}

			TIclipY2 += 1;
			
			// Check for every tile in hitbox
			TIclipX = m+((obj->pos_x+obj->clip_rects.clipX)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)]&0x7F;
				
				// Clip with solid top
				if(tileTI==0x01){
                    uint16_t dy = (mx)&0x07;
                    obj->pos_y = (TIclipY<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY-ck_slopevalues[tileTI-1][dy]);
                    obj->accel_y = 0;
					break; // Get out of loop
				}
			}
			
		}

    }


    // Collide Bottom
    if(obj->accel_y < 0){
		
		TIclipY = (obj->pos_y+obj->clip_rects.clipY+obj->accel_y)>>3;
		TIclipY2 = TIclipY;

		for(m = 0; m < 2; m++){
			// Check for every slope in hitbox
			TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)+( (mx&0x07) < 0x04);
			
			if(TIclipX>=0 && TIclipY2>=0 && TIclipX < ck_level_width && TIclipY2 < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY2*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)+1]&0x7F;
				
				// Clip with sloped bottom
				if(tileTI>=0x02&&tileTI<=0x07){
					uint16_t dy = (mx)&0x07;
					obj->pos_y = (TIclipY2<<3)+8-(obj->clip_rects.clipY+ck_slopevalues[tileTI-1][dy]);
					obj->accel_y = 0;
					break; // Get out of loop
				}
			}

			TIclipY2 += 1;
			
			// Check for every tile in hitbox
			TIclipX = m+((obj->pos_x+obj->clip_rects.clipX)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)+1]&0x7F;
				
				// Clip with solid bottom
				if(tileTI==0x01){
					obj->pos_y = (TIclipY<<3)+8-(obj->clip_rects.clipY);
                    obj->accel_y = 0;
					break; // Get out of loop
				}
			}
			
		}

    }

	// Collide Right

    if(obj->accel_x > 0){
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX+obj->clip_rects.clipW)>>3);

		for(m = 0; m < (obj->clip_rects.clipH>>3); m++){
			// Check for every slope in hitbox
			TIclipY = m+(obj->pos_y+obj->clip_rects.clipY)>>3;
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)];
				
				// Clip with right wall
				if(tileTI&0x8000){
					obj->pos_x = (TIclipX<<3)-(obj->clip_rects.clipX+obj->clip_rects.clipW);
					obj->accel_x = 0;
					break; // Get out of loop
				}
			}
		}
	}
    
	// Collide Left

    if(obj->accel_x < 0){
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)-1;

		for(m = 0; m < (obj->clip_rects.clipH>>3); m++){
			// Check for every slope in hitbox
			TIclipY = m+(obj->pos_y+obj->clip_rects.clipY)>>3;
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)];
				
				// Clip with left wall
				if(tileTI&0x4000){
					obj->pos_x = (TIclipX<<3)+8-(obj->clip_rects.clipX);
					obj->accel_x = 0;
					break; // Get out of loop
				}
			}
		}
	}
};

void LK_UpdateObj(ck_object *obj){
	short absAccelY = GBA_ABS(obj->accel_y);
	short oldAccelY;
	int a;
	if(absAccelY>=8){
		oldAccelY = obj->accel_y;
		if(oldAccelY<0)
			obj->accel_y = -4; // Make it more manageable!
		else
			obj->accel_y = 4; 
		for(a = 0; a < absAccelY; a += 4){
			LK_MapCollideObj(obj);
			obj->pos_y += obj->accel_y;
		}
		if(obj->accel_y!=0){
			obj->accel_y = oldAccelY;
		}
	}else{
		LK_MapCollideObj(obj);
		obj->pos_y += obj->accel_y;
	}

	obj->pos_x += obj->accel_x;
};

//------------------------------------------ BOMB ANIMATIONS ---------------------------------

ck_animation 	ck_BombAnimation,
				ck_BombExplode,
				ck_BombRemove;

ck_animation ck_BombAnimation = {
	{CK_BOMB_PIC,-1,-1},
	{ {0,0},},
	0xF0,
	&ck_BombAnimation
};

ck_animation ck_BombExplode = {
	{CK_BOMB_EXPLODE,-1,-1},
	{ {0,0},},
	0x08,
	&ck_BombRemove
};

ck_animation ck_BombRemove = {
	{CK_BOMB_EXPLODE,-1,-1},
	{ {0,0},},
	0xF0,
	&ck_BombRemove
};


void LK_MapCollideBomb(ck_object *obj){

	// Variables for later
	uint16_t tileTI, TIclipX, TIclipY,TIclipY2;
	const short mx = obj->pos_x+obj->clip_rects.clipX+(obj->clip_rects.clipW>>1);
	int m;
	
    // Collide Top
    if(obj->accel_y > 0){
		
		TIclipY = (obj->pos_y+obj->clip_rects.clipY+obj->clip_rects.clipH+obj->accel_y)>>3;
		TIclipY2 = ((obj->pos_y+12+obj->accel_y)>>3)-1;

		for(m = 0; m < 2; m++){
			// Check for every slope in hitbox
			TIclipX = ((obj->pos_x+4)>>3)+( (mx&0x07) < 0x04);
			
			if(TIclipX>=0 && TIclipY2>=0 && TIclipX < ck_level_width && TIclipY2 < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY2*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)]&0x7F;
				
				// Clip with sloped top
				if(tileTI>=0x02&&tileTI<=0x07){
					uint16_t dy = (mx)&0x07;
					obj->pos_y = (TIclipY2<<3)-(12-ck_slopevalues[tileTI-1][dy]);
                    obj->accel_y = -(obj->accel_y/2);
                    obj->accel_x = -(obj->accel_x/2);
					break; // Get out of loop
				}
			}

			TIclipY2 += 1;
			
			// Check for every tile in hitbox
			TIclipX = m+((obj->pos_x+obj->clip_rects.clipX)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)]&0x7F;
				
				// Clip with solid top
				if(tileTI==0x01){
                    uint16_t dy = (mx)&0x07;
                    //TIclipY -= 1;
                    obj->pos_y = (TIclipY<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY-ck_slopevalues[tileTI-1][dy]);
                    obj->accel_y = -(obj->accel_y/2);
                    obj->accel_x /= 2;
					break; // Get out of loop
				}
			}
			
		}

    }
    
    // Collide Bottom
    if(obj->accel_y < 0){
		
		TIclipY = (obj->pos_y+obj->clip_rects.clipY+obj->accel_y)>>3;
		TIclipY2 = TIclipY;

		for(m = 0; m < 2; m++){
			// Check for every slope in hitbox
			TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)+( (mx&0x07) < 0x04);
			
			if(TIclipX>=0 && TIclipY2>=0 && TIclipX < ck_level_width && TIclipY2 < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY2*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)+1]&0x7F;
				
				// Clip with sloped bottom
				if(tileTI>=0x02&&tileTI<=0x07){
					uint16_t dy = (mx)&0x07;
					obj->pos_y = (TIclipY2<<3)+8-(obj->clip_rects.clipY+ck_slopevalues[tileTI-1][dy]);
					obj->accel_y = -(obj->accel_y/2);
                    obj->accel_x = -(obj->accel_x/2);
					break; // Get out of loop
				}
			}

			TIclipY2 += 1;
			
			// Check for every tile in hitbox
			TIclipX = m+((obj->pos_x+obj->clip_rects.clipX)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)+1]&0x7F;
				
				// Clip with solid bottom
				if(tileTI==0x01){
					obj->pos_y = (TIclipY<<3)+8-(obj->clip_rects.clipY);
                    obj->accel_y = -(obj->accel_y/2);
					break; // Get out of loop
				}
			}
			
		}

    }

	// Collide Right

    if(obj->accel_x > 0){
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX+obj->clip_rects.clipW)>>3);

		for(m = 0; m < (obj->clip_rects.clipH>>3); m++){
			// Check for every slope in hitbox
			TIclipY = m+((obj->pos_y+obj->clip_rects.clipY)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)];
				
				// Clip with right wall
				if(tileTI&0x8000){
					obj->pos_x = (TIclipX<<3)-(obj->clip_rects.clipX+obj->clip_rects.clipW);
                    obj->accel_x = -(obj->accel_x/2);
					break; // Get out of loop
				}
			}
		}
	}
    
	// Collide Left

    if(obj->accel_x < 0){
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)-1;

		for(m = 0; m < (obj->clip_rects.clipH>>3); m++){
			// Check for every slope in hitbox
			TIclipY = m+((obj->pos_y+obj->clip_rects.clipY)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)];
				
				// Clip with left wall
				if(tileTI&0x4000){
					obj->pos_x = (TIclipX<<3)+8-(obj->clip_rects.clipX);
                    obj->accel_x = -(obj->accel_x/2);
					break; // Get out of loop
				}
			}
		}
	}
};

void LK_UpdateBomb(ck_object *obj){
	if(obj->ck_frame == &ck_BombAnimation){
		if(obj->accel_y>=6){
			obj->accel_y = 6;
		}
		if(obj->accel_y<=-6){
			obj->accel_y = -6;
		}
		
		LK_MapCollideBomb(obj);
		obj->pos_y += obj->accel_y;

		obj->pos_x += obj->accel_x;
	}
};



void LK_DrawBomb(ck_object *obj){
	short sprX = obj->ck_frame->spr_offs[0][0] + obj->pos_x - ck_cam_x;
	short sprY = obj->ck_frame->spr_offs[0][1] + obj->pos_y - ck_cam_y;

	// Render the sprite
	GBA_SET_SPRITE_POSITION(obj->spr_id1, sprX, sprY);

	GBA_SET_SPRITE_TILES(obj->spr_id1, obj->ck_frame->offset[0]);
};

void LK_LogicBomb(ck_object *obj){

	obj->var1 += 1;

	if(obj->var2 == 1){
		obj->accel_y = 0;
		obj->accel_x = 0;
	}else{
		obj->accel_y += 1;
	}

	// explode the bomb
	if(obj->var1 == 0x40){
		obj->var2 = 1;
	}
	if(obj->var2==1){
		obj->var2 = 2;
		obj->animation_tick = 0;
		obj->ck_frame = &ck_BombExplode;
		obj->state = ck_objKill;
		obj->accel_x = obj->accel_y = 0;
		LK_SD_PlaySound(CK_SND_SHOT_HIT);
	}

	if(obj->ck_frame == &ck_BombRemove){
		GBA_SET_SPRITE_CLEAR(obj->spr_id1);
		GBA_SET_SPRITE_POSITION(obj->spr_id1, 0xA0, 0xF0);
		obj->ck_frame = NULL;
	}
};

void LK_SpawnBomb(int x,int y, int dir_x, int dir_y, ck_object * keenobj){
	ck_object * obj = LK_GetNewObj(0);

	obj->pos_x = x;
	obj->pos_y = y;

	obj->accel_x = dir_x*3;
	if(keenobj->accel_x>0)
		obj->accel_x += 3;
	if(keenobj->accel_x<0)
		obj->accel_x -= 3;

	obj->accel_y = -6;
	if(dir_y==CK_Dir_Down){
		obj->accel_x = 0;
		obj->accel_y = 6;
	}else{
		obj->accel_y += 3*dir_y;
	}

	obj->dir_x = dir_x;
	obj->dir_y = dir_y;

	obj->state = ck_objIdle;
	obj->type = ck_objBomb;

	obj->var1 = obj->var2 = obj->var3 = obj->var4 = 0;
	
	
	obj->var1 = 0; // bomb timer
	obj->var2 = 0; // should bomb explode
	obj->var4 = keenobj->var4; // obj id

	obj->clip_rects.clipX = 2;
	obj->clip_rects.clipY = 2;
	obj->clip_rects.clipW = 12;
	obj->clip_rects.clipH = 12;

#ifdef LK_MULTIBOOT_ROM
	if(obj->spr_id1==128)
		obj->spr_id1 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,0);
	else
		GBA_RemakeSprite(obj->spr_id1,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,0);
#else
	if(obj->spr_id1==128)
		obj->spr_id1 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,-1);
	else
		GBA_RemakeSprite(obj->spr_id1,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,-1);
#endif
	GBA_SET_FLIPH(obj->spr_id1,0)
	GBA_SET_FLIPV(obj->spr_id1,0)

	obj->animation_tick = 0;
	obj->ck_frame = &ck_BombAnimation;
	obj->ck_render = &LK_DrawBomb;
	obj->ck_input = &LK_LogicBomb;
	obj->ck_collide_obj = &LK_PhysCollideObj;
	obj->ck_update_obj = &LK_UpdateBomb;
};


//------------------------------------------ SHOT ANIMATIONS ---------------------------------

ck_animation 	ck_ShotAnimation,
				ck_ShotSplat,
				ck_ShotRemove;

ck_animation ck_ShotAnimation = {
	{CK_SHOT_PIC},
	{ {0,0},},
	0xF0,
	&ck_ShotAnimation
};

ck_animation ck_ShotSplat = {
	{CK_SHOT_SPLAT},
	{ {0,0},},
	0x08,
	&ck_ShotRemove
};

ck_animation ck_ShotRemove = {
	{CK_SHOT_SPLAT},
	{ {0,0},},
	0x01,
	NULL
};




void LK_MapCollideShot(ck_object *obj){

	// Variables for later
	uint16_t tileTI, TIclipX, TIclipY,TIclipY2;
	const short mx = obj->pos_x+obj->clip_rects.clipX+(obj->clip_rects.clipW>>1);
	int m;
	
    // Collide Top
    if(obj->accel_y >= 0){
		
		TIclipY = (obj->pos_y+obj->clip_rects.clipY+obj->clip_rects.clipH+obj->accel_y)>>3;
		TIclipY2 = TIclipY-1;

		for(m = 0; m < 2; m++){
			// Check for every slope in hitbox
			TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)+( (mx&0x07) < 0x04);
			
			if(TIclipX>=0 && TIclipY2>=0 && TIclipX < ck_level_width && TIclipY2 < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY2*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)]&0x7F;
				
				// Clip with sloped top
				if(tileTI>=0x02&&tileTI<=0x07){
					uint16_t dy = (mx)&0x07;
					obj->pos_y = (TIclipY2<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY-ck_slopevalues[tileTI-1][dy]);
                    obj->accel_y = 0;
                    obj->accel_x = 0;
					break; // Get out of loop
				}
			}

			TIclipY2 += 1;
			
			// Check for every tile in hitbox
			TIclipX = m+((obj->pos_x+obj->clip_rects.clipX)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)]&0x7F;
				
				// Clip with solid top
				if(tileTI==0x01){
                    uint16_t dy = (mx)&0x07;
                    obj->pos_y = (TIclipY<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY-ck_slopevalues[tileTI-1][dy]);
                    obj->accel_y = 0;
                    obj->accel_x = 0;
					break; // Get out of loop
				}
			}
			
		}

    }


    // Collide Bottom
    if(obj->accel_y <= 0){
		
		TIclipY = (obj->pos_y+obj->clip_rects.clipY+obj->accel_y)>>3;
		TIclipY2 = TIclipY;

		for(m = 0; m < 2; m++){
			// Check for every slope in hitbox
			TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)+( (mx&0x07) < 0x04);
			
			if(TIclipX>=0 && TIclipY2>=0 && TIclipX < ck_level_width && TIclipY2 < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY2*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)+1]&0x7F;
				
				// Clip with sloped bottom
				if(tileTI>=0x02&&tileTI<=0x07){
					uint16_t dy = (mx)&0x07;
					obj->pos_y = (TIclipY2<<3)+8-(obj->clip_rects.clipY+ck_slopevalues[tileTI-1][dy]);
					obj->accel_y = 0;
                    obj->accel_x = 0;
					break; // Get out of loop
				}
			}

			TIclipY2 += 1;
			
			// Check for every tile in hitbox
			TIclipX = m+((obj->pos_x+obj->clip_rects.clipX)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)+1]&0x7F;
				
				// Clip with solid bottom
				if(tileTI==0x01){
					obj->pos_y = (TIclipY<<3)+8-(obj->clip_rects.clipY);
                    obj->accel_y = 0;
                    obj->accel_x = 0;
					break; // Get out of loop
				}
			}
			
		}

    }

	// Collide Right

    if(obj->accel_x >= 0){
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX+obj->clip_rects.clipW)>>3);

		for(m = 0; m < (obj->clip_rects.clipH>>3); m++){
			// Check for every slope in hitbox
			TIclipY = m+((obj->pos_y+obj->clip_rects.clipY)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)];
				
				// Clip with right wall
				if(tileTI&0x8000){
					obj->pos_x = (TIclipX<<3)-(obj->clip_rects.clipX+obj->clip_rects.clipW);
					obj->accel_y = 0;
                    obj->accel_x = 0;
					break; // Get out of loop
				}
			}
		}
	}
    
	// Collide Left

    if(obj->accel_x <= 0){
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)-1;

		for(m = 0; m < (obj->clip_rects.clipH>>3); m++){
			// Check for every slope in hitbox
			TIclipY = m+((obj->pos_y+obj->clip_rects.clipY)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)];
				
				// Clip with left wall
				if(tileTI&0x4000){
					obj->pos_x = (TIclipX<<3)+8-(obj->clip_rects.clipX);
					obj->accel_y = 0;
                    obj->accel_x = 0;
					break; // Get out of loop
				}
			}
		}
	}
};

void LK_UpdateShot(ck_object *obj){
	short absAccelY = GBA_ABS(obj->accel_y);
	short oldAccelY;
	int a;
	if(absAccelY>=8){
		oldAccelY = obj->accel_y;
		if(oldAccelY<0)
			obj->accel_y = -4; // Make it more manageable!
		else
			obj->accel_y = 4; 
		for(a = 0; a < absAccelY; a += 4){
			LK_MapCollideShot(obj);
			obj->pos_y += obj->accel_y;
		}
		if(obj->accel_y!=0){
			obj->accel_y = oldAccelY;
		}
	}else{
		LK_MapCollideShot(obj);
		obj->pos_y += obj->accel_y;
	}

	obj->pos_x += obj->accel_x;
};


void LK_DrawShot(ck_object *obj){
	short sprX = obj->ck_frame->spr_offs[0][0] + obj->pos_x - ck_cam_x;
	short sprY = obj->ck_frame->spr_offs[0][1] + obj->pos_y - ck_cam_y;

	if(sprX <= -16 || sprX >= GBA_SCREEN_WIDTH ||
		sprY <= -16 || sprY >= GBA_SCREEN_HEIGHT){
			// Hide the sprite
			sprX = 0xF0;
			sprY = 0xA0;
		}

	// Render the sprite
	GBA_SET_SPRITE_POSITION(obj->spr_id1, sprX, sprY);

	GBA_SET_SPRITE_TILES(obj->spr_id1, obj->ck_frame->offset[0]);

	switch(obj->var1) {
		case 0:
			GBA_SET_FLIPH(obj->spr_id1,0)
			break;
		case 1:
			GBA_SET_FLIPH(obj->spr_id1,1)
			break;
		case 2:
			GBA_SET_FLIPV(obj->spr_id1,0)
			break;
		case 3:
			GBA_SET_FLIPV(obj->spr_id1,1)
			break;
		case 4:
			GBA_SET_FLIPH(obj->spr_id1,1)
			GBA_SET_FLIPV(obj->spr_id1,1)
			break;
	}
};

void LK_LogicShot(ck_object *obj){

	obj->var1 += 1;

	if(obj->var1 > 4)
		obj->var1 = 0;

	// Die against walls
	if(obj->ck_frame == &ck_ShotAnimation){
		if((obj->accel_x==0&&obj->dir_y==0)||
			(obj->accel_y==0&&obj->dir_x==0)){
			obj->accel_x = obj->accel_y = 0;
			obj->animation_tick = 0;
			obj->ck_frame = &ck_ShotSplat;
			LK_SD_PlaySound(CK_SND_SHOT_HIT);
		}
	}
	if(obj->ck_frame == &ck_ShotRemove){
		//GBA_SET_SPRITE_POSITION(obj->spr_id1, 0xF0, 0xA0);
		GBA_SET_SPRITE_CLEAR(obj->spr_id1);
	}
};

void LK_SpawnShot(int x,int y, int dir_x, int dir_y, int obj_id){
	ck_object * obj = LK_GetNewObj(0);

	obj->pos_x = x;
	obj->pos_y = y;
	obj->accel_x = dir_x*8;
	obj->accel_y = dir_y*8;
	obj->dir_x = dir_x;
	obj->dir_y = dir_y;

	obj->state = ck_objIdle;
	obj->type = ck_objShot;

	obj->var1 = obj->var2 = obj->var3 = obj->var4 = 0;
	
	obj->var1 = 0; // shot timer
	obj->var4 = obj_id; // obj id

	obj->clip_rects.clipX = 2;
	obj->clip_rects.clipY = 2;
	obj->clip_rects.clipW = 12;
	obj->clip_rects.clipH = 12;

#ifdef LK_MULTIBOOT_ROM
	if(obj->spr_id1==128)
		obj->spr_id1 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,0);
	else
		GBA_RemakeSprite(obj->spr_id1,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,0);
#else
	if(obj->spr_id1==128)
		obj->spr_id1 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,-1);
	else
		GBA_RemakeSprite(obj->spr_id1,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,-1);
#endif
	obj->animation_tick = 0;
	obj->ck_frame = &ck_ShotAnimation;
	obj->ck_render = &LK_DrawShot;
	obj->ck_input = &LK_LogicShot;
	obj->ck_collide_obj = &LK_PhysCollideObj;
	obj->ck_update_obj = &LK_UpdateShot;

};


//---------------------------------------- COMMANDER KEEN ANIMATIONS -------------------------

ck_animation 	ck_KeenStand, 
				ck_KeenLookUp,
				ck_KeenLookDown,
				ck_KeenDie;
				
ck_animation 	ck_KeenHang, 
				ck_KeenCrawl1,
				ck_KeenCrawl2,
				ck_KeenCrawl3,
				ck_KeenCrawl4;
				

ck_animation 	ck_KeenWalk1, 
				ck_KeenWalk2, 
				ck_KeenWalk3, 
				ck_KeenWalk4;

ck_animation 	ck_KeenJump1, 
				ck_KeenJump2, 
				ck_KeenFall, 
				ck_KeenPogo1,
				ck_KeenPogo2;


ck_animation ck_KeenStand = {
	{CK_KEEN_STAND, CK_KEEN_STAND+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0xF0,
	&ck_KeenStand
};

ck_animation ck_KeenLookUp = {
	{CK_KEEN_LOOK_UP, CK_KEEN_LOOK_UP+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0xF0,
	&ck_KeenLookUp
};

ck_animation ck_KeenLookDown = {
	{CK_KEEN_LOOK_DOWN},
	{ {0,8} },
	0xF0,
	&ck_KeenLookDown
};


ck_animation ck_KeenDie = {
	{CK_KEEN_DEAD},
	{ {0,4} },
	0xF0,
	&ck_KeenDie
};


ck_animation ck_KeenHang = {
	{CK_KEEN_LEDGE1_A, CK_KEEN_LEDGE1_B, CK_KEEN_LEDGE1_A+CK_BOTTOM_HALF+CK_BOTTOM_HALF},
	{ {0,-4}, {0,24-4}, {0,16-4} },
	0xF0,
	&ck_KeenHang
};


ck_animation ck_KeenCrawl1 = {
	{CK_KEEN_LEDGE2},
	{ {0,-4} },
	0x02,
	&ck_KeenCrawl2
};

ck_animation ck_KeenCrawl2 = {
	{CK_KEEN_LEDGE3},
	{ {0,-8} },
	0x02,
	&ck_KeenCrawl3
};

ck_animation ck_KeenCrawl3 = {
	{CK_KEEN_LEDGE4},
	{ {4,-12} },
	0x02,
	&ck_KeenCrawl4
};

ck_animation ck_KeenCrawl4 = {
	{CK_KEEN_LEDGE5,CK_KEEN_LEDGE5+CK_BOTTOM_HALF},
	{ {0,0}, {0,16} },
	0x02,
	&ck_KeenStand
};



ck_animation ck_KeenWalk1 = {
	{CK_KEEN_RUN1, CK_KEEN_RUN1+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x02,
	&ck_KeenWalk2
};
ck_animation ck_KeenWalk2 = {
	{CK_KEEN_RUN2, CK_KEEN_RUN2+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x02,
	&ck_KeenWalk3
};
ck_animation ck_KeenWalk3 = {
	{CK_KEEN_RUN3, CK_KEEN_RUN3+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x02,
	&ck_KeenWalk4
};
ck_animation ck_KeenWalk4 = {
	{CK_KEEN_RUN4, CK_KEEN_RUN4+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x02,
	&ck_KeenWalk1
};


ck_animation ck_KeenJump1 = {
	{CK_KEEN_JUMP1, CK_KEEN_JUMP1+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x04,
	&ck_KeenJump2
};

ck_animation ck_KeenJump2 = {
	{CK_KEEN_JUMP2, CK_KEEN_JUMP2+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x04,
	&ck_KeenFall
};

ck_animation ck_KeenFall = {
	{CK_KEEN_FALL, CK_KEEN_FALL+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0xF0,
	&ck_KeenFall
};


ck_animation ck_KeenPogo1 = {
	{CK_KEEN_POGO1, CK_KEEN_POGO1+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0xF0,
	&ck_KeenPogo2
};

ck_animation ck_KeenPogo2 = {
	{CK_KEEN_POGO2, CK_KEEN_POGO2+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x04,
	&ck_KeenPogo1
};



ck_animation 	ck_KeenEnter1,
				ck_KeenEnter2,
				ck_KeenEnter3,
				ck_KeenSwitch;

ck_animation ck_KeenEnter1 = {
	{CK_KEEN_ENTER1, CK_KEEN_ENTER1+CK_BOTTOM_HALF},
	{ {0,-2}, {0,16-2}},
	0x04,
	&ck_KeenEnter2
};

ck_animation ck_KeenEnter2 = {
	{CK_KEEN_ENTER2, CK_KEEN_ENTER2+CK_BOTTOM_HALF},
	{ {0,-2}, {0,16-2}},
	0x04,
	&ck_KeenEnter3
};

ck_animation ck_KeenEnter3 = {
	{CK_KEEN_ENTER2, CK_KEEN_ENTER2+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x02,
	&ck_KeenStand
};

ck_animation ck_KeenSwitch = {
	{CK_KEEN_ENTER1, CK_KEEN_ENTER1+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x04,
	&ck_KeenStand
};

ck_animation 	ck_KeenShoot,
				ck_KeenShootUp,
				ck_KeenJumpShootUp,
				ck_KeenShootDown,
				ck_KeenThrow1,
				ck_KeenThrow2,
				ck_KeenThrow3,
				ck_KeenJumpThrow1,
				ck_KeenJumpThrow2,
				ck_KeenJumpThrowDown1,
				ck_KeenJumpThrowDown2;

ck_animation ck_KeenShoot = {
	{CK_KEEN_SHOOT, CK_KEEN_SHOOT+CK_BOTTOM_HALF, CK_KEEN_SHOOT_HAND},
	{ {0,0}, {0,16}, {8,0}},
	0x06,
	&ck_KeenStand
};

ck_animation ck_KeenJumpShoot = {
	{CK_KEEN_JUMP_SHOOT, CK_KEEN_JUMP_SHOOT+CK_BOTTOM_HALF,CK_KEEN_JUMP_SHOOT_HAND},
	{ {0,0}, {0,16}, {8,0}},
	0x06,
	&ck_KeenFall
};

ck_animation ck_KeenShootUp = {
	{CK_KEEN_SHOOT_UP_A, CK_KEEN_SHOOT_UP_A+CK_BOTTOM_HALF, CK_KEEN_SHOOT_UP_B},
	{ {0,0}, {0,16},{0,-8}},
	0x08,
	&ck_KeenLookUp
};

ck_animation ck_KeenJumpShootUp = {
	{CK_KEEN_JUMP_SHOOT_UP_A, CK_KEEN_JUMP_SHOOT_UP_A+CK_BOTTOM_HALF, CK_KEEN_JUMP_SHOOT_UP_B},
	{ {0,0}, {0,16},{0,-8}},
	0x08,
	&ck_KeenFall
};

ck_animation ck_KeenShootDown = {
	{CK_KEEN_SHOOT_DOWN, CK_KEEN_SHOOT_DOWN+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x08,
	&ck_KeenFall
};


ck_animation ck_KeenThrow1 = {
	{CK_KEEN_THROW1, CK_KEEN_THROW1+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x02,
	&ck_KeenThrow2
};

ck_animation ck_KeenThrow2 = {
	{CK_KEEN_THROW2, CK_KEEN_THROW2+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x02,
	&ck_KeenThrow3
};

ck_animation ck_KeenThrow3 = {
	{CK_KEEN_THROW3, CK_KEEN_THROW3+CK_BOTTOM_HALF,CK_KEEN_THROW_HAND},
	{ {0,0}, {0,16}, {8,0}},
	0x02,
	&ck_KeenStand
};

ck_animation ck_KeenJumpThrow1 = {
	{CK_KEEN_JUMP_THROW1, CK_KEEN_JUMP_THROW1+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x03,
	&ck_KeenJumpThrow2
};

ck_animation ck_KeenJumpThrow2 = {
	{CK_KEEN_JUMP_THROW2, CK_KEEN_JUMP_THROW2+CK_BOTTOM_HALF,CK_KEEN_JUMP_THROW_HAND},
	{ {0,0}, {0,16}, {8,0}},
	0x03,
	&ck_KeenFall
};

ck_animation ck_KeenJumpThrowDown1 = {
	{CK_KEEN_JUMP_THROW_DOWN1, CK_KEEN_JUMP_THROW_DOWN1+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x03,
	&ck_KeenJumpThrowDown2
};

ck_animation ck_KeenJumpThrowDown2 = {
	{CK_KEEN_JUMP_THROW_DOWN2, CK_KEEN_JUMP_THROW_DOWN2+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x03,
	&ck_KeenFall
};


// Pole stuff

ck_animation 	ck_KeenPoleIdle,
				ck_KeenPole1,
				ck_KeenPole2,
				ck_KeenPole3,
				ck_KeenSlide1,
				ck_KeenSlide2,
				ck_KeenSlide3,
				ck_KeenSlide4,
				ck_KeenShootPole,
				ck_KeenShootPoleUp,
				ck_KeenShootPoleDown;


ck_animation ck_KeenPoleIdle = {
	{CK_KEEN_POLE1, CK_KEEN_POLE1+CK_BOTTOM_HALF},
	{ {-2,0}, {-2,16}},
	0xF0,
	&ck_KeenPoleIdle
};

ck_animation ck_KeenPole1 = {
	{CK_KEEN_POLE1, CK_KEEN_POLE1+CK_BOTTOM_HALF},
	{ {-2,0}, {-2,16}},
	0x02,
	&ck_KeenPole2
};

ck_animation ck_KeenPole2 = {
	{CK_KEEN_POLE2, CK_KEEN_POLE2+CK_BOTTOM_HALF},
	{ {-2,0}, {-2,16}},
	0x02,
	&ck_KeenPole3
};

ck_animation ck_KeenPole3 = {
	{CK_KEEN_POLE3, CK_KEEN_POLE3+CK_BOTTOM_HALF},
	{ {-2,0}, {-2,16}},
	0x02,
	&ck_KeenPole1
};


ck_animation ck_KeenSlide1 = {
	{CK_KEEN_SLIDE1, CK_KEEN_SLIDE1+CK_BOTTOM_HALF},
	{ {-2,0}, {-2,16}},
	0x02,
	&ck_KeenSlide2
};

ck_animation ck_KeenSlide2 = {
	{CK_KEEN_SLIDE2, CK_KEEN_SLIDE2+CK_BOTTOM_HALF},
	{ {-2,0}, {-2,16}},
	0x02,
	&ck_KeenSlide3
};

ck_animation ck_KeenSlide3 = {
	{CK_KEEN_SLIDE3, CK_KEEN_SLIDE3+CK_BOTTOM_HALF},
	{ {1,0}, {1,16}},
	0x02,
	&ck_KeenSlide4
};

ck_animation ck_KeenSlide4 = {
	{CK_KEEN_SLIDE4, CK_KEEN_SLIDE4+CK_BOTTOM_HALF},
	{ {0,0}, {0,16}},
	0x02,
	&ck_KeenSlide1
};

ck_animation ck_KeenShootPole = {
	{CK_KEEN_POLE_SHOOT, CK_KEEN_POLE_SHOOT+CK_BOTTOM_HALF,CK_KEEN_POLE_SHOOT_HAND},
	{ {0,0}, {0,16},{8,0}},
	0x06,
	&ck_KeenPoleIdle
};

ck_animation ck_KeenShootPoleUp = {
	{CK_KEEN_POLE_SHOOT_UP, CK_KEEN_POLE_SHOOT_UP+CK_BOTTOM_HALF},
	{ {-4,0}, {-4,16}},
	0x06,
	&ck_KeenPoleIdle
};

ck_animation ck_KeenShootPoleDown = {
	{CK_KEEN_POLE_SHOOT_DOWN, CK_KEEN_POLE_SHOOT_DOWN+CK_BOTTOM_HALF},
	{ {-4,0}, {-4,16}},
	0x06,
	&ck_KeenPoleIdle
};

ck_animation ck_KeenPoleThrow1,
			 ck_KeenPoleThrow2,
			 ck_KeenPoleThrowDown1,
			 ck_KeenPoleThrowDown2;

ck_animation ck_KeenPoleThrow1 = {
	{CK_KEEN_POLE_THROW1, CK_KEEN_POLE_THROW1+CK_BOTTOM_HALF},
	{ {-4,0}, {-4,16}},
	0x03,
	&ck_KeenPoleThrow2
};

ck_animation ck_KeenPoleThrow2 = {
	{CK_KEEN_POLE_THROW2, CK_KEEN_POLE_THROW2+CK_BOTTOM_HALF, CK_KEEN_POLE_THROW_HAND},
	{ {0,0}, {0,16}, {8,0}},
	0x03,
	&ck_KeenPoleIdle
};


ck_animation ck_KeenPoleThrowDown1 = {
	{CK_KEEN_POLE_THROW_DOWN1, CK_KEEN_POLE_THROW_DOWN1+CK_BOTTOM_HALF},
	{ {-4,0}, {-4,16}},
	0x03,
	&ck_KeenPoleThrowDown2
};

ck_animation ck_KeenPoleThrowDown2 = {
	{CK_KEEN_POLE_THROW_DOWN2, CK_KEEN_POLE_THROW_DOWN2+CK_BOTTOM_HALF},
	{ {-4,0}, {-4,16}},
	0x03,
	&ck_KeenPoleIdle
};

//////////////////////////////////////
// Keen Stuff


void LK_PhysCollideKeen(ck_object *keen_obj,ck_object *obj2){
	if(keen_obj->type!=ck_objPlayer)
		return; // It's not keen, so forget about it!

	if(obj2->type==ck_objKill){
		// Kill keen here
	}
};


void LK_MapCollideKeen(ck_object *obj){

	// Variables for later
	uint16_t tileTI, tileTI2, TIclipX, TIclipY,TIclipY2;
	const short mx = obj->pos_x+obj->clip_rects.clipX+(obj->clip_rects.clipW>>1);
	int m;
	boolean was_on_ground = ck_localGameState.on_ground[obj->var4];
	boolean on_slope = false;
	
	// Collide with map here
	if(obj->type!=ck_objPlayer)
		return; // It's not keen, so forget about it!

	// Kill keen if outside of map
	if(obj->pos_y+obj->clip_rects.clipY+obj->clip_rects.clipH >= (ck_level_height<<3)-12){
		obj->pos_y = (ck_level_height<<3)-(24+obj->clip_rects.clipY+obj->clip_rects.clipH);
		LK_MurderKeen(obj);
	}


	// We aren't on the ground
	ck_localGameState.on_ground[obj->var4] = false;

	// "Collide" with pole

    if(obj->interacting || obj->state == ck_objIdlePole || obj->state == ck_objClimbPole){
		if(obj->state == ck_objIdlePole || obj->state == ck_objClimbPole){
			obj->state = ck_objFalling;
			obj->frozen2 = false;
			obj->can_climb = false;
		}
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)+( (mx&0x07) < 0x04);
		TIclipY = 1+((obj->pos_y+obj->clip_rects.clipY)>>3);
		
		if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
			uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

			tileTI = (((*ck_tileinfo)[256+(tile*3)])>>7)&0x7F;

			// Interact with pole
			if(tileTI==0x01){
				TIclipY += 2;
				
				tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);
				tileTI = (((*ck_tileinfo)[256+(tile*3)]))&0x7F;
				
				if(tileTI==0x01){
					short oldVY = obj->accel_y;
					
					// Only check going down
					if(obj->dir_y == CK_Dir_Down){

						// Stop keen from hitting floor
						uint16_t dy = (mx&0x07);
						obj->can_climb = false;
						obj->pos_y = (TIclipY<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY-ck_slopevalues[tileTI-1][dy]);
						obj->accel_y = 0;
						obj->frozen2 = false;
						ck_localGameState.on_ground[obj->var4] = true;
						obj->state = ck_objStanding;
						TIclipY -= 2;
						goto skipreposition;
					}
				}
				if(tileTI==0x11){
					if(obj->trying_to_move){
						uint16_t dy = (mx&0x07);
						obj->can_climb = false;
						obj->pos_y = (TIclipY<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY-ck_slopevalues[tileTI-1][dy]);
						obj->accel_y = 0;
						obj->frozen2 = false;
						obj->trying_to_move = 0;
						ck_localGameState.on_ground[obj->var4] = true;
						obj->state = ck_objStanding;
						TIclipY -= 2;
						goto skipreposition;
					}
				}
				
				// Only grab if falling
				if(obj->ck_frame != &ck_KeenJump1){
					obj->pos_x = (TIclipX<<3)-(obj->clip_rects.clipX);
					obj->accel_x = 0;
					obj->accel_y = 0;
					obj->frozen2 = true;
					obj->can_climb = true;
					obj->state = ck_objIdlePole;
					ck_localGameState.on_ground[obj->var4] = false;
				}
	skipreposition:
				TIclipY -= 2;

				TIclipY -= 1;
				tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (((*ck_tileinfo)[256+(tile*3)])>>7)&0x7F;
				TIclipY2 = (((*ck_tileinfo)[256+(tile*3)+1]))&0x7F;
				// Stop keen from going off of pole
				if(obj->dir_y == CK_Dir_Up){
					if(tileTI!=0x01||TIclipY2==0x01){
						obj->can_climb = false;
					}
				}
				
			}else{
				if(obj->state == ck_objClimbPole || obj->state == ck_objSlidePole){
					if(obj->dir_y == CK_Dir_Down){
						obj->state = ck_objFalling;
						obj->frozen2 = false;
						obj->can_climb = false;	

						// ANIMATION SET HERE!
						obj->animation_tick = 0;
						obj->ck_frame = &ck_KeenFall;
						LK_SD_PlaySound(CK_SND_FALL);
					}
				}
			}
		}
	}

	// Other tiles
    {
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)+( (mx&0x07) < 0x04);
		TIclipY = 1+((obj->pos_y+obj->clip_rects.clipY)>>3);
		
		if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
			uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

			tileTI = (((*ck_tileinfo)[256+(tile*3)])>>7)&0x7F;
			tileTI2 = LK_MP_GetTile(TIclipX,TIclipY,2);

			// Interact with deadly
			if(tileTI==0x03){
				if(obj->var1==0){
					LK_MurderKeen(obj);
				}
			}

			// Interact with doors
			if(obj->interacting==1){
				if(tileTI==0x02){
					if(tileTI2&0x8000){
						int lx = (tileTI2&0x7F);
						int ly = ((tileTI2>>7)&0x7F);

						obj->pos_x = ((TIclipX+1)<<3) - ((obj->clip_rects.clipX<<1)+(obj->clip_rects.clipW>>1));
						obj->pos_y = ((TIclipY+2)<<3)- (obj->clip_rects.clipY+obj->clip_rects.clipH);

						obj->topos_x = (lx<<3) - (obj->clip_rects.clipX);
						obj->topos_y = ((ly+1)<<3) - (obj->clip_rects.clipY+obj->clip_rects.clipH);
						
						obj->animation_tick = 0;
						obj->accel_y = 0;
						obj->accel_x = 0;
						obj->ck_frame = &ck_KeenEnter1;
						obj->state = ck_objEnterDoor;
						obj->can_move = false;
						obj->interacting = 0;
					}
				}
			}
			
			// Interact with bridge switches
			if(obj->interacting==1){

				if(tileTI==0x0F){
					// :O
					if(obj->state == ck_objStanding && (obj->ck_frame ==  &ck_KeenStand || obj->ck_frame ==  &ck_KeenLookUp)){
						obj->animation_tick = 0;
						obj->ck_frame = &ck_KeenSwitch;
						LK_SD_PlaySound(CK_SND_CLICK);
						
						int bridge_end = 0;
						uint16_t tileTI3 = (((*ck_tileinfo)[256+(tile*3)+1])>>7)&0xFF;

						LK_MP_SetTile(TIclipX,TIclipY,tileTI3,1);
						if(tileTI2&0x8000){
							int i;
							uint16_t lx = (tileTI2&0x7F);
							uint16_t ly = ((tileTI2>>7)&0x7F);
							
							for(bridge_end = lx; bridge_end < lx+64; bridge_end++){
								tile = LK_MP_GetTile(bridge_end,ly,1);
								if(tile == 0xA2 || tile == 0xA6){
									goto skip_bridge;
								}
							}
							skip_bridge:
							// Change the tiles
							tile = (ck_levelbuff[(ly*ck_level_width)+lx + ck_level_size]);
							if(tile == 0xA0){
								LK_MP_SetTile(lx,ly,tile+4,1);
								tile = (ck_levelbuff[(ly*ck_level_width)+lx+1 + ck_level_size]);
								LK_MP_SetTile(lx+1,ly,tile+4,1);
								tile = (ck_levelbuff[((ly-1)*ck_level_width)+lx+1 + ck_level_size]);
								LK_MP_SetTile(lx+1,ly-1,tile+4,1);
								for(i = lx+2; i <= bridge_end; i++){
									if(i<bridge_end)
										LK_MP_SetTile(i,ly,0x92,1);
									LK_MP_SetTile(i,ly-1,0x96,1);
								}
								lx = bridge_end;
								tile = (ck_levelbuff[(ly*ck_level_width)+lx + ck_level_size]);
								LK_MP_SetTile(lx,ly,tile+4,1);
								tile = (ck_levelbuff[(ly*ck_level_width)+lx+1 + ck_level_size]);
								LK_MP_SetTile(lx+1,ly,tile+4,1);
								tile = (ck_levelbuff[((ly-1)*ck_level_width)+lx+1 + ck_level_size]);
								LK_MP_SetTile(lx+1,ly-1,tile+4,1);

							}else if(tile == 0xA4){
								LK_MP_SetTile(lx,ly,tile-4,1);
								tile = (ck_levelbuff[(ly*ck_level_width)+lx+1 + ck_level_size]);
								LK_MP_SetTile(lx+1,ly,tile-4,1);
								tile = (ck_levelbuff[((ly-1)*ck_level_width)+lx+1 + ck_level_size]);
								LK_MP_SetTile(lx+1,ly-1,tile-4,1);
								for(i = lx+2; i <= bridge_end; i++){
									if(i<bridge_end)
										LK_MP_SetTile(i,ly,0x00,1);
									LK_MP_SetTile(i,ly-1,0x00,1);
								}
								lx = bridge_end;
								tile = (ck_levelbuff[(ly*ck_level_width)+lx + ck_level_size]);
								LK_MP_SetTile(lx,ly,tile-4,1);
								tile = (ck_levelbuff[(ly*ck_level_width)+lx+1 + ck_level_size]);
								LK_MP_SetTile(lx+1,ly,tile-4,1);
								tile = (ck_levelbuff[((ly-1)*ck_level_width)+lx+1 + ck_level_size]);
								LK_MP_SetTile(lx+1,ly-1,tile-4,1);
							}
						}
					}
				}
			}
		}
	}

    // Collide Top
    if(obj->accel_y > 0){

		TIclipY = (obj->pos_y+obj->clip_rects.clipY+obj->clip_rects.clipH+obj->accel_y)>>3;		
		TIclipY2 = TIclipY-1;

		for(m = 0; m < 2; m++){
			// Check for every slope in hitbox
			TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)+( (mx&0x07) < 0x04);
			

			if(TIclipX>=0 && TIclipY2>=0 && TIclipX < ck_level_width && TIclipY2 < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY2*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)]&0x7F;
				
				// Clip with sloped top
				if(tileTI>=0x02&&tileTI<=0x07){
					uint16_t dy = (mx&0x07);
					obj->pos_y = (TIclipY2<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY-ck_slopevalues[tileTI-1][dy]);
					obj->accel_y = 0;
					on_slope = true;
					ck_localGameState.on_ground[obj->var4] = true;
					if(obj->state == ck_objFalling){
						obj->state = ck_objStanding;
						LK_SD_PlaySound(CK_SND_LAND);
					}
					break; // Get out of loop
				}
			}

			TIclipY2 += 1;
			
			// Check for every tile in hitbox
			TIclipX = m+((obj->pos_x+obj->clip_rects.clipX)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)]&0x7F;
				tileTI2 = ((*ck_tileinfo)[256+(tile*3)+1]>>7)&0x7F;
				
				// Clip with solid top
				if(tileTI==0x01){
					if(obj->state == ck_objStanding&&obj->dir_y == CK_Dir_Down && on_slope==false){
						tileTI = (*ck_tileinfo)[256+(tile*3)+1]&0x7F;
						
						// If bottom info is fall thru,
						if(tileTI==0x00){
							// Clip thru the block
							obj->pos_y = (TIclipY<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY-8);
							obj->accel_y = 1;
							ck_localGameState.on_ground[obj->var4] = false;
							LK_SD_PlaySound(CK_SND_FALL);
							goto skipYCheck;
						}
					}
                    //uint16_t dy = (mx)&0x07;
                    obj->pos_y = (TIclipY<<3)-(obj->clip_rects.clipH+obj->clip_rects.clipY);//-ck_slopevalues[tileTI-1][dy]);
                    obj->accel_y = 0;
    				ck_localGameState.on_ground[obj->var4] = true;
					if(obj->state == ck_objFalling){
						obj->state = ck_objStanding;
						LK_SD_PlaySound(CK_SND_LAND);
					}
					break; // Get out of loop
				}
				// Kill Keen
				if(tileTI==0x09 || tileTI2 == 0x03){
					LK_MurderKeen(obj);
				}

			}
			
		}

    }
skipYCheck:

    // Collide Bottom
    if(obj->accel_y < 0){
		
		TIclipY = (obj->pos_y+obj->clip_rects.clipY+obj->accel_y)>>3;
		TIclipY2 = TIclipY;

		for(m = 0; m < 2; m++){
			// Check for every slope in hitbox
			TIclipX = ((obj->pos_x+obj->clip_rects.clipX)>>3)+( (mx&0x07) < 0x04);
			
			if(TIclipX>=0 && TIclipY2>=0 && TIclipX < ck_level_width && TIclipY2 < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY2*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)+1]&0x7F;
				
				// Clip with sloped bottom
				if(tileTI>=0x02&&tileTI<=0x07){
					uint16_t dy = (mx)&0x07;
					obj->pos_y = (TIclipY2<<3)+8-(obj->clip_rects.clipY+ck_slopevalues[tileTI-1][dy]);
					obj->accel_y = 0;
					if(obj->state == ck_objJumping)
						obj->state = ck_objFalling;
					break; // Get out of loop
				}
			}

			TIclipY2 += 1;
			
			// Check for every tile in hitbox
			TIclipX = m+((obj->pos_x+obj->clip_rects.clipX)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)+1]&0x7F;
				
				// Clip with solid bottom
				if(tileTI==0x01){
					obj->pos_y = (TIclipY<<3)+8-(obj->clip_rects.clipY);
                    obj->accel_y = 0;
					if(obj->state == ck_objJumping)
						obj->state = ck_objFalling;
					break; // Get out of loop
				}
			}
			
		}

    }


	// Collide Right

    if(obj->accel_x > 0){
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX+obj->clip_rects.clipW+obj->accel_x)>>3);

		for(m = 0; m < (obj->clip_rects.clipH>>3); m++){
			// Check for every slope in hitbox
			TIclipY = m+((obj->pos_y+obj->clip_rects.clipY)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)];
				
				// Clip with right wall
				if(tileTI&0x8000){
					obj->pos_x = (TIclipX<<3)-(obj->clip_rects.clipX+obj->clip_rects.clipW+1);
					obj->accel_x = 0;
					if(obj->state == ck_objWalking)
						obj->state = ck_objStanding;
									
					// Grab hold of side
					if(!ck_localGameState.is_pogoing[obj->var4] && obj->accel_y > 0 && !was_on_ground&&obj->can_grab==1){
						TIclipY -= 1;
						tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);
						tileTI = (*ck_tileinfo)[256+(tile*3)];
						
						// If tile above is not solid
						if(!((tileTI&0x8000) || ((tileTI&0x7F)==1))){
							TIclipY += 1;

							TIclipX -= 1;
							tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);
							tileTI = (*ck_tileinfo)[256+(tile*3)];
							TIclipX += 1;
							// If tile to the left is not solid
							if(!(tileTI&0x8000) ){
								obj->pos_x = (TIclipX<<3)-(obj->clip_rects.clipX+obj->clip_rects.clipW+1);
								obj->pos_y = (TIclipY<<3)-(obj->clip_rects.clipY);
								obj->accel_x = 0;
								obj->accel_y = 0;
								obj->state = ck_objHolding;
								return; // Don't do anything else!
							}
						}
					}
					break; // Get out of loop
				}
			}
		}
	}
    
	// Collide Left

    if(obj->accel_x < 0){
		
		TIclipX = ((obj->pos_x+obj->clip_rects.clipX+obj->accel_x-1)>>3);

		for(m = 0; m < (obj->clip_rects.clipH>>3); m++){
			// Check for every slope in hitbox
			TIclipY = m+((obj->pos_y+obj->clip_rects.clipY)>>3);
			
			if(TIclipX>=0 && TIclipY>=0 && TIclipX < ck_level_width && TIclipY < ck_level_height){
				uint16_t tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);

				tileTI = (*ck_tileinfo)[256+(tile*3)];
				
				// Clip with left wall
				if(tileTI&0x4000){
					obj->pos_x = (TIclipX<<3)+9-(obj->clip_rects.clipX);
					obj->accel_x = 0;
					if(obj->state == ck_objWalking)
						obj->state = ck_objStanding;

					// Grab hold of side
					if(!ck_localGameState.is_pogoing[obj->var4] && obj->accel_y > 0 && !was_on_ground&&obj->can_grab==1){
						TIclipY -= 1;
						tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);
						tileTI = (*ck_tileinfo)[256+(tile*3)];
						
						// If tile above is not solid
						if(!((tileTI&0x4000) || ((tileTI&0x7F)==1) )){
							TIclipY += 1;

							TIclipX += 1;
							tile = (ck_levelbuff[(TIclipY*ck_level_width)+TIclipX + ck_level_size]);
							tileTI = (*ck_tileinfo)[256+(tile*3)];
							TIclipX -= 1;
							// If tile to the right is not solid
							if(!(tileTI&0x4000) ){
								obj->pos_x = (TIclipX<<3)+9-(obj->clip_rects.clipX);
								obj->pos_y = (TIclipY<<3)-(obj->clip_rects.clipY);
								obj->accel_x = 0;
								obj->accel_y = 0;
								obj->state = ck_objHolding;
								return; // Don't do anything else!
							}
						}
					}
					break; // Get out of loop
				}
			}
		}
	}

};




void LK_DrawKeen(ck_object *obj){

	short sprX = obj->ck_frame->spr_offs[0][0] + obj->pos_x - ck_cam_x;
	short sprY = obj->ck_frame->spr_offs[0][1] + obj->pos_y - ck_cam_y;

	if(obj->ck_frame == &ck_KeenPoleIdle || obj->ck_frame == &ck_KeenPole1 || obj->ck_frame == &ck_KeenPole2 || obj->ck_frame == &ck_KeenPole3 ||
	obj->ck_frame == &ck_KeenShootPole || obj->ck_frame == &ck_KeenShootPoleUp || obj->ck_frame == &ck_KeenShootPoleDown || 
	obj->ck_frame == &ck_KeenPoleThrow1 || obj->ck_frame == &ck_KeenPoleThrowDown1 || obj->ck_frame == &ck_KeenPoleThrowDown2){
		sprX = (obj->ck_frame->spr_offs[0][0]*obj->dir_x) + obj->pos_x - ck_cam_x;
	}

	if(obj->dir_x==CK_Dir_Left){
		if(obj->ck_frame == &ck_KeenHang||obj->ck_frame == &ck_KeenCrawl1||obj->ck_frame == &ck_KeenCrawl2||obj->ck_frame == &ck_KeenCrawl3||obj->ck_frame == &ck_KeenCrawl4){
			sprX -= 2+(obj->ck_frame->spr_offs[0][0]<<1);
		}
	}

	if(sprX < -16 || sprY < -16 || sprX > GBA_SCREEN_WIDTH || sprY > GBA_SCREEN_HEIGHT){
		sprX = 0xF0;
		sprY = 0xA0;
	}
	// Render the sprites
	GBA_SET_SPRITE_POSITION(obj->spr_id1, sprX, sprY);
	sprX = obj->ck_frame->spr_offs[1][0] + obj->pos_x - ck_cam_x;
	sprY = obj->ck_frame->spr_offs[1][1] + obj->pos_y - ck_cam_y;

	if(obj->ck_frame == &ck_KeenPoleIdle || obj->ck_frame == &ck_KeenPole1 || obj->ck_frame == &ck_KeenPole2 || obj->ck_frame == &ck_KeenPole3 ||
	obj->ck_frame == &ck_KeenShootPole || obj->ck_frame == &ck_KeenShootPoleUp || obj->ck_frame == &ck_KeenShootPoleDown || 
	obj->ck_frame == &ck_KeenPoleThrow1 || obj->ck_frame == &ck_KeenPoleThrowDown1 || obj->ck_frame == &ck_KeenPoleThrowDown2){
		sprX = (obj->ck_frame->spr_offs[1][0]*obj->dir_x) + obj->pos_x - ck_cam_x;
	}

	if(obj->dir_x==CK_Dir_Left){
		if(obj->ck_frame == &ck_KeenHang||obj->ck_frame == &ck_KeenCrawl1||obj->ck_frame == &ck_KeenCrawl2||obj->ck_frame == &ck_KeenCrawl3||obj->ck_frame == &ck_KeenCrawl4){
			sprX -= 2;
		}
	}
	if(obj->ck_frame == &ck_KeenLookDown || obj->ck_frame == &ck_KeenCrawl1 || obj->ck_frame == &ck_KeenCrawl2 ||
		obj->ck_frame == &ck_KeenCrawl3 || obj->ck_frame == &ck_KeenDie){
		sprX = 0xA0; // Hide that sprite
		sprY = 0xF0;
	}
	if(sprX < -16 || sprY < -16 || sprX > GBA_SCREEN_WIDTH || sprY > GBA_SCREEN_HEIGHT){
		sprX = 0xF0;
		sprY = 0xA0;
	}
	GBA_SET_SPRITE_POSITION(obj->spr_id2, sprX, sprY);


	sprX = 0xA0; // Hide the third sprite
	sprY = 0xF0;

	if(obj->ck_frame == &ck_KeenShoot || obj->ck_frame == &ck_KeenJumpShoot || obj->ck_frame == &ck_KeenThrow3 || obj->ck_frame == &ck_KeenJumpThrow2 ||
		obj->ck_frame == &ck_KeenShootUp || obj->ck_frame == &ck_KeenJumpShootUp || obj->ck_frame == &ck_KeenHang ||
		obj->ck_frame == &ck_KeenShootPole || obj->ck_frame == & ck_KeenPoleThrow2){

		// Extra sprite for gun
		sprX = (obj->ck_frame->spr_offs[2][0]*obj->dir_x) + obj->pos_x - ck_cam_x;
		sprY = obj->ck_frame->spr_offs[2][1] + obj->pos_y - ck_cam_y;
		if(obj->dir_x==CK_Dir_Left){
			if(obj->ck_frame == &ck_KeenHang||obj->ck_frame == &ck_KeenCrawl1||obj->ck_frame == &ck_KeenCrawl2||obj->ck_frame == &ck_KeenCrawl3||obj->ck_frame == &ck_KeenCrawl4){
				sprX -= 2;
			}
		}
	}
	if(sprX < -16 || sprY < -16 || sprX > GBA_SCREEN_WIDTH || sprY > GBA_SCREEN_HEIGHT){
		sprX = 0xF0;
		sprY = 0xA0;
	}
	GBA_SET_SPRITE_POSITION(obj->spr_id3, sprX, sprY);

	// Hack:
	// Change only one keen sprite to save space
#ifdef LK_MULTIBOOT_ROM
	GBA_SET_SPRITE_TILES(obj->spr_id1, (cki_playerGraphicsStart+(obj->var4<<1))*32); 
	GBA_SET_SPRITE_TILES(obj->spr_id2, 2+((cki_playerGraphicsStart+(obj->var4<<1))*32));
	GBA_SET_SPRITE_TILES(obj->spr_id3, 4+((cki_playerGraphicsStart+(obj->var4<<1))*32));
#else
	GBA_SET_SPRITE_TILES(obj->spr_id1, (cki_playerGraphicsStart+(obj->var4<<1))*32); // 256
	GBA_SET_SPRITE_TILES(obj->spr_id2, 4+((cki_playerGraphicsStart+(obj->var4<<1))*32));
	GBA_SET_SPRITE_TILES(obj->spr_id3, 8+((cki_playerGraphicsStart+(obj->var4<<1))*32));
#endif

#ifdef LK_MULTIBOOT_ROM
	if(obj->var1&0x1) {
		GBA_SET_SPRITE_PAL(obj->spr_id1,15);
		GBA_SET_SPRITE_PAL(obj->spr_id2,15);
		GBA_SET_SPRITE_PAL(obj->spr_id3,15);
	}else{
		GBA_SET_SPRITE_PAL(obj->spr_id1,ck_localGameState.player_pics[obj->var4]);
		GBA_SET_SPRITE_PAL(obj->spr_id2,ck_localGameState.player_pics[obj->var4]);
		GBA_SET_SPRITE_PAL(obj->spr_id3,ck_localGameState.player_pics[obj->var4]);
	}
#endif

	if(obj->ck_frame == &ck_KeenJumpShootUp || obj->ck_frame == &ck_KeenShootUp || obj->ck_frame == &ck_KeenHang ){
		int drawwhite = obj->var1&0x1;
		if(obj->var2>0x10) drawwhite = 1;
		// Horizontal extra bit
		LK_CA_HackPlayerSprite(obj->ck_frame->offset[0],obj->ck_frame->offset[1],obj->ck_frame->offset[2], ck_localGameState.player_pics[obj->var4],1,obj->var4,drawwhite);
	}else{
		int drawwhite = obj->var1&0x1;
		if(obj->var2>0x10) drawwhite = 1;

		LK_CA_HackPlayerSprite(obj->ck_frame->offset[0],obj->ck_frame->offset[1],obj->ck_frame->offset[2], ck_localGameState.player_pics[obj->var4],0,obj->var4,drawwhite);
	}
/*	GBA_SET_SPRITE_TILES(obj->spr_id1, obj->ck_frame->offset[0]);
	GBA_SET_SPRITE_TILES(obj->spr_id2, obj->ck_frame->offset[1]);
	GBA_SET_SPRITE_TILES(obj->spr_id3, obj->ck_frame->offset[2]);
*/
	if(obj->dir_x == CK_Dir_Left){
		GBA_SET_FLIPH(obj->spr_id1,0)
		GBA_SET_FLIPH(obj->spr_id2,0)
		GBA_SET_FLIPH(obj->spr_id3,0)
	}else{
		GBA_SET_FLIPH(obj->spr_id1,1)
		GBA_SET_FLIPH(obj->spr_id2,1)
		GBA_SET_FLIPH(obj->spr_id3,1)
	}
	if(obj->ck_frame==&ck_KeenEnter1 || obj->ck_frame==&ck_KeenEnter2){
		if(obj->animation_tick < 2){
			GBA_SET_FLIPH(obj->spr_id1,0)
			GBA_SET_FLIPH(obj->spr_id2,0)
			GBA_SET_FLIPH(obj->spr_id3,0)
		}else{
			GBA_SET_FLIPH(obj->spr_id1,1)
			GBA_SET_FLIPH(obj->spr_id2,1)
			GBA_SET_FLIPH(obj->spr_id3,1)
		}
	}

	// play walking sound?
	if(obj->state==ck_objWalking || obj->state==ck_objEnterDoor){
		if(obj->animation_tick&1){
			obj->int1 = !obj->int1;
			if(obj->int1 == 1){
				LK_SD_PlaySound(CK_SND_WALK1);
			}else{
				LK_SD_PlaySound(CK_SND_WALK2);
			}
		}
	}
	if(obj->ck_frame == &ck_KeenLookUp || obj->ck_frame == &ck_KeenShootUp || 
		obj->ck_frame == &ck_KeenJumpShootUp || obj->ck_frame == &ck_KeenShootDown||
		obj->ck_frame == &ck_KeenSlide1 || obj->ck_frame == &ck_KeenSlide2 || obj->ck_frame == &ck_KeenSlide3 || obj->ck_frame == &ck_KeenSlide4){
		GBA_SET_FLIPH(obj->spr_id1,0)
		GBA_SET_FLIPH(obj->spr_id2,0)
		GBA_SET_FLIPH(obj->spr_id3,0)
	}
};


void LK_KillKeen(ck_object *obj){
	if(obj->type != ck_objPlayer) return;
//	if(ck_localGameState.godmode) return; // We invincable!
	if(obj->state != ck_objDead){
		obj->state = ck_objDead;
		obj->var2 = 0x20;
		obj->animation_tick = 0;
		obj->ck_frame = &ck_KeenDie;
		obj->frozen = true;
		ck_localGameState.update_scorebox = true;
		LK_SD_PlaySound(CK_SND_DIE);
	}
};

void LK_MurderKeen(ck_object *obj){
	if(obj->type != ck_objPlayer) return;
	// No?
//	if(obj->var1) return; // We invincable!
	if(obj->state != ck_objDead){
		obj->state = ck_objDead;
		ck_localGameState.ck_lives[obj->var4] -= 1;
		obj->var2 = 0x20;
		obj->animation_tick = 0;
		obj->ck_frame = &ck_KeenDie;
		obj->frozen = true;
		ck_localGameState.update_scorebox = true;
		LK_SD_PlaySound(CK_SND_DIE);
	}
};


void LK_LogicKeen(ck_object *obj){
	
	if(obj->can_grab == 2){
		obj->can_grab = 0;
	}else{
		obj->can_grab = 1;
	}
	if(obj->var1){
		obj->var1 -= 1;
	}
	if(obj->var2){
		obj->var2 -= 1;
		if(obj->var2<=0){
			// Respawn keen
			if(obj->state == ck_objDead){
				
				// Reset some variables
				ck_localGameState.ck_health[obj->var4] = ck_localGameState.start_health;
				ck_localGameState.ck_shots[obj->var4] = ck_localGameState.start_shots;
				ck_localGameState.ck_bombs[obj->var4] = ck_localGameState.start_bombs;
				ck_localGameState.update_scorebox = true;
				obj->frozen = false;
				obj->frozen2 = false;
				obj->can_move = true;
				obj->trying_to_move = 0;
				obj->interacting = 0;
				obj->animation_tick = 0;
				obj->ck_frame = &ck_KeenStand;
				obj->state = ck_objStanding;
				ck_localGameState.is_pogoing[obj->var4] = false;
				ck_localGameState.on_ground[obj->var4] = false;
				ck_localGameState.has_shot[obj->var4] = false;
				ck_localGameState.has_thrown[obj->var4] = false; 
				ck_localGameState.thrown_bomb[obj->var4] = false;
	
				obj->dir_y = CK_Dir_None;
				obj->dir_x = CK_Dir_Right;

				// Reset clip rect
				obj->clip_rects.clipY = 0;
				obj->clip_rects.clipH = 24;
				
				obj->pos_x = obj->spawn_x;
				obj->pos_y = obj->spawn_y;
				
				obj->accel_x = 0;
				obj->accel_y = 0;
				
				if(GBA_SerialID == obj->var4){
					ck_cam_x = obj->spawn_x - (GBA_SCREEN_WIDTH>>1);
					ck_cam_y = obj->spawn_y - (GBA_SCREEN_HEIGHT>>1);
				}
				
				obj->var1 = 0x20;

			}
		}
	}
	if(ck_localGameState.ck_health[obj->var4] <= 0){
		ck_localGameState.ck_health[obj->var4] = 0;
		LK_KillKeen(obj);
	}
	// Warp keen
	if(obj->ck_frame==&ck_KeenEnter1||obj->ck_frame==&ck_KeenEnter2){
		// Do nothing!
		return;
	}
	if(obj->ck_frame==&ck_KeenEnter3){
		obj->animation_tick = 100; // skip to end of animation?
		obj->state = ck_objStanding;
		obj->pos_x = obj->topos_x;
		obj->pos_y = obj->topos_y;
		obj->var1 = 0x10;
		obj->can_move = true;
		// Move camera too
		if(GBA_SerialID == obj->var4){
			ck_cam_x = (obj->pos_x) - (GBA_SCREEN_WIDTH>>1);
			ck_cam_y = (obj->pos_y) - (GBA_SCREEN_HEIGHT>>1);
		}
	}


	if(obj->state == ck_objHolding){
		obj->animation_tick = 0;
		obj->ck_frame = &ck_KeenHang;
		obj->frozen = true;
	}
	
	if(obj->ck_frame == &ck_KeenCrawl4){
		if(obj->frozen&&obj->state==ck_objClimbing){
			obj->pos_x += 12*obj->dir_x;
			obj->pos_y -= 24;
			ck_localGameState.on_ground[obj->var4] = true;
			obj->state = ck_objStanding;
		}
	}
	
	if(obj->frozen2){
		if(obj->ck_frame == &ck_KeenShootPole || obj->ck_frame == &ck_KeenShootPoleUp || obj->ck_frame == &ck_KeenShootPoleDown){
			obj->state = ck_objShootPole;
		}else{
			if(obj->state == ck_objShootPole){
				obj->state = ck_objIdlePole;
				ck_localGameState.has_shot[obj->var4] = false;
			}
		}
		if(obj->ck_frame == &ck_KeenPoleThrow1 || obj->ck_frame == &ck_KeenPoleThrow2 || 
			obj->ck_frame == &ck_KeenPoleThrowDown1 || obj->ck_frame == & ck_KeenPoleThrowDown2){
				obj->state = ck_objThrowPole;
		}else{
			if(obj->state == ck_objThrowPole){
				obj->state = ck_objIdlePole;
				ck_localGameState.has_thrown[obj->var4] = false;
				ck_localGameState.thrown_bomb[obj->var4] = false;
			}
		}
		if(obj->state == ck_objIdlePole || obj->state == ck_objClimbPole){
			// Look left / right
			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_LEFT){
				obj->trying_to_move = 1;
				obj->dir_x = CK_Dir_Left;
			}
			else if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_RIGHT){
				obj->trying_to_move = 2;
				obj->dir_x = CK_Dir_Right;
			}else{
				obj->trying_to_move = false;
			}

			// Climb up and down
			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_UP){
				if(obj->can_climb){
					obj->pos_y -= 2;
					obj->dir_y = CK_Dir_Up;
					obj->state = ck_objClimbPole;
				}
			}else
			// We can fall off of poles
			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_DOWN){
				obj->pos_y += 4;
				obj->dir_y = CK_Dir_Down;
				obj->state = ck_objSlidePole;
			}else{
				obj->state = ck_objIdlePole;
			}
		}
		
		if(obj->state == ck_objIdlePole){
			// Set idle animation
			if(obj->ck_frame!=&ck_KeenPoleIdle){
				obj->animation_tick = 0;
				obj->ck_frame = &ck_KeenPoleIdle;
			}
		}
		if(obj->state == ck_objClimbPole){
			// Set idle animation
			if(! ( obj->ck_frame == &ck_KeenPole1 || obj->ck_frame == &ck_KeenPole2 || obj->ck_frame == &ck_KeenPole3)){
				obj->animation_tick = 0;
				obj->ck_frame = &ck_KeenPole1;
			}
		}
		if(obj->state == ck_objSlidePole){
			// Set idle animation
			if(! ( obj->ck_frame == &ck_KeenSlide1 || obj->ck_frame == &ck_KeenSlide2 || obj->ck_frame == &ck_KeenSlide3 || obj->ck_frame == &ck_KeenSlide4)){
				obj->animation_tick = 0;
				obj->ck_frame = &ck_KeenSlide1;
			}
		}

		if(ck_localGameState.ck_lastkeeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.pogo_set]){
			obj->dir_y = CK_Dir_Up;
			// Start pogoing
			obj->state = ck_objBouncing;
			obj->animation_tick = 0;
			obj->ck_frame = &ck_KeenPogo2;
			obj->accel_y = 1;
			obj->frozen2 = false;
			ck_localGameState.is_pogoing[obj->var4] = true;
			// Remove last input
			ck_localGameState.ck_lastkeeninputs[obj->var4] = 0;
		}


		if(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.jump_set]){
			obj->dir_y = CK_Dir_Up;
			obj->can_grab = 0;
			// Start jumping
			obj->state = ck_objJumping;
			obj->animation_tick = 0;
			obj->ck_frame = &ck_KeenJump1;
			obj->accel_y = -3;
			obj->frozen2 = false;
			ck_localGameState.on_ground[obj->var4] = false;
			// Play sound
			LK_SD_PlaySound(CK_SND_JUMP);
		}
		
		// Shoting 
		if(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.shoot_set]){
			if(ck_localGameState.has_shot[obj->var4] == false){
				int hasShot = 0;
				ck_localGameState.has_shot[obj->var4] = true;

				if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_UP){

					if(ck_localGameState.ck_shots[obj->var4]){
						LK_SpawnShot(obj->pos_x,obj->pos_y-8,CK_Dir_None,CK_Dir_Up,obj->var4);
						hasShot = 1;
					}

					obj->animation_tick = 0;
					obj->state = ck_objShootPole;
					obj->ck_frame = &ck_KeenShootPoleUp;
				}else
				if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_DOWN){
					if(ck_localGameState.ck_shots[obj->var4]){
						LK_SpawnShot(obj->pos_x,obj->pos_y+16,CK_Dir_None,CK_Dir_Down,obj->var4);
						hasShot = true;
					}
					obj->animation_tick = 0;
					obj->state = ck_objShootPole;
					obj->ck_frame = &ck_KeenShootPoleDown;
				}else{
					obj->animation_tick = 0;
					obj->state = ck_objShootPole;
					obj->ck_frame = &ck_KeenShootPole;

					if(ck_localGameState.ck_shots[obj->var4]){

						if(obj->dir_x == CK_Dir_Left){
							LK_SpawnShot(obj->pos_x-8,obj->pos_y,obj->dir_x,CK_Dir_None,obj->var4);
						}
						if(obj->dir_x == CK_Dir_Right){
							LK_SpawnShot(obj->pos_x+16,obj->pos_y,obj->dir_x,CK_Dir_None,obj->var4);
						}
						hasShot = true;
					}
				}
				if(hasShot){
					ck_localGameState.ck_shots[obj->var4] -= 1;
					ck_localGameState.update_scorebox = true;
					LK_SD_PlaySound(CK_SND_KEEN_SHOT);
				}else{
					LK_SD_PlaySound(CK_SND_CLICK);
				}
			}
		}else{
			if(obj->state != ck_objShootPole){
				ck_localGameState.has_shot[obj->var4] = false;
			}
		}
		
		// Bombing
		if(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.throw_set]){
			if(ck_localGameState.has_thrown[obj->var4] == false){
				ck_localGameState.has_thrown[obj->var4] = true;
				if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_DOWN){
					obj->state = ck_objThrowPole;
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenPoleThrowDown1;
				}else{
					obj->state = ck_objThrowPole;
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenPoleThrow1;
				}
		}
	}else{
		if((obj->ck_frame == &ck_KeenPoleThrow1 || obj->ck_frame == &ck_KeenPoleThrow2 || 
			obj->ck_frame == &ck_KeenPoleThrowDown1 || obj->ck_frame == & ck_KeenPoleThrowDown2) == 0){
			ck_localGameState.has_thrown[obj->var4] = false;
			ck_localGameState.thrown_bomb[obj->var4] = false;

			if(obj->state == ck_objThrowPole){
				obj->state = ck_objIdlePole;
			}
		}
	}
	
	// Spawn the bomb
	if(ck_localGameState.has_thrown[obj->var4]){
		if(ck_localGameState.thrown_bomb[obj->var4]==false){
			short vdir = CK_Dir_None; 
			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_UP){
				vdir = CK_Dir_Up;
			}
			if(obj->ck_frame == & ck_KeenPoleThrow2){
				ck_localGameState.thrown_bomb[obj->var4] = true;
				if(obj->dir_x == CK_Dir_Left){
					if(ck_localGameState.ck_bombs[obj->var4])
						LK_SpawnBomb(obj->pos_x-8,obj->pos_y,obj->dir_x, vdir,obj);
				}
				if(obj->dir_x == CK_Dir_Right){
					if(ck_localGameState.ck_bombs[obj->var4])
						LK_SpawnBomb(obj->pos_x+16,obj->pos_y,obj->dir_x, vdir,obj);
				}
			}
			if(obj->ck_frame == & ck_KeenPoleThrowDown2){
				ck_localGameState.thrown_bomb[obj->var4] = true;
				if(ck_localGameState.ck_bombs[obj->var4])
					LK_SpawnBomb(obj->pos_x,obj->pos_y+16,CK_Dir_None,CK_Dir_Down,obj);
			}
			if(ck_localGameState.thrown_bomb[obj->var4]){
				if(ck_localGameState.ck_bombs[obj->var4]){
					ck_localGameState.ck_bombs[obj->var4] -= 1;
					ck_localGameState.update_scorebox = true;
				}else{	
					LK_SD_PlaySound(CK_SND_NO_WAY);
				}
			}
		}
	}
		

		if(obj->frozen2)
			return;
	}
	if(obj->frozen){

		if(obj->ck_frame==&ck_KeenStand){
			obj->frozen = false;
		}
		if(obj->ck_frame==&ck_KeenHang){
			// Do some logic here
			if((obj->dir_x==CK_Dir_Right && (ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_RIGHT)) ||
				(obj->dir_x==CK_Dir_Left && (ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_LEFT))){
					obj->var3 += 1;
			}else{
				obj->var3 = 7; // Don't delay when left / right is pushed again
			}

			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_UP || obj->var3 >= 8){
				obj->var3 = 0;
				// Climb up
				obj->state = ck_objClimbing;
				obj->animation_tick = 0;
				obj->ck_frame = &ck_KeenCrawl1;
			}
			if( (ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_DOWN) ||
				(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.jump_set]) ||
				(obj->dir_x==CK_Dir_Right && (ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_LEFT)) ||
				(obj->dir_x==CK_Dir_Left && (ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_RIGHT)) ){
				// Drop
				obj->accel_y = 4;
//				obj->pos_y += 4;
//				obj->accel_x = -(obj->dir_x*4);
				obj->state = ck_objFalling;
				obj->animation_tick = 0;
				obj->ck_frame = &ck_KeenFall;
				obj->frozen = false;
				if(obj->can_grab==1){
					obj->can_grab = 2;
					// Play fall sound
					LK_SD_PlaySound(CK_SND_FALL);
				}
			}
		}
		if(obj->frozen)
			return;
	}
	
	if(!ck_localGameState.on_ground[obj->var4]){
		if(obj->state == ck_objIdle||obj->state == ck_objStanding||obj->state == ck_objWalking ){
			if(obj->state == ck_objWalking)
				LK_SD_PlaySound(CK_SND_FALL);
			obj->state = ck_objFalling;
			obj->animation_tick = 0;
			obj->ck_frame = &ck_KeenFall;
		}
	}
	if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_RIGHT){
		if(ck_localGameState.is_pogoing[obj->var4]){
			if(ck_randomTick & 1){
				obj->accel_x += 1;
				if(obj->accel_x>=0){
					obj->dir_x = CK_Dir_Right;
				}
			}
		}
		else {
			obj->dir_x = CK_Dir_Right;
			if(obj->can_move){
				obj->accel_x += 2;
			}
		}
		if(ck_localGameState.on_ground[obj->var4]){
			if(obj->can_move){
				obj->accel_x += 2;
				// Start walking
				if(obj->state == ck_objStanding){
					obj->state = ck_objWalking;
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenWalk1;
				}
			}
		}
	}else if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_LEFT){
		if(ck_localGameState.is_pogoing[obj->var4]){
			if(ck_randomTick & 1){
				obj->accel_x -= 1;
				if(obj->accel_x<=0){
					obj->dir_x = CK_Dir_Left;
				}
			}
		}
		else {
			if(obj->can_move){
				obj->accel_x -= 2;
			}
			obj->dir_x = CK_Dir_Left;
		}
		if(ck_localGameState.on_ground[obj->var4]){
			if(obj->can_move){
				obj->accel_x -= 2;
				// Start walking
				if(obj->state == ck_objStanding){
					obj->state = ck_objWalking;
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenWalk1;
				}
			}
		}
	}else{
		if(obj->state == ck_objWalking || obj->state == ck_objStanding){
			if(obj->ck_frame != &ck_KeenShootUp && obj->ck_frame != &ck_KeenShoot &&
				obj->ck_frame != &ck_KeenThrow1 && obj->ck_frame != &ck_KeenThrow2 && obj->ck_frame != &ck_KeenThrow3 &&
				obj->ck_frame != &ck_KeenSwitch && obj->ck_frame != &ck_KeenEnter1 && obj->ck_frame != &ck_KeenEnter2){
				if(ck_localGameState.on_ground){
					obj->state = ck_objStanding;
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenStand;
					obj->dir_y = CK_Dir_None;

					// Reset clip rect
					obj->clip_rects.clipY = 0;
					obj->clip_rects.clipH = 24;
				}
			}
		}
	}
	
	
	obj->interacting = 0;
	if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_UP){
		if(!ck_localGameState.is_pogoing[obj->var4]){
			obj->interacting = 1;
			if(ck_localGameState.on_ground[obj->var4]){
				// Look Up
				if(obj->state == ck_objStanding && obj->ck_frame != &ck_KeenSwitch){
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenLookUp;
					obj->dir_y = CK_Dir_Up;
					if(obj->var4==ck_localGameState.player_ips[0])
						ck_cam_y -= 2;
				}
			}
		}
	}else if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_DOWN){
		if(!ck_localGameState.is_pogoing[obj->var4]){
			obj->interacting = 2;
			if(ck_localGameState.on_ground[obj->var4]){
				// Look down
				if(obj->state == ck_objStanding){
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenLookDown;
					obj->clip_rects.clipY = 8;
					obj->clip_rects.clipH = 16;
					if(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.jump_set]){
						obj->dir_y = CK_Dir_Down;
						ck_localGameState.ck_keeninputs[obj->var4] &= ~CK_GBA_BUTTON_SET[ck_localGameState.jump_set];
					}
					if(obj->var4==ck_localGameState.player_ips[0])
						ck_cam_y += 2;
				}
			}
		}
	}/*else{
		if(obj->ck_frame == &ck_KeenLookDown){
			obj->animation_tick = 0;
			obj->ck_frame = &ck_KeenStand;
			// Reset clip rect
			obj->clip_rects.clipX = 2;
			obj->clip_rects.clipY = 0;
			obj->clip_rects.clipW = 12;
			obj->clip_rects.clipH = 24;
		}
	}*/

	if(obj->can_move==false)
		obj->accel_x = 0;

	if(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.throw_set]){
		if(ck_localGameState.has_thrown[obj->var4] == false){
			ck_localGameState.has_thrown[obj->var4] = true;

			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_UP){

				if(ck_localGameState.on_ground[obj->var4] && ck_localGameState.is_pogoing[obj->var4]==false){
					obj->animation_tick = 0;
					obj->state = ck_objThrowing;
					obj->ck_frame = &ck_KeenThrow1;
					obj->accel_x = 0;
				}else{
					ck_localGameState.is_pogoing[obj->var4] = false;
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenJumpThrow1;
				}
			}else
			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_DOWN){
				if(!ck_localGameState.on_ground[obj->var4]){

					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenJumpThrowDown1;
					ck_localGameState.is_pogoing[obj->var4] = false;
				}
			}else{
				if(ck_localGameState.on_ground[obj->var4] && ck_localGameState.is_pogoing[obj->var4]==false){
					obj->state = ck_objThrowing;
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenThrow1;
					obj->can_move = false;
				}
				else{
					if(!(obj->ck_frame == &ck_KeenJumpThrow1 || obj->ck_frame == &ck_KeenJumpThrow2)){
						obj->state = ck_objFalling;
						ck_localGameState.is_pogoing[obj->var4] = false;
						obj->animation_tick = 0;
						obj->ck_frame = &ck_KeenJumpThrow1;
					}
				}
			}
		}
	}else{
		if((obj->ck_frame == &ck_KeenThrow1 || obj->ck_frame == &ck_KeenThrow2 || obj->ck_frame == &ck_KeenThrow3 || 
		obj->ck_frame == &ck_KeenJumpThrow1 || obj->ck_frame == &ck_KeenJumpThrow2 || obj->ck_frame == & ck_KeenJumpThrowDown2) == 0){
			ck_localGameState.has_thrown[obj->var4] = false;
			ck_localGameState.thrown_bomb[obj->var4] = false;

			if(obj->state == ck_objThrowing){
				obj->state = ck_objStanding;
				obj->can_move = true;
			}
		}
	}
	
	if(obj->state == ck_objThrowing){
		obj->can_move = false;
	}

	// Spawn the bomb
	if(ck_localGameState.has_thrown[obj->var4]){
		if(ck_localGameState.thrown_bomb[obj->var4]==false){
			short vdir = CK_Dir_None; 
			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_UP){
				vdir = CK_Dir_Up;
			}
			if(obj->ck_frame == & ck_KeenThrow3){
				ck_localGameState.thrown_bomb[obj->var4] = true;
				if(obj->dir_x == CK_Dir_Left){
					if(ck_localGameState.ck_bombs[obj->var4])
						LK_SpawnBomb(obj->pos_x-8,obj->pos_y,obj->dir_x, vdir,obj);
				}
				if(obj->dir_x == CK_Dir_Right){
					if(ck_localGameState.ck_bombs[obj->var4])
						LK_SpawnBomb(obj->pos_x+16,obj->pos_y,obj->dir_x, vdir,obj);
				}
			}
			if(obj->ck_frame == & ck_KeenJumpThrow2){
				ck_localGameState.thrown_bomb[obj->var4] = true;
				if(obj->dir_x == CK_Dir_Left){
					if(ck_localGameState.ck_bombs[obj->var4])
						LK_SpawnBomb(obj->pos_x-8,obj->pos_y,obj->dir_x,vdir,obj);
				}
				if(obj->dir_x == CK_Dir_Right){
					if(ck_localGameState.ck_bombs[obj->var4])
						LK_SpawnBomb(obj->pos_x+16,obj->pos_y,obj->dir_x,vdir,obj);
				}
			}
			if(obj->ck_frame == & ck_KeenJumpThrowDown2){
				ck_localGameState.thrown_bomb[obj->var4] = true;
				if(ck_localGameState.ck_bombs[obj->var4])
					LK_SpawnBomb(obj->pos_x,obj->pos_y+16,CK_Dir_None,CK_Dir_Down,obj);
			}
			if(ck_localGameState.thrown_bomb[obj->var4]){
				if(ck_localGameState.ck_bombs[obj->var4]){
					ck_localGameState.ck_bombs[obj->var4] -= 1;
					ck_localGameState.update_scorebox = true;
				}else{
					LK_SD_PlaySound(CK_SND_NO_WAY);
				}				
			}
		}
	}

	if(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.shoot_set]){
		if(ck_localGameState.has_shot[obj->var4] == false){
			int hasShot = 0;
			ck_localGameState.has_shot[obj->var4] = true;
			ck_localGameState.is_pogoing[obj->var4] = false;

			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_UP){

				if(ck_localGameState.ck_shots[obj->var4]){
					LK_SpawnShot(obj->pos_x,obj->pos_y-8,CK_Dir_None,CK_Dir_Up,obj->var4);
					hasShot = 1;
				}
				if(ck_localGameState.on_ground[obj->var4] && ck_localGameState.is_pogoing[obj->var4]==false){
					obj->animation_tick = 0;
					obj->state = ck_objShooting;
					obj->ck_frame = &ck_KeenShootUp;
					obj->can_move = false;
					obj->accel_x = 0;
				}else{
					ck_localGameState.is_pogoing[obj->var4] = false;
					obj->ck_frame = &ck_KeenJumpShootUp;
				}
			}else
			if(ck_localGameState.ck_keeninputs[obj->var4] & GBA_BUTTON_DOWN){
				if(!ck_localGameState.on_ground[obj->var4]){
					if(ck_localGameState.ck_shots[obj->var4]){
						LK_SpawnShot(obj->pos_x,obj->pos_y+16,CK_Dir_None,CK_Dir_Down,obj->var4);
						hasShot = true;
					}
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenShootDown;
					ck_localGameState.is_pogoing[obj->var4] = false;
				}
			}else{
				if(ck_localGameState.on_ground[obj->var4] && ck_localGameState.is_pogoing[obj->var4]==false){
					obj->state = ck_objShooting;
					obj->animation_tick = 0;
					obj->ck_frame = &ck_KeenShoot;
					obj->can_move = false;
					if(ck_localGameState.ck_shots[obj->var4]){

						if(obj->dir_x == CK_Dir_Left){
							LK_SpawnShot(obj->pos_x-8,obj->pos_y,obj->dir_x,CK_Dir_None,obj->var4);
						}
						if(obj->dir_x == CK_Dir_Right){
							LK_SpawnShot(obj->pos_x+16,obj->pos_y,obj->dir_x,CK_Dir_None,obj->var4);
						}
						hasShot = true;
					}
				}
				else{
					if(obj->ck_frame != &ck_KeenJumpShoot){
						obj->state = ck_objFalling;
						ck_localGameState.is_pogoing[obj->var4] = false;
						obj->animation_tick = 0;
						obj->ck_frame = &ck_KeenJumpShoot;
						if(ck_localGameState.ck_shots[obj->var4]){
							if(obj->dir_x == CK_Dir_Left){
								LK_SpawnShot(obj->pos_x-8,obj->pos_y,obj->dir_x,CK_Dir_None,obj->var4);
							}
							if(obj->dir_x == CK_Dir_Right){
								LK_SpawnShot(obj->pos_x+16,obj->pos_y,obj->dir_x,CK_Dir_None,obj->var4);
							}
							hasShot = true;
						}
					}
				}
			}
			if(hasShot){
				ck_localGameState.ck_shots[obj->var4] -= 1;
				ck_localGameState.update_scorebox = true;
				LK_SD_PlaySound(CK_SND_KEEN_SHOT);
			}else{
				if(ck_localGameState.ck_shots[obj->var4]==0){
					LK_SD_PlaySound(CK_SND_CLICK);
				}
			}
		}
	}else{
		if(obj->ck_frame != &ck_KeenShootUp && obj->ck_frame != &ck_KeenJumpShootUp &&
			obj->ck_frame != &ck_KeenShootDown && obj->ck_frame != &ck_KeenShoot && obj->ck_frame != &ck_KeenShootPole){
			ck_localGameState.has_shot[obj->var4] = false;
			if(obj->state == ck_objShooting){
				obj->state = ck_objStanding;
				obj->can_move = true;
			}
		}
	}


	if(ck_localGameState.ck_lastkeeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.pogo_set]){
		obj->dir_y = CK_Dir_Up;

		// stop
		if(ck_localGameState.is_pogoing[obj->var4]){
			obj->state = ck_objFalling;
			obj->animation_tick = 0;
			obj->ck_frame = &ck_KeenFall;
			ck_localGameState.is_pogoing[obj->var4] = false;
		}else{
			// Start pogoing
			if(obj->state == ck_objStanding || obj->state == ck_objWalking || obj->state == ck_objJumping || obj->state == ck_objFalling){
				obj->state = ck_objBouncing;
				obj->animation_tick = 0;
				obj->ck_frame = &ck_KeenPogo2;
				if(ck_localGameState.on_ground[obj->var4]){
					obj->accel_y = -1;

					if(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.jump_set]){
						obj->accel_y -= 1;
					}
				}
				ck_localGameState.is_pogoing[obj->var4] = true;
			}
		}
		// Remove last input
		ck_localGameState.ck_lastkeeninputs[obj->var4] = 0;
	}


	if(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.jump_set]){
		obj->dir_y = CK_Dir_Up;
		obj->can_grab = 0;
		// Start jumping
		if(obj->state == ck_objStanding || obj->state == ck_objWalking){
			obj->state = ck_objJumping;
			obj->animation_tick = 0;
			obj->ck_frame = &ck_KeenJump1;
			obj->accel_y = -3;
			ck_localGameState.on_ground[obj->var4] = false;
			// Play sound
			LK_SD_PlaySound(CK_SND_JUMP);

		}
		if(obj->state == ck_objJumping){
			if(obj->ck_frame == &ck_KeenJump1)
				if(obj->accel_y < 3){
					obj->accel_y = -6;
				}	
		}
	}


	if(obj->ck_frame == &ck_KeenFall){
		obj->state  = ck_objFalling;
	}

	if(ck_localGameState.on_ground[obj->var4]){
		if(ck_localGameState.is_pogoing[obj->var4]){
			// Play bounce sound
			LK_SD_PlaySound(CK_SND_POGO);

			obj->animation_tick = 0;
			obj->ck_frame = &ck_KeenPogo2; // Reset the animation

			if(ck_localGameState.ck_keeninputs[obj->var4] & CK_GBA_BUTTON_SET[ck_localGameState.jump_set]){
				obj->accel_y -= 12;
				ck_localGameState.on_ground[obj->var4] = false;
			}else{
				obj->accel_y -= 8;
				ck_localGameState.on_ground[obj->var4] = false;
			}
		}
	}


};




short globalCK_X = 0;
short globalCK_Y = 0;
short globalCK_UpdateCam = 0;
short globalCK_Vel = 0;



void LK_UpdateKeen(ck_object *obj){
	short absAccelY = GBA_ABS(obj->accel_y);
	short oldAccelY;
	int a;

	// Keep keen from going too fast
	if(obj->accel_y > 20)
		obj->accel_y = 20;

	if(obj->accel_y < -20)
		obj->accel_y = -20;

	if(obj->accel_x > 4)
		obj->accel_x = 4;

	if(obj->accel_x < -4)
		obj->accel_x = -4;


	if(obj->frozen) return; // Don't change anything!
	if(absAccelY>=8){
		oldAccelY = obj->accel_y;
		if(oldAccelY<0)
			obj->accel_y = -4; // Make it more manageable!
		else 
			obj->accel_y = 4; 
		for(a = 0; a < absAccelY; a += 4){
			LK_MapCollideKeen(obj);
			obj->pos_y += obj->accel_y;
		}
		if(obj->accel_y!=0){
			obj->accel_y = oldAccelY;
		}
	}else{
		LK_MapCollideKeen(obj);
		obj->pos_y += obj->accel_y;
	}
	if(obj->frozen2) goto endupdatefun; // Don't change anything!
	obj->accel_y += 1;

	if(obj->can_move==false)
		obj->accel_x = 0;

	obj->pos_x += obj->accel_x;

	if(!ck_localGameState.is_pogoing[obj->var4]){
		if(obj->state == ck_objStanding || obj->state == ck_objWalking){
			obj->accel_x = 0;
		}else if(obj->state == ck_objStanding){
			if(obj->accel_x < 0) obj->accel_x += 1;
			if(obj->accel_x > 0) obj->accel_x -= 1;
		}else{
			if(ck_randomTick & 1){
				if(obj->accel_x < 0) obj->accel_x += 1;
				if(obj->accel_x > 0) obj->accel_x -= 1;
			}
		}
	}
endupdatefun:
	if(obj->var4 == GBA_SerialID){
		globalCK_X = obj->pos_x;
		globalCK_Y = obj->pos_y;
		
		globalCK_Vel = obj->accel_y;
		
		if(ck_localGameState.on_ground[obj->var4] || obj->frozen || obj->frozen2 || obj->can_move == false){
			globalCK_UpdateCam = 1;
		}else{
			globalCK_UpdateCam = 0;
		}
	}
};


void LK_UpdateNetKeen(ck_object *obj){
	// We dont update keen??
};


ck_object * ck_keen_objs[4];

void LK_SpawnKeen(int x,int y, unsigned short player_id){
	ck_object * obj = LK_GetNewObj(0);
	
	ck_keen_objs[player_id] = obj;

	obj->pos_x = x;
	obj->pos_y = y;
	obj->spawn_x = x;
	obj->spawn_y = y;
	obj->accel_x = 0;
	obj->accel_y = 0;
	obj->dir_x = CK_Dir_Right;
	obj->dir_y = CK_Dir_None;

	obj->state = ck_objStanding;
	obj->type = ck_objPlayer;

	obj->var1 = obj->var2 = obj->var3 = obj->var4 = 0;

	obj->var1 = 0x40; // Invincable timer
	obj->var2 = 0; // Respawn timer
	obj->var3 = 0; // Climb counter
	obj->var4 = player_id;

	obj->trying_to_move = 0;
	obj->interacting = 0;

	obj->clip_rects.clipX = 4;
	obj->clip_rects.clipY = 0;
	obj->clip_rects.clipW = 8;
	obj->clip_rects.clipH = 24;

	obj->can_move = true;
	obj->frozen = false;
	obj->frozen2 = false;

#ifdef LK_MULTIBOOT_ROM
	if(obj->spr_id1==128)
		obj->spr_id1 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,ck_localGameState.player_pics[obj->var4]);
	else
		GBA_RemakeSprite(obj->spr_id1,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,ck_localGameState.player_pics[obj->var4]);
	if(obj->spr_id2==128)
		obj->spr_id2 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_8, 0, GBA_SPRITE_ZMID,ck_localGameState.player_pics[obj->var4]);
	else
		GBA_RemakeSprite(obj->spr_id2,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_8, 0, GBA_SPRITE_ZMID,ck_localGameState.player_pics[obj->var4]);
	if(obj->spr_id3==128)
		obj->spr_id3 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,ck_localGameState.player_pics[obj->var4]);
	else
		GBA_RemakeSprite(obj->spr_id3,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,ck_localGameState.player_pics[obj->var4]);
#else
	if(obj->spr_id1==128)
		obj->spr_id1 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,-1);
	else
		GBA_RemakeSprite(obj->spr_id1,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,-1);
	if(obj->spr_id2==128)
		obj->spr_id2 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_8, 0, GBA_SPRITE_ZMID,-1);
	else
		GBA_RemakeSprite(obj->spr_id2,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_8, 0, GBA_SPRITE_ZMID,-1);
	if(obj->spr_id3==128)
		obj->spr_id3 = GBA_CreateSprite(GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,-1);
	else
		GBA_RemakeSprite(obj->spr_id3,GBA_SCREEN_WIDTH+32, 0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID,-1);
#endif
	obj->animation_tick = 0;
	obj->ck_frame = &ck_KeenStand;
	obj->ck_render = &LK_DrawKeen;
//	if(ck_localGameState.player_ips[GBA_SerialID] == player_id){
		obj->ck_input = &LK_LogicKeen;
		obj->ck_update_obj = &LK_UpdateKeen;
	//}else{
//		obj->ck_input = &LK_LogicNetKeen;
//		obj->ck_update_obj = &LK_UpdateNetKeen;
	//}
	obj->ck_collide_obj = &LK_PhysCollideKeen;
};




////////////////////////////////////////////
// OBJECTS
/////////////////////////////////////////////


ck_object CK_ObjectList[100];
unsigned short ck_NumOfObjects = 0;

void LK_SetupObjects(){
	int i = 0;
	for(i = 0; i < 100; i++){
		CK_ObjectList[i].spr_id1 = 128;
		CK_ObjectList[i].spr_id2 = 128;
		CK_ObjectList[i].spr_id3 = 128;
		CK_ObjectList[i].ck_frame = NULL;
	}
	ck_NumOfObjects = 0;
};

ck_object *LK_GetNewObj(char priority){
	int id = 0;

	for(id = 0; id < ck_NumOfObjects; id++){
		if(CK_ObjectList[id].ck_frame==NULL){
			return &CK_ObjectList[id];
		}
	}
	ck_NumOfObjects += 1;

	if(ck_NumOfObjects>100) ck_NumOfObjects = 100;

	return &CK_ObjectList[ck_NumOfObjects]; // Kill the game! (Dun dun DUN!)
};

void LK_RemoveObj(unsigned short index){
	GBA_RemoveSprite(CK_ObjectList[index].spr_id1);
	GBA_RemoveSprite(CK_ObjectList[index].spr_id2);
	GBA_RemoveSprite(CK_ObjectList[index].spr_id3);
	CK_ObjectList[index].ck_frame = NULL;
};


void LK_RemoveNonKeenObjs(){
	int i;
	for(i = 0; i < 100; i++){
		if(CK_ObjectList[i].type != ck_objPlayer){
			GBA_RemoveSprite(CK_ObjectList[i].spr_id1);
			GBA_RemoveSprite(CK_ObjectList[i].spr_id2);
			GBA_RemoveSprite(CK_ObjectList[i].spr_id3);
			CK_ObjectList[i].ck_frame = NULL;
		}
	}
};


boolean LK_ObjInObj(ck_object *obj1, ck_object *obj2){
	// Return true if object collision box is inside other box
	if( obj1->pos_x+obj1->clip_rects.clipX + obj1->clip_rects.clipW >= obj2->pos_x + obj2->clip_rects.clipX &&
		obj1->pos_x+obj1->clip_rects.clipX <= obj2->pos_x + obj2->clip_rects.clipX +obj2->clip_rects.clipW &&
		obj1->pos_y+obj1->clip_rects.clipY + obj1->clip_rects.clipH >= obj2->pos_y + obj2->clip_rects.clipY &&
		obj1->pos_y+obj1->clip_rects.clipY <= obj2->pos_y + obj2->clip_rects.clipY + obj2->clip_rects.clipH){
		return true;
	}
	return false;
};

short ck_ObjAniLatch = 0;


void LK_UpdateObjects(void){
	int i,e;
	ck_object * obj = NULL;
	ck_object * obj2 = NULL;

	for(i = 0; i <= ck_NumOfObjects; i++){
		obj = &CK_ObjectList[i];
		if(obj->ck_frame != NULL){
			obj->animation_tick += 1;
			if(obj->animation_tick >= obj->ck_frame->ticks){
				obj->animation_tick = 0;
				obj->ck_frame = obj->ck_frame->next;
			}


			// Update position
			obj->ck_input(obj);
			// Update the object
			obj->ck_update_obj(obj);
			
			for(e = 0; e <= ck_NumOfObjects; e++){
				// Don't collide with its self!
				if(i==e) continue; 
				obj2 = &CK_ObjectList[e];
				if(LK_ObjInObj(obj,obj2))
					obj->ck_collide_obj(obj,obj2);
			}
		}else{
			// Render that sprite offscreen
			if(obj->pos_x != 0xF0){
				obj->pos_x = 0xF0;
				obj->pos_y = 0xA0;
				GBA_SET_SPRITE_POSITION(obj->spr_id1, 0xF0, 0xA0);
			}
		}
	}

};

void LK_RenderObjects(void){
	int i;
	ck_object * obj = NULL;

	for(i = 0; i <= ck_NumOfObjects; i++){
		obj = &CK_ObjectList[i];
		if(obj->ck_frame != NULL){
			obj->ck_render(obj);
		}
	}

	GBA_UPDATE_SPRITES()

};
