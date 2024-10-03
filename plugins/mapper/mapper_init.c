#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Assume the args are in the right positions.
    char *fileName = argv[1];
    unsigned short maxSize = strtoul(argv[2], NULL, 10);

    // Open the file (overwrite) and write the map size.
    FILE *handle;
    handle = fopen(fileName, "wb");
    fwrite(&maxSize, sizeof(maxSize), 1, handle);
    fclose(handle);

    // That's it!
    return 0;
}
