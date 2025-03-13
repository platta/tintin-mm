#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/mapinfo.h"
#include "lib/roominfo.h"

int main(int argc, char *argv[]) {
    // Read arguments.
    // File containing the map.
    const char *file = argv[1];

    // Player's current x and y position.
    const int x = strtol(argv[2], NULL, 10);
    const int y = strtol(argv[3], NULL, 10);

    // Width and height of the map to print (player is always assumed to be in
    // the middle of the map that is printed).
    const int width = strtol(argv[4], NULL, 10);
    const int height = strtol(argv[5], NULL, 10);

    // Calculate the map coordinates of the area we will print. Using floor to
    // deal with widths/heights that don't divide evenly.
    const int top = y - floor((height - 1) / 2.0);
    const int bottom = top + height;
    const int left = x - floor((width - 1) / 2.0);
    const int right = left + width;

    // Open the map file in read mode
    FILE *fptr;
    fptr = fopen(file, "rb");

    MapInfo *map = malloc(sizeof(MapInfo));
    read_map_info(map, fptr);

    // This stores the color of the previous character we printed, so we won't
    // print color codes for every single character, only when the color changes
    // or we start a new line.
    unsigned char previousColor;

    bool seek;
    int rawX, rawY;
    int wrappedY, wrappedX;
    RoomInfo *room = malloc(sizeof(RoomInfo));
    for (rawY = top; rawY < bottom; rawY++) {
        if (!wrap_y(map, rawY, &wrappedY)) {
            printf("\n");
            continue;
        }

        previousColor = 0;
        seek = true;

        for (rawX = left; rawX < right; rawX++) {
            if (!wrap_x(map, rawX, &wrappedX)) {
                if (rawX < 0) {
                    printf(" ");
                    continue;
                } else {
                    break;
                }
            }

            if (seek) {
                // printf("s");
                if (!seek_room(map, wrappedX, wrappedY, fptr)) {
                    break;
                }
                seek = false;
            }

            if (map->hwrap && wrappedX == map->width - 1) {
                seek = true;
            }

            if (!read_room(room, fptr)) {
                room->symbol = ' ';
            }

            if (wrappedX == x && wrappedY == y) {
                room->symbol = '@';
                room->color = 13;
            }
            // printf("-%d_%d_%d-", wrappedX, wrappedY, ftell(fptr));
            if (previousColor != room->color) {
                if (room->color == 0) {
                    printf("\x1b[0m");
                } else {
                    printf("\x1b[38;5;%dm", room->color);
                }
                previousColor = room->color;
            }
            // printf("Room (%2d, %2d): %d\n", wrappedX, wrappedY, ftell(fptr));
            printf("%c", room->symbol);

            // printf("(%2d, %2d) ", wrappedX, wrappedY);
        }

        printf("\x1b[0m\n");
    }

    // // Iterator variables. Start at the top of the bounding box.
    // int currentY = top;
    // int currentX;

    // // Deal with being close to the top of the map where we need to represent
    // // "negative" Y coordinates. Get us to the first real row of map data.
    // while (currentY < 0 && currentY++ < bottom) {
    //     printf("\x1b[0m\n");
    // }

    // // Loop over rows.
    // for (currentY; currentY < bottom; currentY++) {
    //     if (currentY >= mapHeight) {
    //         // Deal with being close to the bottom of the map where we need to
    //         // represent Y coordinates past the maximum Y. Print blank rows
    //         // until we're done.
    //         while (currentY >= mapHeight && currentY++ < bottom) {
    //             printf("\x1b[0m\n");
    //         }
    //         break;
    //     }

    //     // Start at the left X coordinate.
    //     currentX = left;
    //     while (currentX < 0 && currentX++ < right) {
    //         // Deal with being close to the left of the map where we need to
    //         // represent "negative" X coordinates. Get us to the first real X
    //         // coordinate in map data.
    //         printf(" ");
    //     }

    //     previousColor = 0;

    //     // Go to the first item we want for this row. We seek based on the size
    //     // Of the map dimensions at the beginning of the file plus how many
    //     // rooms into the map we are. Which is y * mapWidth + x rooms.
    //     if (fseek(fptr, 64 * sizeof(unsigned short) + (currentY * mapWidth + currentX) * ROOM_SIZE, SEEK_SET) != 0) {
    //         // Print a blank row if we can't seek here in the file.
    //         printf("\x1b[0m\n");
    //         continue;
    //     }

    //     // Loop over x coordinates for this row.
    //     for (currentX; currentX < right; currentX++) {
    //         if (currentX >= mapWidth) {
    //             // Stop if we hit the end of the map.
    //             break;
    //         }
    //         if (currentY == y && currentX == x) {
    //             // This is the player's location, print @ instead of the
    //             // terrain character.
    //             symbol = '@';
    //             color = 13;

    //             // Manually advance the file pointer since we're not reading the
    //             // data at this coordinate.
    //             fseek(fptr, ROOM_SIZE, SEEK_CUR);
    //         } else if (fread(&symbol, sizeof(char), 1, fptr) == 0 || fread(&color, sizeof(unsigned char), 1, fptr) == 0 || !symbol) {
    //             // Handle no data found. Print a blank space and keep the
    //             // current color.
    //             symbol = ' ';
    //             color = previousColor;
    //         }

    //         if (color != previousColor) {
    //             // If the color changed, output the escape code for the new one.
    //             previousColor = color;
    //             if (previousColor > 0) {
    //                 printf("\x1b[38;5;%dm", previousColor);
    //             } else {
    //                 printf("\x1b[38;5;%dm", previousColor);
    //             }
    //         }
    //         // Print the symbol.
    //         printf("%c", symbol);
    //     }

    //     // When we're all done, reset colors and end this line.
    //     printf("\x1b[0m\n");
    // }

    // Close the file
    fclose(fptr);

    return 0;
}
