
#ifndef __LK_OBJECT__
#define __LK_OBJECT__


#define CK_Dir_Left -1
#define CK_Dir_Up -1
#define CK_Dir_None 0
#define CK_Dir_Right 1
#define CK_Dir_Down 1

typedef struct ck_animation {
//	GBA_SpriteSizes ck_frame_size[3]; // Size of animation frame
	unsigned short offset[3]; // Offset in sprites memory to frame
	short spr_offs[3][2]; // offsets for each sprite
	unsigned short ticks; // How long does the animation last
	struct ck_animation * next; // Next animation
}ck_animation;


typedef struct ck_clip_box {
	short clipX;
	short clipY;
	short clipW;
	short clipH;
}ck_clip_box;

typedef enum {
	ck_objIdle,
	ck_objStanding,
	ck_objWalking,
	ck_objJumping,
	ck_objIdlePole,
	ck_objClimbPole,
	ck_objSlidePole,
	ck_objShootPole,
	ck_objThrowPole,
	ck_objFalling,
	ck_objShooting,
	ck_objThrowing,
	ck_objBouncing,
	ck_objHolding,
	ck_objClimbing,
	ck_objDead,
	ck_objEnterDoor,
}ck_objectState;

typedef enum {
	ck_objNone,
	ck_objPlayer,
	ck_objShot,
	ck_objEnemyShot,
	ck_objBomb,
	ck_objKill,
	ck_objItem,
	ck_objShadow,
}ck_objectType;

typedef struct ck_object {
	short pos_x; // global x
	short pos_y; // global y
	short spawn_x; // local x
	short spawn_y; // local y
	short topos_x; // to x
	short topos_y; // to y
	short accel_x;
	short accel_y;
	short dir_x;
	short dir_y;

	ck_clip_box clip_rects;

	ck_objectState state; // What state is the object in
	ck_objectType type; // What kind of object is it

	short var1; // variables for random things
	short var2;
	short var3;
	short var4;

	int int1; // variables for random things
	int int2;

	boolean can_move;
	boolean can_grab;
	boolean can_climb;
	short interacting; // Is keen interacting with anything (aka. looking up or down)
	short trying_to_move; // Is keen trying to move left or right
	boolean frozen; // Keeps keen still
	boolean frozen2; // Keeps keen still but collides with the map

	unsigned short spr_id1; // index of first sprite
	unsigned short spr_id2; // index of second sprite
	unsigned short spr_id3; // index of third sprite

	unsigned short animation_tick; // How long has the animation been going for
	ck_animation *ck_frame; // Pointer to animation frame
	void (*ck_render)(struct ck_object *obj);
	void (*ck_input)(struct ck_object *obj);
	void (*ck_collide_obj)(struct ck_object *obj1,struct ck_object *obj2);
	void (*ck_update_obj)(struct ck_object *obj);
	
}ck_object;


extern ck_object * ck_keen_objs[4];
extern short globalCK_X;
extern short globalCK_Y;
extern short globalCK_UpdateCam;
extern short globalCK_Vel;

// Objects stuff
void LK_SetupObjects();
ck_object *LK_GetNewObj(char priority);
void LK_RemoveObj(unsigned short index);
void LK_RemoveNonKeenObjs();

void LK_UpdateObjects(void);
void LK_RenderObjects(void);


// Objects.c stuff
void LK_KillKeen(ck_object *obj);
void LK_SpawnKeen(int x,int y, unsigned short player_id);
void LK_SpawnKeenNPC(int x,int y, unsigned short player_id);

// Act1.c stuff
void LK_ACT_SpawnStunner(int x,int y);

#endif

