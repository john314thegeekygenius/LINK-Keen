/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"


uint16_t ck_lastInput = 0;
uint16_t ck_curInput = 0;

void LK_IN_ReadInput(){
	if((~GBA_BUTTONS)!=ck_curInput){
		ck_lastInput = ck_curInput;
		ck_curInput = ~GBA_BUTTONS;

		if(!ck_localGameState.multiplayerGame){
			ck_localGameState.ck_keeninputs[GBA_SerialID] = ck_curInput;
			ck_localGameState.ck_lastkeeninputs[GBA_SerialID] = ck_lastInput;
		}
	}
};


