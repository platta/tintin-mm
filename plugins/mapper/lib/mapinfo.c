#include "mapinfo.h"

void initialize_map_info(MapInfo *map) {
    map->width = MAP_DEFAULT_WIDTH;
    map->height = MAP_DEFAULT_HEIGHT;
    map->hwrap = MAP_DEFAULT_HWRAP;
    map->vwrap = MAP_DEFAULT_VWRAP;
}

void read_map_info(MapInfo *map, FILE *fptr) {
    // Capture current pointer position.
    long previous = ftell(fptr);

    // Start at the first byte.
    fseek(fptr, 0, SEEK_SET);

    // Read map width and height.
    fread(&map->width, sizeof(unsigned short), 1, fptr);
    fread(&map->height, sizeof(unsigned short), 1, fptr);

    // Flags are next. Read into an unsigned char and parse.
    unsigned char flags;
    fread(&flags, sizeof(unsigned char), 1, fptr);
    map->hwrap = flags & MAP_FLAG_HWRAP == MAP_FLAG_HWRAP;
    map->vwrap = flags & MAP_FLAG_VWRAP == MAP_FLAG_VWRAP;

    // Put pointer back.
    fseek(fptr, previous, SEEK_SET);
}

void write_map_info(MapInfo *map, FILE *fptr) {
    // Capture current pointer position.
    long previous = ftell(fptr);

    // Start at the first byte.
    fseek(fptr, 0, SEEK_SET);

    // Write map width and height.
    fwrite(&map->width, sizeof(unsigned short), 1, fptr);
    fwrite(&map->height, sizeof(unsigned short), 1, fptr);

    // Construct the flags byte.
    unsigned char output = 0;
    if (map->hwrap) output = output | MAP_FLAG_HWRAP;
    if (map->vwrap) output = output | MAP_FLAG_VWRAP;

    // Write flags.
    fwrite(&output, sizeof(unsigned char), 1, fptr);

    // Put pointer back.
    fseek(fptr, previous, SEEK_SET);
}

void parse_flags_from_args(MapInfo *map, int argc, char *argv[], int start_arg) {
    // Loop through all args.
    int i;
    for (i = start_arg; i < argc; i++) {
        // Make the arg lowercase.
        char *arg = argv[i];
        for (int j = 0; arg[j]; j++) {
            arg[j] = tolower(arg[j]);
        }

        // Match argument against the list of supported flags.
        if (strcmp(arg, "wrap") == 0) {
            map->hwrap = true;
            map->vwrap = true;
        } else if (strcmp(arg, "nowrap") == 0) {
            map->hwrap = false;
            map->vwrap = false;
        } else if (strcmp(arg, "hwrap") == 0) {
            map->hwrap = true;
        } else if (strcmp(arg, "nohwrap") == 0) {
            map->hwrap = false;
        } else if (strcmp(arg, "vwrap") == 0) {
            map->vwrap = true;
        } else if (strcmp(arg, "novwrap") == 0) {
            map->vwrap = false;
        }
    }
}

bool wrap_x(MapInfo *map, int x_in, int *x_out) {
    if (x_in < 0 || x_in >= map->width) {
        if (map->hwrap) {
            *x_out = ((x_in % map->width) + map->width) % map->width;
            return 2;
        } else {
            return 0;
        }
    } else {
        *x_out = x_in;
        return 1;
    }
}

bool wrap_y(MapInfo *map, int y_in, int *y_out) {
    if (y_in < 0 || y_in >= map->height) {
        if (map->vwrap) {
            *y_out = ((y_in % map->height) + map->height) % map->height;
            return 2;
        } else {
            return 0;
        }
    } else {
        *y_out = y_in;
        return 1;
    }
}