/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"

#include "LK_GFX.h"


//--------------- Floating Items ---------------------------


void LK_ACT_ItemCollidePlayer(ck_object *obj,ck_object *obj2){
	// Do nothing
	if(obj2->type==ck_objPlayer){
	}
};

void LK_ACT_UpdateItem(ck_object *obj){
	// Do nothing
};

void LK_ACT_UpdateShadow(ck_object *obj){
	// Make it go up
	obj->pos_y -= 1;
};

void LK_ACT_DrawItem(ck_object *obj){
	short sprX = obj->ck_frame->spr_offs[0][0] + obj->pos_x - ck_cam_x;
	short sprY = obj->ck_frame->spr_offs[0][1] + obj->pos_y - ck_cam_y;

	// Hide the item if collected
	// or off screen
	if(obj->int1 || sprX < -16 || sprY < -16 || sprX > GBA_SCREEN_WIDTH || sprY > GBA_SCREEN_HEIGHT){
		sprX = 0xF0;
		sprY = 0xA0;
	}

	// Render the sprite
	GBA_SET_SPRITE_POSITION(obj->spr_id1, sprX, sprY);

	GBA_SET_SPRITE_TILES(obj->spr_id1, obj->ck_frame->offset[0]);
};

void LK_ACT_LogicItem(ck_object *obj){
	// Count down timeout timer
	if(obj->int1){
		obj->int1 -= 1;
	}

};




//------------------------------------------ ITEM SHADOW ANIMATIONS ---------------------------------

ck_animation 	ck_StunnerShadow,
				ck_BombShadow,
				ck_HealShadow1,
				ck_HealShadow2,
				ck_HealShadow3,
				ck_HealShadow4;

ck_animation ck_StunnerShadow = {
	{CK_GUN_PICKUP},
	{ {0,0},},
	0x10,
	NULL
};

ck_animation ck_BombShadow = {
	{CK_BOMB_PICKUP},
	{ {0,0},},
	0x10,
	NULL
};

ck_animation ck_HealShadow1 = {
	{CK_HEAL_SHADOW1},
	{ {0,0},},
	0x04,
	&ck_HealShadow2
};

ck_animation ck_HealShadow2 = {
	{CK_HEAL_SHADOW2},
	{ {0,0},},
	0x04,
	&ck_HealShadow3
};

ck_animation ck_HealShadow3 = {
	{CK_HEAL_SHADOW3},
	{ {0,0},},
	0x04,
	&ck_HealShadow4
};

ck_animation ck_HealShadow4 = {
	{CK_HEAL_SHADOW4},
	{ {0,0},},
	0x04,
	NULL
};


void LK_ACT_SpawnShadow(int x,int y, int id){
	ck_object * obj = LK_GetNewObj(0);

	obj->pos_x = x;
	obj->pos_y = y;

	obj->accel_x = 0;
	obj->accel_y = 0;

	obj->dir_x = CK_Dir_None;
	obj->dir_y = CK_Dir_None;

	obj->state = ck_objIdle;
	obj->type = ck_objShadow;

	obj->var1 = obj->var2 = obj->var3 = obj->var4 = 0;
	
	
	obj->int1 = 0; // Respawn timer

	obj->clip_rects.clipX = 2;
	obj->clip_rects.clipY = 2;
	obj->clip_rects.clipW = 12;
	obj->clip_rects.clipH = 12;

	if(obj->spr_id1==128)
		obj->spr_id1 = GBA_CreateSprite(0xF0, 0xA0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID);
	else
		GBA_RemakeSprite(obj->spr_id1,0xF0, 0xA0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID);

	GBA_SET_FLIPH(obj->spr_id1,0)
	GBA_SET_FLIPV(obj->spr_id1,0)

	obj->animation_tick = 0;
	switch(id){
		case 0:
			obj->ck_frame = &ck_StunnerShadow;
		break;
		case 1:
			obj->ck_frame = &ck_BombShadow;
		break;
		default:
		obj->ck_frame = NULL;
	}
	obj->ck_render = &LK_ACT_DrawItem;
	obj->ck_input = &LK_ACT_LogicItem;
	obj->ck_collide_obj = &LK_ACT_ItemCollidePlayer;
	obj->ck_update_obj = &LK_ACT_UpdateShadow;
};


void LK_ACT_SpawnHealthShadow(int x,int y){
	ck_object * obj = LK_GetNewObj(0);

	obj->pos_x = x;
	obj->pos_y = y;

	obj->accel_x = 0;
	obj->accel_y = 0;

	obj->dir_x = CK_Dir_None;
	obj->dir_y = CK_Dir_None;

	obj->state = ck_objIdle;
	obj->type = ck_objShadow;

	obj->var1 = obj->var2 = obj->var3 = obj->var4 = 0;
	
	
	obj->int1 = 0; // Respawn timer

	obj->clip_rects.clipX = 2;
	obj->clip_rects.clipY = 2;
	obj->clip_rects.clipW = 12;
	obj->clip_rects.clipH = 12;

	if(obj->spr_id1==128)
		obj->spr_id1 = GBA_CreateSprite(0xF0, 0xA0, GBA_SPR_16_8, 0, GBA_SPRITE_ZMID);
	else
		GBA_RemakeSprite(obj->spr_id1,0xF0, 0xA0, GBA_SPR_16_8, 0, GBA_SPRITE_ZMID);

	GBA_SET_FLIPH(obj->spr_id1,0)
	GBA_SET_FLIPV(obj->spr_id1,0)

	obj->animation_tick = 0;
	obj->ck_frame = &ck_HealShadow1;
	obj->ck_render = &LK_ACT_DrawItem;
	obj->ck_input = &LK_ACT_LogicItem;
	obj->ck_collide_obj = &LK_ACT_ItemCollidePlayer;
	obj->ck_update_obj = &LK_ACT_UpdateItem;
};


//------------------------------------------ ITEM ANIMATIONS ---------------------------------

ck_animation 	ck_StunnerItem,
				ck_StunnerItemFlash,
				ck_BombItem,
				ck_BombItemFlash,
				ck_HealItem1,
				ck_HealItem2,
				ck_HealItem3,
				ck_HealItem4;

ck_animation ck_StunnerItem = {
	{CK_GUN_PIC},
	{ {0,0},},
	0x08,
	&ck_StunnerItemFlash
};

ck_animation ck_StunnerItemFlash = {
	{CK_GUN_PIC},
	{ {0,-1},}, // Make it animate up 1 pixel?
	0x08,
	&ck_StunnerItem
};


ck_animation ck_BombItem = {
	{CK_BOMB_PIC},
	{ {0,0},},
	0x08,
	&ck_BombItemFlash
};

ck_animation ck_BombItemFlash = {
	{CK_BOMB_PIC},
	{ {0,-1},}, // Make it animate up 1 pixel?
	0x08,
	&ck_BombItem
};

ck_animation ck_HealItem1 = {
	{CK_HEAL_PIC1},
	{ {0,0},},
	0x08,
	&ck_HealItem2
};

ck_animation ck_HealItem2 = {
	{CK_HEAL_PIC2},
	{ {0,0},},
	0x08,
	&ck_HealItem3
};

ck_animation ck_HealItem3 = {
	{CK_HEAL_PIC3},
	{ {0,0},},
	0x08,
	&ck_HealItem4
};

ck_animation ck_HealItem4 = {
	{CK_HEAL_PIC4},
	{ {0,0},},
	0x08,
	&ck_HealItem1
};


// ????
// ~18 * seconds = frames

void LK_ACT_StunnerCol(ck_object *obj,ck_object *obj2){
	if(obj2->type==ck_objPlayer){
		if(obj->int1==0){
			obj->int1 = 540; // wait ~30 seconds
			ck_localGameState.ck_shots[obj2->var4] += 5; // Add 5 shots to the player
			// Cap shots
			if(ck_localGameState.ck_shots[obj2->var4]>99){
				ck_localGameState.ck_shots[obj2->var4] = 99;
			}
			ck_localGameState.update_scorebox = true;
			LK_SD_PlaySound(CK_SND_PICKUP_STUNNER);
			LK_ACT_SpawnShadow(obj->pos_x,obj->pos_y,0);
		}
	}
};


void LK_ACT_BombCol(ck_object *obj,ck_object *obj2){
	if(obj2->type==ck_objPlayer){
		if(obj->int1==0){
			obj->int1 = 540; // wait ~30 seconds
			ck_localGameState.ck_bombs[obj2->var4] += 3; // Add 3 bombs to the player
			if(ck_localGameState.ck_bombs[obj2->var4]>9){
				ck_localGameState.ck_bombs[obj2->var4] = 9;
			}
			ck_localGameState.update_scorebox = true;
			LK_SD_PlaySound(CK_SND_PICKUP_SPECIAL);
			LK_ACT_SpawnShadow(obj->pos_x,obj->pos_y,1);
		}
	}
};

void LK_ACT_HealthCol(ck_object *obj,ck_object *obj2){
	if(obj2->type==ck_objPlayer){
		if(obj->int1==0){
			obj->int1 = 1080; // wait ~60 seconds
			ck_localGameState.ck_health[obj2->var4] += 5; // Add 5 hitpoints to the player
			if(ck_localGameState.ck_health[obj2->var4]>200){
				ck_localGameState.ck_health[obj2->var4] = 200;
			}
			ck_localGameState.update_scorebox = true;
			LK_SD_PlaySound(CK_SND_PICKUP_HEALTH);
			LK_ACT_SpawnHealthShadow(obj->pos_x,obj->pos_y);
		}
	}
};


void LK_ACT_SpawnItem(int x,int y,int id){
	ck_object * obj = LK_GetNewObj(0);

	// Center the items on the tile
	obj->pos_x = x-6;
	obj->pos_y = y-6;

	obj->accel_x = 0;
	obj->accel_y = 0;

	obj->dir_x = CK_Dir_None;
	obj->dir_y = CK_Dir_None;

	obj->state = ck_objIdle;
	obj->type = ck_objItem;

	obj->var1 = obj->var2 = obj->var3 = obj->var4 = 0;
	
	
	obj->int1 = 0; // Respawn timer

	obj->clip_rects.clipX = 2;
	obj->clip_rects.clipY = 2;
	obj->clip_rects.clipW = 12;
	obj->clip_rects.clipH = 12;

	if(obj->spr_id1==128)
		obj->spr_id1 = GBA_CreateSprite(0xF0, 0xA0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID);
	else
		GBA_RemakeSprite(obj->spr_id1,0xF0, 0xA0, GBA_SPR_16_16, 0, GBA_SPRITE_ZMID);

	GBA_SET_FLIPH(obj->spr_id1,0)
	GBA_SET_FLIPV(obj->spr_id1,0)

	obj->animation_tick = 0;
	switch(id){
		case 0:
			obj->ck_frame = &ck_StunnerItem;
			obj->ck_collide_obj = &LK_ACT_StunnerCol;
			break;
		case 1:
			obj->ck_frame = &ck_BombItem;
			obj->ck_collide_obj = &LK_ACT_BombCol;
			break;
		case 2:
			obj->ck_frame = &ck_HealItem1;
			obj->ck_collide_obj = &LK_ACT_HealthCol;
			break;
		default:
			obj->ck_frame = NULL;
			obj->ck_collide_obj = &LK_ACT_ItemCollidePlayer;
		break;
	}
	obj->ck_render = &LK_ACT_DrawItem;
	obj->ck_input = &LK_ACT_LogicItem;
	obj->ck_update_obj = &LK_ACT_UpdateItem;
};


