#include <math.h>
#include "chunk.h"

char*** chunk_place_block(
    char*** chunk, int width, int height, int depth, int x, int y, int z,
    char block
) {
    if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
        return chunk;
    }

    chunk[x][y][z] = block;
    return chunk;
}

char*** chunk_fill_cuboid(
    char*** chunk, int width, int height, int depth, int x0, int y0, int z0,
    int x1, int y1, int z1, char block
) {
    if (x0 > x1) {
        int temp = x0;
        x0 = x1;
        x1 = temp;
    }
    if (y0 > y1) {
        int temp = y0;
        y0 = y1;
        y1 = temp;
    }
    if (z0 > z1) {
        int temp = z0;
        z0 = z1;
        z1 = temp;
    }

    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            for (int z = z0; z <= z1; z++) {
                chunk = chunk_place_block(
                    chunk, width, height, depth, x, y, z, block
                );
            }
        }
    }

    return chunk;
}

double block_dist(int x1, int y1, int z1, int x2, int y2, int z2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
}

char*** chunk_fill_sphere(
    char*** chunk, int width, int height, int depth, int x, int y, int z,
    double radius, char block
) {
    for (int x1 = 0; x1 < width; x1++) {
        for (int y1 = 0; y1 < height; y1++) {
            for (int z1 = 0; z1 < depth; z1++) {
                if (block_dist(x, y, z, x1, y1, z1) <= radius) {
                    chunk = chunk_place_block(
                        chunk, width, height, depth, x1, y1, z1, block
                    );
                }
            }
        }
    }

    return chunk;
}

unsigned char*
    chunk_encode(char*** chunk, int width, int height, int depth, int* length) {
    unsigned char* code = malloc(width * height * depth * sizeof *code);
    return code;
}

char*** chunk_decode(unsigned char* code, int width, int height, int depth) {
    char*** chunk = malloc(width * sizeof *chunk);
    return chunk;
}