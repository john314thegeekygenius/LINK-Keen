
#ifndef __LK_MAP__
#define __LK_MAP__


#define LK_4BPP_MAPS

// Make this 90?
#define MAX_LEVEL_WIDTH 60
#define MAX_LEVEL_HEIGHT 60
#define MAX_LEVEL_ANIMATIONS 64

// Max of 64 levels
#define CK_MAX_LEVELS 4

extern uint16_t * ck_level_data;
extern unsigned short ck_level_width;
extern unsigned short ck_level_height;
extern unsigned short ck_level_music;
extern uint16_t ck_levelbuff[MAX_LEVEL_WIDTH*MAX_LEVEL_HEIGHT*2];
extern uint16_t ck_map_animations[MAX_LEVEL_ANIMATIONS*2];
extern uint16_t **ck_tileinfo;
extern int ck_level_size;

extern int ck_cam_x;
extern int ck_cam_y;
extern int ck_cam_scrollx;
extern int ck_cam_scrolly;

// Tileinfo is stored in a 48bit value
/*

    First byte:

    F.E.D.C  B.A.9.8  7.6.5.4  3.2.1.0

    0-6     Top Flag
    7-D     Bottom Flag
    E       Left Side Solid 
    F       Right Side Solid 


    Second byte:
    0-6     Misc Flag

    7-E     Animation to tile (old - Do not use)
    F       Front

    Third byte:

    0-8     Animation to tile ( Use )
    9-F     Animation Time
    
*/

/*

std::string TileTopFlags[0x7F] = {
    "Fall through",
    "Flat top",
    "Top > Middle",
    "Middle > Bottom",
    "Top > Bottom",
    "Middle > Top",
    "Bottom > Middle",
    "Bottom > Top",
    "",
    "Deadly (No godmode)",
    "","","","","","","",
    "Flat top w/ pole",
    "","","","","","","","","","","","","","","",
    "Giant switch On (ck6)",
    "","","","","","","",
    "Conveyor belt (ck6)",
    "","","","","","","","","","","","","","",
    "Fuse (ck5)",
    ""
};


std::string TileBottomFlags[0x7F] = {
    "Jump through",
    "Flat bottom",
    "Bottom > Middle",
    "Middle > Top",
    "Bottom > Top",
    "Middle > Bottom",
    "Top > Middle",
    "Top > Bottom",
    "",
    "",
    "","","","","","","",
    "Pole going through",
    "","","","","","","","","","","","","","","",
    "Giant switch Off (ck6)",
    "","","","","","","",
    "Conveyor belt (ck6)",
    "","","","","","","","","","","","","","",
    "Fuse (ck5)",
    ""
};


std::string TileMiscFlags[0x7F] = {
    "No special properties",
    "Pole",
    "Door Also entry to Miragia in (Keen 4)",
    "Kill keen",
    "Collect (1/100) 1up",
    "Switch for moving platforms (off) (Keen 4 & 5)",
    "Switch for moving platforms (on) (Keen 4 & 5)",
    "Red keygem holder",
    "Yellow keygem holder",
    "Blue keygem holder",
    "Green keygem holder",
    "Top water entrance (Keen 4)",
    "Right water entrance (Keen 4)",
    "Bottom water entrance (Keen 4)",
    "Left water entrance (Keen 4)",
    "Switch for bridges (Keen 4 & 5)",
    "Moon floor tile (Keen 4)",
    "Sprite path arrow (Keen 5 & 6)",
    "Bridge Keen (Keen 5 & 6)",
    "Active zapper (top) (Keen 6)",
    "Teleport entrance (Keen 5 & 6)",
    "100 points",
    "200 points",
    "500 points",
    "1000 points",
    "2000 points",
    "5000 points",
    "1UP",
    "Neural stunner",
    "",
    "Inactive zapper (Keen 6)",
    "Little Ampton computer (Keen 5)",
    "Keycard door (Keen 5)",
    "Elevator on map (left side) (Keen 5)",
    "Elevator on map (right side) (Keen 5)",
    "",
//$7F     Top of Exit Sign (Keen Dreams) 
};


*/


// Map stuff
char** LK_MP_GetMapInfo(uint16_t mapid);

extern const uint16_t *ck_levels_data[] ;

extern const unsigned char ** ck_levels_tileset[];

extern const uint16_t *ck_levels_tileinfo[] ;

extern const uint16_t ck_levels_width[];

extern const uint16_t ck_levels_height[];

extern const char ** ck_levels_info[];

extern const uint16_t ck_levels_music[];

extern boolean ck_mapneeds_updated;

// Other stuff
void LK_MP_SetTile(uint16_t x, uint16_t y, uint16_t tile, uint16_t plane);
uint16_t LK_MP_GetTile(uint16_t x, uint16_t y, uint16_t plane);

void LK_MP_UpdateCamera();

void LK_MP_LoadMap(uint16_t mapid);
void LK_MP_UpdateMap();
void LK_MP_RenderMap();
void LK_MP_ResetScroll();

// Physics

void LK_MP_ObjVsMap(ck_object *obj);

#endif

