#include <math.h>
#include <stdlib.h>

#include "lib/mapinfo.h"

int main(int argc, char *argv[]) {
    // Assume the args are in the right positions. First three args are
    // required.

    // File name.
    char *fileName = argv[1];

    MapInfo *map = malloc(sizeof(MapInfo));
    initialize_map_info(map);

    // Get width and height.
    map->width = strtoul(argv[2], NULL, 10);
    map->height = strtoul(argv[3], NULL, 10);

    // Parse other flags.
    parse_flags_from_args(map, argc, argv, 4);

    printf("Creating map %s\nWidth: %d\nHeight: %d\n", fileName, map->width, map->height);

    // Open the file (overwrite) and write the map info.
    FILE *fptr;
    fptr = fopen(fileName, "wb");
    write_map_info(map, fptr);

    // That's it!
    fclose(fptr);
    return 0;
}
