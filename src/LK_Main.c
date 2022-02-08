/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"

// Entery point of ROM
int main(void) {

	LK_Init();

	LK_DoGameLoop();

	while(1){} // Loop forever if an error occurs
	return 0; // Crash and Burn!
};

