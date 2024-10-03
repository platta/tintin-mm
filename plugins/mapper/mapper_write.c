#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
};

const int COLOR_COUNT = sizeof(COLORS) / sizeof(COLORS[0]);

const unsigned int ROOM_SIZE = sizeof(char) + sizeof(unsigned char);

unsigned char getColorIndex(char *colorCode) {
    for (int i = 0; i < COLOR_COUNT; i++) {
        if (strcmp(COLORS[i], colorCode) == 0) {
            return i;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    // const unsigned int shortSize = sizeof(short);
    // const unsigned short symbolSize = sizeof(char);
    // const unsigned short colorSize = sizeof(unsigned char);
    // const unsigned short roomSize = symbolSize + colorSize;

    // Assume the args are in the right positions.
    const char *file = argv[1];

    // Open the file in read/write mode.
    FILE *fptr;
    fptr = fopen(file, "r+b");

    // Read the map size first.
    unsigned short mapSize;
    fread(&mapSize, sizeof(unsigned short), 1, fptr);

    // The number of bits required to represent the x or y coordinate, and we
    // need that in order to create the packed address.
    // int bitCount = ceil(log2(mapSize));

    // printf("Max Size: %i\n", mapSize);
    // // printf("Bit Count: %i\n", bitCount);
    // printf("Room Size: %i\n", ROOM_SIZE);

    int x;
    int y;
    char *line;
    char currentSymbol;
    unsigned char currentColorIndex;
    bool escaped;

    char *currentColorCode = malloc(20 * sizeof(char));
    int currentColorLength;

    for (int i = 2; i < argc; i += 3) {
        // printf("%s\n", argv[i]);
        // printf("%s\n", argv[i + 1]);
        x = strtol(argv[i], NULL, 10) - 1;
        y = strtol(argv[i + 1], NULL, 10) - 1;
        line = argv[i + 2];

        // printf("%i %i: %s\n", x, y, line);

        fseek(fptr, (y * mapSize + x) * ROOM_SIZE + sizeof(unsigned short), SEEK_SET);
        // fseek(fptr, ((y << bitCount) | x) * roomSize + shortSize, SEEK_SET);

        escaped = false;
        currentColorIndex = 0;
        for (int j = 0; j < strlen(line); j++) {
            currentSymbol = line[j];
            // printf("%c", currentSymbol);
            if (escaped) {
                currentColorCode[currentColorLength] = currentSymbol;
                currentColorCode[++currentColorLength] = '\0';
                if (currentSymbol == 'm') {
                    escaped = false;
                    // printf("%sCURRENT COLOR!\x1b[0m\n", currentColorCode);
                    currentColorIndex = getColorIndex(currentColorCode);
                }
            } else {
                if (currentSymbol == '\x1b') {
                    escaped = true;
                    currentColorLength = 1;
                    currentColorCode[0] = currentSymbol;
                    currentColorCode[1] = '\0';
                } else {
                    if (
                        line[j] == '@' && currentColorIndex == 4 || line[j] == '4' && currentColorIndex == 4 || line[j] == '4' && currentColorIndex == 15 || line[j] == '4' && currentColorIndex == 6 || line[j] == 'Q' && currentColorIndex == 6 || line[j] == '4' && currentColorIndex == 1 || line[j] == '@' && currentColorIndex == 2) {
                        // Skip things we don't actually want to record.
                        fseek(fptr, ROOM_SIZE, SEEK_CUR);
                        continue;
                    }
                    fwrite(&line[j], sizeof(char), 1, fptr);
                    fwrite(&currentColorIndex, sizeof(unsigned char), 1, fptr);
                }
            }
        }
    }

    // Close the file
    fclose(fptr);

    return 0;

    // const int x = strtol(argv[2], NULL, 10) - 1;
    // const int y = strtol(argv[3], NULL, 10) - 1;
    // const char symbol = *argv[4];
    // const unsigned char color = strtol(argv[5], NULL, 10);

    // // Open the file in read/write mode.
    // FILE *fptr;
    // fptr = fopen(file, "r+b");

    // // Read the map size first.
    // unsigned short mapSize;
    // fread(&mapSize, sizeof(unsigned short), 1, fptr);

    // // The number of bits required to represent the x or y coordinate, and we
    // // need that in order to create the packed address.
    // // unsigned short bitCount = ceil(log2(mapSize));

    // fseek(fptr, (y * mapSize + x) * ROOM_SIZE + sizeof(unsigned short), SEEK_SET);
    // // fseek(fptr, ((y << bitCount) | x) * roomSize + shortSize, SEEK_SET);
    // fwrite(&symbol, sizeof(char), 1, fptr);
    // fwrite(&color, sizeof(unsigned char), 1, fptr);
}
