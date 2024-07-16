#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "include/dir_file.h"
#include "include/utils.h"

// int main(){
//     return 0;
// }
int open_file(char* fname, FILE** input) {
    if (access(fname, F_OK) == -1) {
        ERR_PRINT("The '%s' file does not exists\n", fname);
        return -1;
    }

    *input = fopen(fname, "r");
    if (input == NULL) {
        ERR_PRINT("Failed open '%s' file\n", fname);
        return -1;
    }

    return 1;
}   

// This function splits the input string (const char* str) to tokens
// and put the tokens in token_list. The return value must be the number
// of tokens in the given input string.
int parse_str_to_list(const char* str, char** token_list) {
    
    char *mutable_str;
    mutable_str = (char *)malloc(strlen(str) + 1);
    strcpy(mutable_str, str);

    // int height = strlen(str) + 10, width = 20;
    // for(int i=0; i<height; i++){
    //     token_list[i] = (char*) malloc ( sizeof(char) * width );
    // }

    int num_token = 0;
    char*ptr = strtok(mutable_str, "\n/");
    while(ptr != NULL)
    {
        token_list[num_token] = (char*) malloc (strlen(ptr) + 1);
        strcpy(token_list[num_token], ptr);
        ptr = strtok(NULL, "\n/");
        num_token++;
    }

    for(int i; i < num_token; i++){
        // printf("list elements >> %s", token_list[i]);
    }
    return num_token;
    /* Fill this function */
}

void free_token_list(char** token_list, int num_tokens) {
    for (int i = 0; i < num_tokens; i++) {
        free(token_list[i]);
    }
    // printf("none");
    free(token_list);
}

