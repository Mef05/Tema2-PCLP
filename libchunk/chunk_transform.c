#include <stdbool.h>
#include <stdlib.h>
#include "chunk.h"

#define BASE_3D_SIZE 6

void free_chunk(char*** chunk, int width, int height, int depth) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            free(chunk[x][y]);
        }
        free(chunk[x]);
    }
    free(chunk);
}

char*** calloc_chunk(int width, int height, int depth) {
    char*** chunk = (char***) calloc(width, sizeof(char**));
    for (int i = 0; i < width; i++) {
        chunk[i] = (char**) calloc(height, sizeof(char*));
        for (int j = 0; j < height; j++) {
            chunk[i][j] = (char*) calloc(depth, sizeof(char));
        }
    }

    return chunk;
}

char*** chunk_rotate_y(char*** chunk, int width, int height, int depth) {
    char*** new_chunk = calloc_chunk(depth, height, width);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int z = 0; z < depth; z++) {
                new_chunk[depth - 1 - z][y][x] = chunk[x][y][z];
            }
        }
    }

    free_chunk(chunk, width, height, depth);

    return new_chunk;
}

// Structure to track body blocks
typedef struct {
    int* x;
    int* y;
    int* z;
    int size;
    int capacity;
} Body;

// Initialize body structure
Body* create_body(int initial_capacity) {
    Body* body = malloc(sizeof(Body));
    body->x = malloc(initial_capacity * sizeof(int));
    body->y = malloc(initial_capacity * sizeof(int));
    body->z = malloc(initial_capacity * sizeof(int));
    body->size = 0;
    body->capacity = initial_capacity;
    return body;
}

static bool
    is_within_bounds(int x, int y, int z, int width, int height, int depth) {
    return (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth);
}

// Add block to body
void add_to_body(Body* body, int x, int y, int z) {
    if (body->size >= body->capacity) {
        body->capacity *= 2;
        body->x = realloc(body->x, body->capacity * sizeof(int));
        body->y = realloc(body->y, body->capacity * sizeof(int));
        body->z = realloc(body->z, body->capacity * sizeof(int));
    }
    body->x[body->size] = x;
    body->y[body->size] = y;
    body->z[body->size] = z;
    body->size++;
}

// Find connected blocks using flood fill
void find_connected_blocks(
    char*** chunk, int width, int height, int depth, int x, int y, int z,
    char block_type, bool*** visited, Body* body
) {
    if (!is_within_bounds(x, y, z, width, height, depth) || visited[x][y][z]
        || chunk[x][y][z] != block_type) {
        return;
    }

    visited[x][y][z] = true;
    add_to_body(body, x, y, z);

    // Check all 6 adjacent blocks
    int dirs[6][3] = { { 1, 0, 0 },  { -1, 0, 0 }, { 0, 1, 0 },
                       { 0, -1, 0 }, { 0, 0, 1 },  { 0, 0, -1 } };
    for (int i = 0; i < 6; i++) {
        find_connected_blocks(
            chunk, width, height, depth, x + dirs[i][0], y + dirs[i][1],
            z + dirs[i][2], block_type, visited, body
        );
    }
}

void move_body(
    char*** chunk, int width, int height, int depth, int x, int y, int z
) {
    if (!is_within_bounds(x, y, z, width, height, depth)
        || chunk[x][y][z] == BLOCK_AIR) {
        return;
    }

    // Initialize visited array
    bool*** visited = malloc(width * sizeof(bool**));
    for (int i = 0; i < width; i++) {
        visited[i] = malloc(height * sizeof(bool*));
        for (int j = 0; j < height; j++) {
            visited[i][j] = calloc(depth, sizeof(bool));
        }
    }

    // Find all blocks in this body
    Body* body = create_body(16);
    find_connected_blocks(
        chunk, width, height, depth, x, y, z, chunk[x][y][z], visited, body
    );

    // Move body down until it hits something
    bool can_move = true;
    while (can_move) {
        // Check if body can move down
        for (int i = 0; i < body->size; i++) {
            int bx = body->x[i];
            int by = body->y[i];
            int bz = body->z[i];
            if (by == 0
                || (chunk[bx][by - 1][bz] != BLOCK_AIR
                    && !visited[bx][by - 1][bz])) {
                can_move = false;
                break;
            }
        }

        // Move body down one step
        if (can_move) {
            for (int i = 0; i < body->size; i++) {
                int bx = body->x[i];
                int by = body->y[i];
                int bz = body->z[i];
                chunk[bx][by - 1][bz] = chunk[bx][by][bz];
                chunk[bx][by][bz] = BLOCK_AIR;
                body->y[i]--;
            }
        }
    }

    // Cleanup
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            free(visited[i][j]);
        }
        free(visited[i]);
    }
    free(visited);
    free(body->x);
    free(body->y);
    free(body->z);
    free(body);
}

char*** chunk_apply_gravity(
    char*** chunk, int width, int height, int depth, int* new_height
) {
    move_body(chunk, width, height, depth, 0, height, 0);

    // char*** new_chunk = calloc_chunk(width, height, depth);

    // for (int x = 0; x < width; x++) {
    //     for (int z = 0; z < depth; z++) {
    //         int empty_y = 0;
    //         for (int y = 0; y < height; y++) {
    //             if (chunk[x][y][z] != BLOCK_AIR) {
    //                 if (empty_y != y) {
    //                     new_chunk[x][empty_y][z] = chunk[x][y][z];
    //                 }
    //                 empty_y++;
    //             }
    //         }
    //     }
    // }

    // *new_height = 0;
    // for (int y = height - 1; y >= 0; y--) {
    //     int is_empty = 1;
    //     for (int x = 0; x < width && is_empty; x++) {
    //         for (int z = 0; z < depth && is_empty; z++) {
    //             if (new_chunk[x][y][z] != 0) {
    //                 is_empty = 0;
    //             }
    //         }
    //     }
    //     if (!is_empty) {
    //         *new_height = y + 1;
    //         break;
    //     }
    // }

    // char*** final_chunk = calloc_chunk(width, *new_height, depth);
    // for (int x = 0; x < width; x++) {
    //     for (int y = 0; y < *new_height; y++) {
    //         for (int z = 0; z < depth; z++) {
    //             final_chunk[x][y][z] = new_chunk[x][y][z];
    //         }
    //     }
    // }

    // free_chunk(new_chunk, width, height, depth);
    // free_chunk(chunk, width, height, depth);

    return final_chunk;
}
