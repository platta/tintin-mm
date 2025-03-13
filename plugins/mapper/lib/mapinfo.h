#ifndef MAPPER_H
#define MAPPER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAP_HEADER_BYTES 64  // The number of bytes at the beginning of the file reserved for header info.

/**
 * Defaults
 */
#define MAP_DEFAULT_WIDTH 0
#define MAP_DEFAULT_HEIGHT 0
#define MAP_DEFAULT_HWRAP false
#define MAP_DEFAULT_VWRAP false

/**
 * Flags
 */
#define MAP_FLAG_HWRAP 0b00000001  // Flag for if the map should wrap at the left and right.
#define MAP_FLAG_VWRAP 0b00000010  // Flag for if the map should wrap at the top and bottom.

/**
 * Represents possible flags for the map.
 */
typedef struct MapInfo {
    unsigned short width;   // Width of the map.
    unsigned short height;  // Height of the map.
    bool hwrap;             // Indicates if the map should wrap at the left and right.
    bool vwrap;             // Indicates if the map should wrap at the top and bottom.
} MapInfo;

/**
 * Get the default flags.
 * @param map Pointer to the MapInfo object to populate.
 */
void initialize_map_info(MapInfo *map);

/**
 * Read flags from an open file.
 * @param map Pointer to the MapInfo object to populate.
 * @param fptr Open file handle to read.
 */
void read_map_info(MapInfo *map, FILE *fptr);

/**
 * Write map flags to an open map file.
 * @param map Pointer to the MapInfo object to write.
 * @param fptr Open file handle to write.
 */
void write_map_info(MapInfo *map, FILE *fptr);

/**
 * Read program arguments to generate flags.
 * @param map Pointer to the MapInfo object to populate.
 * @param argc Argument count passed to main().
 * @param argv Argument array passed to main().
 * @param start_arg Which argument to start with.
 */
void parse_flags_from_args(MapInfo *map, int argc, char *argv[], int start_arg);

/**
 * Wraps an x coordinate, if necessary and enabled.
 * @param map The map to use for dimensions and flags.
 * @param x_in Current x coordinate, maybe exceeed the bounds of the map.
 * @param x_out Pointer to x variable to update.
 * @returns 0 if out of bounds and wrapping is disabled, 1 if no wrapping was
 *          applied, 2 if wrapping was applied.
 */
bool wrap_x(MapInfo *map, int x_in, int *x_out);

/**
 * Wraps a y coordinate, if necessary and enabled.
 * @param map The map to use for dimensions and flags.
 * @param y_in Current y coordinate, maybe exceeed the bounds of the map.
 * @param y_out Pointer to y variable to update.
 * @returns 0 if out of bounds and wrapping is disabled, 1 if no wrapping was
 *          applied, 2 if wrapping was applied.
 */
bool wrap_y(MapInfo *map, int y_in, int *y_out);

#endif
