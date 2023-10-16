#include "utils.h"
#include <stdlib.h>


int count_newlines(char text[]){
    int count = 0;
    for(int i = 0; text[i] != '\0'; i++){
        if(text[i] == '\n'){
            count++;
        }
    }
    return count;
}
