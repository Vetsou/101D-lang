#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }

result_t process_file(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file [%s]\n", filepath);
        return RESULT_FAIL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_len = ftell(file);
    rewind(file);

    char *file_buffer = (char *)malloc(sizeof(char) * (file_len + 1));
    size_t read_size = fread(file_buffer, 1, file_len, file);

    if (read_size != file_len) {
        fprintf(stderr, "Error reading file [%s]\n", filepath);
        fclose(file);
        free(file_buffer);
        return RESULT_FAIL;
    }

    file_buffer[file_len] = '\0';
    fclose(file);

    // Lexer
    lexer_t lexer;
    lexer_init(&lexer, file_buffer, read_size);

    token_t token;
    while (true) {
        token = lexer_scan(&lexer);
        printf("TOK [%d][L:%d][%.*s]\n", token.type, token.line, token.length, token.start);
        if (token.type == TOK_EOF || token.type == TOK_ERROR) break;
    }

    return RESULT_OK;
}

int main(int argc, char *argv[]) {
    ASSERT(argc >= 2, "Error: Specify input files\n");

    for (int i = 1; i < argc; i++) {
        if (process_file(argv[i]) == RESULT_FAIL) {
            fprintf(stderr, "Error: Cannot process file [%s]\n", argv[i]);
        }
    }

    return 0;
}