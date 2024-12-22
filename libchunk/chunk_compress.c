#include <stdio.h>
#include <stdlib.h>
#include "chunk.h"

#define ONE_CHAR_MAX_BLOCKS 32
#define TWO_CHAR_MAX_BLOCKS 4096
#define BLOCK_TYPE_SHIFT 6
#define MASK 0xFF

typedef struct {
    int chars;
    int runs;
} Run_info;

typedef struct {
    int block;
    int count;
    Run_info run_info;
} Block_info;

unsigned char* flatten(char*** chunk, int width, int height, int depth) {
    unsigned char* flat_chunk =
        malloc(width * height * depth * sizeof(unsigned char));

    if (flat_chunk == NULL) {
        printf("Failed to allocate memory for flat chunk\n");
        return NULL;
    }

    int contor = 0;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int z = 0; z < depth; z++) {
                flat_chunk[contor++] = chunk[x][y][z];
            }
        }
    }

    return flat_chunk;
}

Run_info get_run_info(int count) {
    Run_info run_info;

    if (count < ONE_CHAR_MAX_BLOCKS) {
        run_info.chars = 1;
        run_info.runs = 1;
    } else if (count < TWO_CHAR_MAX_BLOCKS) {
        run_info.chars = 2;
        run_info.runs = 1;
    } else {
        Run_info sub_run_info = get_run_info(count - TWO_CHAR_MAX_BLOCKS);
        run_info.chars = sub_run_info.chars + 2;
        run_info.runs = sub_run_info.runs + 1;
    }

    return run_info;
}

void encode_block(
    unsigned char* encoded_chunk, int* encoded_index, int block, int count
) {
    if (count < ONE_CHAR_MAX_BLOCKS) {
        // Single-byte encoding
        encoded_chunk[(*encoded_index)++] = (block << BLOCK_TYPE_SHIFT) | count;
    } else {
        // Two-byte encoding
        encoded_chunk[(*encoded_index)++] =
            (block << BLOCK_TYPE_SHIFT) | (2 << 4) | (count >> 8);
        encoded_chunk[(*encoded_index)++] = count & MASK;
    }
}

unsigned char*
    chunk_encode(char*** chunk, int width, int height, int depth, int* length) {
    unsigned char* flat_chunk = flatten(chunk, width, height, depth);
    if (flat_chunk == NULL) {
        return NULL;
    }

    int flat_length = width * height * depth;

    // Step 1: Count the number of runs
    int run_count = 1;
    for (int i = 1; i < flat_length; i++) {
        if (flat_chunk[i] != flat_chunk[i - 1]) {
            run_count++;
        }
    }

    // Step 2: Allocate memory for `to_encode`
    Block_info* to_encode = calloc(run_count, sizeof(Block_info));
    if (to_encode == NULL) {
        printf("Failed to allocate memory for to_encode\n");
        free(flat_chunk);
        return NULL;
    }

    // Step 3: Fill the `to_encode` array
    int curr_block_index = 0;
    to_encode[0].block = flat_chunk[0];
    to_encode[0].count = 1;

    for (int i = 1; i < flat_length; i++) {
        if (flat_chunk[i] == flat_chunk[i - 1]) {
            to_encode[curr_block_index].count++;
        } else {
            curr_block_index++;
            to_encode[curr_block_index].block = flat_chunk[i];
            to_encode[curr_block_index].count = 1;
        }
    }

    // Step 4: Calculate run info for each block
    for (int i = 0; i < run_count; i++) {
        to_encode[i].run_info = get_run_info(to_encode[i].count);
    }

    // Step 5: Calculate the total encoded length
    int encoded_length = 0;
    for (int i = 0; i < run_count; i++) {
        encoded_length += to_encode[i].run_info.chars;
    }

    // Step 6: Allocate memory for encoded chunk
    unsigned char* encoded_chunk =
        malloc(encoded_length * sizeof(unsigned char));
    if (encoded_chunk == NULL) {
        printf("Failed to allocate memory for encoded_chunk\n");
        free(flat_chunk);
        free(to_encode);
        return NULL;
    }

    // Step 7: Encode the blocks
    int encoded_index = 0;
    for (int i = 0; i < run_count; i++) {
        int remaining_count = to_encode[i].count;
        int block_type = to_encode[i].block;

        while (remaining_count > 0) {
            int chunk_count = remaining_count < TWO_CHAR_MAX_BLOCKS
                                  ? remaining_count
                                  : TWO_CHAR_MAX_BLOCKS;

            encode_block(
                encoded_chunk, &encoded_index, block_type, chunk_count
            );
            remaining_count -= chunk_count;
        }
    }

    // Step 8: Clean up and return
    free(flat_chunk);
    free(to_encode);

    *length = encoded_length;
    return encoded_chunk;
}

char*** chunk_decode(unsigned char* code, int width, int height, int depth) {
}
