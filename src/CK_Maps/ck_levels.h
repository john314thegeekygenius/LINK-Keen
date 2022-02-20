

#include "ck5_tileset0_info.h"
#ifdef LK_MULTIBOOT_ROM
#include "ck5_tileset04b.h"
#include "ck5_tileset14b.h"
#else
#include "ck5_tileset0.h"
#include "ck5_tileset1.h"
#endif
#include "ck_map0.h"
#include "ck_map1.h"
#include "ck_map2.h"


// Define music ids
const uint16_t ck_map0_music = 0;
const uint16_t ck_map1_music = 1;
const uint16_t ck_map2_music = 2;


// Define map info
const char *ck_map0_info[] = {
	"Test map 0",
	"By John314",
	"30 x 20"
};

const char *ck_map1_info[] = {
	"Test map 1",
	"By John314",
	"64 x 20"
};

const char *ck_map2_info[] = {
	"QED Arena",
	"By Mink",
	"64 x 45"
};

const uint16_t *ck_levels_data[] = {
	&ck_map0_data,
	&ck_map1_data,
	&ck_map2_data,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,

};

const unsigned char ** ck_levels_tileset[] = {
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

const uint16_t *ck_levels_tileinfo[] = {
	&ck5_tileset0_tileinfo,
	&ck5_tileset0_tileinfo,
	&ck5_tileset0_tileinfo,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,

};

const uint16_t ck_levels_width[] = {
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

const uint16_t ck_levels_height[] = {
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


const char ** ck_levels_info[] = {
	&ck_map0_info,
	&ck_map1_info,
	&ck_map2_info,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,
	(uint16_t*)NULL,

};

const uint16_t ck_levels_music[] = {
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
