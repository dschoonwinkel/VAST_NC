#include "random_walkertalker.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

int main() {

    srand((unsigned) time(NULL));

    char sentence[100];
    int len = sizeof(sentence) / sizeof(sentence[0]);
    strcpy(sentence, "");

    for (int i = 0; i < 1; i++) {
        gen_random_sentence(sentence, len);
        std::cout << sentence << std::endl <<std::endl;
    }

    return 0;

}
