#include <math.h>
#include "chunk.h"

#define BASE_SIZE 18
#define BASE_3D_SIZE 6
#define MAGIC_BLOCK 4

char*** replace_block(
    char*** chunk, int width, int height, int depth, int x, int y, int z,
    char block, char target_block
) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int z = 0; z < depth; z++) {
                if (chunk[x][y][z] == target_block) {
                    chunk = chunk_place_block(
                        chunk, width, height, depth, x, y, z, block
                    );
                }
            }
        }
    }

    return chunk;
}

char*** chunk_shell(
    char*** chunk, int width, int height, int depth, char target_block,
    char shell_block
) {
    int flag = 0;
    int base[BASE_SIZE][3] = { { 1, 0, 0 },   { -1, 0, 0 },  { 0, 1, 0 },
                               { 0, -1, 0 },  { 0, 0, 1 },   { 0, 0, -1 },
                               { 1, 1, 0 },   { 1, -1, 0 },  { -1, 1, 0 },
                               { -1, -1, 0 }, { 1, 0, 1 },   { 1, 0, -1 },
                               { -1, 0, 1 },  { -1, 0, -1 }, { 0, 1, 1 },
                               { 0, 1, -1 },  { 0, -1, 1 },  { 0, -1, -1 } };

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int z = 0; z < depth; z++) {
                if (chunk[x][y][z] == target_block) {
                    for (int i = 0; i < BASE_SIZE; i++) {
                        int nx = x + base[i][0];
                        int ny = y + base[i][1];
                        int nz = z + base[i][2];

                        char shell = (target_block == shell_block)
                                         ? MAGIC_BLOCK
                                         : shell_block;
                        flag = (target_block == shell_block) ? 1 : 0;

                        if (nx >= 0 && nx < width && ny >= 0 && ny < height
                            && nz >= 0 && nz < depth) {
                            if (chunk[nx][ny][nz] != target_block) {
                                chunk = chunk_place_block(
                                    chunk, width, height, depth, nx, ny, nz,
                                    shell
                                );
                            }
                        }
                    }
                }
            }
        }
    }

    if (flag) {
        chunk = replace_block(
            chunk, width, height, depth, 0, 0, 0, shell_block, MAGIC_BLOCK
        );
    }

    return chunk;
}

void flood_fill_xz(
    char*** chunk, int width, int height, int depth, int x, int y, int z,
    char block, char target_block
) {
    if (x < 0 || x >= width || z < 0 || z >= depth || y < 0 || y >= height) {
        return;
    }
    if (chunk[x][y][z] != target_block) {
        return;
    }

    chunk[x][y][z] = block;

    int base[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };

    for (int i = 0; i < 4; i++) {
        int nx = x + base[i][0];
        int nz = z + base[i][1];
        flood_fill_xz(
            chunk, width, height, depth, nx, y, nz, block, target_block
        );
    }
}

char*** chunk_fill_xz(
    char*** chunk, int width, int height, int depth, int x, int y, int z,
    char block
) {
    char target_block = chunk[x][y][z];
    flood_fill_xz(chunk, width, height, depth, x, y, z, block, target_block);

    return chunk;
}

void flood_fill(
    char*** chunk, int width, int height, int depth, int x, int y, int z,
    char block, char target_block
) {
    if (x < 0 || x >= width || z < 0 || z >= depth || y < 0 || y >= height) {
        return;
    }
    if (chunk[x][y][z] != target_block) {
        return;
    }

    chunk[x][y][z] = block;

    int base[BASE_3D_SIZE][3] = { { 1, 0, 0 },  { -1, 0, 0 }, { 0, 1, 0 },
                                  { 0, -1, 0 }, { 0, 0, 1 },  { 0, 0, -1 } };

    for (int i = 0; i < BASE_3D_SIZE; i++) {
        int nx = x + base[i][0];
        int ny = y + base[i][1];
        int nz = z + base[i][2];
        flood_fill(
            chunk, width, height, depth, nx, ny, nz, block, target_block
        );
    }
}

char*** chunk_fill(
    char*** chunk, int width, int height, int depth, int x, int y, int z,
    char block
) {
    char target_block = chunk[x][y][z];
    flood_fill(chunk, width, height, depth, x, y, z, block, target_block);

    return chunk;
}
