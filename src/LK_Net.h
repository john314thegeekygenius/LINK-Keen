
#ifndef __LK_NET__
#define __LK_NET__


#define CK_DATA_PACKET 0x1000
#define CK_LOCATE_PACKET 0x2000
#define CK_HANDSHAKE_PACKET 0x4403

#define CK_INPUT_PACKET 0x8000
#define CK_SUCCESS_PACKET 0xC000

#define CK_QUIT_MATCH 0xF0F0
#define CK_ABORT_PACKET 0xA0A0

extern uint16_t ck_localPacket;

boolean LK_NT_InitMatch();

void LK_NT_UpdatePackets();

void LK_NT_Send(unsigned short packet);


#endif

