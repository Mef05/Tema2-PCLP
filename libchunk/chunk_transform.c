#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "chunk.h"

#define OBJ_INIT_SIZE 16
#define BASE_3D_SIZE 6
#define INIT_CAPACITY 10

int is_in_bounds(int width, int height, int depth, int x, int y, int z) {
    return x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth;
}

void free_chunk(char*** chunk, int width, int height) {
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
    if (chunk == NULL) {
        printf("Memory allocation chunk failed\n");
        return NULL;
    }
    for (int i = 0; i < width; i++) {
        chunk[i] = (char**) calloc(height, sizeof(char*));
        if (chunk[i] == NULL) {
            printf("Memory allocation chunk[%d] failed\n", i);
            return NULL;
        }
        for (int j = 0; j < height; j++) {
            chunk[i][j] = (char*) calloc(depth, sizeof(char));
            if (chunk[i][j] == NULL) {
                printf("Memory allocation chunk[%d][%d] failed\n", i, j);
                return NULL;
            }
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

    free_chunk(chunk, width, height);

    return new_chunk;
}

typedef struct {
    int x;
    int y;
    int z;
} Point3D;

typedef struct {
    Point3D* points;
    int size;
    int capacity;
    bool fallable;
} Body;

Body* create_body() {
    Body* body = calloc(1, sizeof(Body));
    if (body == NULL) {
        printf("Memory allocation body failed\n");
        return NULL;
    }

    body->points = calloc(OBJ_INIT_SIZE, sizeof(Point3D));
    if (body->points == NULL) {
        printf("Memory allocation body->points failed\n");
        free(body->points);
        return NULL;
    }

    for (int i = 0; i < OBJ_INIT_SIZE; i++) {
        body->points[i] = (Point3D){ 0, 0, 0 };
    }

    body->size = 0;
    body->capacity = OBJ_INIT_SIZE;
    body->fallable = false;

    return body;
}

void free_body(Body* body) {
    free(body->points);
}

void free_body_array(Body* bodies, int body_count) {
    for (int i = 0; i < body_count; i++) {
        free_body(&bodies[i]);
    }
    free(bodies);
}

void add_toBody(Body* body, int x, int y, int z) {
    if (body->size == body->capacity) {
        body->capacity *= 2;
        Point3D* new_points =
            realloc(body->points, body->capacity * sizeof(Point3D));
        if (new_points == NULL) {
            printf("Memory reallocation body->points failed\n");
            return;
        }
        body->points = new_points;
    }
    body->points[body->size++] = (Point3D){ x, y, z };
}

void flood_fill_body(
    char*** chunk, int width, int height, int depth, int x, int y, int z,
    char block_type, int*** visited, Body* body
) {
    if (!is_in_bounds(width, height, depth, x, y, z) || visited[x][y][z]
        || chunk[x][y][z] != block_type) {
        return;
    }

    visited[x][y][z] = 1;
    add_toBody(body, x, y, z);

    int base[BASE_3D_SIZE][3] = { { 1, 0, 0 },  { -1, 0, 0 }, { 0, 1, 0 },
                                  { 0, -1, 0 }, { 0, 0, 1 },  { 0, 0, -1 } };

    for (int i = 0; i < BASE_3D_SIZE; i++) {
        flood_fill_body(
            chunk, width, height, depth, x + base[i][0], y + base[i][1],
            z + base[i][2], block_type, visited, body
        );
    }
}

Body* find_bodies(
    char*** chunk, int width, int height, int depth, int* body_count
) {
    int*** visited = malloc(width * sizeof(int**));
    if (visited == NULL) {
        printf("Memory allocation visited failed\n");
        return NULL;
    }
    for (int x = 0; x < width; x++) {
        visited[x] = malloc(height * sizeof(int*));
        if (visited[x] == NULL) {
            printf("Memory allocation visited[%d] failed\n", x);
            for (int i = 0; i < x; i++)
                free(visited[i]);
            free(visited);
            return NULL;
        }
        for (int y = 0; y < height; y++) {
            visited[x][y] = calloc(depth, sizeof(int));
            if (visited[x][y] == NULL) {
                printf("Memory allocation visited[%d][%d] failed\n", x, y);
                for (int i = 0; i < y; i++)
                    free(visited[x][i]);
                free(visited[x]);
                for (int i = 0; i < x; i++)
                    free(visited[i]);
                free(visited);
                return NULL;
            }
        }
    }

    *body_count = 0;
    int capacity = INIT_CAPACITY;
    Body* bodies = calloc(capacity, sizeof(Body));

    if (bodies == NULL) {
        printf("Initial memory allocation for bodies failed\n");
        return NULL;
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int z = 0; z < depth; z++) {
                if (!visited[x][y][z] && chunk[x][y][z] != BLOCK_AIR) {
                    if (*body_count == capacity) {
                        capacity *= 2;
                        Body* temp = realloc(bodies, capacity * sizeof(Body));
                        if (temp == NULL) {
                            printf("Memory reallocation for bodies failed\n");
                            return NULL;
                        }
                        bodies = temp;
                    }

                    bodies[*body_count] = *create_body();
                    flood_fill_body(
                        chunk, width, height, depth, x, y, z, chunk[x][y][z],
                        visited, &bodies[*body_count]
                    );
                    (*body_count)++;
                }
            }
        }
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            free(visited[x][y]);
        }
        free(visited[x]);
    }
    free(visited);

    return bodies;
}

bool body_is_fallable(Body body, char*** chunk) {
    for (int i = 0; i < body.size; i++) {
        Point3D point = body.points[i];
        if (point.y == 0
            || (chunk[point.x][point.y - 1][point.z] != BLOCK_AIR
                && chunk[point.x][point.y - 1][point.z]
                       != chunk[point.x][point.y][point.z])) {
            return false;
        }
    }
    return true;
}

void copy_chunk(char*** src, char*** dest, int width, int height, int depth) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int z = 0; z < depth; z++) {
                dest[x][y][z] = src[x][y][z];
            }
        }
    }
}

void clear_chunk(char*** chunk, int width, int height, int depth) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int z = 0; z < depth; z++) {
                chunk[x][y][z] = BLOCK_AIR;
            }
        }
    }
}

void move_body(
    Body body, char*** new_chunk, char*** chunk, int width, int height,
    int depth, int down_by
) {
    for (int i = 0; i < body.size; i++) {
        Point3D point = body.points[i];
        if (!is_in_bounds(
                width, height, depth, point.x, point.y - down_by, point.z
            )
            || !is_in_bounds(width, height, depth, point.x, point.y, point.z)) {
            continue;
        }
        new_chunk[point.x][point.y - down_by][point.z] =
            chunk[point.x][point.y][point.z];
    }
}

char*** chunk_apply_gravity(
    char*** chunk, int width, int height, int depth, int* new_height
) {
    char*** new_chunk = calloc_chunk(width, height, depth);
    if (new_chunk == NULL) {
        printf("Memory allocation new_chunk failed\n");
        return NULL;
    }

    bool flag = true;
    while (flag) {
        int body_count = 0;
        Body* bodies = find_bodies(chunk, width, height, depth, &body_count);

        for (int i = 0; i < body_count; i++) {
            bodies[i].fallable = body_is_fallable(bodies[i], chunk);
        }

        for (int i = 0; i < body_count; i++) {
            if (bodies[i].fallable) {
                move_body(bodies[i], new_chunk, chunk, width, height, depth, 1);
            } else {
                move_body(bodies[i], new_chunk, chunk, width, height, depth, 0);
            }
        }

        copy_chunk(new_chunk, chunk, width, height, depth);
        clear_chunk(new_chunk, width, height, depth);

        free_body_array(bodies, body_count);

        flag = false;
        for (int i = 0; i < body_count; i++) {
            if (bodies[i].fallable) {
                flag = true;
                break;
            }
        }
    }

    int max_y = height - 1;
    for (; max_y >= 0; max_y--) {
        bool empty = true;

        for (int x = 0; x < width; x++) {
            for (int z = 0; z < depth; z++) {
                if (chunk[x][max_y][z] != BLOCK_AIR) {
                    empty = false;
                }
            }
        }
        if (!empty) {
            break;
        }
    }
    max_y++;

    *new_height = max_y;
    copy_chunk(chunk, new_chunk, width, max_y, depth);

    free_chunk(chunk, width, height);

    return new_chunk;
}
