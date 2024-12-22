#include <stdio.h>
#include <stdlib.h>
#include "../libchunk/chunk.h"
// #include "../libchunk/chunk_compress.c"
#include "../libchunk/chunk_gen.c"
#include "../libchunk/chunk_process.c"
#include "../libchunk/chunk_transform.c"

void print_chunk(char*** chunk, int width, int height, int depth) {
    printf("%d\n%d\n%d\n", width, height, depth);

    for (int x = 0; x < width; x++) {
        for (int y = height - 1; y >= 0; y--) {
            for (int z = 0; z < depth; z++)
                printf("%d ", chunk[x][y][z]);
            printf("\n");
        }
        printf("\n");
    }
}

char*** read_chunk(int* width, int* height, int* depth) {
    scanf("%d%d%d", width, height, depth);

    char*** chunk = malloc(*width * sizeof *chunk);

    for (int x = 0; x < *width; x++) {
        chunk[x] = malloc(*height * sizeof **chunk);
        for (int y = *height - 1; y >= 0; y--) {
            chunk[x][y] = malloc(*depth * sizeof ***chunk);

            for (int z = 0; z < *depth; z++)
                scanf("%hhd", &chunk[x][y][z]);
        }
    }

    return chunk;
}

int main(void) {
    int width = 5, height = 5, depth = 6;
    char*** chunk = read_chunk(&width, &height, &depth);

    chunk = chunk_apply_gravity(chunk, width, height, depth, &height);

    print_chunk(chunk, width, height, depth);

    return 0;
}
