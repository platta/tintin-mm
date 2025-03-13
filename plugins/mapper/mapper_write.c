#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/mapinfo.h"
#include "lib/roominfo.h"

// const char *COLORS[] = {
// 0       //     "\x1b[0m",
//     1   //     "\x1b[38;5;10m",
//     2   //     "\x1b[38;5;11m",
//     3   //     "\x1b[38;5;12m",
//     4   //     "\x1b[38;5;13m",
//     5   //     "\x1b[38;5;14m",
//     6   //     "\x1b[38;5;15m",
//     7   //     "\x1b[38;5;1m",
//     8   //     "\x1b[38;5;2m",
//     9   //     "\x1b[38;5;3m",
//     10  //     "\x1b[38;5;4m",
//     11  //     "\x1b[38;5;5m",
//     12  //     "\x1b[38;5;6m",
//     13  //     "\x1b[38;5;7m",
//     14  //     "\x1b[38;5;8m",
//     15  //     "\x1b[38;5;9m",
//     16  //     "\x1b[38;5;31m",
//     17  //     "\x1b[38;5;19m",
//     18  //     "\x1b[38;5;163m",
//     19  //     "\x1b[38;5;21m",
//     20  //     "\x1b[38;5;185m",
//     21  //     "\x1b[38;5;124m",
//     22  //     "\x1b[38;5;77m",
//     //     "\x1b[38;5;132m",
//     "\x1b[38;5;117m",
//     "\x1b[38;5;71m",
// };

// const int COLOR_COUNT = sizeof(COLORS) / sizeof(COLORS[0]);

// const unsigned int ROOM_SIZE = sizeof(char) + sizeof(unsigned char);

unsigned char getColorIndex(char *colorCode) {
    char *ptr = strrchr(colorCode, ';');

    if (ptr) {
        return (char)strtoul(++ptr, NULL, 10);
    }

    // Not found, just return 0.
    return 0;
}

int main(int argc, char *argv[]) {
    // Read the file name as the first arg.
    const char *file = argv[1];

    // Open the file in read/write mode.
    FILE *fptr;
    fptr = fopen(file, "r+b");

    MapInfo *map = malloc(sizeof(MapInfo));
    read_map_info(map, fptr);

    // Capture command line args.
    RoomInfo *room = malloc(sizeof(RoomInfo));
    int rawX, rawY;
    int wrappedX, wrappedY;
    bool seek;
    char *line;

    char currentSymbol;

    // Keep track of the color we've been writing so we can capture it for each
    // room of that color even though the input won't duplicate the color code
    // for every room.
    unsigned char currentColorIndex;

    // Track whether or not we're in an escape sequence. If we see the escape
    // character, we're in an escape sequence until we then see "m".
    bool escaped;

    char *currentColorCode = malloc(20 * sizeof(char));
    int currentColorLength;

    // Loop through the remaining arguments. We expect them to be in triplets of
    // x coordinate, y coordinate, string of map data.
    for (int i = 2; i < argc; i += 3) {
        // Read from command line args.
        rawX = strtol(argv[i], NULL, 10);
        rawY = strtol(argv[i + 1], NULL, 10);

        // If line is out of bounds.
        if (!wrap_y(map, rawY, &wrappedY)) {
            continue;
        }

        line = argv[i + 2];

        // Reset tracking variables.
        seek = true;
        escaped = false;
        currentColorIndex = 0;

        for (int j = 0; j < strlen(line); j++) {
            currentSymbol = line[j];

            if (escaped) {
                // Capture escape code data.
                // Capture new character.
                currentColorCode[currentColorLength] = currentSymbol;
                // Ensure we're still null terminated.
                currentColorCode[++currentColorLength] = '\0';

                // The character "m" terminates the escape sequence.
                if (currentSymbol == 'm') {
                    escaped = false;
                    currentColorIndex = getColorIndex(currentColorCode);
                }
            } else {
                if (currentSymbol == '\x1b') {
                    // Beginning an escape sequence. Capture it.
                    escaped = true;
                    currentColorLength = 1;
                    currentColorCode[0] = currentSymbol;
                    currentColorCode[1] = '\0';
                } else {
                    if (!wrap_x(map, rawX++, &wrappedX)) {
                        continue;
                    }

                    if (seek) {
                        seek_room(map, wrappedX, wrappedY, fptr);
                        seek = false;
                    }

                    if (map->hwrap && wrappedX == map->width - 1) {
                        seek = true;
                    }

                    if (
                        line[j] == '@' && currentColorIndex == 13 || line[j] == '4' && currentColorIndex == 13 || line[j] == '4' && currentColorIndex == 9 || line[j] == '4' && currentColorIndex == 15 || line[j] == 'Q' && currentColorIndex == 15 || line[j] == '4' && currentColorIndex == 10 || line[j] == '@' && currentColorIndex == 11) {
                        // Skip things we don't actually want to record, like
                        // player markers, boats, or hot air balloons. In these
                        // cases, just skip to the next spot in the file.
                        advance_one_room(fptr);
                        continue;
                    }

                    // Write the room out to the file.
                    room->symbol = line[j];
                    room->color = currentColorIndex;
                    write_room(room, fptr);
                }
            }
        }
        // // Read from command line args.
        // rawX = strtol(argv[i], NULL, 10);
        // rawY = strtol(argv[i + 1], NULL, 10);
        // line = argv[i + 2];

        // if (rawY < 0 || rawY >= map->height) {
        //     // Skip rows that aren't actually in range on the map file.
        //     continue;
        // }
        // // Seek the write position we need. This is the two unsigned shorts we
        // // read at the beginning of the file + the number of rooms we need to
        // // skip past. That number is y * mapWidth + x rooms.
        // seek_room(map, rawX, rawY, fptr);
        // // fseek(fptr, 64 * sizeof(unsigned short) + (rawY * mapWidth + x) * ROOM_SIZE, SEEK_SET);

        // // Reset tracking variables.
        // escaped = false;
        // currentColorIndex = 0;

        // // Loop over line of input.
        // for (int j = 0; j < strlen(line); j++) {
        //     currentSymbol = line[j];

        //     if (escaped) {
        //         // Capture escape code data.
        //         // Capture new character.
        //         currentColorCode[currentColorLength] = currentSymbol;
        //         // Ensure we're still null terminated.
        //         currentColorCode[++currentColorLength] = '\0';

        //         // The character "m" terminates the escape sequence.
        //         if (currentSymbol == 'm') {
        //             escaped = false;
        //             currentColorIndex = getColorIndex(currentColorCode);
        //         }
        //     } else {
        //         if (currentSymbol == '\x1b') {
        //             // Beginning an escape sequence. Capture it.
        //             escaped = true;
        //             currentColorLength = 1;
        //             currentColorCode[0] = currentSymbol;
        //             currentColorCode[1] = '\0';
        //         } else {
        //             if (
        //                 line[j] == '@' && currentColorIndex == 13 || line[j] == '4' && currentColorIndex == 13 || line[j] == '4' && currentColorIndex == 9 || line[j] == '4' && currentColorIndex == 15 || line[j] == 'Q' && currentColorIndex == 15 || line[j] == '4' && currentColorIndex == 10 || line[j] == '@' && currentColorIndex == 11) {
        //                 // Skip things we don't actually want to record, like
        //                 // player markers, boats, or hot air balloons. In these
        //                 // cases, just skip to the next spot in the file.
        //                 fseek(fptr, ROOM_SIZE, SEEK_CUR);
        //                 continue;
        //             }

        //             // Write the room out to the file.
        //             if (rawX >= 0 || rawX < map->width) {
        //                 fwrite(&line[j], sizeof(char), 1, fptr);
        //                 fwrite(&currentColorIndex, sizeof(unsigned char), 1, fptr);
        //             }

        //             rawX++;
        //         }
        //     }
        // }
    }

    // Close the file
    fclose(fptr);

    return 0;
}
