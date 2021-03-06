
#ifdef LK_MULTIBOOT_ROM

#define CK_SHOT_PIC (cki_itemsOffset) + 64
#define CK_SHOT_SPLAT cki_itemsOffset + 64 + 2

#define CK_BAD_SHOT_PIC (cki_itemsOffset) 
#define CK_BAD_SHOT_SPLAT cki_itemsOffset + 2

#define CK_GUN_PIC (cki_itemsOffset) + 4
#define CK_GUN_PICKUP cki_itemsOffset + 64 + 4

#define CK_BOMB_PIC cki_itemsOffset + 6
#define CK_BOMB_PICKUP cki_itemsOffset + 64 + 6
#define CK_BOMB_EXPLODE cki_itemsOffset + 128

#define CK_HEAL_PIC1 cki_itemsOffset + 320 + 4
#define CK_HEAL_PIC2 cki_itemsOffset + 320 + 6
#define CK_HEAL_PIC3 cki_itemsOffset + 320 + 8
#define CK_HEAL_PIC4 cki_itemsOffset + 320 + 10

#define CK_HEAL_SHADOW1 cki_itemsOffset + 320 + 12
#define CK_HEAL_SHADOW2 cki_itemsOffset + 320 + 14
#define CK_HEAL_SHADOW3 cki_itemsOffset + 352 + 12
#define CK_HEAL_SHADOW4 cki_itemsOffset + 352 + 14
#else

#define CK_SHOT_PIC (cki_itemsOffset) + 64
#define CK_SHOT_SPLAT cki_itemsOffset + 64 + 4

#define CK_BAD_SHOT_PIC (cki_itemsOffset) 
#define CK_BAD_SHOT_SPLAT cki_itemsOffset + 4

#define CK_GUN_PIC (cki_itemsOffset) + 8
#define CK_GUN_PICKUP cki_itemsOffset + 64 + 8

#define CK_BOMB_PIC cki_itemsOffset + 12
#define CK_BOMB_PICKUP cki_itemsOffset + 64 + 12
#define CK_BOMB_EXPLODE cki_itemsOffset + 128

#define CK_HEAL_PIC1 cki_itemsOffset + 320 + 8
#define CK_HEAL_PIC2 cki_itemsOffset + 320 + 12
#define CK_HEAL_PIC3 cki_itemsOffset + 320 + 16
#define CK_HEAL_PIC4 cki_itemsOffset + 320 + 20

#define CK_HEAL_SHADOW1 cki_itemsOffset + 320 + 24
#define CK_HEAL_SHADOW2 cki_itemsOffset + 320 + 28
#define CK_HEAL_SHADOW3 cki_itemsOffset + 352 + 24
#define CK_HEAL_SHADOW4 cki_itemsOffset + 352 + 28
#endif

#define CK_BOTTOM_HALF 94
#define CK_NEXT_LINE 47


#define CK_KEEN_STAND 0
#define CK_KEEN_RUN1 2
#define CK_KEEN_RUN2 4
#define CK_KEEN_RUN3 6
#define CK_KEEN_RUN4 8

#define CK_KEEN_JUMP1 10
#define CK_KEEN_JUMP2 12
#define CK_KEEN_FALL  14

#define CK_KEEN_POGO1 16
#define CK_KEEN_POGO2 18

#define CK_KEEN_POLE1 20
#define CK_KEEN_POLE2 22
#define CK_KEEN_POLE3 24

#define CK_KEEN_LOOK_UP 26

#define CK_KEEN_LOOK_DOWN 90
#define CK_KEEN_DEAD 88

#define CK_KEEN_ENTER1 28
#define CK_KEEN_ENTER2 30

#define CK_KEEN_LEDGE1_A 32
#define CK_KEEN_LEDGE1_B 82 + CK_BOTTOM_HALF
#define CK_KEEN_LEDGE2 82
#define CK_KEEN_LEDGE3 84
#define CK_KEEN_LEDGE4 86
#define CK_KEEN_LEDGE5 34

#define CK_KEEN_SLIDE1 36
#define CK_KEEN_SLIDE2 38
#define CK_KEEN_SLIDE3 40
#define CK_KEEN_SLIDE4 42

#define CK_KEEN_SHOOT_DOWN 44

#define CK_KEEN_SHOOT_UP_A 46
#define CK_KEEN_SHOOT_UP_B 92 + CK_BOTTOM_HALF

#define CK_KEEN_JUMP_SHOOT_UP_A 48
#define CK_KEEN_JUMP_SHOOT_UP_B 92

#define CK_KEEN_SHOOT 50
#define CK_KEEN_JUMP_SHOOT 52

#define CK_KEEN_SHOOT_HAND 84 + CK_BOTTOM_HALF + CK_BOTTOM_HALF

#define CK_KEEN_JUMP_SHOOT_HAND 86 + CK_BOTTOM_HALF + CK_BOTTOM_HALF

#define CK_KEEN_POLE_SHOOT 54
#define CK_KEEN_POLE_SHOOT_UP 56
#define CK_KEEN_POLE_SHOOT_DOWN 58

// Uses the same animation as jump shooting
#define CK_KEEN_POLE_SHOOT_HAND 86 + CK_BOTTOM_HALF + CK_BOTTOM_HALF

#define CK_KEEN_THROW1 60
#define CK_KEEN_THROW2 62
#define CK_KEEN_THROW3 64

#define CK_KEEN_THROW_HAND 88 + CK_BOTTOM_HALF + CK_BOTTOM_HALF

#define CK_KEEN_JUMP_THROW1 66
#define CK_KEEN_JUMP_THROW2 68

#define CK_KEEN_JUMP_THROW_HAND 90 + CK_BOTTOM_HALF + CK_BOTTOM_HALF

#define CK_KEEN_JUMP_THROW_DOWN1 70
#define CK_KEEN_JUMP_THROW_DOWN2 72

#define CK_KEEN_POLE_THROW1 74
#define CK_KEEN_POLE_THROW2 76

#define CK_KEEN_POLE_THROW_HAND 92 + CK_BOTTOM_HALF + CK_BOTTOM_HALF

#define CK_KEEN_POLE_THROW_DOWN1 78
#define CK_KEEN_POLE_THROW_DOWN2 80



