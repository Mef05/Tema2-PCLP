#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../libchunk/chunk.h"

void print_chunk(char*** chunk, int width, int height, int depth) {
    printf("%d\n%d\n%d\n", width, height, depth);

    for (int x = 0; x < width; x++) { 
        for (int y = height-1; y >= 0; y--) {
            for (int z = 0; z < depth; z++)
                printf("%d ", chunk[x][y][z]);
            printf("\n");
        }
        printf("\n");
    }
}

char*** read_chunk(int* width, int* height, int* depth) {
    scanf("%d%d%d", width, height, depth);
    
    char ***chunk = malloc(*width * sizeof *chunk);
    
    for (int x = 0; x < *width; x++) {
        chunk[x] = malloc(*height * sizeof **chunk);
        for (int y = *height-1; y >= 0; y--) {
            chunk[x][y] = malloc(*depth * sizeof ***chunk);

            for (int z = 0; z < *depth; z++)
                scanf("%hhd", &chunk[x][y][z]);
        }
    }

    return chunk;
}

char*** test_task1(char*** chunk, int width, int height, int depth, FILE* params_file) {
    int x, y, z;
    char block;
    fscanf(params_file, "%d%d%d%hhd", &x, &y, &z, &block);
    
    return chunk_place_block(chunk, width, height, depth, x, y, z, block);
}

char*** test_task2(char*** chunk, int width, int height, int depth, FILE* params_file) {
    int x0, y0, z0, x1, y1, z1;
    char block;
    fscanf(params_file, "%d%d%d%d%d%d%hhd", &x0, &y0, &z0, &x1, &y1, &z1, &block);

    return chunk_fill_cuboid(chunk, width, height, depth, x0, y0, z0, x1, y1, z1, block);
}

char*** test_task3(char*** chunk, int width, int height, int depth, FILE* params_file) {
    int x, y, z;
    char block;
    double radius;
    fscanf(params_file, "%d%d%d%lg%hhd", &x, &y, &z, &radius, &block);
    
    return chunk_fill_sphere(chunk, width, height, depth, x, y, z, radius, block);
}

char*** test_task4(char*** chunk, int width, int height, int depth, FILE* params_file) {
    char target_block;
    char shell_block;
    fscanf(params_file, "%hhd%hhd", &target_block, &shell_block);
    
    return chunk_shell(chunk, width, height, depth, target_block, shell_block);
}

char*** test_task5(char*** chunk, int width, int height, int depth, FILE* params_file) {
    int x, y, z;
    char block;
    fscanf(params_file, "%d%d%d%hhd", &x, &y, &z, &block);
    
    return chunk_fill_xz(chunk, width, height, depth, x, y, z, block);
}

char*** test_task6(char*** chunk, int width, int height, int depth, FILE* params_file) {
    int x, y, z;
    char block;
    fscanf(params_file, "%d%d%d%hhd", &x, &y, &z, &block);
    
    return chunk_fill(chunk, width, height, depth, x, y, z, block);
}

char*** test_task7(char*** chunk, int width, int height, int depth) {
    return chunk_rotate_y(chunk, width, height, depth);
}

char*** test_task8(char*** chunk, int width, int height, int depth, int* new_height) {
    return chunk_apply_gravity(chunk, width, height, depth, new_height);
}

unsigned char* test_task9(char*** chunk, int width, int height, int depth, int* length) {
    return chunk_encode(chunk, width, height, depth, length);
}

char*** test_task10(unsigned char* code, FILE* params_file, int* width, int* height, int* depth) {
    fscanf(params_file, "%d%d%d", width, height, depth);
    return chunk_decode(code, *width, *height, *depth);
}

int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        fputs("Usage: ./check <task_number> [params_file]\n", stderr);
        return EXIT_FAILURE;
    }

    long task = strtol(argv[1], 0, 0);
    int retval = EXIT_SUCCESS;

    int width = { 0 }, height = { 0 }, depth = { 0 };
    char*** chunk = { 0 };

    unsigned char* code = { 0 };
    int code_length = { 0 };
    FILE* params_file = { 0 };

    char*** (*test_funcs_params[])(char***, int, int, int, FILE*) = {
        [0] = 0,  // Task 0 does not exist
        test_task1,
        test_task2,
        test_task3,
	    test_task4,
	    test_task5,
        test_task6,
    };

    // For all tasks except 10, read the chunk
    if (task != 10)
        chunk = read_chunk(&width, &height, &depth);
    
    // For all tasks except 7, 8, 9, read the params file
    if (task != 7 && task != 8 && task != 9)
        params_file = fopen(argv[2], "r");

    switch (task) {
    case 1 ... 6:
        chunk = test_funcs_params[task](chunk, width, height, depth, params_file);
        break;

    case 7:
        chunk = test_task7(chunk, width, height, depth);

        int temp = width;
        width = depth;
        depth = temp;
        break;
    
    case 8:
        chunk = test_task8(chunk, width, height, depth, &height);
        break;

    case 9:
        // Check if stdout is a file
        if (isatty(STDOUT_FILENO)) {
            fputs("Binary output must be redirected to a file or piped to a program.\n", stderr);
            retval = EXIT_FAILURE;
            break;
        }

        code = test_task9(chunk, width, height, depth, &code_length);

        setvbuf(stdout, NULL, _IONBF, 0);
        fwrite(code, code_length, 1, stdout);
        free(code);
        break;
    
    case 10:
        // Check if stdin is a file
        if (isatty(STDIN_FILENO)) {
            fputs("Binary input must be redirected from a file or piped from a program.\n", stderr);
            retval = EXIT_FAILURE;
            break;
        }

        // Find length of code from stdin
        struct stat info = { 0 };
        if (fstat(STDIN_FILENO, &info) != 0) {
            perror("fstat");
            retval = EXIT_FAILURE;
            break;
        }
        
        code_length = info.st_size;

        // Read code from stdin
        code = malloc(code_length);

        size_t blocks_read = fread(code, code_length, 1, stdin);
        if (blocks_read != 1) {
            perror("fread");
            free(code);
            retval = EXIT_FAILURE;
            break;
        }

        // Decode chunk
        chunk = test_task10(code, params_file, &width, &height, &depth);
        free(code);
        break;

    default:
        
        fputs("Invalid task number.\n", stderr);
        retval = EXIT_FAILURE;
    }

    // For all tasks except 9, print chunk
    if (task != 9)
        print_chunk(chunk, width, height, depth);

    for (int x = 0; x < width; x++) {
        for (int y = height-1; y >= 0; y--)
            free(chunk[x][y]);
        free(chunk[x]);
    }
    free(chunk);

    // For tasks requiring a params file, close it
    if (params_file)
        fclose(params_file);

    return retval;
}
