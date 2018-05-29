#ifndef MY_DEMO_H
#define MY_DEMO_H

#include "VASTVerse.h"

size_t send_to_neighbours (Vast::VAST* vastclient, string to_send);
void gen_random_str(char *s, const int len);

#endif