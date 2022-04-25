/*

	Link-Keen GBA Port of Netkeen

	Started Jan 25 2022 by John314

*/

// Include the LK_Heads.h
#include "LK_Heads.h"


char * waitingtext[5] = {
	"Waiting for players 1 of 3",
	"Waiting for players 2 of 3",
	"Waiting for players 1 of 4",
	"Waiting for players 2 of 4",
	"Waiting for players 3 of 4",
};

uint16_t ck_localPacket = 0;


boolean LK_NT_InitMatch(){
	int i,e;
	unsigned short packet = 0;
	int playerCount = 0; 
	int GB_Connected = 0;
	int connectedPlayers[4];
	boolean redrawme = true;
	boolean canbreak = false;

	for(i = 0; i < 4; i++){
		connectedPlayers[i] = 0;
	}
	
	// Our IP is GBA_SerialID
//	ck_localGameState.player_ips[0] = GBA_SerialID;
//	connectedPlayers[GBA_SerialID] = 1;

	// Fix image
	ck_localGameState.player_pics[GBA_SerialID] = ck_localGameState.player_pics[0];
	// Fix team
	ck_localGameState.player_teams[GBA_SerialID] = ck_localGameState.player_teams[0];

	// Get the number of GBAs needed to connect
	if(GBA_SerialID!=0){
		GBA_UpdateSerial();
		if((GBA_SerialData[0] & 0xFF00) == CK_LOCATE_PACKET){
			ck_localGameState.num_players = GBA_SerialData[0]&0xFF;
		}
	}
	GB_Connected = 0;
	
	while(GB_Connected<ck_localGameState.num_players){
		
		// Cancel if B is pressed
		if( (~GBA_BUTTONS) & GBA_BUTTON_B){
			// Send abort byte
			GBA_Serial_SendWord(CK_ABORT_PACKET);
			// Wait a bit so we make sure everyone gets it
			GBA_Delay(100);
			GBA_UpdateSerial();
			return true;
		}

		GB_Connected = 0;
		if(GBA_SerialID==0){
			for(i=0;i<ck_localGameState.num_players;i++){
				if(connectedPlayers[i]==0){
					if((GBA_SerialData[i] & 0xFF00) == CK_LOCATE_PACKET){
						ck_localGameState.player_ips[i] = i;
						connectedPlayers[i] = 1;
						playerCount ++;
						redrawme = true;
					}
				}
			}
			for(i=0;i<4;i++)
				if(connectedPlayers[i]==1)
					GB_Connected ++;
		}else{
			GB_Connected = 0;
			for(i=0;i<4;i++)
				if(connectedPlayers[i]==1)
					GB_Connected ++;

			for(i=0;i<ck_localGameState.num_players;i++){
				if(connectedPlayers[i]==0){
					if((GBA_SerialData[i] & 0xFF00) == CK_LOCATE_PACKET){
						// Found a player!
						for(e = 0; e < ck_localGameState.num_players; e++)
							if(connectedPlayers[e]==0)
								ck_localGameState.player_ips[e] = i;
						connectedPlayers[i] = 1;
						redrawme = true;
						playerCount ++;
					}
				}else{
					if(GBA_SerialData[i] == CK_ABORT_PACKET){
						// That player left, so reset slot
						connectedPlayers[i] = 0;
						ck_localGameState.player_ips[i] = 0;
						playerCount --;
						redrawme = true;
					}
				}
			}
		}
		if(redrawme){
			redrawme = false;
			// Render a cyan screen with text
			GBA_DMA_MemSet16((uint16_t*)GBA_BG0_Map,0x96,32*32);
			GBA_DMA_MemSet16((uint16_t*)GBA_BG1_Map,0,32*32);
			// Render text
			US_TextX = 1; US_TextY = 8;
			if(ck_localGameState.num_players==2){
				LK_US_TextBox("Waiting for players 1 of 2");
			}else if(ck_localGameState.num_players==3){
				if(playerCount>=1&&playerCount<3)
					LK_US_TextBox(waitingtext[playerCount-1]);
			}else if(ck_localGameState.num_players==4){
				if(playerCount>=1&&playerCount<4)
					LK_US_TextBox(waitingtext[playerCount+1]);
			}
			/*
			US_TextX = 1; US_TextY = 0;
			LK_US_PrintXY(LK_US_Itoa(GB_Connected));
			US_TextX = 12; US_TextY = 0;
			LK_US_PrintXY(LK_US_Itoa(playerCount));
			US_TextX = 1; US_TextY = 3;
			LK_US_PrintXY(LK_US_Itoa(ck_localGameState.num_players));
			*/
		}
		// Send data
		if(GBA_SerialID==0){
			GBA_Serial_SendWord(CK_LOCATE_PACKET | ck_localGameState.num_players);
		}else{
			GBA_Serial_SendWord(CK_LOCATE_PACKET);
		}
		// Find players
		GBA_UpdateSerial();
	};

	// Render a cyan screen with text
	GBA_DMA_MemSet16((uint16_t*)GBA_BG0_Map,0x96,32*32);
	GBA_DMA_MemSet16((uint16_t*)GBA_BG1_Map,0,32*32);
	// Render text
	US_TextX = 6; US_TextY = 8;
	LK_US_TextBox("Please wait...");

	if(GBA_SerialID==0){
		GB_Connected = 0;
		while(GB_Connected<ck_localGameState.num_players){
			for(i =0 ; i < ck_localGameState.num_players;i++){
				if(GBA_SerialData[i] == CK_HANDSHAKE_PACKET){
					GB_Connected += 1;
				}
			}
			GBA_Serial_SendWord(CK_HANDSHAKE_PACKET);
			// Make sure we send that packet
			GBA_UpdateSerial();
		}
	}else{
		while(GBA_SerialData[0] == CK_HANDSHAKE_PACKET){
			GBA_Serial_SendWord(CK_HANDSHAKE_PACKET);
			// Make sure we send that packet
			GBA_UpdateSerial();
		}
		GBA_ClearSerial();
	}
	
	if(GBA_SerialID==0){
		// Wait a bit
		GBA_Delay(100);
		GBA_ClearSerial();
	}
	

	for(i=0;i<4;i++)
		connectedPlayers[i] = 0;
	
	canbreak = false;
	while(canbreak==false){
		// Send data
		GBA_Serial_SendWord(CK_DATA_PACKET | 
			(ck_localGameState.player_pics[GBA_SerialID]&0xF) | 
			((ck_localGameState.level_id&0xFF)<<4)
			);
		// Collect information
		GBA_UpdateSerial();

		for(i=0;i<ck_localGameState.num_players;i++){
			if((GBA_SerialData[i]&0xF000) == CK_DATA_PACKET){
				connectedPlayers[i] = 1;
				ck_localGameState.player_pics[i] = GBA_SerialData[i]&0xF;
				if(i==0)
					ck_localGameState.level_id = (GBA_SerialData[i]>>4)&0xFF;
			}else if(GBA_SerialData[i] == CK_HANDSHAKE_PACKET){
				// That GBA is lagging behind
			}else{
				int d = GBA_SerialData[i];
				LK_US_ThrowError("Bad Packet");
				LK_US_ThrowError(LK_US_Htoa16(d));
				return true;
			}
		}
		canbreak = true;
		for(i=0;i<ck_localGameState.num_players;i++)
			if(connectedPlayers[i] == 0)
				canbreak = false;
	}
	canbreak = false;
	while(canbreak==false){
		// Send data
		GBA_Serial_SendWord(CK_DATA2_PACKET | 
			((ck_localGameState.start_shots/5)&0x3F) | 
			(((ck_localGameState.start_health/5)&0x3F)<<6)
			);
//			((ck_localGameState.start_bombs&0xF)<<4) |

		// Collect information
		GBA_UpdateSerial();

		for(i=0;i<ck_localGameState.num_players;i++){
			if((GBA_SerialData[i]&0xF000) == CK_DATA2_PACKET){
				connectedPlayers[i] = 1;
				if(i==0){
					ck_localGameState.start_shots = (GBA_SerialData[i]&0x3F)*5;
					ck_localGameState.start_health = ((GBA_SerialData[i]>>6)&0x3F)*5;
				}
			}else if(GBA_SerialData[i] == CK_HANDSHAKE_PACKET){
				// That GBA is lagging behind
			}else{
				int d = GBA_SerialData[i];
				LK_US_ThrowError("Bad Packet");
				LK_US_ThrowError(LK_US_Htoa16(d));
				return true;
			}
		}
		canbreak = true;
		for(i=0;i<ck_localGameState.num_players;i++)
			if(connectedPlayers[i] == 0)
				canbreak = false;
	}
	canbreak = false;
	while(canbreak==false){
		// Send data
		GBA_Serial_SendWord(CK_DATA_PACKET | 
			((ck_localGameState.start_bombs&0xF)) |
			((ck_localGameState.end_kills&0x7F)<<4)			
			);

		// Collect information
		GBA_UpdateSerial();

		for(i=0;i<ck_localGameState.num_players;i++){
			if((GBA_SerialData[i]&0xF000) == CK_DATA_PACKET){
				connectedPlayers[i] = 1;
				if(i==0){
					ck_localGameState.start_bombs = GBA_SerialData[i]&0xF;
					ck_localGameState.end_kills = (GBA_SerialData[i]>>4)&0x7F;
				}
			}else if(GBA_SerialData[i] == CK_HANDSHAKE_PACKET){
				// That GBA is lagging behind
			}else{
				int d = GBA_SerialData[i];
				LK_US_ThrowError("Bad Packet");
				LK_US_ThrowError(LK_US_Htoa16(d));
				return true;
			}
		}
		canbreak = true;
		for(i=0;i<ck_localGameState.num_players;i++)
			if(connectedPlayers[i] == 0)
				canbreak = false;
	}
	canbreak = false;
	while(canbreak==false){
		// Send data
		GBA_Serial_SendWord(CK_DATA2_PACKET | 
			((ck_localGameState.teamGame&0x1)) |
			((ck_localGameState.hazard_penalty&0x1)<<1) |
			((ck_localGameState.player_teams[GBA_SerialID]&0x7)<<2)
			);
//			((ck_localGameState.start_bombs&0xF)<<4) |

		// Collect information
		GBA_UpdateSerial();

		for(i=0;i<ck_localGameState.num_players;i++){
			if((GBA_SerialData[i]&0xF000) == CK_DATA2_PACKET){
				connectedPlayers[i] = 1;
				ck_localGameState.player_teams[i] = (GBA_SerialData[i]&0x7)>>2;
				if(i==0){
					ck_localGameState.teamGame = (GBA_SerialData[i]&0x1);
					ck_localGameState.hazard_penalty = (GBA_SerialData[i]&0x2)>>1;
				}
			}else if(GBA_SerialData[i] == CK_HANDSHAKE_PACKET){
				// That GBA is lagging behind
			}else{
				int d = GBA_SerialData[i];
				LK_US_ThrowError("Bad Packet");
				LK_US_ThrowError(LK_US_Htoa16(d));
				return true;
			}
		}
		canbreak = true;
		for(i=0;i<ck_localGameState.num_players;i++)
			if(connectedPlayers[i] == 0)
				canbreak = false;
	}
	
	return false;
};


void LK_NT_UpdatePackets(){
	int i,e;
	GBA_Serial_SendWord(ck_localPacket);
	ck_localPacket = 0; // We don't want to send the same packet twice!
	GBA_UpdateSerial();

	for(i=0;i<ck_localGameState.num_players;i++){
		if(GBA_SerialData[i]==CK_QUIT_MATCH){
			// Quit to main menu
			ck_localGameState.in_game = false;
			LK_EndMatch();
			return;
		}else if((GBA_SerialData[i] & 0xF000) == CK_INPUT_PACKET){
			if(ck_localGameState.ck_keeninputs[i]!=(GBA_SerialData[i]&0x3FF)){
				ck_localGameState.ck_lastkeeninputs[i] = ck_localGameState.ck_keeninputs[i];
			}
			ck_localGameState.ck_keeninputs[i] = GBA_SerialData[i]&0x3FF;
		}else if((GBA_SerialData[i] & 0xFF00) == CK_DATA_PACKET){
			// Uh what?
		}else if((GBA_SerialData[i] & 0xFF00) == CK_LOCATE_PACKET){
			// Ummm....
		}else if((GBA_SerialData[i] & 0xFF00) == CK_HANDSHAKE_PACKET){
			// Uhhh....
		}else if(GBA_SerialData[i] == GBA_COM_NODATA){
		}else if(GBA_SerialData[i] == GBA_COM_DISCONNECTED){
			ck_localGameState.player_connected[i] = false;
			// Throw an error?
//				CK_US_ThrowError("Player Disconected");
		}else{
			int d = GBA_SerialData[i];
			LK_US_ThrowError("Bad Packet");
			LK_US_ThrowError(LK_US_Htoa16(d));
			// Uh oh!
			GBA_Serial_SendWord(CK_QUIT_MATCH);
			// Quit to main menu
			ck_localGameState.in_game = false;
			LK_EndMatch();
			return;
		}
	}
	GBA_ClearSerial();

};


void LK_NT_Send(unsigned short packet){
	ck_localPacket = packet;
};

