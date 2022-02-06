

#include "ck5_tileset0_info.h"
#include "ck5_tileset0.h"
#include "ck_map0.h"
#include "ck_map1.h"
#include "ck_map2.h"


uint16_t *ck_levels_data[] = {
	&ck_map0_data,
	&ck_map1_data,
	&ck_map2_data,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,

};

unsigned char * ck_levels_tileset[] = {
	&ck_map0_tileset,
	&ck_map1_tileset,
	&ck_map2_tileset,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

};
uint16_t *ck_levels_tileinfo[] = {
	&ck5_tileset0_tileinfo,
	&ck5_tileset0_tileinfo,
	&ck5_tileset0_tileinfo,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,

};

uint16_t ck_levels_width[] = {
	ck_map0_width,
	ck_map1_width,
	ck_map2_width,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

uint16_t ck_levels_height[] = {
	ck_map0_height,
	ck_map1_height,
	ck_map2_height,
	0,
	0,
	0,
	0,
	0,
	0,
};


char ** ck_levels_info[] = {
	&ck_map0_info,
	&ck_map1_info,
	&ck_map2_info,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,

};

uint16_t ck_levels_music[] = {
	ck_map0_music,
	ck_map1_music,
	ck_map2_music,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
};
