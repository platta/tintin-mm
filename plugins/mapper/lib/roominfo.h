/**
 * Definition and utility functions for RoomInfo struct.
 */
#ifndef ROOMINFO_H
#define ROOMINFO_H

#include <stdbool.h>
#include <stdio.h>

#include "mapinfo.h"

#define SYMBOL_SIZE sizeof(char)
#define COLOR_SIZE sizeof(unsigned char)
#define ROOM_SIZE (SYMBOL_SIZE + COLOR_SIZE)

/**
 * Represents a single room on the map.
 */
typedef struct RoomInfo {
    char symbol;          // The symbol for this room on the map.
    unsigned char color;  // The value (0-255) for the color code of this room.
} RoomInfo;

/**
 * Read a room from file at the current position.
 * @param room The RoomInfo object to populate.
 * @param fptr Open file sitting at the correct position to read.
 * @returns Boolean indicating success or failure of read.
 */
bool read_room(RoomInfo *room, FILE *fptr);

/**
 * Write a room to file at the current position.
 * @param room The RoomInfo object to write.
 * @param fptr Open file sitting at the correct position to write.
 * @returns Boolean indicating success or failure of the write.
 */
bool write_room(RoomInfo *room, FILE *fptr);

/**
 * Seek the specified room coordinates in a map file.
 * @param map A MapInfo object describing the map file.
 * @param x The x coordinate to seek.
 * @param y The y coordinate to seek.
 * @param fptr Open map file pointer.
 * @returns Boolean indicating success or failure of the seek.
 */
bool seek_room(MapInfo *map, int x, int y, FILE *fptr);

/**
 * Advance the file pointer by one room.
 * @param fptr Open map file pointer.
 * @returns Boolean indicating success or failure of the seek.
 */
bool advance_one_room(FILE *fptr);

/**
 * Advance the file pointer by multiple rooms.
 * @param count The number of rooms to advance.
 * @param fptr Open map file pointer.
 * @returns Boolean indicating success or failure of the seek.
 */
bool advance_rooms(int count, FILE *fptr);

#endif
