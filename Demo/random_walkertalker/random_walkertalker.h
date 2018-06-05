#ifndef RANDOM_WALKERTALKER_H
#define RANDOM_WALKERTALKER_H

#include "VASTVerse.h"
#include "VASTTypes.h"

#define DEBUG false
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

void send_to_neighbours (Vast::VAST* vastclient, string to_send);
void gen_random_str(char *s, const int len);
void gen_random_sentence(char *sentence, const size_t len);
void printMessage(Vast::Message msg);

#endif
