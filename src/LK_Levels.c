/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"

#include "CK_Maps/ck_levels.h"

char** LK_MP_GetMapInfo(uint16_t mapid){
	return ck_levels_info[mapid];
};



