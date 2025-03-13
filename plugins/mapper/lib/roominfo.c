#include "roominfo.h"

bool read_room(RoomInfo *room, FILE *fptr) {
    bool success = true;

    if (fread(&room->symbol, SYMBOL_SIZE, 1, fptr) != 1) success = false;
    if (fread(&room->color, COLOR_SIZE, 1, fptr) != 1) success = false;

    if (!success) {
        room->symbol = ' ';
        room->color = 0;
    } else if (!room->symbol) {
        room->symbol = ' ';
    }

    return success;
}

bool write_room(RoomInfo *room, FILE *fptr) {
    bool success = true;

    if (fwrite(&room->symbol, SYMBOL_SIZE, 1, fptr) != 1) success = false;
    if (fwrite(&room->color, COLOR_SIZE, 1, fptr) != 1) success = false;

    return success;
}

bool seek_room(MapInfo *map, int x, int y, FILE *fptr) {
    return fseek(fptr, MAP_HEADER_BYTES + (y * map->width + x) * ROOM_SIZE, SEEK_SET) == 0;
}

bool advance_one_room(FILE *fptr) {
    return advance_rooms(1, fptr);
}

bool advance_rooms(int count, FILE *fptr) {
    return fseek(fptr, count * ROOM_SIZE, SEEK_CUR) == 0;
}