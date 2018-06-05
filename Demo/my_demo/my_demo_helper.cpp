//my_demo_helper.cpp

#include "my_demo.h"
using namespace Vast;

void send_to_neighbours (VAST* vastclient, string to_send) {
// send off input to AOI neighbors
    // note: self is also included as the 1st neighbor
    Message send_msg (12);

    send_msg.clear (123);
    send_msg.store(vastclient->getSelf()->id);
    send_msg.store (to_send.c_str(), to_send.length(), true);

    /* SEND-based */
    vector<Node *>&nodes = vastclient->list ();

    for (size_t j=0; j < nodes.size (); j++)
    {
        Position pos = nodes[j]->aoi.center;
        Vast::id_t id      = nodes[j]->id;
        send_msg.addTarget (id);
    }
    // send away message
    vastclient->send (send_msg);

}

void gen_random_str(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}
