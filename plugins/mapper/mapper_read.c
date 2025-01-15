#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const char *COLORS[] = {
    "\x1b[0m",
    "\x1b[38;5;10m",
    "\x1b[38;5;11m",
    "\x1b[38;5;12m",
    "\x1b[38;5;13m",
    "\x1b[38;5;14m",
    "\x1b[38;5;15m",
    "\x1b[38;5;1m",
    "\x1b[38;5;2m",
    "\x1b[38;5;3m",
    "\x1b[38;5;4m",
    "\x1b[38;5;5m",
    "\x1b[38;5;6m",
    "\x1b[38;5;7m",
    "\x1b[38;5;8m",
    "\x1b[38;5;9m",
    "\x1b[38;5;31m",
    "\x1b[38;5;19m",
    "\x1b[38;5;163m",
    "\x1b[38;5;21m",
    "\x1b[38;5;185m",
    "\x1b[38;5;124m",
    "\x1b[38;5;77m",
    "\x1b[38;5;132m",
    "\x1b[38;5;117m",
    "\x1b[38;5;71m",
};

const unsigned int ROOM_SIZE = sizeof(char) + sizeof(unsigned char);

int main(int argc, char *argv[]) {
    // const int shortSize = sizeof(short);
    // const short symbolSize = sizeof(char);
    // const short colorSize = sizeof(short);
    // const short roomSize = symbolSize + colorSize;

    // Assume the args are in the right positions.
    const char *file = argv[1];
    const int x = strtol(argv[2], NULL, 10) - 1;
    const int y = strtol(argv[3], NULL, 10) - 1;
    const int width = strtol(argv[4], NULL, 10);
    const int height = strtol(argv[5], NULL, 10);

    // Calculate bounding box.
    const int top = y - floor((height - 1) / 2.0);
    const int bottom = top + height;
    const int left = x - floor((width - 1) / 2.0);
    const int right = left + width;

    // printf("\x1b[38;5;1mCOLOR TEST\x1b[0m\n");
    // char *foo = "\x1b[38;5;1m";
    // printf("%lu\n", sizeof(short));

    // char *bar = "\x1b[38;5;1m";
    // printf("%s\n", bar);
    // printf("%lu\n", strlen(bar));

    // union
    // {
    //     int i;
    //     char c[sizeof(int)];
    // } q;
    // q.i = 1;
    // if (q.c[0] == 1)
    //     printf("little-endian\n");
    // else
    //     printf("big-endian\n");

    FILE *fptr;

    // Open a file in read mode
    fptr = fopen(file, "rb");

    // Read the first 4 bytes to see what the max dimension of the map is.
    unsigned short mapSize;
    fread(&mapSize, sizeof(unsigned short), 1, fptr);

    // for (int i = 0; i < 10; i++)
    // {
    //     fread(&mapSize, 3, 1, fptr);
    //     printf("%u", mapSize);
    // }
    const int rowSize = mapSize * ROOM_SIZE;

    // The number of bits required to represent the x or y coordinate, and we
    // need that in order to create the packed address.
    // short bitCount = ceil(log2(mapSize));

    // printf("Max Size: %i\n", mapSize);
    // printf("Bit Count: %i\n", bitCount);
    // printf("Room Size: %i\n", ROOM_SIZE);
    // for (int i = 0; i < mapSize; i++)
    // {
    //     for (int j = 0; j < mapSize; j++)
    //     {
    //         // printf("%i %i: (%i * %i) + (%i * %i) + %u = %i\n", i, j, i,
    //         rowSize, j, roomSize, shortSize, i * rowSize + j * roomSize +
    //         shortSize); printf("%i %i: (%d + %i) * %i + %u = %u\n", i, j, i <<
    //         bitCount, j, roomSize, shortSize, ((i << bitCount) | j) * roomSize
    //         + shortSize);
    //     }
    // }

    // return 0;

    // For calculating the address of stuff.
    char symbol;
    unsigned char color;
    unsigned char currentColor;

    int currentY = top;
    int currentX;
    while (currentY < 0 && currentY++ < bottom) {
        printf("\x1b[0m\n");
    }

    for (currentY; currentY < bottom; currentY++) {
        while (currentY >= mapSize && currentY++ < bottom) {
            printf("\x1b[0m\n");
        }
        currentX = left;
        while (currentX < 0 && currentX++ < right) {
            printf(" ");
        }

        currentColor = 0;

        // Go to the first item we want for this row.
        if (fseek(fptr, (currentY * mapSize + currentX) * ROOM_SIZE + sizeof(unsigned short), SEEK_SET) != 0)
        // if (fseek(fptr, ((currentY << bitCount) | left) * roomSize + shortSize,
        // SEEK_SET) != 0)
        {
            printf("\x1b[0m\n");
            continue;
        }

        // printf("ROW START %u, %u: %lu\n", currentY, currentX, ftell(fptr));

        for (currentX; currentX < right; currentX++) {
            if (currentX >= mapSize) {
                break;
            }
            if (currentY == y && currentX == x) {
                symbol = '@';
                color = 4;

                fseek(fptr, ROOM_SIZE, SEEK_CUR);
            } else if (fread(&symbol, sizeof(char), 1, fptr) == 0 || fread(&color, sizeof(unsigned char), 1, fptr) == 0 || !symbol) {
                symbol = ' ';
                color = currentColor;
            }

            if (color != currentColor) {
                currentColor = color;
                printf("%s", COLORS[currentColor]);
            }
            printf("%c", symbol);
        }

        printf("\x1b[0m\n");
    }

    // // Store the content of the file
    // char myString[100];

    // // Read the content and store it inside myString
    // fgets(myString, 100, fptr);

    // // Print the file content
    // printf("%s", myString);

    // Close the file
    fclose(fptr);

    return 0;
}
